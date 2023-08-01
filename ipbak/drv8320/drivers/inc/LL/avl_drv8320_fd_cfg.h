#ifndef __DRV8320V1_H__
#define __DRV8320V1_H__



#ifndef NIOS2_DCACHE_BYPASS_MASK
#define NIOS2_DCACHE_BYPASS_MASK 0
#endif

#define AVL_DRV8320_INIT(name, dev) Drv8320_init(&dev)

#define AVL_DRV8320_INSTANCE(name, dev)                                         \
    static struct Drv8320_config_t dev##_cfg;                                 \
    static struct Drv8320_dev_t dev =                                         \
        {                                                                              \
            {                                                                          \
                ALT_LLIST_ENTRY,                                                       \
                name##_NAME,                                                           \
                NULL, /* open */                                                       \
                NULL, /* close */                                                      \
                NULL, /* read */                                                       \
                NULL, /* write */                                                      \
                NULL, /* lseek */                                                      \
                NULL, /* fstat */                                                      \
                NULL, /* ioctl */                                                      \
            },                                                                         \
            (struct Drv8320_reg_t *)(NIOS2_DCACHE_BYPASS_MASK | name##_BASE), \
            name##_IRQ,                                                                \
            name##_IRQ_INTERRUPT_CONTROLLER_ID,                                        \
            name##_FREQ,                                                               \
            &dev##_cfg}

void Drv8320_init(struct Drv8320_dev_t *dev);

#endif // __DRV8320V1_H__