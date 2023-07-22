#ifndef __SYSTEM_MOCK_H__
#define __SYSTEM_MOCK_H__
#include <stdint.h>

namespace sys_mock
{
    class MemoryMap
    {
        const char *name;

    public:
        MemoryMap(const char *name)
            : name(name)
        {
        }

        virtual ~MemoryMap() = default;

        virtual uint32_t iord(unsigned reg) = 0;
        virtual void iowr(unsigned reg, uint32_t data) = 0;
    };

    class IrqManager
    {
        typedef void (*IrqHandler_t)(void *arg);

        IrqHandler_t handler[32]{0};
        void *arg[32];

        IrqManager() = default;

    public:
        static IrqManager *GetInst();

        void Invoke(unsigned id);

        void Register(unsigned id, IrqHandler_t handler, void *arg);
    };
} // namespace sys_mock

#endif // __SYSTEM_MOCK_H__