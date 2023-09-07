#include <Service/Command.h>
#include <system.h>

#include <stdint.h>

#include <avl_fifo_uart.h>
#include <mavlink/servomsg/mavlink.h>

namespace service
{
class MavlinkService : public ICommandService
{
	FifoUart_Dev *uartDev;
	uint8_t sysID;
	uint8_t comID;


	void SendCpuUsage(uint8_t seq, uint8_t cpuUsage);
	void SendTaskInfo(const os::TaskInfo &taskInfo);
	void SendServoPos(uint8_t seq, uint8_t isRunning, const int16_t pos[CONFIG_NUM_SERVO]);
public:
	MavlinkService();
};
}

service::MavlinkService::MavlinkService()
{

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
