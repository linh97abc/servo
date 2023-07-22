#include "alt_irq.h"
#include <system_mock.h>

int alt_ic_isr_register(alt_u32 ic_id,
                        alt_u32 irq,
                        alt_isr_func isr,
                        void *isr_context,
                        void *flags)
{

    sys_mock::IrqManager *irqMang = sys_mock::IrqManager::GetInst();
    irqMang->Register(irq, isr, isr_context);
    return 0;
}