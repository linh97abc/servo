#ifndef __AVL_FIFO_UART_FD_H__
#define __AVL_FIFO_UART_FD_H__

#include <sys/alt_dev.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // call by sys_init
    int FifoUart_Init(FifoUart_Dev *dev);

    int __FifoUart_ReadFd(alt_fd *fd, char *ptr, int len);
    int __FifoUart_WriteFd(alt_fd *fd, const char *ptr, int len);

#ifndef NIOS2_DCACHE_BYPASS_MASK
#define NIOS2_DCACHE_BYPASS_MASK 0
#endif

#define AVL_FIFO_UART_INSTANCE(name, dev)                             \
    static FifoUart_Dev dev =                                         \
        {                                                             \
            {                                                         \
                ALT_LLIST_ENTRY,                                      \
                name##_NAME,                                          \
                NULL,               /* open */                        \
                NULL,               /* close */                       \
                __FifoUart_ReadFd,  /* read */                        \
                __FifoUart_WriteFd, /* write */                       \
                NULL,               /* lseek */                       \
                NULL,               /* fstat */                       \
                NULL,               /* ioctl */                       \
            },                                                        \
            (FifoUart_Reg *)(NIOS2_DCACHE_BYPASS_MASK | name##_BASE), \
            name##_IRQ,                                               \
            name##_IRQ_INTERRUPT_CONTROLLER_ID,                       \
            name##_FREQ,                                              \
            name##_DATA_BIT,                                          \
            (enum FifoUart_ParityBits_t)name##_PARITY_BIT,            \
            (enum FifoUart_StopBits_t)name##_STOP_BIT,                \
            1u << name##_RX_FIFO_DEPTH,                               \
            1u << name##_TX_FIFO_DEPTH,                               \
    }

#define AVL_FIFO_UART_INIT(name, dev) FifoUart_Init(&dev)
#ifdef __cplusplus
}
#endif
#endif
