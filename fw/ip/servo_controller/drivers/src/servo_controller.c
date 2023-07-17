#include "servo_controller.h"

struct servo_controller_dev_t *servo_controller_open_dev(const char *name)
{
	return (struct servo_controller_dev_t *)0;
}

int servo_controller_configure(
	struct servo_controller_dev_t *dev,
	struct servo_controller_config_t *static_cfg)
{
	return 0;
}

int servo_controller_start(struct servo_controller_dev_t *dev)
{
	return 0;
}

int servo_controller_stop(struct servo_controller_dev_t *dev)
{
	return 0;
}

int servo_controller_set_duty(
	struct servo_controller_dev_t *dev,
	enum Servo_controller_servo_id_t servo_id,
	int16_t duty)
{
	return 0;
}

int servo_controller_update_process(struct servo_controller_dev_t *dev)
{
	return 0;
}

int servo_controller_set_and_update(
	struct servo_controller_dev_t *dev,
	int16_t duty[SERVO_CONTROLLER_NUM_SERVO])
{
	return 0;
}

int servo_controller_get_position(
	struct servo_controller_dev_t *dev,
	uint16_t position[SERVO_CONTROLLER_NUM_SERVO])
{
	return 0;
}

int servo_controller_get_current(
	struct servo_controller_dev_t *dev,
	uint16_t current[SERVO_CONTROLLER_NUM_SERVO])
{
	return 0;
}

int servo_controller_get_duty(
	struct servo_controller_dev_t *dev,
	int16_t duty[SERVO_CONTROLLER_NUM_SERVO])
{
	return 0;
}

void servo_controller_init(struct servo_controller_dev_t *dev)
{
}