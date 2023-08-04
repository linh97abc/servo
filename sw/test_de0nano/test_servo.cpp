#include <stdio.h>
#include <ucos_ii.h>
#include <system.h>

#define TEST_SERVO
#ifdef TEST_SERVO

#include <servo_controller.h>

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    task2_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */

#define TASK1_PRIORITY      1
#define TASK2_PRIORITY      2

struct servo_controller_dev_t *servoDev;

void task1(void* pdata)
{
    printf("Hello from task1\n");

    servoDev = servo_controller_open_dev(SERVO4X_NAME);

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
    SERVO_CONTROLLER_CFG(servoDev)->pidArgument[0].E_lsb = 1;
    SERVO_CONTROLLER_CFG(servoDev)->pidArgument[0].U_lsb = 1;
    SERVO_CONTROLLER_CFG(servoDev)->pidArgument[0].dT = 1;
    SERVO_CONTROLLER_CFG(servoDev)->pidArgument[0].kp = 100;

    servo_controller_apply_configure(servoDev);

    servo_controller_start(servoDev);

    int16_t duty[SERVO_CONTROLLER_NUM_SERVO];
//
//    duty[0] = SERVO_CONTROLLER_FLOAT_TO_FIXED(0.4f);
//    duty[1] = 0;
//    duty[2] = 0;
//    duty[3] = 0;
//
//    servo_controller_update_duty(servoDev, duty);

//    servo_controller_set_position(servoDev, SERVO_CONTROLLER_SERVO_ID_0, 1000);
    servo_controller_set_position(servoDev, SERVO_CONTROLLER_SERVO_ID_0, 200);


  while (1)
  {
	  OSTimeDly(100);

	  servo_controller_get_duty(servoDev, duty);
	  printf("pos = %ld\n", servoDev->BASE->pos_phase[0]);
	  printf("duty = %d\n", duty[0]);
	  printf("realtime_err = %d\n", servoDev->BASE->flag.field.realtime_err);

	  if (servoDev->BASE->pos_phase[0] > 1000)
	  {
		  servo_controller_stop(servoDev);
	  }
  }
}
/* Prints "Hello World" and sleeps for three seconds */
void task2(void* pdata)
{
	OSTaskSuspend(OS_PRIO_SELF);

  while (1)
  {

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


  OSTaskCreateExt(task2,
                  NULL,
                  &task2_stk[TASK_STACKSIZE-1],
                  TASK2_PRIORITY,
                  TASK2_PRIORITY,
                  task2_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);
  OSStart();
  return 0;
}

#endif
