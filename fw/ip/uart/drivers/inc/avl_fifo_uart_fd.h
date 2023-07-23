#ifndef __AVL_FIFO_UART_FD_H__
#define __AVL_FIFO_UART_FD_H__


#ifdef __cplusplus
extern "C"
{
#endif


enum FifoUart_ioctl_option_t
{
    FIFO_UART_IOCTL_GET_BAUDRATE,
    FIFO_UART_IOCTL_SET_BAUDRATE,
    FIFO_UART_IOCTL_SET_TIMEOUT,
};

#ifdef __cplusplus
}
#endif

#endif
