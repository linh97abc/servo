#ifndef __TMP101_H__
#define __TMP101_H__

#include <stdint.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/alt_dev.h>

#define TMP101_CR_REG 0
#define TMP101_FLAG_REG 1
#define TMP101_TEMPER_REG 2

#define TMP101_CR_EN (1u << 0)
#define TMP101_CR_RESET (1u << 1)

#define TMP101_FLAG_DATA_VALID (1u << 0)
#define TMP101_FLAG_I2C_NACK (1u << 1)
#define TMP101_FLAG_I2C_BUSY (1u << 2)

#ifdef __cplusplus
extern "C"
{
#endif

	typedef union
	{
		struct
		{
			uint32_t en : 1;
			uint32_t reset : 1;
			/* data */
		} field;
		uint32_t data;
	} avl_tmp101_CR_REG;

	typedef union
	{
		struct
		{
			uint32_t data_valid : 1;
			uint32_t i2c_nack : 1;
			uint32_t i2c_busy : 1;

		} field;
		uint32_t data;
	} avl_tmp101_IE_FLAG_REG;

	typedef union
	{
		struct
		{
			int16_t temper;
		} field;

		uint32_t data;
	} avl_tmp101_TEMPER_REG;

	typedef struct tag_Tmp101_Reg
	{
		volatile avl_tmp101_CR_REG cr;
		volatile avl_tmp101_IE_FLAG_REG flag;
		volatile avl_tmp101_TEMPER_REG temper;
	} TMP101_Reg;

	/// @brief TMP101 Device Type
	typedef struct tag_TMP101_Dev
	{
		struct alt_dev_s dev;

		/// @brief TMP101 Base Address
		struct tag_Tmp101_Reg *const BASE;

		/// @brief Core frequency
		const unsigned CORE_FREQ;

		const unsigned BUS_CLK;
		const unsigned UPDATE_FREQ;
		const unsigned I2C_ADDR;

	} TMP101_Dev;

	TMP101_Dev *TMP101_OpenDev(const char *name);

	int16_t TMP101_GetTemperature(TMP101_Dev *dev);

	uint32_t TMP101_GetStatus(TMP101_Dev *dev);

#ifdef __cplusplus
}
#endif

#include "LL/avl_tmp101_fd_ll.h"
#endif
