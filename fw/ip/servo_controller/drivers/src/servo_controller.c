#include "servo_controller.h"
#include "LL/servo_controller_reg.h"
#include <string.h>
#include <ucos_ii.h>
#include <io.h>
#include <priv/alt_file.h>
#include <sys/alt_irq.h>
#include <sys/alt_dev.h>
#include <sys/alt_debug.h>

#define SERVO_IOWR(dev, reg, data) IOWR((uintptr_t)dev->BASE, reg, data)
#define SERVO_IORD(dev, reg) IORD((uintptr_t)dev->BASE, reg)

static int servo_controller_get_phase_position_in_critical(
	struct servo_controller_dev_t *dev,
	int16_t position[SERVO_CONTROLLER_NUM_SERVO]) __attribute__((section(".exceptions")));

static int servo_controller_update_duty_in_critical(
	struct servo_controller_dev_t *dev,
	int16_t duty[SERVO_CONTROLLER_NUM_SERVO]) __attribute__((section(".exceptions")));

static int servo_controller_get_position_in_critical(
	struct servo_controller_dev_t *dev,
	int16_t position[SERVO_CONTROLLER_NUM_SERVO]) __attribute__((section(".exceptions")));

static int32_t predict_position1x_step(
	int32_t pk1, int16_t K_filter, int32_t K_predict_to_mea,
	int16_t deltaP, int16_t measured_val) __attribute__((section(".exceptions")));

static void servo_controller_process_step(
	struct servo_controller_dev_t *dev) __attribute__((section(".exceptions")));

static int servo_controller_get_phase_position_in_critical(
	struct servo_controller_dev_t *dev,
	int16_t position[SERVO_CONTROLLER_NUM_SERVO])
{
	servo_controller_reg_I16 i16Reg;

	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_POS_PHASE0_OFFSET);
	position[0] = i16Reg.i16_val;
	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_POS_PHASE1_OFFSET);
	position[1] = i16Reg.i16_val;
	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_POS_PHASE2_OFFSET);
	position[2] = i16Reg.i16_val;
	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_POS_PHASE3_OFFSET);
	position[3] = i16Reg.i16_val;

	return 0;
}

static int servo_controller_get_position_in_critical(
	struct servo_controller_dev_t *dev,
	int16_t position[SERVO_CONTROLLER_NUM_SERVO])
{
	servo_controller_reg_I16 i16Reg;

	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_POS0_OFFSET);
	position[0] = i16Reg.i16_val;
	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_POS1_OFFSET);
	position[1] = i16Reg.i16_val;
	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_POS2_OFFSET);
	position[2] = i16Reg.i16_val;
	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_POS3_OFFSET);
	position[3] = i16Reg.i16_val;

	return 0;
}

static int32_t predict_position1x_step(
	int32_t pk1, int16_t K_filter, int32_t K_predict_to_mea,
	int16_t deltaP, int16_t measured_val)
{
	int32_t pk = pk1 + K_predict_to_mea * deltaP;
	int32_t error = measured_val - (pk >> 15);
	pk = pk + K_filter * error;

	return pk;
}

static void servo_controller_process_step(
	struct servo_controller_dev_t *dev)
{
	int16_t delta_pos[SERVO_CONTROLLER_NUM_SERVO];
	int16_t pos[SERVO_CONTROLLER_NUM_SERVO];
	int16_t controlVal[SERVO_CONTROLLER_NUM_SERVO];

	servo_controller_get_phase_position_in_critical(dev, delta_pos);
	servo_controller_get_position_in_critical(dev, pos);
	unsigned i;
	struct servo_controller_data_t *data = dev->data;

	for (i = 0; i < SERVO_CONTROLLER_NUM_SERVO; i++)
	{
		if (dev->cfg->drv_en[i])
		{
			int32_t pk = predict_position1x_step(
				data->filter_position[i], data->K_filter[i], data->K_phase_to_mea[i], delta_pos[i], pos[i]);
			data->filter_position[i] = pk;
			controlVal[i] = PID_Step(&data->pidState[i], data->position_sp[i], pk >> 15);
		}
	}

	servo_controller_update_duty_in_critical(dev, controlVal);
}

void task_servo_business(void *arg)
{
	struct servo_controller_dev_t *dev = servo_controller_open_dev((const char *)arg);
	ALT_DEBUG_ASSERT((dev != NULL));

	struct servo_controller_data_t *data = (struct servo_controller_data_t *)dev->data;
	INT8U err;

	OSFlagPend(data->flag, SERVO_CONTROLLER_FLAG_ADC_VALID_BIT, OS_FLAG_WAIT_SET_ANY, 0, &err);
	ALT_DEBUG_ASSERT((err == OS_ERR_NONE));

	for (;;)
	{
		OSFlagPend(data->flag, SERVO_CONTROLLER_FLAG_MEA_TRIG_BIT, OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err);
		ALT_DEBUG_ASSERT((err == OS_ERR_NONE));

		if (dev->cfg->closed_loop_en)
		{
			servo_controller_process_step(dev);
		}
	}
}

static int servo_controller_update_duty_in_critical(
	struct servo_controller_dev_t *dev,
	int16_t duty[SERVO_CONTROLLER_NUM_SERVO])
{
	SERVO_IOWR(dev, SERVO_CONTROLLER_U0_OFFSET, -duty[0]);
	SERVO_IOWR(dev, SERVO_CONTROLLER_U1_OFFSET, -duty[1]);
	SERVO_IOWR(dev, SERVO_CONTROLLER_U2_OFFSET, -duty[2]);
	SERVO_IOWR(dev, SERVO_CONTROLLER_U3_OFFSET, -duty[3]);

	SERVO_IOWR(dev, SERVO_CONTROLLER_TR_OFFSET,
			   SERVO_CONTROLLER_TR_U_VALID_BIT);

	return 0;
}

struct servo_controller_dev_t *servo_controller_open_dev(const char *name)
{
	extern alt_llist alt_dev_list;
	struct servo_controller_dev_t *dev = (struct servo_controller_dev_t *)alt_find_dev(name, &alt_dev_list);

	return dev;
}

static int do_servo_controller_update_pid_parameter(struct servo_controller_dev_t *dev,
													enum Servo_controller_servo_id_t channel,
													const struct FixedPIDArgument *pidParameter)
{
	struct servo_controller_config_t *cfg = dev->cfg;
	float K_phase_to_mea = (60 << 15) /
						   (cfg->n_motor_pole[channel] * cfg->n_motor_ratio[channel] * pidParameter->E_lsb);

	if (K_phase_to_mea > (INT32_MAX - 1))
	{
		return -EINVAL;
	}
	else if (K_phase_to_mea < -INT32_MAX)
	{
		return -EINVAL;
	}
	else
	{
		int stt = PID_Init(&dev->data->pidState[channel], pidParameter);
		if (stt < 0)
		{
			return stt;
		}

		dev->data->K_phase_to_mea[channel] = (int32_t)K_phase_to_mea;
	}

	return 0;
}

int servo_controller_update_pid_parameter(struct servo_controller_dev_t *dev,
										  enum Servo_controller_servo_id_t channel,
										  const struct FixedPIDArgument *pidParameter)
{
	ALT_DEBUG_ASSERT(((channel >= 0) && (channel < +SERVO_CONTROLLER_NUM_SERVO)));
	memcpy(&dev->cfg->pidArgument[channel], pidParameter, sizeof(struct FixedPIDArgument));
	return do_servo_controller_update_pid_parameter(dev, channel, pidParameter);
}

int servo_controller_apply_configure(struct servo_controller_dev_t *dev)
{
	ALT_DEBUG_ASSERT((dev));

	servo_controller_reg_CR crReg;
	uint32_t prescale;
	servo_controller_reg_IE ieReg;
	struct servo_controller_config_t *cfg = dev->cfg;
	unsigned i;

	ALT_DEBUG_ASSERT((cfg->i_max[0] > 0));
	ALT_DEBUG_ASSERT((cfg->i_max[1] > 0));
	ALT_DEBUG_ASSERT((cfg->i_max[2] > 0));
	ALT_DEBUG_ASSERT((cfg->i_max[3] > 0));

	if (cfg->closed_loop_en)
	{
		for (i = 0; i < SERVO_CONTROLLER_NUM_SERVO; i++)
		{
			if (cfg->drv_en[i])
			{
				do_servo_controller_update_pid_parameter(dev, i, &cfg->pidArgument[i]);
				dev->data->K_filter[i] = cfg->K_position_filter[i];
			}
		}
	}

	crReg.val = 0;
	crReg.field.en = 0;
	crReg.field.filter_level = cfg->filter_level;
	crReg.field.protected_0_en = cfg->protected_en[0];
	crReg.field.protected_1_en = cfg->protected_en[1];
	crReg.field.protected_2_en = cfg->protected_en[2];
	crReg.field.protected_3_en = cfg->protected_en[3];
	crReg.field.drv_0_en = cfg->drv_en[0];
	crReg.field.drv_1_en = cfg->drv_en[1];
	crReg.field.drv_2_en = cfg->drv_en[2];
	crReg.field.drv_3_en = cfg->drv_en[3];

	OS_CPU_SR cpu_sr = 0;
	OS_ENTER_CRITICAL();
	SERVO_IOWR(dev, SERVO_CONTROLLER_CR_OFFSET, crReg.val);

	prescale = dev->CORE_FREQ / cfg->spi_speed - 1;
	SERVO_IOWR(dev, SERVO_CONTROLLER_SPI_PRES_OFFSET, prescale);

	servo_controller_reg_PWM_PRES pwm_pres;
	pwm_pres.field.base_pres = dev->CORE_FREQ / cfg->pwm_base_freq - 1;
	pwm_pres.field.half_period = cfg->pwm_base_freq / (cfg->pwm_freq << 1) - 1;
	SERVO_IOWR(dev, SERVO_CONTROLLER_PWM_PRES_OFFSET, pwm_pres.val);

	SERVO_IOWR(dev, SERVO_CONTROLLER_PWM_TRIG_RATE_OFFSET, cfg->pwm_trig_rate);

	SERVO_IOWR(dev, SERVO_CONTROLLER_PULSE_MODE0_OFFSET, cfg->drv_mode[0]);
	SERVO_IOWR(dev, SERVO_CONTROLLER_PULSE_MODE1_OFFSET, cfg->drv_mode[1]);
	SERVO_IOWR(dev, SERVO_CONTROLLER_PULSE_MODE2_OFFSET, cfg->drv_mode[2]);
	SERVO_IOWR(dev, SERVO_CONTROLLER_PULSE_MODE3_OFFSET, cfg->drv_mode[3]);

	SERVO_IOWR(dev, SERVO_CONTROLLER_I0_MAX_OFFSET, cfg->i_max[0]);
	SERVO_IOWR(dev, SERVO_CONTROLLER_I1_MAX_OFFSET, cfg->i_max[1]);
	SERVO_IOWR(dev, SERVO_CONTROLLER_I2_MAX_OFFSET, cfg->i_max[2]);
	SERVO_IOWR(dev, SERVO_CONTROLLER_I3_MAX_OFFSET, cfg->i_max[3]);

	// clear flag
	SERVO_IOWR(dev, SERVO_CONTROLLER_FLAG_OFFSET, 0xFFFFFFFFu);

	ieReg.val = 0;
	ieReg.field.adc_valid = cfg->on_adc_valid ? 1 : 0;
	ieReg.field.mea_trig = (cfg->closed_loop_en || cfg->on_new_process) ? 1 : 0;
	ieReg.field.realtime_err = cfg->on_realtime_err ? 1 : 0;

	if (cfg->on_stop_err)
	{
		ieReg.field.drv_0_stop = 1;
		ieReg.field.drv_1_stop = 1;
		ieReg.field.drv_2_stop = 1;
		ieReg.field.drv_3_stop = 1;
	}

	if (cfg->on_drv_fault)
	{
		ieReg.field.drv_0_fault = 1;
		ieReg.field.drv_1_fault = 1;
		ieReg.field.drv_2_fault = 1;
		ieReg.field.drv_3_fault = 1;
	}

	SERVO_IOWR(dev, SERVO_CONTROLLER_IE_OFFSET, ieReg.val);

	SERVO_IOWR(dev, SERVO_CONTROLLER_TR_OFFSET, SERVO_CONTROLLER_TR_RESET_BIT);

	crReg.field.en = 1;
	SERVO_IOWR(dev, SERVO_CONTROLLER_CR_OFFSET, crReg.val);

	SERVO_IOWR(dev, SERVO_CONTROLLER_TR_OFFSET, SERVO_CONTROLLER_TR_ADC_INIT_BIT);

	OS_EXIT_CRITICAL();

	return 0;
}

int servo_controller_start(struct servo_controller_dev_t *dev)
{
	ALT_DEBUG_ASSERT((dev));

	OS_CPU_SR cpu_sr = 0;
	int16_t pos[SERVO_CONTROLLER_NUM_SERVO];
	OS_ENTER_CRITICAL();

	servo_controller_get_position_in_critical(dev, pos);

	unsigned i;

	for (i = 0; i < SERVO_CONTROLLER_NUM_SERVO; i++)
	{
		dev->data->filter_position[i] = pos[i] << 15;
	}

	servo_controller_reg_CR crReg;
	crReg.val = SERVO_IORD(dev, SERVO_CONTROLLER_CR_OFFSET);
	crReg.field.en = 1;
	SERVO_IOWR(dev, SERVO_CONTROLLER_CR_OFFSET, crReg.val);

	SERVO_IOWR(dev, SERVO_CONTROLLER_TR_OFFSET,
			   SERVO_CONTROLLER_TR_START_SERVO3_BIT |
				   SERVO_CONTROLLER_TR_START_SERVO2_BIT |
				   SERVO_CONTROLLER_TR_START_SERVO1_BIT |
				   SERVO_CONTROLLER_TR_START_SERVO0_BIT);

	OS_EXIT_CRITICAL();

	return 0;
}

int servo_controller_stop(struct servo_controller_dev_t *dev)
{
	ALT_DEBUG_ASSERT((dev));

	OS_CPU_SR cpu_sr = 0;
	OS_ENTER_CRITICAL();

	servo_controller_reg_CR crReg;
	crReg.val = SERVO_IORD(dev, SERVO_CONTROLLER_CR_OFFSET);
	crReg.field.en = 0;
	SERVO_IOWR(dev, SERVO_CONTROLLER_CR_OFFSET, crReg.val);
	OS_EXIT_CRITICAL();

	return 0;
}

int servo_controller_update_duty(
	struct servo_controller_dev_t *dev,
	int16_t duty[SERVO_CONTROLLER_NUM_SERVO])
{
	ALT_DEBUG_ASSERT((dev));
	ALT_DEBUG_ASSERT((duty));

	int err;
	OS_CPU_SR cpu_sr = 0;
	OS_ENTER_CRITICAL();

	err = servo_controller_update_duty_in_critical(dev, duty);
	OS_EXIT_CRITICAL();

	return err;
}

int servo_controller_get_position(
	struct servo_controller_dev_t *dev,
	int16_t position[SERVO_CONTROLLER_NUM_SERVO])
{
	ALT_DEBUG_ASSERT((dev));
	ALT_DEBUG_ASSERT((position));

	OS_CPU_SR cpu_sr = 0;
	int err;
	OS_ENTER_CRITICAL();

	err = servo_controller_get_position_in_critical(dev, position);

	OS_EXIT_CRITICAL();

	return err;
}

int servo_controller_get_current(
	struct servo_controller_dev_t *dev,
	int16_t current[SERVO_CONTROLLER_NUM_SERVO])
{
	ALT_DEBUG_ASSERT((dev));
	ALT_DEBUG_ASSERT((current));

	OS_CPU_SR cpu_sr = 0;
	servo_controller_reg_I16 i16Reg;
	OS_ENTER_CRITICAL();

	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_I0_OFFSET);
	current[0] = i16Reg.i16_val;
	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_I1_OFFSET);
	current[1] = i16Reg.i16_val;
	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_I2_OFFSET);
	current[2] = i16Reg.i16_val;
	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_I3_OFFSET);
	current[3] = i16Reg.i16_val;

	OS_EXIT_CRITICAL();

	return 0;
}

int servo_controller_get_duty(
	struct servo_controller_dev_t *dev,
	int16_t duty[SERVO_CONTROLLER_NUM_SERVO])
{
	ALT_DEBUG_ASSERT((dev));
	ALT_DEBUG_ASSERT((duty));

	OS_CPU_SR cpu_sr = 0;
	servo_controller_reg_I16 i16Reg;
	OS_ENTER_CRITICAL();

	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_U0_OFFSET);
	duty[0] = i16Reg.i16_val;
	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_U1_OFFSET);
	duty[1] = i16Reg.i16_val;
	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_U2_OFFSET);
	duty[2] = i16Reg.i16_val;
	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_U3_OFFSET);
	duty[3] = i16Reg.i16_val;

	OS_EXIT_CRITICAL();

	return 0;
}

static void servo_controller_irq_handler(void *arg) __attribute__((section(".exceptions")));

static void servo_controller_irq_handler(void *arg)
{
	servo_controller_reg_FLAG flag;
	servo_controller_reg_IE ie;
	INT8U err;
	struct servo_controller_dev_t *dev = (struct servo_controller_dev_t *)arg;

	ie.val = SERVO_IORD(dev, SERVO_CONTROLLER_IE_OFFSET);
	flag.val = SERVO_IORD(dev, SERVO_CONTROLLER_FLAG_OFFSET);
	SERVO_IOWR(dev, SERVO_CONTROLLER_FLAG_OFFSET, 0xFFFFFFFFu);

	OSFlagPost(dev->data->flag, flag.val, OS_FLAG_SET, &err);
	flag.val &= ie.val;

	ALT_DEBUG_ASSERT((err == OS_ERR_NONE));

	if (flag.val & SERVO_CONTROLLER_FLAG_MEA_TRIG_BIT)
	{
		if (dev->cfg->on_new_process)
		{
			dev->cfg->on_new_process(dev, dev->cfg->callback_arg);
		}
	}

	if (flag.val & ~SERVO_CONTROLLER_FLAG_MEA_TRIG_BIT)
	{
		if (flag.val & SERVO_CONTROLLER_FLAG_ADC_VALID_BIT)
		{
			dev->cfg->on_adc_valid(dev, dev->cfg->callback_arg);
			ie.field.adc_valid = 0;
			SERVO_IOWR(dev, SERVO_CONTROLLER_IE_OFFSET, ie.val);
		}

		if (flag.val & SERVO_CONTROLLER_FLAG_REALTIME_ERR_BIT)
		{
			dev->cfg->on_realtime_err(dev, dev->cfg->callback_arg);
		}

		if (flag.val & SERVO_CONTROLLER_FLAG_STOP0_BIT)
		{
			dev->cfg->on_stop_err(dev, 0, dev->cfg->callback_arg);
		}

		if (flag.val & SERVO_CONTROLLER_FLAG_STOP1_BIT)
		{
			dev->cfg->on_stop_err(dev, 1, dev->cfg->callback_arg);
		}

		if (flag.val & SERVO_CONTROLLER_FLAG_STOP2_BIT)
		{
			dev->cfg->on_stop_err(dev, 2, dev->cfg->callback_arg);
		}

		if (flag.val & SERVO_CONTROLLER_FLAG_STOP3_BIT)
		{
			dev->cfg->on_stop_err(dev, 3, dev->cfg->callback_arg);
		}

		if (flag.val & SERVO_CONTROLLER_FLAG_DRV8320_FAULT0_BIT)
		{
			dev->cfg->on_drv_fault(dev, 0, dev->cfg->callback_arg);
		}

		if (flag.val & SERVO_CONTROLLER_FLAG_DRV8320_FAULT1_BIT)
		{
			dev->cfg->on_drv_fault(dev, 1, dev->cfg->callback_arg);
		}

		if (flag.val & SERVO_CONTROLLER_FLAG_DRV8320_FAULT2_BIT)
		{
			dev->cfg->on_drv_fault(dev, 2, dev->cfg->callback_arg);
		}

		if (flag.val & SERVO_CONTROLLER_FLAG_DRV8320_FAULT3_BIT)
		{
			dev->cfg->on_drv_fault(dev, 3, dev->cfg->callback_arg);
		}

		
	}
}

void servo_controller_init(struct servo_controller_dev_t *dev)
{
	INT8U err;

	SERVO_IOWR(dev, SERVO_CONTROLLER_CR_OFFSET, 0);

	SERVO_IOWR(dev, SERVO_CONTROLLER_TR_OFFSET, SERVO_CONTROLLER_TR_RESET_BIT);

	// disable irq
	SERVO_IOWR(dev, SERVO_CONTROLLER_IE_OFFSET, 0);

	// clear flag
	SERVO_IOWR(dev, SERVO_CONTROLLER_FLAG_OFFSET, 0xFFFFFFFFu);

	dev->data->flag = OSFlagCreate(0, &err);
	ALT_DEBUG_ASSERT((err == OS_ERR_NONE));

	alt_ic_isr_register(dev->IC_ID, dev->IRQ, servo_controller_irq_handler, dev, 0);
	alt_dev_reg(&dev->dev);
}

int servo_controller_set_position(
	struct servo_controller_dev_t *dev,
	enum Servo_controller_servo_id_t channel,
	int16_t pos)
{
	ALT_DEBUG_ASSERT((dev));

	OS_CPU_SR cpu_sr = 0;
	OS_ENTER_CRITICAL();

	dev->data->position_sp[channel] = pos;
	OS_EXIT_CRITICAL();

	return 0;
}

int servo_controller_get_phase_position(
	struct servo_controller_dev_t *dev,
	int16_t position[SERVO_CONTROLLER_NUM_SERVO])
{
	ALT_DEBUG_ASSERT((dev));
	ALT_DEBUG_ASSERT((position));

	OS_CPU_SR cpu_sr = 0;
	unsigned i;
	OS_ENTER_CRITICAL();

	for (i = 0; i < SERVO_CONTROLLER_NUM_SERVO; i++)
	{
		position[i] = dev->data->filter_position[i] >> 15;
	}

	OS_EXIT_CRITICAL();

	return 0;
}
