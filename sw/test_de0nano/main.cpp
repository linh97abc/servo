#include <stdio.h>
#include <ucos_ii.h>
#include <system.h>
#include <avl_fifo_uart.h>

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    task2_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */

#define TASK1_PRIORITY      1
#define TASK2_PRIORITY      2

FifoUart_Dev *fifoUartDev;
/* Prints "Hello World" and sleeps for three seconds */
void task1(void* pdata)
{
    printf("Hello from task1\n");

    fifoUartDev = FifoUart_OpenDev(AVL_FIFO_UART_0_NAME);
//    FifoUart_SetBaudrate(fifoUartDev, 921600);
//    FifoUart_SetRxThreshold(fifoUartDev, 32);


    char buff[64];

  while (1)
  {
	  int len = FifoUart_Read(fifoUartDev, buff, sizeof(buff) - 1);
	  buff[len] = '\0';

	  puts(buff);

  }
}
/* Prints "Hello World" and sleeps for three seconds */
void task2(void* pdata)
{
    printf("Hello from task2\n");

    char buff[64];
    int seq = 0;

  while (1)
  {
	  int len = sprintf(buff, "seq: %d: UART Send", seq++);
	  FifoUart_Write(fifoUartDev, buff, len);

    OSTimeDlyHMSM(0, 0, 1, 0);
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
