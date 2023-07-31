#ifndef __TMP101_FD_LL_H__
#define __TMP101_FD_LL_H__

#include <sys/alt_dev.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // call by sys_init
    int TMP101_Init(TMP101_Dev *dev);


#ifdef __cplusplus
}
#endif

#ifndef NIOS2_DCACHE_BYPASS_MASK
#define NIOS2_DCACHE_BYPASS_MASK 0
#endif

#define AVL_TMP101_INSTANCE(name, dev)                             \
    static TMP101_Dev dev =                                         \
        {                                                             \
            {                                                         \
                ALT_LLIST_ENTRY,                                      \
                name##_NAME,                                          \
                NULL,               /* open */                        \
                NULL,               /* close */                       \
                NULL,  /* read */                        \
                NULL, /* write */                       \
                NULL,               /* lseek */                       \
                NULL,               /* fstat */                       \
                NULL, /* ioctl */                       \
            },                                                        \
            (FifoUart_Reg *)(NIOS2_DCACHE_BYPASS_MASK | name##_BASE), \
            name##_FREQ,                                              \
            name##_BUS_CLK,                                          \
            name##_UPDATE_FREQ,            \
            name##_I2C_ADDR,                \
    }

#define AVL_TMP101_INIT(name, dev) FifoUart_Init(&dev)

#endif // __AVL_TMP101_FD_LL_H__