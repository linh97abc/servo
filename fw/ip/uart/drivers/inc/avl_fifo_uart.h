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
		uint32_t en : 1;
		uint32_t reset : 1;
		uint32_t reserved : 6;
		uint32_t baudrate : 16;
		uint32_t reserved2 : 8;
	} avl_fifo_uart_CR_REG;

	typedef union
	{
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

	typedef struct tag_FifoUart_Dev
	{
		struct alt_dev_s dev;
		struct tag_FifoUart_Reg *base;
		uint8_t irq;
		uint8_t ic_id;
		unsigned coreFreq;

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

#include "avl_fifo_uart_fd.h"
#endif
