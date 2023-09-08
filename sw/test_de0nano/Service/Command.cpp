#include <Service/Command.h>
#include <system.h>
#include <string.h>
#include <stdint.h>

#include <avl_fifo_uart.h>
#include <mavlink/servomsg/mavlink.h>

#include <appconfig.h>
#include <ucos_ii.h>
#include <sys/alt_debug.h>
#include <os_wrapper.h>
#include <stdio.h>

#include <Logging.h>

#define FLAG_GET_CONF_REQ 1u
#define FLAG_SET_CONF_REQ (1u << 1)

OS_STACK_DEF(mavlink_recv_task_stk);
OS_STACK_DEF(msg_async_recv_task_stk);

namespace service
{
class MavlinkService : public ICommandService
{
	FifoUart_Dev *uartDev;
	uint8_t sysID;
	uint8_t comID;
	OS_FLAG_GRP *flag;

	bool isConfigMsgReceived;
	mavlink_config_t configMsgRecv;



	void SendCpuUsage(uint8_t seq, uint8_t cpuUsage);
	void SendTaskInfo(const os::TaskInfo &taskInfo);
	void SendServoPos(uint8_t seq, uint8_t isRunning, const int16_t pos[CONFIG_NUM_SERVO]);

	void OnMsgReceived(const mavlink_message_t &msg);
	void SendACK(uint8_t seq);
public:
	static void TaskMavlinkRecv(void* arg);
	static void TaskOnMsgReceivedAsync(void* arg);

	MavlinkService();
};
}

service::MavlinkService::MavlinkService()
:sysID(0), comID(0), isConfigMsgReceived(false), configMsgRecv()
{
	INT8U err;
	this->uartDev = FifoUart_OpenDev(AVL_FIFO_UART_0_NAME);
	ALT_DEBUG_ASSERT((this->uartDev != NULL));

	FifoUart_SetBaudrate(this->uartDev, 921600);

	this->flag = OSFlagCreate(0, &err);
	ALT_DEBUG_ASSERT((err == OS_ERR_NONE));

	os::CreateTask("TaskMavlinkRecv", TaskMavlinkRecv, this, mavlink_recv_task_stk, MAVLINK_RECV_TASK_PRIO);
	os::CreateTask("TaskMsgAssyncRecv", TaskOnMsgReceivedAsync, this, msg_async_recv_task_stk, MSG_MAVLINK_ASYNC_RECV_TASK_PRIO);
}

void service::MavlinkService::SendACK(uint8_t seq)
{
	mavlink_message_t msg;
	mavlink_msg_ack_pack(this->sysID, this->comID, &msg, seq);

	FifoUart_Write(this->uartDev, &msg.magic, msg.len + MAVLINK_NUM_NON_PAYLOAD_BYTES);
}

void service::MavlinkService::OnMsgReceived(const mavlink_message_t &msg)
{
	INT8U err;
	switch (msg.msgid) {
		case MAVLINK_MSG_ID_CONFIG:
		{
			mavlink_config_t *configMsg = (mavlink_config_t *)&msg.payload64[0];
			this->SendACK(configMsg->seq);

			LOG_DEBUG("MAVLINK_MSG_ID_CONFIG received, seq: %d\n", configMsg->seq);

			if (!this->isConfigMsgReceived || (configMsg->seq != this->configMsgRecv.seq))
			{

				memcpy(&this->configMsgRecv, configMsg, sizeof(mavlink_config_t));

				OSFlagPost(this->flag, FLAG_SET_CONF_REQ, OS_FLAG_SET, &err);
				ALT_DEBUG_ASSERT((err == OS_ERR_NONE));

			}

			this->isConfigMsgReceived = true;
			break;

		}

		case MAVLINK_MSG_ID_CONTROL:
		{
			mavlink_control_t *controlMsg = (mavlink_control_t *)&msg.payload64[0];
			this->SendACK(controlMsg->seq);

			LOG_DEBUG("MAVLINK_MSG_ID_CONTROL received, start: %d\n", controlMsg->start);
			if(controlMsg->start)
			{

				this->servoService->Start();
			}
			else
			{
				this->servoService->Stop();
			}

			break;

		}

		case MAVLINK_MSG_ID_GET_CONF:
		{
			mavlink_get_conf_t *getConfMsg = (mavlink_get_conf_t *)&msg.payload64[0];
			this->SendACK(getConfMsg->seq);

			LOG_DEBUG("MAVLINK_MSG_ID_GET_CONF received, seq: %d\n", getConfMsg->seq);

			OSFlagPost(this->flag, FLAG_GET_CONF_REQ, OS_FLAG_SET, &err);
			ALT_DEBUG_ASSERT((err == OS_ERR_NONE));

			break;

		}

		case MAVLINK_MSG_ID_POS_MON:
		{
			mavlink_pos_mon_t *posMsg = (mavlink_pos_mon_t *)&msg.payload64[0];
			this->servoService->SetPosition(posMsg->pos);
			break;

		}

		default:
			break;
	}
}

void service::MavlinkService::TaskOnMsgReceivedAsync(void* arg)
{
	MavlinkService *self = (MavlinkService*)arg;
	INT8U err;

	uint8_t msgConfSeq = 0;

	while(true)
	{
		OS_FLAGS flag = OSFlagPend(self->flag, FLAG_GET_CONF_REQ | FLAG_SET_CONF_REQ, OS_FLAG_WAIT_SET_ANY, 0, &err);
		ALT_DEBUG_ASSERT((err == OS_ERR_NONE));

		if (flag & FLAG_GET_CONF_REQ)
		{
			LOG_DEBUG("On FLAG_GET_CONF_REQ");
			ServoConfigArg cfg;
			mavlink_message_t msg;
			self->flashService->GetServoConfig(cfg);

			mavlink_msg_config_pack(self->sysID, self->comID, &msg, msgConfSeq++, cfg.kp, cfg.ki, cfg.kd);
			FifoUart_Write(self->uartDev, &msg.magic, msg.len + MAVLINK_NUM_NON_PAYLOAD_BYTES);
		}

		if (flag & FLAG_SET_CONF_REQ)
		{
			LOG_DEBUG("On FLAG_SET_CONF_REQ");
			ServoConfigArg cfg;

			memcpy(cfg.kp, self->configMsgRecv.Kp, sizeof(cfg.kp));
			memcpy(cfg.ki, self->configMsgRecv.Ki, sizeof(cfg.ki));
			memcpy(cfg.kd, self->configMsgRecv.Kd, sizeof(cfg.kd));

			self->flashService->SaveServoConfig(cfg);
		}

		OSFlagPost(self->flag, FLAG_GET_CONF_REQ | FLAG_SET_CONF_REQ, OS_FLAG_CLR, &err);
		ALT_DEBUG_ASSERT((err == OS_ERR_NONE));
	}


}

void service::MavlinkService::TaskMavlinkRecv(void *arg)
{
	MavlinkService *self = (MavlinkService*)arg;

	uint8_t buff[CONFIG_UART_RECV_LEN];
	int len;
	mavlink_status_t status;
	mavlink_message_t msg;

	while(true)
	{
		len = FifoUart_Read(self->uartDev, buff, CONFIG_UART_RECV_LEN);

		for (int i = 0; i < len; i++)
		{
			if (mavlink_parse_char(0, buff[i], &msg, &status))
			{
				self->OnMsgReceived(msg);
			}

		}
	}
}

void service::MavlinkService::SendCpuUsage(uint8_t seq, uint8_t cpuUsage)
{
	mavlink_message_t msg;
	mavlink_msg_cpu_usage_pack(this->sysID, this->comID, &msg, seq, cpuUsage);

	FifoUart_Write(this->uartDev, &msg.magic, msg.len + MAVLINK_NUM_NON_PAYLOAD_BYTES);

}

void service::MavlinkService::SendTaskInfo(const os::TaskInfo &taskInfo)
{
	mavlink_message_t msg;
	mavlink_msg_task_manager_pack(
			this->sysID,
			this->comID,
			&msg, taskInfo.taskName, taskInfo.prio, taskInfo.stk_used, taskInfo.stk_size, taskInfo.swCnt);

	FifoUart_Write(this->uartDev, &msg.magic, msg.len + MAVLINK_NUM_NON_PAYLOAD_BYTES);
}

void service::MavlinkService::SendServoPos(uint8_t seq, uint8_t isRunning, const int16_t pos[CONFIG_NUM_SERVO])
{
	mavlink_message_t msg;
	mavlink_msg_pos_mon_pack(this->sysID, this->comID, &msg, seq, isRunning, pos);

	FifoUart_Write(this->uartDev, &msg.magic, msg.len + MAVLINK_NUM_NON_PAYLOAD_BYTES);
}

service::ICommandService *service::Get_MavlinkCommandService()
{
	static MavlinkService inst;
	return &inst;
}
