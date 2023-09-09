#include <Service/Servo.h>
#include <servo_controller.h>
#include <system.h>
#include <sys/alt_debug.h>

#include <appconfig.h>
#include <os_wrapper.h>
#include <Logging.h>

OS_STACK_DEF(servoTaskStk);

namespace service
{
class ServoService : public IServoService
{
	servo_controller_dev_t *servoDev;

	int ApplyConfig(ServoConfigArg &arg);
	int Start();
	int Stop();
	int SetPosition(int16_t pos[CONFIG_NUM_SERVO]);
	int GetPosition(int16_t pos[CONFIG_NUM_SERVO]);
//	static void TaskServoBusiness(void *arg);

//	void DoBusiness();
public:
	ServoService();
};
}

//void service::ServoService::TaskServoBusiness(void *arg)
//{
//	ServoService *self = (ServoService*)arg;
//	self->DoBusiness();
//}
//
//void service::ServoService::DoBusiness()
//{
//	servo_controller_
//}

void task_servo_wrppaer(servo_controller_dev_t *dev, void *arg)
{
	int16_t duty[4] = {0};
	servo_controller_update_duty(dev, duty );
}

service::ServoService::ServoService()
{
	this->servoDev = servo_controller_open_dev(SERVO4X_NAME);
	ALT_DEBUG_ASSERT((this->servoDev != NULL));


    SERVO_CONTROLLER_CFG(servoDev)->drv_en[0] = true;
    SERVO_CONTROLLER_CFG(servoDev)->spi_speed = 1000000;
    SERVO_CONTROLLER_CFG(servoDev)->pwm_base_freq = 1000000;
    SERVO_CONTROLLER_CFG(servoDev)->pwm_freq = 5000;
    SERVO_CONTROLLER_CFG(servoDev)->pwm_trig_rate = 10;
//    SERVO_CONTROLLER_CFG(servoDev)->pwm_trig_rate = 1;

    SERVO_CONTROLLER_CFG(servoDev)->i_max[0] = SERVO_CONTROLLER_FLOAT_TO_FIXED(0.5f);
    SERVO_CONTROLLER_CFG(servoDev)->i_max[1] = SERVO_CONTROLLER_FLOAT_TO_FIXED(0.5f);
    SERVO_CONTROLLER_CFG(servoDev)->i_max[2] = SERVO_CONTROLLER_FLOAT_TO_FIXED(0.5f);
    SERVO_CONTROLLER_CFG(servoDev)->i_max[3] = SERVO_CONTROLLER_FLOAT_TO_FIXED(0.5f);

    SERVO_CONTROLLER_CFG(servoDev)->closed_loop_en = true;
    SERVO_CONTROLLER_CFG(servoDev)->pidArgument[0].E_lsb = 0.010986;
    SERVO_CONTROLLER_CFG(servoDev)->pidArgument[0].U_lsb = 1;
    SERVO_CONTROLLER_CFG(servoDev)->pidArgument[0].dT = 1.0/500;
    SERVO_CONTROLLER_CFG(servoDev)->pidArgument[0].kp = 100;
    SERVO_CONTROLLER_CFG(servoDev)->pidArgument[0].ramp_rate = 1000;

    SERVO_CONTROLLER_CFG(servoDev)->n_motor_pole[0] = 7;
    SERVO_CONTROLLER_CFG(servoDev)->n_motor_ratio[0] = 7;
    SERVO_CONTROLLER_CFG(servoDev)->K_position_filter[0] = SERVO_CONTROLLER_FLOAT_TO_FIXED(0);

//    SERVO_CONTROLLER_CFG(servoDev)->on_new_process = task_servo_wrppaer;
//    SERVO_CONTROLLER_CFG(servoDev)->callback_arg = (void*) SERVO4X_NAME;


//    task_servo_business
//    os::CreateTask("TaskServo", task_servo_business, (void*) SERVO4X_NAME, servoTaskStk, SERVO_TASK_PRIO);
}

int service::ServoService::ApplyConfig(ServoConfigArg &arg)
{
	int stt;

    SERVO_CONTROLLER_CFG(servoDev)->pidArgument[0].kp = arg.kp[0];
    SERVO_CONTROLLER_CFG(servoDev)->pidArgument[0].ki = arg.ki[0];
    SERVO_CONTROLLER_CFG(servoDev)->pidArgument[0].kd = arg.kd[0];

    stt = servo_controller_apply_configure(this->servoDev);
    return stt;
}

int service::ServoService::Start()
{
	LOG_DEBUG("Servo Start\n");

	// servo_controller_start(this->servoDev);

    // int16_t duty[SERVO_CONTROLLER_NUM_SERVO];
    // duty[0] = SERVO_CONTROLLER_FLOAT_TO_FIXED(-0.4f);
	// servo_controller_update_duty(this->servoDev, duty);
	// return 0;

	return servo_controller_start(this->servoDev);
}

int service::ServoService::Stop()
{
	LOG_DEBUG("Servo Stop\n");
	return servo_controller_stop(this->servoDev);
}

int service::ServoService::SetPosition(int16_t pos[CONFIG_NUM_SERVO])
{
	servo_controller_set_position(this->servoDev, SERVO_CONTROLLER_SERVO_ID_0, pos[0]);
	servo_controller_set_position(this->servoDev, SERVO_CONTROLLER_SERVO_ID_1, pos[1]);
	servo_controller_set_position(this->servoDev, SERVO_CONTROLLER_SERVO_ID_2, pos[2]);
	servo_controller_set_position(this->servoDev, SERVO_CONTROLLER_SERVO_ID_3, pos[3]);

	return 0;
}

int service::ServoService::GetPosition(int16_t pos[CONFIG_NUM_SERVO])
{
	return servo_controller_get_position(this->servoDev, pos);
}

service::IServoService *service::Get_HwServoService()
{
	static ServoService inst;
	return &inst;
}
