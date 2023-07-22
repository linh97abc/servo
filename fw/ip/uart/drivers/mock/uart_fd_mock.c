#include "avl_fifo_uart.h"
#include "avl_fifo_uart_fd.h"
#include <sys/alt_dev.h>

int __FifoUart_ReadFd(alt_fd *fd, char *ptr, int len)
{
    return 0;
}

int __FifoUart_WriteFd(alt_fd *fd, const char *ptr, int len)
{
    return 0;
}