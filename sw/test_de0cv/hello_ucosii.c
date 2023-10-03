#include <stdio.h>
#include <ucos_ii.h>
#include <system.h>
#include <sys/alt_debug.h>

#define TEST_SERVO
#ifdef TEST_SERVO

#include <servo_controller.h>
#include <avl_fifo_uart.h>
#include <avl_tmp101.h>

TMP101_Reg *tmp101Dev = (TMP101_Reg*)TMP101_BASE;

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

void task1(void* pdata)
{
	int stt;

    servoDev = servo_controller_open_dev(SERVO4X_NAME);

    SERVO_CONTROLLER_CFG(servoDev)->drv_en[0] = true;
    SERVO_CONTROLLER_CFG(servoDev)->spi_speed = 100000;
    SERVO_CONTROLLER_CFG(servoDev)->pwm_base_freq = 1000000;
    SERVO_CONTROLLER_CFG(servoDev)->pwm_freq = 5000;
    SERVO_CONTROLLER_CFG(servoDev)->pwm_trig_rate = 10;

    SERVO_CONTROLLER_CFG(servoDev)->i_max[0] = 10;

    SERVO_CONTROLLER_CFG(servoDev)->Pos_lsb[0] = 0.2;
    SERVO_CONTROLLER_CFG(servoDev)->Current_lsb[0] = 0.2;


    SERVO_CONTROLLER_CFG(servoDev)->n_motor_pole[0] = 5;
    SERVO_CONTROLLER_CFG(servoDev)->n_motor_ratio[0] = 1;

    stt = servo_controller_apply_configure(servoDev);
    ALT_DEBUG_ASSERT((stt == 0));

    servo_controller_start(servoDev);

    int16_t duty[SERVO_CONTROLLER_NUM_SERVO] = {0};
//
//    duty[0] = SERVO_CONTROLLER_FLOAT_TO_FIXED(-0.4f);
//    duty[0] = 0;
//    duty[1] = 0;
//    duty[2] = 0;
//    duty[3] = 0;
//
    servo_controller_update_duty(servoDev, duty);


  while (1)
  {
	  OSTimeDly(100);

	  servo_controller_get_duty(servoDev, duty);
	  printf("pos = %ld\n", servoDev->data->hall_position[0] >> 15);
	  printf("duty = %d\n", duty[0]);
	  printf("realtime_err = %d\n", servoDev->BASE->flag.field.realtime_err);
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

  OSTaskCreateExt(task_servo_business,
		          (void*) SERVO4X_NAME,
				  &task_servo_stk[TASK_STACKSIZE-1],
				  TASK_SERVO_PRIORITY,
				  TASK_SERVO_PRIORITY,
				  task_servo_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  OSStart();
  return 0;
}

#endif
