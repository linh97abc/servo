#ifndef __AVL_LTC2992_H__
#define __AVL_LTC2992_H__

#include <stdint.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/alt_dev.h>
#include <ucos_ii.h>

#define LTC2992_CR_REG 0
#define LTC2992_IE_REG 1
#define LTC2992_FLAG_REG 2
#define LTC2992_TX_REG 3
#define LTC2992_RX_REG 4

#define LTC2992_CR_RW (1u << 24)
#define LTC2992_CR_EN (1u << 29)
#define LTC2992_CR_TRANS (1u << 30)
#define LTC2992_CR_RESET (1u << 31)

// status
#define LTC2992_FLAG_DONE (1u << 0)
#define LTC2992_FLAG_I2C_NACK (1u << 1)
#define LTC2992_FLAG_I2C_BUSY (1u << 2)

#define LTC2992_FLAG_LOCK (1u << 7)

#ifdef __cplusplus
extern "C"
{
#endif

	typedef union
	{
		struct
		{
			uint32_t command : 8;
			uint32_t i2c_addr : 7;
			uint32_t _r1 : 1;
			uint32_t data_len : 3;
			uint32_t _r2 : 5;
			uint32_t rw : 1;
			uint32_t _r3 : 4;
			uint32_t en : 1;
			uint32_t trans_trigger : 1;
			uint32_t reset : 1;
			/* data */
		} field;
		uint32_t data;
	} avl_ltc2992_CR_REG;

	typedef union
	{
		struct
		{
			uint32_t done : 1;
			uint32_t i2c_nack : 1;
			uint32_t i2c_busy : 1;

		} field;
		uint32_t data;
	} avl_ltc2992_FLAG_REG;

	typedef union
	{
		struct
		{
			uint8_t d8[4];
		} field;

		uint32_t d32;
	} avl_ltc2992_RTX_REG;

	typedef struct tag_ltc2992_Reg
	{
		volatile avl_ltc2992_CR_REG cr;
		volatile uint32_t ie;
		volatile avl_ltc2992_FLAG_REG flag;
		volatile avl_ltc2992_RTX_REG tx;
		volatile avl_ltc2992_RTX_REG rx;
	} LTC2992_Reg;

	/// @brief LTC2992 Device Type
	typedef struct tag_LTC2992_Dev
	{
		struct alt_dev_s dev;

		/// @brief LTC2992 Base Address
		struct tag_LTC2992_Reg *const BASE;

		/// @brief Interrupt ID
		const uint8_t IRQ;

		/// @brief Interrupt controller ID
		const uint8_t IC_ID;

		/// @brief Core frequency
		const unsigned CORE_FREQ;

		/// @brief I2C Speed
		const unsigned BUS_CLK;

		OS_FLAG_GRP *flag;

	} LTC2992_Dev;

	/// @brief Find TMP101 Device by device name
	/// @param name Device name
	/// @return Pointer to TMP101 Device, NULL if device not found
	LTC2992_Dev *LTC2992_OpenDev(const char *name);

	/// @brief Get device status
	/// @param dev
	/// @return status mask bit: LTC2992_FLAG_DONE, LTC2992_FLAG_I2C_NACK, LTC2992_FLAG_I2C_BUSY
	uint32_t LTC2992_GetStatus(LTC2992_Dev *dev);

	// call by sys_init
	int LTC2992_Init(LTC2992_Dev *dev);

#ifndef NIOS2_DCACHE_BYPASS_MASK
#define NIOS2_DCACHE_BYPASS_MASK 0
#endif

#define AVL_LTC2992_INSTANCE(name, dev)                              \
	static LTC2992_Dev dev =                                         \
		{                                                            \
			{                                                        \
				ALT_LLIST_ENTRY,                                     \
				name##_NAME,                                         \
				NULL, /* open */                                     \
				NULL, /* close */                                    \
				NULL, /* read */                                     \
				NULL, /* write */                                    \
				NULL, /* lseek */                                    \
				NULL, /* fstat */                                    \
				NULL, /* ioctl */                                    \
			},                                                       \
			(LTC2992_Reg *)(NIOS2_DCACHE_BYPASS_MASK | name##_BASE), \
			name##_IRQ,                                              \
			name##_IRQ_INTERRUPT_CONTROLLER_ID,                      \
			name##_FREQ,                                             \
			name##_BUS_CLK,                                          \
	}

#define AVL_LTC2992_INIT(name, dev) LTC2992_Init(&dev)

#ifdef __cplusplus
}
#endif

#endif
