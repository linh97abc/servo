#ifndef __SERVO_CONTROLLER_H__
#define __SERVO_CONTROLLER_H__

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/alt_dev.h>
#include <ucos_ii.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// @brief Maximun channel supported
#define SERVO_CONTROLLER_NUM_SERVO 4

/// @brief Convert float number to fixed(16, 0) number
#define SERVO_CONTROLLER_FLOAT_TO_FIXED(num) ((int16_t)((num)*INT16_MAX))

/// @brief Convert fixed(16, 0) number to float number
#define SERVO_CONTROLLER_FIXED_TO_FLOAT(num) ((float)(num) / INT16_MAX)

	enum Servo_controller_filter_level_t
	{
		SERVO_CONTROLLER_FILTER_LEVEL_1,
		SERVO_CONTROLLER_FILTER_LEVEL_2,
		SERVO_CONTROLLER_FILTER_LEVEL_4,
		SERVO_CONTROLLER_FILTER_LEVEL_8,
		SERVO_CONTROLLER_FILTER_LEVEL_16,
		SERVO_CONTROLLER_FILTER_LEVEL_32,
		SERVO_CONTROLLER_FILTER_LEVEL_64,
		SERVO_CONTROLLER_FILTER_LEVEL_128,
		SERVO_CONTROLLER_FILTER_LEVEL_256,
		SERVO_CONTROLLER_FILTER_LEVEL_512,
		SERVO_CONTROLLER_FILTER_LEVEL_1024,
		SERVO_CONTROLLER_FILTER_LEVEL_2048,
		SERVO_CONTROLLER_FILTER_LEVEL_4096,
		SERVO_CONTROLLER_FILTER_LEVEL_8192,
		SERVO_CONTROLLER_FILTER_LEVEL_16384,
		SERVO_CONTROLLER_FILTER_LEVEL_32768,
	};

	enum Servo_controller_servo_id_t
	{
		SERVO_CONTROLLER_SERVO_ID_0 = 0,
		SERVO_CONTROLLER_SERVO_ID_1,
		SERVO_CONTROLLER_SERVO_ID_2,
		SERVO_CONTROLLER_SERVO_ID_3
	};

	enum Servo_controller_drv_mode_t
	{
		SERVO_CONTROLLER_DRV_MODE_6x = 0,
		SERVO_CONTROLLER_DRV_MODE_3x = 1,
		SERVO_CONTROLLER_DRV_MODE_1x = 2,
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
			uint32_t ctrl_step_pending : 1;
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
		struct
		{
			uint32_t half_period : 15;
			uint32_t reserved : 1;
			uint32_t base_pres : 15;
		} field;
		uint32_t val;
	} servo_controller_reg_PWM_PRES;

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

	typedef struct
	{
		uint32_t hc_0 : 1;
		uint32_t hb_0 : 1;
		uint32_t ha_0 : 1;
		uint32_t hc_1 : 1;
		uint32_t hb_1 : 1;
		uint32_t ha_1 : 1;
		uint32_t hc_2 : 1;
		uint32_t hb_2 : 1;
		uint32_t ha_2 : 1;
		uint32_t hc_3 : 1;
		uint32_t hb_3 : 1;
		uint32_t ha_3 : 1;
	} servo_controller_reg_HALL_SIGNAL;

	struct servo_controller_reg_t
	{
		volatile servo_controller_reg_CR cr;
		volatile uint32_t tr;
		volatile servo_controller_reg_IE ie;
		volatile servo_controller_reg_FLAG flag;

		volatile uint32_t spi_prescale;
		volatile servo_controller_reg_PWM_PRES pwm_prescale;
		volatile uint32_t pwm_trig_rate;

		volatile servo_controller_reg_DRV_MODE drv_mode[SERVO_CONTROLLER_NUM_SERVO];
		volatile servo_controller_reg_I16 duty[SERVO_CONTROLLER_NUM_SERVO];
		volatile servo_controller_reg_I16 i_max[SERVO_CONTROLLER_NUM_SERVO];
		const volatile servo_controller_reg_I16 i[SERVO_CONTROLLER_NUM_SERVO];
		const volatile servo_controller_reg_I16 pos[SERVO_CONTROLLER_NUM_SERVO];
		const volatile servo_controller_reg_I16 dpos_phase[SERVO_CONTROLLER_NUM_SERVO];
		volatile uint32_t pos_phase[SERVO_CONTROLLER_NUM_SERVO];
		const volatile servo_controller_reg_HALL_SIGNAL hall_sig;
	};

	struct servo_controller_dev_t;
	struct servo_controller_config_t
	{
		/// @brief filter level for measurement data
		enum Servo_controller_filter_level_t filter_level;

		/// @brief stop motor if motor current overload
		bool protected_en[SERVO_CONTROLLER_NUM_SERVO];

		/// @brief enable motor driver
		bool drv_en[SERVO_CONTROLLER_NUM_SERVO];

		float Pos_lsb[SERVO_CONTROLLER_NUM_SERVO];
		float Current_lsb[SERVO_CONTROLLER_NUM_SERVO];

		/// @brief spi speed of measurement IC
		uint32_t spi_speed;

		/// @brief Base frequence of PWM (min period of PWM pulse), dependence of switch on/off/deadtime
		uint32_t pwm_base_freq;

		/// @brief PWM frequency
		uint32_t pwm_freq;

		/// @brief PWM trigger rate
		uint8_t pwm_trig_rate;

		/// @brief Mode of pulse for motor driver
		enum Servo_controller_drv_mode_t drv_mode[SERVO_CONTROLLER_NUM_SERVO];

		/// @brief Limit of motor current value
		float i_max[SERVO_CONTROLLER_NUM_SERVO];

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

		/// @brief Callback argument
		void *callback_arg;
	};

	struct servo_controller_data_t
	{
		uint8_t _reserved;
	};

	struct servo_controller_dev_t
	{
		struct alt_dev_s dev;
		struct servo_controller_reg_t *const BASE;
		const uint8_t IRQ;
		const uint8_t IC_ID;
		const unsigned CORE_FREQ;
		struct servo_controller_config_t *const cfg;
		struct servo_controller_data_t *const data;
	};

#define SERVO_CONTROLLER_CFG(dev) ((dev)->cfg)

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
	///     SERVO_CONTROLLER_CFG(servo_dev)->spi_speed = 1000000;
	///     SERVO_CONTROLLER_CFG(servo_dev)->pwm_base_freq = 1000000;
	///     SERVO_CONTROLLER_CFG(servo_dev)->pwm_freq = 1000;
	///     servo_controller_apply_configure(servo_dev);
	int servo_controller_apply_configure(struct servo_controller_dev_t *dev);

	/// @brief Start servo
	/// @param dev Pointer to servo device
	/// @return Error code
	int servo_controller_start(struct servo_controller_dev_t *dev);

	/// @brief ReStart servo
	/// @param dev Pointer to servo device
	/// @return Error code
	int servo_controller_restart_channel(
		struct servo_controller_dev_t *dev,
		enum Servo_controller_servo_id_t channel);

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
	///         int duty[SERVO_CONTROLLER_NUM_SERVO];
	///         // check if new_process_irq occurred
	///         // caculate new duty cycle
	///
	///         servo_controller_update_duty(dev, duty);
	///     }
	int servo_controller_set_and_notify_duty_changed(
		struct servo_controller_dev_t *dev,
		int16_t duty[SERVO_CONTROLLER_NUM_SERVO]);

	/// @brief Set duty cycle for 1 channel
	/// @param dev Pointer to servo device
	/// @param chanel Channel
	/// @param duty Duty cycle in fixed number (16, 0) , range [-1, 1)
	/// @return Error code
	int servo_controller_set_duty_1channel(
		struct servo_controller_dev_t *dev,
		enum Servo_controller_servo_id_t chanel,
		int16_t duty);

	/// @brief Use with @b servo_controller_set_duty_1channel , call after each servo process
	/// @param dev Pointer to servo device
	/// @return Error code
	int servo_controller_notify_duty_changed(
		struct servo_controller_dev_t *dev);

	/// @brief Get motor postion
	/// @param dev Pointer to servo device
	/// @param position Motor position
	/// @return Error code
	int servo_controller_get_position(
		struct servo_controller_dev_t *dev,
		int16_t position[SERVO_CONTROLLER_NUM_SERVO]);

	/// @brief Update motor phase postion
	/// @param dev Pointer to servo device
	/// @param pos_phase Motor position phase
	/// @return Error code
	int servo_controller_update_pos_phase(
		struct servo_controller_dev_t *dev,
		int32_t pos_phase[SERVO_CONTROLLER_NUM_SERVO]);

	/// @brief Get motor phase postion
	/// @param dev Pointer to servo device
	/// @param pos_phase Motor position phase
	/// @return Error code
	int servo_controller_get_phase_position(
		struct servo_controller_dev_t *dev,
		int32_t pos_phase[SERVO_CONTROLLER_NUM_SERVO]);

	/// @brief Get motor current
	/// @param dev Pointer to servo device
	/// @param current Motor current
	/// @return Error code
	int servo_controller_get_current(
		struct servo_controller_dev_t *dev,
		int16_t current[SERVO_CONTROLLER_NUM_SERVO]);

	/// @brief Get duty cycle of pwm
	/// @param dev Pointer to servo device
	/// @param duty Duty cycle in fixed number (16, 0) , range [-1, 1)
	/// @return Error code
	int servo_controller_get_duty(
		struct servo_controller_dev_t *dev,
		int16_t duty[SERVO_CONTROLLER_NUM_SERVO]);

	/// @brief Convert position (in fixed(16,0)) to position (in float)
	/// @param dev Pointer to servo device
	/// @param chanel Channel
	/// @param pos Position in fixed(16, 0) type
	/// @return position in float type
	static inline float servo_controller_code_to_position(
		struct servo_controller_dev_t *dev,
		enum Servo_controller_servo_id_t channel,
		int16_t pos)
	{
		return pos * dev->cfg->Pos_lsb[channel];
	}

	/// @brief Convert position (in fixed(16,0)) to position (in float)
	/// @param dev Pointer to servo device
	/// @param chanel Channel
	/// @param current Current in fixed(16, 0) type
	/// @return current in float type
	static inline float servo_controller_code_to_current(
		struct servo_controller_dev_t *dev,
		enum Servo_controller_servo_id_t channel,
		int16_t current)
	{
		return current * dev->cfg->Current_lsb[channel];
	}

#ifdef __cplusplus
}
#endif

#endif // __SERVO_CONTROLLER_H__
