#include "avl_ltc2992.h"
#include <io.h>

#include <priv/alt_file.h>
#include <sys/alt_irq.h>
#include <sys/alt_dev.h>
#include <sys/alt_debug.h>

#define LTC2992_IOWR(dev, addr, data) IOWR((uintptr_t)dev->BASE, addr, data)
#define LTC2992_IORD(dev, addr) IORD((uintptr_t)dev->BASE, addr)

static void LTC2992_IrqHandler(void *arg) __attribute__((section(".exceptions")));

static void LTC2992_IrqHandler(void *arg)
{
	LTC2992_Dev *dev = (LTC2992_Dev *)arg;
	INT8U err;

	OS_FLAGS flags = LTC2992_IORD(dev, LTC2992_FLAG_REG);

	OSFlagPost(dev->flag, flags, OS_FLAG_SET, &err);
	ALT_DEBUG_ASSERT((err == OS_ERR_NONE));

	// clear flag
	LTC2992_IOWR(dev, LTC2992_FLAG_REG, 0xFFFFFFFFu);
}

LTC2992_Dev *LTC2992_OpenDev(const char *name)
{
	extern alt_llist alt_dev_list;
	LTC2992_Dev *dev = (LTC2992_Dev *)alt_find_dev(name, &alt_dev_list);

	return dev;
}

int LTC2992_Init(LTC2992_Dev *dev)
{
	ALT_DEBUG_ASSERT((dev));
	ALT_DEBUG_ASSERT((dev->BASE));
	INT8U err;

	dev->flag = OSFlagCreate(0, &err);
	ALT_DEBUG_ASSERT((err == OS_ERR_NONE));

	LTC2992_IOWR(dev, LTC2992_CR_REG, LTC2992_CR_RESET);
	LTC2992_IOWR(dev, LTC2992_CR_REG, LTC2992_CR_EN);

	alt_ic_isr_register(dev->IC_ID, dev->IRQ, LTC2992_IrqHandler, dev, 0);

	LTC2992_IOWR(dev, LTC2992_FLAG_REG, LTC2992_FLAG_DONE);
	LTC2992_IOWR(dev, LTC2992_IE_REG, 1);

	alt_dev_reg(&dev->dev);

	return 0;
}

uint32_t LTC2992_GetStatus(LTC2992_Dev *dev)
{
	ALT_DEBUG_ASSERT((dev));

	return LTC2992_IORD(dev, LTC2992_FLAG_REG);
}
