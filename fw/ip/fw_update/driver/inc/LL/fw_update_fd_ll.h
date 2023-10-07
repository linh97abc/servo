#ifndef __FW_UPDATE_FD_LL_H__
#define __FW_UPDATE_FD_LL_H__

#include <sys/alt_dev.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // call by sys_init
    int fw_update_init(fw_update_dev *dev);

#ifdef __cplusplus
}
#endif

#ifndef NIOS2_DCACHE_BYPASS_MASK
#define NIOS2_DCACHE_BYPASS_MASK 0
#endif

#define AVL_TMP101_INSTANCE(name, dev)                                 \
    static TMP101_Dev dev =                                            \
        {                                                              \
            {                                                          \
                ALT_LLIST_ENTRY,                                       \
                name##_NAME,                                           \
                NULL, /* open */                                       \
                NULL, /* close */                                      \
                NULL, /* read */                                       \
                NULL, /* write */                                      \
                NULL, /* lseek */                                      \
                NULL, /* fstat */                                      \
                NULL, /* ioctl */                                      \
            },                                                         \
            (fw_update_Reg *)(NIOS2_DCACHE_BYPASS_MASK | name##_BASE), \
            name##_PRODUCT_ID,                                         \
            name##_VERSION_MAJOR,                                      \
            name##_VERSION_MINOR,                                      \
    }

#define AVL_TMP101_INIT(name, dev) fw_update_init(&dev)

#endif // __AVL_TMP101_FD_LL_H__