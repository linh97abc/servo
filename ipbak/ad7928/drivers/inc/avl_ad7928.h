#ifndef __AD7928_H__
#define __AD7928_H__

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/alt_dev.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define AD7928_NUM_SERVO 4

	enum Servo_controller_filter_level_t
	{
		AD7928_FILTER_LEVEL_0,
		AD7928_FILTER_LEVEL_1,
		AD7928_FILTER_LEVEL_2,
		AD7928_FILTER_LEVEL_3,
		AD7928_FILTER_LEVEL_4,
		AD7928_FILTER_LEVEL_5,
		AD7928_FILTER_LEVEL_6,
		AD7928_FILTER_LEVEL_7,
		AD7928_FILTER_LEVEL_8,
		AD7928_FILTER_LEVEL_9,
		AD7928_FILTER_LEVEL_10,
		AD7928_FILTER_LEVEL_11,
		AD7928_FILTER_LEVEL_12,
		AD7928_FILTER_LEVEL_13,
		AD7928_FILTER_LEVEL_14,
		AD7928_FILTER_LEVEL_15,
	};

	enum Servo_controller_servo_id_t
	{
		AD7928_SERVO_ID_0,
		AD7928_SERVO_ID_1,
		AD7928_SERVO_ID_2,
		AD7928_SERVO_ID_3
	};

	enum Servo_controller_drv_mode_t
	{
		AD7928_DRV_MODE_6x = 0,
		AD7928_DRV_MODE_3x = 1,
		AD7928_DRV_MODE_1x = 2,
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
	} servo_controller_reg_CR;

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
	} servo_controller_reg_FLAG;

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
	} servo_controller_reg_IE;

	typedef union
	{
		enum Servo_controller_drv_mode_t enum_val;
		uint32_t u32_val;
	} servo_controller_reg_DRV_MODE;

	typedef union
	{
		int16_t i16_val;
		uint32_t u32_val;
	} servo_controller_reg_I16;

	struct servo_controller_reg_t
	{
		volatile servo_controller_reg_CR cr;
		volatile uint32_t tr;
		volatile servo_controller_reg_IE ie;
		volatile servo_controller_reg_FLAG flag;

		volatile uint32_t spi_prescale;
		volatile uint32_t pwm_prescale;
		volatile uint32_t pwm_hperiod;

		volatile servo_controller_reg_DRV_MODE drv_mode[AD7928_NUM_SERVO];
		volatile servo_controller_reg_I16 duty[AD7928_NUM_SERVO];
		volatile servo_controller_reg_I16 i_max[AD7928_NUM_SERVO];
		const volatile servo_controller_reg_I16 i[AD7928_NUM_SERVO];
		const volatile servo_controller_reg_I16 pos[AD7928_NUM_SERVO];
	};

	struct servo_controller_dev_t;
	struct servo_controller_config_t
	{
		/// @brief filter level for measurement data
		enum Servo_controller_filter_level_t filter_level;

		/// @brief stop motor if motor current overload
		bool protected_en[AD7928_NUM_SERVO];

		/// @brief enable motor driver
		bool drv_en[AD7928_NUM_SERVO];

		/// @brief spi speed of measurement IC
		uint32_t spi_speed;

		/// @brief Base frequence of PWM (min period of PWM pulse), dependence of switch on/off/deadtime
		uint32_t pwm_base_freq;

		/// @brief PWM frequency
		uint32_t pwm_freq;

		/// @brief Mode of pulse for motor driver
		enum Servo_controller_drv_mode_t drv_mode[AD7928_NUM_SERVO];

		/// @brief Limit of motor current value, in fixed(16, 0), range [-1, 0)
		int16_t i_max[AD7928_NUM_SERVO];

		/// @brief Adc init done event handler
		void (*on_adc_valid)(struct servo_controller_dev_t *dev, void *arg);

		/// @brief New sample event handler
		void (*on_new_process)(struct servo_controller_dev_t *dev, void *arg);

		/// @brief Realtime violation event handler
		void (*on_realtime_err)(struct servo_controller_dev_t *dev, void *arg);

		/// @brief Motor current ovverload event handler
		void (*on_stop_err)(struct servo_controller_dev_t *dev, enum Servo_controller_servo_id_t servo_id, void *arg);

		/// @brief Motor driver fault event handler
		void (*on_drv_fault)(struct servo_controller_dev_t *dev, enum Servo_controller_servo_id_t servo_id, void *arg);

		/// @brief on_adc_valid event argument
		void *on_adc_valid_arg;

		/// @brief on_new_process_arg event argument
		void *on_new_process_arg;

		/// @brief on_realtime_err_arg event argument
		void *on_realtime_err_arg;

		/// @brief on_stop_err_arg event argument
		void *on_stop_err_arg;

		/// @brief on_drv_fault_arg event argument
		void *on_drv_fault_arg;
	};

	struct servo_controller_dev_t
	{
		struct alt_dev_s dev;
		struct servo_controller_reg_t *const BASE;
		const uint8_t IRQ;
		const uint8_t IC_ID;
		const unsigned CORE_FREQ;
		struct servo_controller_config_t *const cfg;
	};

#define AD7928_CFG(dev) ((dev)->cfg)

	/// @brief Find Servo device by name
	/// @param name Servo device name
	/// @return Pointer to servo device, NULL if device not found
	/// @example Open servo device named "/dev/servo"
	///     struct servo_controller_dev_t *servo_dev;
	///
	///     void main() {
	///         servo_dev = servo_controller_open_dev("/dev/servo")
	///         // do something
	///     }
	struct servo_controller_dev_t *servo_controller_open_dev(const char *name);

	/// @brief Apply new config for device
	/// @param dev Pointer to servo device
	/// @return Error code
	/// @example Configure spi speed, pwm frequency
	///     AD7928_CFG(servo_dev)->spi_speed = 1000000;
	///     AD7928_CFG(servo_dev)->pwm_base_freq = 1000000;
	///     AD7928_CFG(servo_dev)->pwm_freq = 1000;
	///     servo_controller_apply_configure(servo_dev);
	int servo_controller_apply_configure(struct servo_controller_dev_t *dev);

	/// @brief Start servo
	/// @param dev Pointer to servo device
	/// @return Error code
	int servo_controller_start(struct servo_controller_dev_t *dev);

	/// @brief Stop servo
	/// @param dev Pointer to servo device
	/// @return Error code
	int servo_controller_stop(struct servo_controller_dev_t *dev);

	/// @brief Update duty cycle, must to call after each servo process
	/// @param dev Pointer to servo device
	/// @param duty Duty cycle in fixed number (16, 0) , range [-1, 1)
	/// @return Error code
	/// @example
	///     void servo_irq_handler(void *arg) {
	///         int duty[AD7928_NUM_SERVO];
	///         // check if new_process_irq occurred
	///         // caculate new duty cycle
	///
	///         servo_controller_update_duty(dev, duty);
	///     }
	int servo_controller_update_duty(
		struct servo_controller_dev_t *dev,
		int16_t duty[AD7928_NUM_SERVO]);

	/// @brief Get motor postion
	/// @param dev Pointer to servo device
	/// @param position Motor position in fixed number (16, 0) , range [-1, 1)
	/// @return Error code
	int servo_controller_get_position(
		struct servo_controller_dev_t *dev,
		int16_t position[AD7928_NUM_SERVO]);

	/// @brief Get motor current
	/// @param dev Pointer to servo device
	/// @param current Motor current in fixed number (16, 0) , range [-1, 1)
	/// @return Error code
	int servo_controller_get_current(
		struct servo_controller_dev_t *dev,
		int16_t current[AD7928_NUM_SERVO]);

	/// @brief Get duty cycle of pwm
	/// @param dev Pointer to servo device
	/// @param duty Duty cycle in fixed number (16, 0) , range [-1, 1)
	/// @return Error code
	int servo_controller_get_duty(
		struct servo_controller_dev_t *dev,
		int16_t duty[AD7928_NUM_SERVO]);

#ifdef __cplusplus
}
#endif

#endif // __AD7928_H__