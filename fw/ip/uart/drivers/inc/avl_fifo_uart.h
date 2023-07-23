#ifndef __FIFO_UART_H__
#define __FIFO_UART_H__

#include "ucos_ii.h"
#include <stdint.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/alt_dev.h>

#ifdef __cplusplus
extern "C"
{
#endif

	typedef union
	{
		struct
		{
			uint32_t en : 1;
			uint32_t reset : 1;
			uint32_t reserved : 6;
			uint32_t baudrate : 16;
			uint32_t reserved2 : 8;
			/* data */
		} field;
		uint32_t data;
	} avl_fifo_uart_CR_REG;

	typedef union
	{
		struct {
			uint32_t rx_idle : 1;
			uint32_t rx_full : 1;
			uint32_t rx_empty : 1;
			uint32_t tx_full : 1;
			uint32_t tx_empty : 1;
			uint32_t rx_threshold : 1;
			uint32_t rx_valid : 1;
			uint32_t tx_ready : 1;
			uint32_t overrun_err : 1;
			uint32_t parity_err : 1;
			uint32_t stop_err : 1;
			uint32_t reserved : 21;

		} field;
		uint32_t data;
	} avl_fifo_uart_IE_FLAG_REG;
	typedef struct tag_FifoUart_Reg
	{
		volatile avl_fifo_uart_CR_REG cr;
		volatile avl_fifo_uart_IE_FLAG_REG ie;
		volatile uint32_t ie_dis;
		volatile avl_fifo_uart_IE_FLAG_REG flag;
		const volatile uint32_t tx_cnt;
		const volatile uint32_t rx_cnt;
		volatile uint32_t rx_threshold;
		volatile uint32_t tx;
	} FifoUart_Reg;

	enum FifoUart_ParityBits_t
	{
		FIFO_UART_PARITY_BITS_NONE = 0,
		FIFO_UART_PARITY_BITS_EVEN = 2,
		FIFO_UART_PARITY_BITS_OLD = 3
	};

	enum FifoUart_StopBits_t
	{
		FIFO_UART_STOP_BITS_1 = 0,
		FIFO_UART_STOP_BITS_1_5 = 1,
		FIFO_UART_STOP_BITS_2 = 2
	};
	typedef struct tag_FifoUart_Dev
	{
		struct alt_dev_s dev;
		struct tag_FifoUart_Reg *const BASE;
		const uint8_t IRQ;
		const uint8_t IC_ID;
		const unsigned CORE_FREQ;
		const uint8_t DATA_BITS;
		const enum FifoUart_ParityBits_t PARITY_BITS;
		const enum FifoUart_StopBits_t STOP_BITS;
		const unsigned RX_FIFO_SIZE;
		const unsigned TX_FIFO_SIZE;

		uint16_t timeout;
		bool isReady;
		OS_FLAG_GRP *flag;

	} FifoUart_Dev;

	FifoUart_Dev *FifoUart_OpenDev(const char *name);
	int FifoUart_SetBaudrate(FifoUart_Dev *dev, unsigned baud);
	int FifoUart_GetBaudrate(FifoUart_Dev *dev);
	int FifoUart_SetTimeout(FifoUart_Dev *dev, unsigned timeout_tick);

	int FifoUart_ReadNonBlock(FifoUart_Dev *dev, void *buff, unsigned len);
	int FifoUart_Read(FifoUart_Dev *dev, void *buff, unsigned len);
	int FifoUart_Write(FifoUart_Dev *dev, const void *buff, unsigned len);

#ifdef __cplusplus
}
#endif

#include "LL/avl_fifo_uart_fd_ll.h"
#endif
