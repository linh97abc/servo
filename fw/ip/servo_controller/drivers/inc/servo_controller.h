#ifndef __SERVO_CONTROLLER_H__
#define __SERVO_CONTROLLER_H__

#include "servo_controller_reg.h"
#include <stdbool.h>
#include <sys/alt_dev.h>

enum Servo_controller_filter_level_t
{
	SERVO_CONTROLLER_FILTER_LEVEL_0,
	SERVO_CONTROLLER_FILTER_LEVEL_1,
	SERVO_CONTROLLER_FILTER_LEVEL_2,
	SERVO_CONTROLLER_FILTER_LEVEL_3,
	SERVO_CONTROLLER_FILTER_LEVEL_4,
	SERVO_CONTROLLER_FILTER_LEVEL_5,
	SERVO_CONTROLLER_FILTER_LEVEL_6,
	SERVO_CONTROLLER_FILTER_LEVEL_7,
	SERVO_CONTROLLER_FILTER_LEVEL_8,
	SERVO_CONTROLLER_FILTER_LEVEL_9,
	SERVO_CONTROLLER_FILTER_LEVEL_10,
	SERVO_CONTROLLER_FILTER_LEVEL_11,
	SERVO_CONTROLLER_FILTER_LEVEL_12,
	SERVO_CONTROLLER_FILTER_LEVEL_13,
	SERVO_CONTROLLER_FILTER_LEVEL_14,
	SERVO_CONTROLLER_FILTER_LEVEL_15,
};

enum Servo_controller_servo_id_t
{
	SERVO_CONTROLLER_SERVO_ID_0,
	SERVO_CONTROLLER_SERVO_ID_1,
	SERVO_CONTROLLER_SERVO_ID_2,
	SERVO_CONTROLLER_SERVO_ID_3
};

struct servo_controller_config_t
{
	enum Servo_controller_filter_level_t filter_level;
	bool protected_en[SERVO_CONTROLLER_NUM_SERVO];
	bool drv_en[SERVO_CONTROLLER_NUM_SERVO];
	uint32_t spi_speed;
	uint32_t pwm_base_freq;
	uint32_t pwm_freq;
	enum Servo_controller_drv_mode_t drv_mode[SERVO_CONTROLLER_NUM_SERVO];

	// protected
	uint16_t i_max[SERVO_CONTROLLER_NUM_SERVO];

	// irq
	bool realtime_err_irq_en;
	bool new_process_irq_en;
	bool adc_valid_irq_en;
	bool stop_irq_en[SERVO_CONTROLLER_NUM_SERVO];
	bool drv_fault_irq_en[SERVO_CONTROLLER_NUM_SERVO];

	void (*on_adc_valid)(struct servo_controller_dev_t *dev, void *arg);
	void (*on_new_process)(struct servo_controller_dev_t *dev, void *arg);
	void (*on_realtime_err)(struct servo_controller_dev_t *dev, void *arg);
	void (*on_stop_err)(struct servo_controller_dev_t *dev, void *arg);
	void (*on_drv_fault)(struct servo_controller_dev_t *dev, void *arg);
};

struct servo_controller_dev_t
{
	struct alt_dev_s dev;
	struct servo_controller_reg_t *base;
	const uint8_t irq;
	const uint8_t ic_id;
	const unsigned coreFreq;
	struct servo_controller_config_t *cfg;
};

struct servo_controller_dev_t *servo_controller_open_dev(const char *name);

int servo_controller_configure(
	struct servo_controller_dev_t *dev,
	struct servo_controller_config_t *static_cfg);

int servo_controller_start(struct servo_controller_dev_t *dev);
int servo_controller_stop(struct servo_controller_dev_t *dev);

int servo_controller_set_duty(
	struct servo_controller_dev_t *dev,
	enum Servo_controller_servo_id_t servo_id,
	int16_t duty);

int servo_controller_update_process(struct servo_controller_dev_t *dev);

int servo_controller_set_and_update(
	struct servo_controller_dev_t *dev,
	int16_t duty[SERVO_CONTROLLER_NUM_SERVO]);

int servo_controller_get_position(
	struct servo_controller_dev_t *dev,
	uint16_t position[SERVO_CONTROLLER_NUM_SERVO]);

int servo_controller_get_current(
	struct servo_controller_dev_t *dev,
	uint16_t current[SERVO_CONTROLLER_NUM_SERVO]);

int servo_controller_get_duty(
	struct servo_controller_dev_t *dev,
	int16_t duty[SERVO_CONTROLLER_NUM_SERVO]);

#endif // __SERVO_CONTROLLER_H__