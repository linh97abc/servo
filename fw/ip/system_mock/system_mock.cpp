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

sys_mock::IrqManager *sys_mock::IrqManager::GetInst()
{
    static IrqManager inst;

    return &inst;
}

void sys_mock::IrqManager::Invoke(unsigned id)
{
    if (this->handler[id])
    {
        /* code */
        this->handler[id](this->arg[id]);
    }
}

void sys_mock::IrqManager::Register(unsigned id, IrqHandler_t handler, void *arg)
{
    this->handler[id] = handler;
    this->arg[id] = arg;
}
