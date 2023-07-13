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

	typedef struct tag_FifoUart_Reg
	{
		uint32_t CR;
		uint32_t IE;
		uint32_t IE_DIS;
		uint32_t FLAG;
		uint32_t TX_CNT;
		uint32_t RX_CNT;
		uint32_t RX_THRESHOLD;
		uint32_t TX;
	} FifoUart_Reg;

	typedef struct tag_FifoUart_Dev
	{
		struct alt_dev_s dev;
		volatile FifoUart_Reg *base;
		uint8_t irq;
		uint8_t ic_id;
		unsigned coreFreq;

		uint16_t timeout;
		bool isReady;
		OS_FLAG_GRP *flag;
		OS_EVENT *read_lock;
		OS_EVENT *write_lock;

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
