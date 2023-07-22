#ifndef __ALT_IRQ_H__
#define __ALT_IRQ_H__

#include <alt_types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*alt_isr_func)(void* isr_context);

int alt_ic_isr_register(alt_u32 ic_id,
                        alt_u32 irq,
                        alt_isr_func isr,
                        void *isr_context,
                        void *flags);

#ifdef __cplusplus
}
#endif
#endif // __ALT_IRQ_H__