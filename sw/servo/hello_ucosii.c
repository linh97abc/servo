#include <stdio.h>
#include "includes.h"

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    task2_stk[TASK_STACKSIZE];
OS_STK    task3_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */

#define TASK1_PRIORITY      1
#define TASK2_PRIORITY      2
#define TASK3_PRIORITY      3

#include "avl_fifo_uart.h"
#include <avl_drv8320.h>


FifoUart_Dev *fifo_uart_0;
struct Drv8320_dev_t *drv8320_dev;

/* Prints "Hello World" and sleeps for three seconds */
void task1(void* pdata)
{
	fifo_uart_0 = FifoUart_OpenDev(UART_RS485_NAME);

	FifoUart_SetBaudrate(fifo_uart_0, 921600);

	char buff[10];
	int len;

  while (1)
  { 
    printf("Hello from task1\n");

    len = FifoUart_Read(fifo_uart_0, buff, 10);
    FifoUart_Write(fifo_uart_0, buff, len);
  }
}
/* Prints "Hello World" and sleeps for three seconds */
void task2(void* pdata)
{
	char buff[] = "Hello from task2\n";

  while (1)
  { 
	  FifoUart_Write(fifo_uart_0, buff, sizeof(buff));
    OSTimeDlyHMSM(0, 0, 3, 0);
  }
}


void task3(void* pdata)
{
	drv8320_dev = Drv8320_open_dev(AVL_DRV8320_0_NAME);

	DRV8320_CFG(drv8320_dev)->pwm_base_freq = 10000000;
	DRV8320_CFG(drv8320_dev)->pwm_freq = 1000;
	Drv8320_apply_configure(drv8320_dev);

	int16_t duty[DRV8320_NUM_CHANNEL] = {0};

	Drv8320_start(drv8320_dev);

	// set duty cycle
	duty[0] = DRV8320_FLOAT_TO_FIXED(0.2);
	duty[1] = DRV8320_FLOAT_TO_FIXED(-0.2);
	Drv8320_update_duty(drv8320_dev, duty);

  while (1)
  {
    OSTimeDlyHMSM(0, 0, 3, 0);
  }
}

/* The main function creates two task and starts multi-tasking */
int main(void)
{
  printf("MicroC/OS-II\n");


  OSTaskCreateExt(task1,
                  NULL,
                  (void *)&task1_stk[TASK_STACKSIZE-1],
                  TASK1_PRIORITY,
                  TASK1_PRIORITY,
                  task1_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);
              
               
  OSTaskCreateExt(task2,
                  NULL,
                  (void *)&task2_stk[TASK_STACKSIZE-1],
                  TASK2_PRIORITY,
                  TASK2_PRIORITY,
                  task2_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  OSTaskCreateExt(task3,
                  NULL,
                  (void *)&task3_stk[TASK_STACKSIZE-1],
                  TASK3_PRIORITY,
                  TASK3_PRIORITY,
                  task3_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  OSStart();
  return 0;
}

