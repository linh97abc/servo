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
FifoUart_Dev *fifoUartDev;

void task1(void* pdata)
{
	int stt;
    printf("Hello from task1\n");

    servoDev = servo_controller_open_dev(SERVO4X_NAME);

    SERVO_CONTROLLER_CFG(servoDev)->drv_en[0] = true;
    SERVO_CONTROLLER_CFG(servoDev)->spi_speed = 100000;
    SERVO_CONTROLLER_CFG(servoDev)->pwm_base_freq = 1000000;
    SERVO_CONTROLLER_CFG(servoDev)->pwm_freq = 5000;
    SERVO_CONTROLLER_CFG(servoDev)->pwm_trig_rate = 10;
//    SERVO_CONTROLLER_CFG(servoDev)->pwm_trig_rate = 1;

    SERVO_CONTROLLER_CFG(servoDev)->i_max[0] = SERVO_CONTROLLER_FLOAT_TO_FIXED(0.5f);
    SERVO_CONTROLLER_CFG(servoDev)->i_max[1] = SERVO_CONTROLLER_FLOAT_TO_FIXED(0.5f);
    SERVO_CONTROLLER_CFG(servoDev)->i_max[2] = SERVO_CONTROLLER_FLOAT_TO_FIXED(0.5f);
    SERVO_CONTROLLER_CFG(servoDev)->i_max[3] = SERVO_CONTROLLER_FLOAT_TO_FIXED(0.5f);

    SERVO_CONTROLLER_CFG(servoDev)->closed_loop_en = true;
    SERVO_CONTROLLER_CFG(servoDev)->pidArgument[0].E_lsb = 0.2;
    SERVO_CONTROLLER_CFG(servoDev)->pidArgument[0].U_lsb = 1;
    SERVO_CONTROLLER_CFG(servoDev)->pidArgument[0].dT = 1.0/500;
    SERVO_CONTROLLER_CFG(servoDev)->pidArgument[0].kp = 10;
    SERVO_CONTROLLER_CFG(servoDev)->pidArgument[0].ramp_rate = 1000;

    SERVO_CONTROLLER_CFG(servoDev)->n_motor_pole[0] = 5;
    SERVO_CONTROLLER_CFG(servoDev)->n_motor_ratio[0] = 1;
    SERVO_CONTROLLER_CFG(servoDev)->K_position_filter[0] = SERVO_CONTROLLER_FLOAT_TO_FIXED(0);

    stt = servo_controller_apply_configure(servoDev);
    ALT_DEBUG_ASSERT((stt == 0));



    servo_controller_start(servoDev);

    int16_t duty[SERVO_CONTROLLER_NUM_SERVO];
//
    duty[0] = SERVO_CONTROLLER_FLOAT_TO_FIXED(-0.4f);
//    duty[0] = 0;
//    duty[1] = 0;
//    duty[2] = 0;
//    duty[3] = 0;
//
    servo_controller_update_duty(servoDev, duty);
//    servo_controller_update_duty(servoDev, 0);

//    servo_controller_set_position(servoDev, SERVO_CONTROLLER_SERVO_ID_0, 1000);
//    servo_controller_set_position(servoDev, SERVO_CONTROLLER_SERVO_ID_0,
//    		-180/SERVO_CONTROLLER_CFG(servoDev)->pidArgument[0].E_lsb);


  while (1)
  {
	  OSTimeDly(100);

	  servo_controller_get_duty(servoDev, duty);
	  printf("pos = %ld\n", servoDev->data->filter_position[0] >> 15);
	  printf("duty = %d\n", duty[0]);
	  printf("realtime_err = %d\n", servoDev->BASE->flag.field.realtime_err);

//	  printf("pos_phase = %d\n", servoDev->BASE->pos_phase[0]);
//	  printf("ha=%d, hb=%d, hc=%d\n",
//			  servoDev->BASE->hall_sig.ha_0,
//			  servoDev->BASE->hall_sig.hb_0,
//			  servoDev->BASE->hall_sig.hc_0
//			  );


//	  if (servoDev->BASE->pos_phase[0] > 1000)
//	  {
//		  servo_controller_stop(servoDev);
//	  }
  }
}
/* Prints "Hello World" and sleeps for three seconds */
void task2(void* pdata)
{

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


//  OSTaskCreateExt(task2,
//                  NULL,
//                  &task2_stk[TASK_STACKSIZE-1],
//                  TASK2_PRIORITY,
//                  TASK2_PRIORITY,
//                  task2_stk,
//                  TASK_STACKSIZE,
//                  NULL,
//                  0);

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
