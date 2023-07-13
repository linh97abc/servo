#include "avl_fifo_uart.h"
#include <io.h>
#include <sys/alt_irq.h>
#include <sys/alt_dev.h>
#include <priv/alt_file.h>
#include "avl_fifo_uart_reg.h"

#define _REG_BASE ((struct tag_FifoUart_Reg *)0)

//#define FIFO_UART_IOWR(dev, addr, data)  IOWR((uintptr_t)dev->base, ((uintptr_t)(addr)) >> 2, data)
//#define FIFO_UART_IORD(dev, addr)  IORD((uintptr_t)dev->base, ((uintptr_t)(addr)) >> 2)

static inline void FIFO_UART_IOWR(FifoUart_Dev *dev, uint32_t *addr, uint32_t data)
{
	IOWR((uintptr_t)dev->base, ((uintptr_t)addr) >> 2, data);
}

static inline uint32_t FIFO_UART_IORD(FifoUart_Dev *dev, uint32_t *addr)
{
	return IORD((uintptr_t)dev->base, ((uintptr_t)addr) >> 2);
}

static void FifoUart_IrqHandler(void *arg)
{
	FifoUart_Dev *dev = (FifoUart_Dev *)arg;
	INT8U err;

	OS_FLAGS flags = FIFO_UART_IORD(dev, &_REG_BASE->FLAG);

	OSFlagPost(dev->flag, flags, OS_FLAG_SET, &err);
}

FifoUart_Dev* FifoUart_OpenDev(const char *name)
{
	extern alt_llist alt_dev_list;
	FifoUart_Dev *dev = (FifoUart_Dev *) alt_find_dev(name, &alt_dev_list);

	return dev;
}

int FifoUart_Init(FifoUart_Dev *dev)
{
	INT8U err;

	if(!dev) return -EINVAL;
	if (!dev->base) return -EINVAL;
	if(dev->isReady) return -EEXIST;

	dev->flag = OSFlagCreate(0, &err);

	if (err != OS_ERR_NONE) {
		return -ECANCELED;
	}

	dev->read_lock = OSSemCreate(1);
	dev->write_lock = OSSemCreate(1);

	FIFO_UART_IOWR(dev, &_REG_BASE->IE_DIS, 0xFFFFFFFFu);

	alt_ic_isr_register(dev->ic_id, dev->irq, FifoUart_IrqHandler, dev, 0);

    uint32_t baud_regVal = FIFO_UART_IORD(dev, &_REG_BASE->CR) & ~FIFO_UART_CR_EN;

	FIFO_UART_IOWR(dev, &_REG_BASE->CR, baud_regVal | FIFO_UART_CR_RESET);
	FIFO_UART_IOWR(dev, &_REG_BASE->CR, baud_regVal | FIFO_UART_CR_EN);

	alt_dev_reg(&dev->dev);


	dev->isReady = true;

	return 0;
}

int FifoUart_GetBaudrate(FifoUart_Dev *dev)
{
	if(!dev) return -EINVAL;
	uint32_t cr = FIFO_UART_IORD(dev, &_REG_BASE->CR);

	uint32_t pres = FIFO_UART_CR_BAUD_GET(cr);

	return (int) dev->coreFreq/ (pres + 1);
}

int FifoUart_SetBaudrate(FifoUart_Dev *dev, unsigned baud)
{
	if(!dev) return -EINVAL;
	if (!baud) return -EINVAL;

	uint16_t baudrate = dev->coreFreq/baud - 1;
	uint32_t baud_regVal = FIFO_UART_CR_BAUD_SET(baudrate);
	FIFO_UART_IOWR(dev, &_REG_BASE->CR, baud_regVal | FIFO_UART_CR_RESET);
	FIFO_UART_IOWR(dev, &_REG_BASE->CR, baud_regVal | FIFO_UART_CR_EN);

	return 0;
}

int FifoUart_SetTimeout(FifoUart_Dev *dev, unsigned timeout_tick)
{
	if(!dev) {
		return -EINVAL;
	}

	dev->timeout = timeout_tick;

	return 0;
}

int FifoUart_ReadNonBlock(FifoUart_Dev *dev, void* buff, unsigned len)
{
	INT8U err;
//	uint32_t core_status;
	uint8_t *pBuff = (uint8_t *)buff;

	if(!dev) {
		return -EINVAL;
	}

	if(!buff) {
		return -EINVAL;
	}

	if(!len) {
		return -EINVAL;
	}


	OSSemPend(dev->read_lock, 1, &err);

	if (err == OS_ERR_TIMEOUT) {
		return 0;
	}

	if (err != OS_ERR_NONE) {
		return -EBUSY;
	}

	unsigned i;
	unsigned recv_byte = 0;

	for(i = 0; i < len; i++) {
		uint32_t data = IORD( (uintptr_t) (dev->base), FIFO_UART_RX_REG);
		if (data & FIFO_UART_RX_REG_VALID) {
			pBuff[i] = data & 0xFFu;
			recv_byte++;
		} else {
			break;
		}
	}

	err = OSSemPost(dev->read_lock);

	if (err != OS_ERR_NONE) {
		return -EAGAIN;
	}

	return recv_byte;
}

int FifoUart_Read(FifoUart_Dev *dev, void* buff, unsigned len)
{
	INT8U err;
	uint32_t core_status;
	uint8_t *pBuff = (uint8_t *)buff;

	if(!dev) {
		return -EINVAL;
	}

	if(!buff) {
		return -EINVAL;
	}

	if(!len) {
		return -EINVAL;
	}


	OSSemPend(dev->read_lock, 0, &err);

	if (err != OS_ERR_NONE) {
		return -EBUSY;
	}

	core_status = FIFO_UART_IORD(dev, &_REG_BASE->FLAG);

	if (!(core_status & FIFO_UART_FLAG_RX_VALID)) {
		OSFlagPost(dev->flag, FIFO_UART_FLAG_RX_VALID, OS_FLAG_CLR, &err);
		if (err != OS_ERR_NONE) {
			OSSemPost(dev->read_lock);
			return -EAGAIN;
		}


		FIFO_UART_IOWR(dev, &_REG_BASE->IE, FIFO_UART_FLAG_RX_VALID);
		OSFlagPend(dev->flag, FIFO_UART_FLAG_RX_VALID, OS_FLAG_WAIT_SET_ANY, dev->timeout, &err);

		if (err == OS_ERR_TIMEOUT) {
			OSSemPost(dev->read_lock);
			return 0;
		} else if (err != OS_ERR_NONE) {
			OSSemPost(dev->read_lock);
			return -EAGAIN;
		}

	}

	unsigned i;
	unsigned recv_byte = 0;

	for(i = 0; i < len; i++) {
		uint32_t data = IORD( (uintptr_t) (dev->base), FIFO_UART_RX_REG);
		if (data & FIFO_UART_RX_REG_VALID) {
			pBuff[i] = data & 0xFFu;
			recv_byte++;
		} else {
			break;
		}
	}

	err = OSSemPost(dev->read_lock);

	if (err != OS_ERR_NONE) {
		return -EAGAIN;
	}

	return recv_byte;

}

int FifoUart_Write(FifoUart_Dev *dev, const void* buff, unsigned len)
{
	INT8U err;
	uint32_t core_status;
	uint8_t *pBuff = (uint8_t *)buff;
	unsigned i;
	unsigned send_byte = 0;

	if(!dev) {
		return -EINVAL;
	}

	if(!buff) {
		return -EINVAL;
	}

	if(!len) {
		return -EINVAL;
	}

	OSSemPend(dev->write_lock, 0, &err);

	if (err != OS_ERR_NONE) {
		return -EBUSY;
	}

	for(i = 0; i < len; i++) {
		core_status = FIFO_UART_IORD(dev, &_REG_BASE->FLAG);
		if (core_status & FIFO_UART_FLAG_TX_READY) {
			FIFO_UART_IOWR(dev, &_REG_BASE->TX, pBuff[i]);
			send_byte++;
		} else {
			break;
		}
	}

	err = OSSemPost(dev->write_lock);

	if (err != OS_ERR_NONE) {
		return -EAGAIN;
	}

	return send_byte;
}
