#include <stdio.h>
#include "includes.h"

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    task2_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */

#define TASK1_PRIORITY      1
#define TASK2_PRIORITY      2

#include "avl_fifo_uart.h"


/* Prints "Hello World" and sleeps for three seconds */
void task1(void* pdata)
{
	FifoUart_Dev *fifo_uart_0 = FifoUart_OpenDev(UART_RS485_NAME);

	FifoUart_SetBaudrate(fifo_uart_0, 921600);

	char buff[10];
	int len;

  while (1)
  { 
    printf("Hello from task1\n");
    OSTimeDlyHMSM(0, 0, 3, 0);

    len = FifoUart_Read(fifo_uart_0, buff, 10);
    FifoUart_Write(fifo_uart_0, buff, len);
  }
}
/* Prints "Hello World" and sleeps for three seconds */
void task2(void* pdata)
{
  while (1)
  { 
    printf("Hello from task2\n");
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
  OSStart();
  return 0;
}

