#ifndef __SERVO_CONTROLLERV1_H__
#define __SERVO_CONTROLLERV1_H__

#include "servo_controller.h"

#ifndef NIOS2_DCACHE_BYPASS_MASK
#define NIOS2_DCACHE_BYPASS_MASK 0
#endif

#define SERVO_CONTROLLERV1_INIT(name, dev) servo_controller_init(&dev)

#define SERVO_CONTROLLERV1_INSTANCE(name, dev)                                         \
    static struct servo_controller_config_t dev##_cfg;                                 \
    static struct servo_controller_data_t dev##data;                                   \
    static struct servo_controller_dev_t dev =                                         \
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
            (struct servo_controller_reg_t *)(NIOS2_DCACHE_BYPASS_MASK | name##_BASE), \
            name##_IRQ,                                                                \
            name##_IRQ_INTERRUPT_CONTROLLER_ID,                                        \
            name##_FREQ,                                                               \
            &dev##_cfg,                                                                \
            &dev##data,                                                                \
    }

void servo_controller_init(struct servo_controller_dev_t *dev);

#endif // __SERVO_CONTROLLERV1_H__