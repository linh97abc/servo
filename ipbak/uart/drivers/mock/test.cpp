#include <iostream>
#include "avl_fifo_uart.h"
#include <ucos_ii.h>

#define UART_DEBUG_NAME "/dev/uart_debug"
FifoUart_Dev *uart_test_dev;

extern void sys_init();

#define TASK_STACKSIZE 2048
OS_STK task1_stk[TASK_STACKSIZE];

#define TASK1_PRIORITY 1

static OS_EVENT *pevent;

/* Prints "Hello World" and sleeps for three seconds */
void task1(void *pdata)
{
    uart_test_dev = FifoUart_OpenDev(UART_DEBUG_NAME);

    FifoUart_SetBaudrate(uart_test_dev, 115200);

    unsigned baud = FifoUart_GetBaudrate(uart_test_dev);

    std::cout << "Baudrate: " << baud << std::endl;

    uint8_t recv_data[10];

    while (1)
    {
        OSTimeDlyHMSM(0, 0, 1, 0);

        FifoUart_Write(uart_test_dev, "12345678", 8);
        std::cout << std::endl;

        FifoUart_Read(uart_test_dev, recv_data, 8);
        for (size_t i = 0; i < 8; i++)
        {
            std::cout << unsigned(recv_data[i]) << " ";
        }
        std::cout << std::endl;

        
    }
}

int main()
{
    std::cout << "Start Test UART" << std::endl;
    OSInit();

    sys_init();

    OSTaskCreateExt(task1,
                    NULL,
                    &task1_stk[TASK_STACKSIZE - 1],
                    TASK1_PRIORITY,
                    TASK1_PRIORITY,
                    task1_stk,
                    TASK_STACKSIZE,
                    NULL,
                    0);

    OSStart();

    return 0;
}