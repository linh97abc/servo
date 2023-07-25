#ifndef __DRV8320_H__
#define __DRV8320_H__

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/alt_dev.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// @brief Maximun channel supported
#define DRV8320_NUM_CHANNEL 4

/// @brief Convert float number to fixed(16, 0) number
#define DRV8320_FLOAT_TO_FIXED(num) ((int16_t)((num)*INT16_MAX))

/// @brief Convert fixed(16, 0) number to float number
#define DRV8320_FIXED_TO_FLOAT(num) ((float) (num)/INT16_MAX)

	/// @brief Channel ID
	enum Drv8320_channel_id_t
	{
		DRV8320_CHANNEL_ID_0,
		DRV8320_CHANNEL_ID_1,
		DRV8320_CHANNEL_ID_2,
		DRV8320_CHANNEL_ID_3
	};

	/// @brief Mode PWM
	enum Drv8320_drv_mode_t
	{
		DRV8320_DRV_MODE_6x = 0,
		DRV8320_DRV_MODE_3x = 1,
		DRV8320_DRV_MODE_1x = 2,
	};

	typedef union
	{
		struct
		{
			uint32_t en : 1;
			uint32_t filter_level : 4;
			uint32_t protected_0_en : 1;
			uint32_t protected_1_en : 1;
			uint32_t protected_2_en : 1;
			uint32_t protected_3_en : 1;
			uint32_t drv_0_en : 1;
			uint32_t drv_1_en : 1;
			uint32_t drv_2_en : 1;
			uint32_t drv_3_en : 1;
		} field;
		uint32_t val;
	} Drv8320_reg_CR;

	typedef union
	{
		struct
		{
			uint32_t realtime_err : 1;
			uint32_t mea_trig : 1;
			uint32_t adc_valid : 1;
			uint32_t drv_0_stop : 1;
			uint32_t drv_1_stop : 1;
			uint32_t drv_2_stop : 1;
			uint32_t drv_3_stop : 1;
			uint32_t drv_0_fault : 1;
			uint32_t drv_1_fault : 1;
			uint32_t drv_2_fault : 1;
			uint32_t drv_3_fault : 1;
		} field;
		uint32_t val;
	} Drv8320_reg_FLAG;

	typedef union
	{
		struct
		{
			uint32_t realtime_err : 1;
			uint32_t mea_trig : 1;
			uint32_t adc_valid : 1;
			uint32_t drv_0_stop : 1;
			uint32_t drv_1_stop : 1;
			uint32_t drv_2_stop : 1;
			uint32_t drv_3_stop : 1;
			uint32_t drv_0_fault : 1;
			uint32_t drv_1_fault : 1;
			uint32_t drv_2_fault : 1;
			uint32_t drv_3_fault : 1;
		} field;
		uint32_t val;
	} Drv8320_reg_IE;

	typedef union
	{
		enum Drv8320_drv_mode_t enum_val;
		uint32_t u32_val;
	} Drv8320_reg_DRV_MODE;

	typedef union
	{
		int16_t i16_val;
		uint32_t u32_val;
	} Drv8320_reg_I16;

	struct Drv8320_reg_t
	{
		volatile Drv8320_reg_CR cr;
		volatile uint32_t tr;
		volatile Drv8320_reg_IE ie;
		volatile Drv8320_reg_FLAG flag;

		uint32_t reserved;
		volatile uint32_t pwm_prescale;
		volatile uint32_t pwm_hperiod;

		volatile Drv8320_reg_DRV_MODE drv_mode[DRV8320_NUM_CHANNEL];
		volatile Drv8320_reg_I16 duty[DRV8320_NUM_CHANNEL];
		volatile uint32_t hall[DRV8320_NUM_CHANNEL];
	};

	struct Drv8320_dev_t;
	struct Drv8320_config_t
	{
		/// @brief Base frequence of PWM (min period of PWM pulse), dependence of switch on/off/deadtime
		uint32_t pwm_base_freq;

		/// @brief PWM frequency
		uint32_t pwm_freq;

		/// @brief Mode of pulse for motor driver
		enum Drv8320_drv_mode_t drv_mode[DRV8320_NUM_CHANNEL];

		/// @brief New sample event handler
		void (*on_new_process)(struct Drv8320_dev_t *dev, void *arg);

		/// @brief Realtime violation event handler
		void (*on_realtime_err)(struct Drv8320_dev_t *dev, void *arg);

		/// @brief Motor current ovverload event handler
		void (*on_stop_err)(struct Drv8320_dev_t *dev, enum Drv8320_channel_id_t channel, void *arg);

		/// @brief Motor driver fault event handler
		void (*on_drv_fault)(struct Drv8320_dev_t *dev, enum Drv8320_channel_id_t channel, void *arg);

		/// @brief on_new_process_arg event argument
		void *on_new_process_arg;

		/// @brief on_realtime_err_arg event argument
		void *on_realtime_err_arg;

		/// @brief on_stop_err_arg event argument
		void *on_stop_err_arg;

		/// @brief on_drv_fault_arg event argument
		void *on_drv_fault_arg;
	};

	struct Drv8320_dev_t
	{
		struct alt_dev_s dev;
		struct Drv8320_reg_t *const BASE;
		const uint8_t IRQ;
		const uint8_t IC_ID;
		const unsigned CORE_FREQ;
		struct Drv8320_config_t *const cfg;
	};

#define DRV8320_CFG(dev) ((dev)->cfg)

	/// @brief Find Servo device by name
	/// @param name Servo device name
	/// @return Pointer to servo device, NULL if device not found
	/// @example Open servo device named "/dev/servo"
	///     struct Drv8320_dev_t *servo_dev;
	///
	///     void main() {
	///         servo_dev = Drv8320_open_dev("/dev/servo")
	///         // do something
	///     }
	struct Drv8320_dev_t *Drv8320_open_dev(const char *name);

	/// @brief Apply new config for device
	/// @param dev Pointer to servo device
	/// @return Error code
	/// @example Configure spi speed, pwm frequency
	///     DRV8320_CFG(servo_dev)->spi_speed = 1000000;
	///     DRV8320_CFG(servo_dev)->pwm_base_freq = 1000000;
	///     DRV8320_CFG(servo_dev)->pwm_freq = 1000;
	///     Drv8320_apply_configure(servo_dev);
	int Drv8320_apply_configure(struct Drv8320_dev_t *dev);

	/// @brief Start servo
	/// @param dev Pointer to servo device
	/// @return Error code
	int Drv8320_start(struct Drv8320_dev_t *dev);

	/// @brief Stop servo
	/// @param dev Pointer to servo device
	/// @return Error code
	int Drv8320_stop(struct Drv8320_dev_t *dev);

	/// @brief Update duty cycle, must to call after each servo process
	/// @param dev Pointer to servo device
	/// @param duty Duty cycle in fixed number (16, 0) , range [-1, 1)
	/// @return Error code
	/// @example
	///     void servo_irq_handler(void *arg) {
	///         int duty[DRV8320_NUM_CHANNEL];
	///         // check if new_process_irq occurred
	///         // caculate new duty cycle
	///
	///         Drv8320_update_duty(dev, duty);
	///     }
	int Drv8320_update_duty(
		struct Drv8320_dev_t *dev,
		int16_t duty[DRV8320_NUM_CHANNEL]);



	/// @brief Get duty cycle of pwm
	/// @param dev Pointer to servo device
	/// @param duty Duty cycle in fixed number (16, 0) , range [-1, 1)
	/// @return Error code
	int Drv8320_get_duty(
		struct Drv8320_dev_t *dev,
		int16_t duty[DRV8320_NUM_CHANNEL]);

#include "LL/avl_drv8320_fd_cfg.h"

#ifdef __cplusplus
}
#endif


#endif // __DRV8320_H__