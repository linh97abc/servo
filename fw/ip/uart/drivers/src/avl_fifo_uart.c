#include "avl_fifo_uart.h"
#include "LL/avl_fifo_uart_reg.h"
#include <io.h>

#include <priv/alt_file.h>
#include <sys/alt_irq.h>
#include <sys/alt_dev.h>
#include <sys/alt_debug.h>

#define FIFO_UART_IOWR(dev, addr, data) IOWR((uintptr_t)dev->BASE, addr, data)
#define FIFO_UART_IORD(dev, addr) IORD((uintptr_t)dev->BASE, addr)

static void FifoUart_IrqHandler(void *arg)
{
	FifoUart_Dev *dev = (FifoUart_Dev *)arg;
	INT8U err;

	OS_FLAGS flags = FIFO_UART_IORD(dev, FIFO_UART_FLAG_REG);

	OSFlagPost(dev->flag, flags, OS_FLAG_SET, &err);
	ALT_DEBUG_ASSERT((err == OS_ERR_NONE));

	// clear error
	FIFO_UART_IOWR(dev, FIFO_UART_FLAG_REG, 0xFFFFFFFFu);

	// disable irq
	FIFO_UART_IOWR(dev, FIFO_UART_IE_DIS_REG,
				   FIFO_UART_IE_TX_READY |
					   FIFO_UART_IE_RX_VALID |
					   FIFO_UART_IE_RX_THRESHHOLD |
					   FIFO_UART_IE_TX_EMPTY |
					   FIFO_UART_IE_TX_FULL |
					   FIFO_UART_IE_RX_EMPTY |
					   FIFO_UART_IE_RX_FULL |
					   FIFO_UART_IE_RX_IDLE);
}

FifoUart_Dev *FifoUart_OpenDev(const char *name)
{
	extern alt_llist alt_dev_list;
	FifoUart_Dev *dev = (FifoUart_Dev *)alt_find_dev(name, &alt_dev_list);

	return dev;
}

int FifoUart_Init(FifoUart_Dev *dev)
{
	INT8U err;

	ALT_DEBUG_ASSERT((dev));
	ALT_DEBUG_ASSERT((dev->BASE));

	if (dev->isReady)
		return -EEXIST;

	dev->flag = OSFlagCreate(0, &err);

	ALT_DEBUG_ASSERT((err == OS_ERR_NONE));

	FIFO_UART_IOWR(dev, FIFO_UART_IE_DIS_REG, 0xFFFFFFFFu);
	FIFO_UART_IOWR(dev, FIFO_UART_FLAG_REG, 0xFFFFFFFFu);

	alt_ic_isr_register(dev->IC_ID, dev->IRQ, FifoUart_IrqHandler, dev, 0);

	uint32_t baud_regVal = FIFO_UART_IORD(dev, FIFO_UART_CR_REG) & ~FIFO_UART_CR_EN;

	FIFO_UART_IOWR(dev, FIFO_UART_CR_REG, baud_regVal | FIFO_UART_CR_RESET);
	FIFO_UART_IOWR(dev, FIFO_UART_CR_REG, baud_regVal | FIFO_UART_CR_EN);

	alt_dev_reg(&dev->dev);

	dev->isReady = true;

	return 0;
}

int FifoUart_GetBaudrate(FifoUart_Dev *dev)
{
	if (!dev)
		return -EINVAL;
	uint32_t cr = FIFO_UART_IORD(dev, FIFO_UART_CR_REG);

	uint32_t pres = FIFO_UART_CR_BAUD_GET(cr);

	return (int)dev->CORE_FREQ / (pres + 1);
}

int FifoUart_SetBaudrate(FifoUart_Dev *dev, unsigned baud)
{
	if (!dev)
		return -EINVAL;
	if (!baud)
		return -EINVAL;

	uint16_t baudrate = dev->CORE_FREQ / baud - 1;
	uint32_t baud_regVal = FIFO_UART_CR_BAUD_SET(baudrate);
	FIFO_UART_IOWR(dev, FIFO_UART_CR_REG, baud_regVal | FIFO_UART_CR_RESET);
	FIFO_UART_IOWR(dev, FIFO_UART_CR_REG, baud_regVal | FIFO_UART_CR_EN);

	return 0;
}

int FifoUart_SetTimeout(FifoUart_Dev *dev, unsigned timeout_tick)
{
	if (!dev)
	{
		return -EINVAL;
	}

	dev->timeout = timeout_tick;

	return 0;
}

int FifoUart_ReadNonBlock(FifoUart_Dev *dev, void *buff, unsigned len)
{
	uint8_t *pBuff = (uint8_t *)buff;

	if (!dev)
	{
		return -EINVAL;
	}

	if (!buff)
	{
		return -EINVAL;
	}

	if (!len)
	{
		return -EINVAL;
	}

	OSSchedLock();

	unsigned i = 0;

	for (; i < len; i++)
	{
		uint32_t data = FIFO_UART_IORD(dev, FIFO_UART_RX_REG);
		if (data & FIFO_UART_RX_REG_VALID)
		{
			pBuff[i] = data & 0xFFu;
		}
		else
		{
			break;
		}
	}

	OSSchedUnlock();

	return i;
}

int FifoUart_Read(FifoUart_Dev *dev, void *buff, unsigned len)
{
	INT8U err;
	uint32_t core_status;
	uint8_t *pBuff = (uint8_t *)buff;

	if (!dev)
	{
		return -EINVAL;
	}

	if (!buff)
	{
		return -EINVAL;
	}

	if (!len)
	{
		return -EINVAL;
	}

	core_status = FIFO_UART_IORD(dev, FIFO_UART_FLAG_REG);

	if (!(core_status & FIFO_UART_FLAG_RX_VALID))
	{
		OSFlagPost(dev->flag, FIFO_UART_FLAG_RX_VALID, OS_FLAG_CLR, &err);
		ALT_DEBUG_ASSERT((err == OS_ERR_NONE));

		FIFO_UART_IOWR(dev, FIFO_UART_IE_REG, FIFO_UART_FLAG_RX_VALID);
		OSFlagPend(dev->flag, FIFO_UART_FLAG_RX_VALID,
				   OS_FLAG_WAIT_SET_ANY,
				   dev->timeout, &err);

		if (err == OS_ERR_TIMEOUT)
		{
			return 0;
		}

		ALT_DEBUG_ASSERT((err == OS_ERR_NONE));
	}

	unsigned i = 0;

	OSSchedLock();

	for (; i < len; i++)
	{
		uint32_t data = FIFO_UART_IORD(dev, FIFO_UART_RX_REG);
		if (data & FIFO_UART_RX_REG_VALID)
		{
			pBuff[i] = data & 0xFFu;
		}
		else
		{
			break;
		}
	}

	OSSchedUnlock();

	return i;
}

int FifoUart_Write(FifoUart_Dev *dev, const void *buff, unsigned len)
{
	uint32_t core_status;
	uint8_t *pBuff = (uint8_t *)buff;
	unsigned i = 0;

	if (!dev)
	{
		return -EINVAL;
	}

	if (!buff)
	{
		return -EINVAL;
	}

	if (!len)
	{
		return -EINVAL;
	}

	OSSchedLock();

	for (; i < len; i++)
	{
		core_status = FIFO_UART_IORD(dev, FIFO_UART_FLAG_REG);
		if (core_status & FIFO_UART_FLAG_TX_READY)
		{
			FIFO_UART_IOWR(dev, FIFO_UART_TX_REG, pBuff[i]);
		}
		else
		{
			break;
		}
	}

	OSSchedUnlock();

	return i;
}
