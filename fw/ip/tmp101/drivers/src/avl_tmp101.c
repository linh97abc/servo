#include "avl_tmp101.h"
#include <io.h>

#include <priv/alt_file.h>
#include <sys/alt_dev.h>
#include <sys/alt_debug.h>

#define TMP101_IOWR(dev, addr, data) IOWR((uintptr_t)dev->BASE, addr, data)
#define TMP101_IORD(dev, addr) IORD((uintptr_t)dev->BASE, addr)


TMP101_Dev *TMP101_OpenDev(const char *name)
{
	extern alt_llist alt_dev_list;
	TMP101_Dev *dev = (TMP101_Dev *)alt_find_dev(name, &alt_dev_list);

	return dev;
}

int TMP101_Init(TMP101_Dev *dev)
{
	ALT_DEBUG_ASSERT((dev));
	ALT_DEBUG_ASSERT((dev->BASE));

	TMP101_IOWR(dev, TMP101_CR_REG, TMP101_CR_RESET);
	TMP101_IOWR(dev, TMP101_CR_REG, TMP101_CR_EN);


	alt_dev_reg(&dev->dev);

	return 0;
}

int16_t TMP101_GetRawTemperature(TMP101_Dev *dev)
{
	ALT_DEBUG_ASSERT((dev));
	avl_tmp101_TEMPER_REG temper;

	temper.data = TMP101_IORD(dev, TMP101_TEMPER_REG);
	return temper.field.temper;
}

float TMP101_GetTemperature(TMP101_Dev *dev)
{
	ALT_DEBUG_ASSERT((dev));
	avl_tmp101_TEMPER_REG temper;

	temper.data = TMP101_IORD(dev, TMP101_TEMPER_REG);

	return temper.data * TMP101_TEMPERATURE_LSB;
}

uint32_t TMP101_GetStatus(TMP101_Dev *dev)
{
	ALT_DEBUG_ASSERT((dev));

	return TMP101_IORD(dev, TMP101_FLAG_REG);
}
