#ifndef __IO_H__
#define __IO_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    uint32_t IORD(uintptr_t base, unsigned reg);
    void IOWR(uintptr_t base, unsigned reg, uint32_t data);

#ifdef __cplusplus
}
#endif
#endif // __IO_H__