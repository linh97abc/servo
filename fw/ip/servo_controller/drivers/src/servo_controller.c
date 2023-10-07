#include "servo_controller.h"
#include "LL/servo_controller_reg.h"
#include <string.h>
#include <ucos_ii.h>
#include <io.h>
#include <priv/alt_file.h>
#include <sys/alt_irq.h>
#include <sys/alt_dev.h>
#include <sys/alt_debug.h>

#ifndef BIT
#define BIT(i) (1u << (i))
#endif

#define SERVO_IOWR(dev, reg, data) IOWR((uintptr_t)dev->BASE, reg, data)
#define SERVO_IORD(dev, reg) IORD((uintptr_t)dev->BASE, reg)

static int servo_controller_update_duty_in_critical(
	struct servo_controller_dev_t *dev,
	int16_t duty[SERVO_CONTROLLER_NUM_SERVO]) __attribute__((section(".exceptions")));

static int servo_controller_get_position_in_critical(
	struct servo_controller_dev_t *dev,
	int16_t position[SERVO_CONTROLLER_NUM_SERVO]) __attribute__((section(".exceptions")));

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

static void servo_controller_init_pos_phase(struct servo_controller_dev_t *dev)
{
	INT8U err;
	struct servo_controller_data_t *data = (struct servo_controller_data_t *)dev->data;
	OSFlagPend(data->flag, SERVO_CONTROLLER_FLAG_ADC_VALID_BIT, OS_FLAG_WAIT_SET_ANY, 0, &err);
	ALT_DEBUG_ASSERT((err == OS_ERR_NONE));

	servo_controller_reg_I16 pos;

	unsigned i;
	for (i = 0; i < SERVO_CONTROLLER_NUM_SERVO; i++)
	{
		pos.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_POS0_OFFSET + i);
		int32_t tmp = pos.i16_val << 15;
		SERVO_IOWR(dev, SERVO_CONTROLLER_POS_PHASE0_OFFSET + i, tmp / dev->data->K_phase_to_mea[i]);
	}
}

void task_servo_business(void *arg)
{
	struct servo_controller_dev_t *dev = servo_controller_open_dev((const char *)arg);
	servo_controller_reg_IE ie;
	ALT_DEBUG_ASSERT((dev != NULL));

	struct servo_controller_data_t *data = (struct servo_controller_data_t *)dev->data;
	INT8U err;

	servo_controller_init_pos_phase(dev);
	ie.val = SERVO_IORD(dev, SERVO_CONTROLLER_IE_OFFSET);
	SERVO_IOWR(dev, SERVO_CONTROLLER_IE_OFFSET, ie.val | SERVO_CONTROLLER_FLAG_MEA_TRIG_BIT);

	for (;;)
	{
		OSFlagPend(data->flag, SERVO_CONTROLLER_FLAG_MEA_TRIG_BIT, OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err);
		ALT_DEBUG_ASSERT((err == OS_ERR_NONE));

		if (dev->cfg->on_new_process)
		{
			dev->cfg->on_new_process(dev, dev->cfg->callback_arg);
		}
		else
		{
			SERVO_IOWR(dev, SERVO_CONTROLLER_FLAG_OFFSET, SERVO_CONTROLLER_FLAG_MEA_TRIG_BIT);
		}

		ie.val = SERVO_IORD(dev, SERVO_CONTROLLER_IE_OFFSET);
		SERVO_IOWR(dev, SERVO_CONTROLLER_IE_OFFSET, ie.val | SERVO_CONTROLLER_FLAG_MEA_TRIG_BIT);
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

	SERVO_IOWR(dev, SERVO_CONTROLLER_FLAG_OFFSET,
			   SERVO_CONTROLLER_FLAG_MEA_TRIG_BIT);

	return 0;
}

struct servo_controller_dev_t *servo_controller_open_dev(const char *name)
{
	extern alt_llist alt_dev_list;
	struct servo_controller_dev_t *dev = (struct servo_controller_dev_t *)alt_find_dev(name, &alt_dev_list);

	return dev;
}

static int caculate_K_phase_to_mea(struct servo_controller_dev_t *dev,
								   enum Servo_controller_servo_id_t channel)
{
	struct servo_controller_config_t *cfg = dev->cfg;

	if (!(cfg->Pos_lsb[channel] > 0))
		return -EINVAL;
	if (!(cfg->n_motor_pole[channel] > 0))
		return -EINVAL;
	if (!(cfg->n_motor_ratio[channel] > 0))
		return -EINVAL;

	float K_phase_to_mea = (60 << 15) /
						   (cfg->n_motor_pole[channel] *
							cfg->n_motor_ratio[channel] *
							cfg->Pos_lsb[channel]);

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
		dev->data->K_phase_to_mea[channel] = (int32_t)K_phase_to_mea;
	}

	return 0;
}

static int caculate_I_max(struct servo_controller_config_t *cfg,
						  enum Servo_controller_servo_id_t channel,
						  int16_t *i_max)
{
	if (!(cfg->i_max[channel] > 0))
		return -EINVAL;

	if (!(cfg->Current_lsb[channel] > 0))
		return -EINVAL;

	float tmp = cfg->i_max[channel] / cfg->Current_lsb[channel];

	if (tmp > (INT16_MAX - 1))
	{
		*i_max = INT16_MAX - 1;
	}
	else
	{
		*i_max = (int16_t)tmp;
	}

	return 0;
}

int servo_controller_apply_configure(struct servo_controller_dev_t *dev)
{
	ALT_DEBUG_ASSERT((dev));

	servo_controller_reg_CR crReg;
	uint32_t prescale;
	servo_controller_reg_IE ieReg;
	struct servo_controller_config_t *cfg = dev->cfg;
	unsigned i;

	int16_t i_max[SERVO_CONTROLLER_NUM_SERVO];

	int ret;

	if (!cfg->spi_speed)
		return -EINVAL;
	if (!cfg->pwm_base_freq)
		return -EINVAL;
	if (!cfg->pwm_freq)
		return -EINVAL;

	for (i = 0; i < SERVO_CONTROLLER_NUM_SERVO; i++)
	{
		ret = caculate_K_phase_to_mea(dev, i);
		if (ret)
		{
			return ret;
		}

		if (cfg->drv_en[i])
		{

			ret = caculate_I_max(cfg, i, &i_max[i]);
			if (ret)
			{
				return ret;
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

	SERVO_IOWR(dev, SERVO_CONTROLLER_I0_MAX_OFFSET, i_max[0]);
	SERVO_IOWR(dev, SERVO_CONTROLLER_I1_MAX_OFFSET, i_max[1]);
	SERVO_IOWR(dev, SERVO_CONTROLLER_I2_MAX_OFFSET, i_max[2]);
	SERVO_IOWR(dev, SERVO_CONTROLLER_I3_MAX_OFFSET, i_max[3]);

	// clear flag
	SERVO_IOWR(dev, SERVO_CONTROLLER_FLAG_OFFSET, 0xFFFFFFFFu);

	ieReg.val = 0;
	ieReg.field.adc_valid = 1;
	ieReg.field.mea_trig = 0;
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

	int16_t duty[SERVO_CONTROLLER_NUM_SERVO] = {0, 0, 0, 0};
	int ret;

	OS_CPU_SR cpu_sr = 0;
	OS_ENTER_CRITICAL();

	servo_controller_reg_CR crReg;
	crReg.val = SERVO_IORD(dev, SERVO_CONTROLLER_CR_OFFSET);
	crReg.field.en = 1;
	SERVO_IOWR(dev, SERVO_CONTROLLER_CR_OFFSET, crReg.val);

	ret = servo_controller_update_duty_in_critical(dev, duty);
	if (ret)
	{
		OS_EXIT_CRITICAL();
		return ret;
	}
	

	SERVO_IOWR(dev, SERVO_CONTROLLER_TR_OFFSET,
			   SERVO_CONTROLLER_TR_START_SERVO3_BIT |
				   SERVO_CONTROLLER_TR_START_SERVO2_BIT |
				   SERVO_CONTROLLER_TR_START_SERVO1_BIT |
				   SERVO_CONTROLLER_TR_START_SERVO0_BIT);

	OS_EXIT_CRITICAL();

	return 0;
}

int servo_controller_restart_channel(
	struct servo_controller_dev_t *dev,
	enum Servo_controller_servo_id_t channel)
{
	ALT_DEBUG_ASSERT((dev));
	ALT_DEBUG_ASSERT((channel >= 0));
	ALT_DEBUG_ASSERT((channel < SERVO_CONTROLLER_NUM_SERVO));

	OS_CPU_SR cpu_sr = 0;
	OS_ENTER_CRITICAL();

	uint32_t cr_val = SERVO_IORD(dev, SERVO_CONTROLLER_CR_OFFSET);
	
	SERVO_IOWR(dev, SERVO_CONTROLLER_CR_OFFSET, cr_val & ~BIT(SERVO_CONTROLLER_CR_DRV_EN0_BIT_INDEX + channel));
	OSTimeDly(1);
	SERVO_IOWR(dev, SERVO_CONTROLLER_CR_OFFSET, cr_val);
	SERVO_IOWR(dev, SERVO_CONTROLLER_TR_OFFSET, BIT(SERVO_CONTROLLER_TR_START_SERVO0_BIT_INDEX + channel));

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

int servo_controller_set_and_notify_duty_changed(
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

	OSFlagPost(dev->data->flag, flag.val, OS_FLAG_SET, &err);
	flag.val &= ie.val;

	ALT_DEBUG_ASSERT((err == OS_ERR_NONE));

	if (flag.val & SERVO_CONTROLLER_FLAG_MEA_TRIG_BIT)
	{
		SERVO_IOWR(dev, SERVO_CONTROLLER_IE_OFFSET, ie.val & ~SERVO_CONTROLLER_FLAG_MEA_TRIG_BIT);
	}

	if (flag.val & ~SERVO_CONTROLLER_FLAG_MEA_TRIG_BIT)
	{
		if (flag.val & SERVO_CONTROLLER_FLAG_ADC_VALID_BIT)
		{
			if (dev->cfg->on_adc_valid)
			{
				dev->cfg->on_adc_valid(dev, dev->cfg->callback_arg);
			}

			ie.field.adc_valid = 0;
			SERVO_IOWR(dev, SERVO_CONTROLLER_IE_OFFSET, ie.val);
		}

		if (flag.val & SERVO_CONTROLLER_FLAG_REALTIME_ERR_BIT)
		{
			dev->cfg->on_realtime_err(dev, dev->cfg->callback_arg);
			SERVO_IOWR(dev, SERVO_CONTROLLER_FLAG_OFFSET, SERVO_CONTROLLER_FLAG_REALTIME_ERR_BIT);
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

static void set_default_config(struct servo_controller_config_t *cfg)
{
	unsigned i;

	for (i = 0; i < SERVO_CONTROLLER_NUM_SERVO; i++)
	{
		cfg->i_max[i] = 5;
		cfg->Pos_lsb[i] = 1;
		cfg->Current_lsb[i] = 1;
		cfg->n_motor_pole[i] = 1;
		cfg->n_motor_pole[i] = 1;
		cfg->n_motor_ratio[i] = 1;
	}

	cfg->spi_speed = 1000000;
	cfg->pwm_base_freq = 5000000;
	cfg->pwm_freq = 5000;
	cfg->pwm_trig_rate = 1;
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

	set_default_config(dev->cfg);
	alt_ic_isr_register(dev->IC_ID, dev->IRQ, servo_controller_irq_handler, dev, 0);
	alt_dev_reg(&dev->dev);
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

	int32_t pos;

	for (i = 0; i < SERVO_CONTROLLER_NUM_SERVO; i++)
	{
		pos = SERVO_IORD(dev, SERVO_CONTROLLER_POS0_OFFSET);
		position[i] = (pos * dev->data->K_phase_to_mea[i]) >> 15;
	}

	OS_EXIT_CRITICAL();

	return 0;
}

int servo_controller_set_duty_1channel(
	struct servo_controller_dev_t *dev,
	enum Servo_controller_servo_id_t chanel,
	int16_t duty)
{
	ALT_DEBUG_ASSERT((dev));
	ALT_DEBUG_ASSERT((chanel >= 0));
	ALT_DEBUG_ASSERT((chanel < SERVO_CONTROLLER_NUM_SERVO));

	OS_CPU_SR cpu_sr = 0;
	OS_ENTER_CRITICAL();

	const uint8_t offset[] = {
		SERVO_CONTROLLER_U0_OFFSET,
		SERVO_CONTROLLER_U1_OFFSET,
		SERVO_CONTROLLER_U2_OFFSET,
		SERVO_CONTROLLER_U3_OFFSET};

	SERVO_IOWR(dev, offset[chanel], -duty);

	SERVO_IOWR(dev, SERVO_CONTROLLER_TR_OFFSET,
			   SERVO_CONTROLLER_TR_U_VALID_BIT);

	OS_EXIT_CRITICAL();

	return 0;
}

int servo_controller_notify_duty_changed(
	struct servo_controller_dev_t *dev)
{
	ALT_DEBUG_ASSERT((dev));

	SERVO_IOWR(dev, SERVO_CONTROLLER_FLAG_OFFSET,
			   SERVO_CONTROLLER_FLAG_MEA_TRIG_BIT);

	return 0;
}
