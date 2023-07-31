#ifndef __AVL_FIFO_UART_REG_H__
#define __AVL_FIFO_UART_REG_H__

#define FIFO_UART_CR_REG 0
#define FIFO_UART_IE_REG 1
#define FIFO_UART_IE_DIS_REG 2
#define FIFO_UART_FLAG_REG 3

#define FIFO_UART_TX_COUNT_REG 4
#define FIFO_UART_RX_COUNT_REG 5

#define FIFO_UART_RX_THRESHOLD_REG 6
#define FIFO_UART_TX_REG 7
#define FIFO_UART_RX_REG 10

#define FIFO_UART_CR_EN (1u << 0)
#define FIFO_UART_CR_RESET (1u << 1)
#define FIFO_UART_CR_BAUD_SET(baud) (((baud) &0xFFFFu) << 8)
#define FIFO_UART_CR_BAUD_GET(regVal) (((regVal) >> 8) & 0xFFFFu)

#define FIFO_UART_IE_START_ERR (1u << 11)
#define FIFO_UART_IE_STOP_ERR (1u << 10)
#define FIFO_UART_IE_PARITY_ERR (1u << 9)
#define FIFO_UART_IE_OVERRUN_ERR (1u << 8)
#define FIFO_UART_IE_TX_READY (1u << 7)
#define FIFO_UART_IE_RX_VALID (1u << 6)
#define FIFO_UART_IE_RX_THRESHHOLD (1u << 5)
#define FIFO_UART_IE_TX_EMPTY (1u << 4)
#define FIFO_UART_IE_TX_FULL (1u << 3)
#define FIFO_UART_IE_RX_EMPTY (1u << 2)
#define FIFO_UART_IE_RX_FULL (1u << 1)
#define FIFO_UART_IE_RX_IDLE (1u << 0)

#define FIFO_UART_FLAG_START_ERR (1u << 11)
#define FIFO_UART_FLAG_STOP_ERR (1u << 10)
#define FIFO_UART_FLAG_PARITY_ERR (1u << 9)
#define FIFO_UART_FLAG_OVERRUN_ERR (1u << 8)
#define FIFO_UART_FLAG_TX_READY (1u << 7)
#define FIFO_UART_FLAG_RX_VALID (1u << 6)
#define FIFO_UART_FLAG_RX_THRESHHOLD (1u << 5)
#define FIFO_UART_FLAG_TX_EMPTY (1u << 4)
#define FIFO_UART_FLAG_TX_FULL (1u << 3)
#define FIFO_UART_FLAG_RX_EMPTY (1u << 2)
#define FIFO_UART_FLAG_RX_FULL (1u << 1)
#define FIFO_UART_FLAG_RX_IDLE (1u << 0)

#define FIFO_UART_RX_REG_VALID (1u << 31)
#define FIFO_UART_RX_REG_EMPTY (1u << 30)

#endif
