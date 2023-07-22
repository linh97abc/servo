#include "avl_fifo_uart.h"
#include "fifo_uart_mock.h"
#include <stdint.h>


#define ALT_MODULE_CLASS_uart_debug avl_fifo_uart
#define UART_DEBUG_BASE ((uintptr_t)&fifo_Uart_Debug_inst)
#define UART_DEBUG_DATA_BIT 8
#define UART_DEBUG_FREQ 100000000
#define UART_DEBUG_IRQ 2
#define UART_DEBUG_IRQ_INTERRUPT_CONTROLLER_ID 0
#define UART_DEBUG_NAME "/dev/uart_debug"
#define UART_DEBUG_PARITY_BIT 0
#define UART_DEBUG_RX_FIFO_DEPTH 8
#define UART_DEBUG_SPAN 64
#define UART_DEBUG_STOP_BIT 0
#define UART_DEBUG_TX_FIFO_DEPTH 12
#define UART_DEBUG_TYPE "avl_fifo_uart"

static sys_mock::FifoUart_MemMap fifo_Uart_Debug_inst(UART_DEBUG_NAME);

AVL_FIFO_UART_INSTANCE ( UART_DEBUG, uart_test);

void sys_init()
{
    AVL_FIFO_UART_INIT ( UART_DEBUG, uart_test);
}