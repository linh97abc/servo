#include "avl_fifo_uart.h"
#include "avl_fifo_uart_fd.h"
#include <sys/alt_dev.h>
#include <fcntl.h>
#include <errno.h>

int __FifoUart_ReadFd(alt_fd *fd, char *ptr, int len)
{
	FifoUart_Dev *dev = (FifoUart_Dev *)fd->dev;

	if (fd->fd_flags & O_NONBLOCK)
	{
		return FifoUart_ReadNonBlock(dev, ptr, len);
	}

	return FifoUart_Read(dev, ptr, len);
}

int __FifoUart_WriteFd(alt_fd *fd, const char *ptr, int len)
{
	FifoUart_Dev *dev = (FifoUart_Dev *)fd->dev;
	return FifoUart_Write(dev, ptr, len);
}

int __FifoUart_IoctlFd(alt_fd *fd, int req, void *arg)
{
	FifoUart_Dev *dev = (FifoUart_Dev *)fd->dev;
	int stt;

	switch (req)
	{
	case FIFO_UART_IOCTL_GET_BAUDRATE:
	{
		int baud = FifoUart_GetBaudrate(dev);
		if (baud >= 0)
		{
			*((unsigned *)arg) = baud;
		}
		else
		{
			return baud;
		}

		return 0;
	}
	case FIFO_UART_IOCTL_SET_BAUDRATE:
	{
		stt = FifoUart_SetBaudrate(dev, *((unsigned *)arg));
		return stt;
	}
	case FIFO_UART_IOCTL_SET_TIMEOUT:
	{
		stt = FifoUart_SetTimeout(dev, *((unsigned *)arg));
		return stt;
	}

	case FIFO_UART_IOCTL_SET_RX_THRESHOLD:
	{
		stt = FifoUart_SetRxThreshold(dev, *((unsigned *)arg));
		return stt;
	}
	default:
		break;
	}

	return -EINVAL;
}