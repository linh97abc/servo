#include "io.h"
#include "system_mock.h"

uint32_t IORD(uintptr_t base, unsigned reg)
{
    sys_mock::MemoryMap *map = reinterpret_cast<sys_mock::MemoryMap *>(base);

    return map->iord(reg);
}

void IOWR(uintptr_t base, unsigned reg, uint32_t data)
{
    sys_mock::MemoryMap *map = reinterpret_cast<sys_mock::MemoryMap *>(base);
    map->iowr(reg, data);
}
