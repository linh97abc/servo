#include <stdio.h>
#include <ucos_ii.h>
#include <system.h>
#include <sys/alt_debug.h>

#define TEST_SERVO
#ifdef TEST_SERVO

#include <servo_controller.h>
#include <avl_fifo_uart.h>


#define SERVO4X_NAME SERVO_CONTROLLERV1_0_NAME

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    task2_stk[TASK_STACKSIZE];
OS_STK    task_servo_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */

#define TASK_SERVO_PRIORITY      2
#define TASK1_PRIORITY      4
#define TASK2_PRIORITY      3

struct servo_controller_dev_t *servoDev;

#define SELECTED_CHANNEL 1

void on_new_process(struct servo_controller_dev_t *dev)
{
	servo_controller_notify_duty_changed(dev);
}



void on_err(
		struct servo_controller_dev_t* dev,
		enum Servo_controller_servo_id_t id,
		enum Servo_controller_err_t err)
{
//	servo_controller_notify_duty_changed(dev);
}

void task1(void* pdata)
{
	int stt;

    servoDev = servo_controller_open_dev(SERVO4X_NAME);

    SERVO_CONTROLLER_CFG(servoDev)->drv_en[SELECTED_CHANNEL] = true;
    SERVO_CONTROLLER_CFG(servoDev)->filter_level = SERVO_CONTROLLER_FILTER_LEVEL_32;
    SERVO_CONTROLLER_CFG(servoDev)->spi_speed = 5000000;
    SERVO_CONTROLLER_CFG(servoDev)->pwm_base_freq = 1000000;
    SERVO_CONTROLLER_CFG(servoDev)->pwm_freq = 5000;
    SERVO_CONTROLLER_CFG(servoDev)->pwm_trig_rate = 10;
    SERVO_CONTROLLER_CFG(servoDev)->i_max[SELECTED_CHANNEL] = 3000;

    SERVO_CONTROLLER_CFG(servoDev)->on_new_process = on_new_process;
    SERVO_CONTROLLER_CFG(servoDev)->on_err = on_err;

    stt = servo_controller_apply_configure(servoDev);
    ALT_DEBUG_ASSERT((stt == 0));

    servo_controller_start(servoDev);

    servo_controller_enable_interrupt(servoDev, SERVO_CONTROLLER_IE_MEA_TRIG_BIT);

    int16_t duty[SERVO_CONTROLLER_NUM_SERVO] = {0};
//
    duty[SELECTED_CHANNEL] = SERVO_CONTROLLER_FLOAT_TO_FIXED(-0.4f);
//    duty[0] = 0;
//    duty[1] = 0;
//    duty[2] = 0;
//    duty[3] = 0;
//
    servo_controller_set_and_notify_duty_changed(servoDev, duty);


  while (1)
  {
	  OSTimeDlyHMSM(0, 0, 0, 100);
	  int16_t current[SERVO_CONTROLLER_NUM_SERVO];
	  int16_t pos[SERVO_CONTROLLER_NUM_SERVO];
	  int32_t pos_phase[SERVO_CONTROLLER_NUM_SERVO];

	  servo_controller_get_duty(servoDev, duty);
	  servo_controller_get_position(servoDev, current);
	  servo_controller_get_current(servoDev, pos);
	  servo_controller_get_phase_position(servoDev, pos_phase);
	  printf("pos_phase = %ld\n", pos_phase[SELECTED_CHANNEL]);
	  printf("pos = %d\n", current[SELECTED_CHANNEL]);
	  printf("current = %d\n", pos[SELECTED_CHANNEL]);

	  printf("real_time_err = %d\n", servoDev->BASE->flag.field.realtime_err);

  }
}

/* The main function creates two task and starts multi-tasking */
int main(void)
{

  printf("MicroC/OS-II\n");

  OSTaskCreateExt(task1,
                  NULL,
                  &task1_stk[TASK_STACKSIZE-1],
                  TASK1_PRIORITY,
                  TASK1_PRIORITY,
                  task1_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);


  OSStart();
  return 0;
}

#endif
