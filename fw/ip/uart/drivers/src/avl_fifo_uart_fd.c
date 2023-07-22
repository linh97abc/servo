#include "avl_fifo_uart.h"
#include "avl_fifo_uart_fd.h"
#include <sys/alt_dev.h>
#include <fcntl.h>

int __FifoUart_ReadFd(alt_fd *fd, char *ptr, int len)
{
	FifoUart_Dev* dev = (FifoUart_Dev*) fd->dev;

	if (fd->fd_flags & O_NONBLOCK) {
		return FifoUart_ReadNonBlock(dev, ptr, len);
	}

    return FifoUart_Read(dev, ptr, len);
}

int __FifoUart_WriteFd(alt_fd *fd, const char *ptr, int len)
{
	FifoUart_Dev* dev = (FifoUart_Dev*) fd->dev;
    return FifoUart_Write(dev, ptr, len);
}

