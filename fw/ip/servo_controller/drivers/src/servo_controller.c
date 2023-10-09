#include "servo_controller.h"
#include "LL/servo_controller_reg.h"
#include <string.h>
#include <ucos_ii.h>
#include <io.h>
#include <priv/alt_file.h>
#include <sys/alt_irq.h>
#include <sys/alt_dev.h>
#include <sys/alt_debug.h>
#include <system.h>

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

int servo_controller_update_pos_phase(
	struct servo_controller_dev_t *dev,
	int32_t pos_phase[SERVO_CONTROLLER_NUM_SERVO])
{
	ALT_DEBUG_ASSERT((dev != NULL));
	ALT_DEBUG_ASSERT((pos_phase != NULL));

	unsigned i;
	for (i = 0; i < SERVO_CONTROLLER_NUM_SERVO; i++)
	{
		SERVO_IOWR(dev, SERVO_CONTROLLER_POS_PHASE0_OFFSET + i, pos_phase[i]);
	}

	return 0;
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
			   SERVO_CONTROLLER_FLAG_CTRL_STEP_PENDING_BIT);

	return 0;
}

struct servo_controller_dev_t *servo_controller_open_dev(const char *name)
{
	extern alt_llist alt_dev_list;
	struct servo_controller_dev_t *dev = (struct servo_controller_dev_t *)alt_find_dev(name, &alt_dev_list);

	return dev;
}

int servo_controller_apply_configure(struct servo_controller_dev_t *dev)
{
	ALT_DEBUG_ASSERT((dev));

	servo_controller_reg_CR crReg;
	uint32_t prescale;
	struct servo_controller_config_t *cfg = dev->cfg;

	if (!cfg->spi_speed)
		return -EINVAL;
	if (!cfg->pwm_base_freq)
		return -EINVAL;
	if (!cfg->pwm_freq)
		return -EINVAL;

	crReg.val = 0;
	crReg.field.en = 0;
	crReg.field.filter_level = cfg->filter_level;
	crReg.field.protected_0_en = cfg->protected_en[0];
	crReg.field.protected_1_en = cfg->protected_en[1];
	crReg.field.protected_2_en = cfg->protected_en[2];
	crReg.field.protected_3_en = cfg->protected_en[3];

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

	SERVO_IOWR(dev, SERVO_CONTROLLER_U0_OFFSET, 0);
	SERVO_IOWR(dev, SERVO_CONTROLLER_U1_OFFSET, 0);
	SERVO_IOWR(dev, SERVO_CONTROLLER_U2_OFFSET, 0);
	SERVO_IOWR(dev, SERVO_CONTROLLER_U3_OFFSET, 0);

	// clear flag
	SERVO_IOWR(dev, SERVO_CONTROLLER_FLAG_OFFSET, 0xFFFFFFFFu);
	SERVO_IOWR(dev, SERVO_CONTROLLER_IE_OFFSET, 0);
	SERVO_IOWR(dev, SERVO_CONTROLLER_TR_OFFSET,
			   SERVO_CONTROLLER_TR_RESET_BIT);

	crReg.field.en = 1;
	SERVO_IOWR(dev, SERVO_CONTROLLER_CR_OFFSET, crReg.val);
	SERVO_IOWR(dev, SERVO_CONTROLLER_TR_OFFSET,
			   SERVO_CONTROLLER_TR_U_VALID_BIT |
				   SERVO_CONTROLLER_TR_ADC_INIT_BIT);

	OS_EXIT_CRITICAL();
	OSTimeDly(TIMER_TICKS_PER_SEC / 100);

	return 0;
}

int servo_controller_start(struct servo_controller_dev_t *dev)
{
	ALT_DEBUG_ASSERT((dev));

	int16_t duty[SERVO_CONTROLLER_NUM_SERVO] = {0, 0, 0, 0};
	int ret;
//	servo_controller_reg_IE ie;
	servo_controller_reg_CR crReg;

	ret = servo_controller_update_duty_in_critical(dev, duty);
	if (ret)
	{
		return ret;
	}

	crReg.val = SERVO_IORD(dev, SERVO_CONTROLLER_CR_OFFSET);
	crReg.field.en = 1;
	crReg.field.drv_0_en = 0;
	crReg.field.drv_1_en = 0;
	crReg.field.drv_2_en = 0;
	crReg.field.drv_3_en = 0;
	SERVO_IOWR(dev, SERVO_CONTROLLER_CR_OFFSET, crReg.val);

	OSTimeDly(1);

	crReg.field.drv_0_en = dev->cfg->drv_en[0];
	crReg.field.drv_1_en = dev->cfg->drv_en[1];
	crReg.field.drv_2_en = dev->cfg->drv_en[2];
	crReg.field.drv_3_en = dev->cfg->drv_en[3];
	SERVO_IOWR(dev, SERVO_CONTROLLER_CR_OFFSET, crReg.val);

	SERVO_IOWR(dev, SERVO_CONTROLLER_TR_OFFSET,
			   SERVO_CONTROLLER_TR_START_SERVO3_BIT |
				   SERVO_CONTROLLER_TR_START_SERVO2_BIT |
				   SERVO_CONTROLLER_TR_START_SERVO1_BIT |
				   SERVO_CONTROLLER_TR_START_SERVO0_BIT);

	return 0;
}

int servo_controller_restart_channel(
	struct servo_controller_dev_t *dev,
	enum Servo_controller_servo_id_t channel)
{
	ALT_DEBUG_ASSERT((dev));
	ALT_DEBUG_ASSERT((channel >= 0));
	ALT_DEBUG_ASSERT((channel < SERVO_CONTROLLER_NUM_SERVO));

	uint32_t cr_val = SERVO_IORD(dev, SERVO_CONTROLLER_CR_OFFSET);

	SERVO_IOWR(dev, SERVO_CONTROLLER_CR_OFFSET, cr_val & ~BIT(SERVO_CONTROLLER_CR_DRV_EN0_BIT_INDEX + channel));
	OSTimeDly(1);
	SERVO_IOWR(dev, SERVO_CONTROLLER_CR_OFFSET, cr_val);
	SERVO_IOWR(dev, SERVO_CONTROLLER_TR_OFFSET, BIT(SERVO_CONTROLLER_TR_START_SERVO0_BIT_INDEX + channel));

	return 0;
}

int servo_controller_stop(struct servo_controller_dev_t *dev)
{
	ALT_DEBUG_ASSERT((dev));

	OS_CPU_SR cpu_sr = 0;
	OS_ENTER_CRITICAL();

	servo_controller_reg_CR crReg;
	crReg.val = SERVO_IORD(dev, SERVO_CONTROLLER_CR_OFFSET);
	crReg.field.drv_0_en = 0;
	crReg.field.drv_1_en = 0;
	crReg.field.drv_2_en = 0;
	crReg.field.drv_3_en = 0;
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

	uint32_t flag = SERVO_IORD(dev, SERVO_CONTROLLER_FLAG_OFFSET);

	if (!(flag & SERVO_CONTROLLER_FLAG_ADC_VALID_BIT))
	{
		return -ENODATA;
	}

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

	uint32_t flag = SERVO_IORD(dev, SERVO_CONTROLLER_FLAG_OFFSET);

	if (!(flag & SERVO_CONTROLLER_FLAG_ADC_VALID_BIT))
	{
		return -ENODATA;
	}

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
static void servo_controller_check_and_handle_err(
	struct servo_controller_dev_t *dev,
	uint32_t flag,
	uint32_t ieVal) __attribute__((section(".exceptions")));

static void servo_controller_check_and_handle_err(
	struct servo_controller_dev_t *dev,
	uint32_t flag,
	uint32_t ieVal)
{
	servo_controller_reg_IE ie;
	struct servo_controller_config_t *cfg = dev->cfg;
	ie.val = ieVal;

	if (flag & SERVO_CONTROLLER_FLAG_REALTIME_ERR_BIT)
	{
		if (cfg->on_err)
		{
			cfg->on_err(dev, 0, SERVO_CTRL_REALTIME_ERR);
		}

		SERVO_IOWR(dev, SERVO_CONTROLLER_FLAG_OFFSET,
				   SERVO_CONTROLLER_FLAG_CTRL_STEP_PENDING_BIT |
					   SERVO_CONTROLLER_FLAG_REALTIME_ERR_BIT);
	}

	if (flag & SERVO_CONTROLLER_FLAG_STOP0_BIT)
	{
		if (cfg->on_err)
		{
			cfg->on_err(dev, 0, SERVO_CTRL_OVERLOAD_ERR);
		}
		ie.field.drv_0_stop = 0;
	}

	if (flag & SERVO_CONTROLLER_FLAG_STOP1_BIT)
	{
		if (cfg->on_err)
		{
			cfg->on_err(dev, 1, SERVO_CTRL_OVERLOAD_ERR);
		}
		ie.field.drv_1_stop = 0;
	}

	if (flag & SERVO_CONTROLLER_FLAG_STOP2_BIT)
	{
		if (cfg->on_err)
		{
			cfg->on_err(dev, 2, SERVO_CTRL_OVERLOAD_ERR);
		}
		ie.field.drv_2_stop = 0;
	}

	if (flag & SERVO_CONTROLLER_FLAG_STOP3_BIT)
	{
		if (cfg->on_err)
		{
			cfg->on_err(dev, 3, SERVO_CTRL_OVERLOAD_ERR);
		}
		ie.field.drv_3_stop = 0;
	}

	if (flag & SERVO_CONTROLLER_FLAG_DRV8320_FAULT0_BIT)
	{
		if (cfg->on_err)
		{
			cfg->on_err(dev, 0, SERVO_CTRL_DRV_ERR);
		}
		ie.field.drv_0_fault = 0;
	}

	if (flag & SERVO_CONTROLLER_FLAG_DRV8320_FAULT1_BIT)
	{
		if (cfg->on_err)
		{
			cfg->on_err(dev, 1, SERVO_CTRL_DRV_ERR);
		}
		ie.field.drv_1_fault = 0;
	}

	if (flag & SERVO_CONTROLLER_FLAG_DRV8320_FAULT2_BIT)
	{
		if (cfg->on_err)
		{
			cfg->on_err(dev, 2, SERVO_CTRL_DRV_ERR);
		}
		ie.field.drv_2_fault = 0;
	}

	if (flag & SERVO_CONTROLLER_FLAG_DRV8320_FAULT3_BIT)
	{
		if (cfg->on_err)
		{
			cfg->on_err(dev, 3, SERVO_CTRL_DRV_ERR);
		}
		ie.field.drv_3_fault = 0;
	}

	if (flag & SERVO_CONTROLLER_FLAG_HALL_ERR0_BIT)
	{
		if (cfg->on_err)
		{
			cfg->on_err(dev, 0, SERVO_CTRL_HALL_ERR);
		}
		ie.field.hall_0_err = 0;
	}

	if (flag & SERVO_CONTROLLER_FLAG_HALL_ERR1_BIT)
	{
		if (cfg->on_err)
		{
			cfg->on_err(dev, 1, SERVO_CTRL_HALL_ERR);
		}
		ie.field.hall_1_err = 0;
	}

	if (flag & SERVO_CONTROLLER_FLAG_HALL_ERR2_BIT)
	{
		if (cfg->on_err)
		{
			cfg->on_err(dev, 2, SERVO_CTRL_HALL_ERR);
		}
		ie.field.hall_2_err = 0;
	}

	if (flag & SERVO_CONTROLLER_FLAG_HALL_ERR3_BIT)
	{
		if (cfg->on_err)
		{
			cfg->on_err(dev, 3, SERVO_CTRL_HALL_ERR);
		}
		ie.field.hall_3_err = 0;
	}

	SERVO_IOWR(dev, SERVO_CONTROLLER_IE_OFFSET, ie.val);
}

static void servo_controller_irq_handler(void *arg)
{
	servo_controller_reg_FLAG flag;
	servo_controller_reg_IE ie;
	struct servo_controller_dev_t *dev = (struct servo_controller_dev_t *)arg;

	ie.val = SERVO_IORD(dev, SERVO_CONTROLLER_IE_OFFSET);
	flag.val = SERVO_IORD(dev, SERVO_CONTROLLER_FLAG_OFFSET);
	flag.val &= ie.val;

	if (flag.val & SERVO_CONTROLLER_FLAG_MEA_TRIG_BIT)
	{
		if (dev->cfg->on_new_process)
		{
			dev->cfg->on_new_process(dev);
		}

		SERVO_IOWR(dev, SERVO_CONTROLLER_FLAG_OFFSET,
				   SERVO_CONTROLLER_FLAG_MEA_TRIG_BIT);
	}

	if (flag.val & ~SERVO_CONTROLLER_FLAG_MEA_TRIG_BIT)
	{
		servo_controller_check_and_handle_err(dev, flag.val, ie.val);
	}
}

static void set_default_config(struct servo_controller_config_t *cfg)
{
	unsigned i;

	for (i = 0; i < SERVO_CONTROLLER_NUM_SERVO; i++)
	{
		cfg->i_max[i] = 2048;
	}

	cfg->spi_speed = 1000000;
	cfg->pwm_base_freq = 5000000;
	cfg->pwm_freq = 5000;
	cfg->pwm_trig_rate = 1;
}

void servo_controller_init(struct servo_controller_dev_t *dev)
{
	SERVO_IOWR(dev, SERVO_CONTROLLER_CR_OFFSET, 0);

	SERVO_IOWR(dev, SERVO_CONTROLLER_TR_OFFSET, SERVO_CONTROLLER_TR_RESET_BIT);

	// disable irq
	SERVO_IOWR(dev, SERVO_CONTROLLER_IE_OFFSET, 0);

	// clear flag
	SERVO_IOWR(dev, SERVO_CONTROLLER_FLAG_OFFSET, 0xFFFFFFFFu);

	set_default_config(dev->cfg);
	alt_ic_isr_register(dev->IC_ID, dev->IRQ, servo_controller_irq_handler, dev, 0);
	alt_dev_reg(&dev->dev);
}

int servo_controller_get_phase_position(
	struct servo_controller_dev_t *dev,
	int32_t position[SERVO_CONTROLLER_NUM_SERVO])
{
	ALT_DEBUG_ASSERT((dev));
	ALT_DEBUG_ASSERT((position));

	OS_CPU_SR cpu_sr = 0;
	unsigned i;
	OS_ENTER_CRITICAL();

	for (i = 0; i < SERVO_CONTROLLER_NUM_SERVO; i++)
	{
		position[i] = SERVO_IORD(dev, SERVO_CONTROLLER_POS_PHASE0_OFFSET + i);
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
			   SERVO_CONTROLLER_FLAG_CTRL_STEP_PENDING_BIT);

	return 0;
}

int servo_controller_get_position_channel(
	struct servo_controller_dev_t *dev,
	enum Servo_controller_servo_id_t channel,
	int16_t *position)
{
	ALT_DEBUG_ASSERT((dev));
	ALT_DEBUG_ASSERT((channel >= 0));
	ALT_DEBUG_ASSERT((channel < SERVO_CONTROLLER_NUM_SERVO));
	ALT_DEBUG_ASSERT((position));

	uint32_t flag = SERVO_IORD(dev, SERVO_CONTROLLER_FLAG_OFFSET);

	if (!(flag & SERVO_CONTROLLER_FLAG_ADC_VALID_BIT))
	{
		return -ENODATA;
	}

	servo_controller_reg_I16 i16Reg;
	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_POS0_OFFSET + channel);
	*position = i16Reg.i16_val;

	return 0;
}

int servo_controller_update_pos_phase_channel(
	struct servo_controller_dev_t *dev,
	enum Servo_controller_servo_id_t channel,
	int32_t pos_phase)
{
	ALT_DEBUG_ASSERT((dev));
	ALT_DEBUG_ASSERT((channel >= 0));
	ALT_DEBUG_ASSERT((channel < SERVO_CONTROLLER_NUM_SERVO));

	SERVO_IOWR(dev,
			   SERVO_CONTROLLER_POS_PHASE0_OFFSET + channel,
			   pos_phase);

	return 0;
}

int servo_controller_get_phase_position_channel(
	struct servo_controller_dev_t *dev,
	enum Servo_controller_servo_id_t channel,
	int32_t *pos_phase)
{
	ALT_DEBUG_ASSERT((dev));
	ALT_DEBUG_ASSERT((channel >= 0));
	ALT_DEBUG_ASSERT((channel < SERVO_CONTROLLER_NUM_SERVO));
	ALT_DEBUG_ASSERT((pos_phase));

	*pos_phase = SERVO_IORD(dev, SERVO_CONTROLLER_POS_PHASE0_OFFSET + channel);

	return 0;
}

int servo_controller_get_current_channel(
	struct servo_controller_dev_t *dev,
	enum Servo_controller_servo_id_t channel,
	int16_t *current)
{
	ALT_DEBUG_ASSERT((dev));
	ALT_DEBUG_ASSERT((channel >= 0));
	ALT_DEBUG_ASSERT((channel < SERVO_CONTROLLER_NUM_SERVO));
	ALT_DEBUG_ASSERT((current));

	uint32_t flag = SERVO_IORD(dev, SERVO_CONTROLLER_FLAG_OFFSET);

	if (!(flag & SERVO_CONTROLLER_FLAG_ADC_VALID_BIT))
	{
		return -ENODATA;
	}

	servo_controller_reg_I16 i16Reg;
	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_I0_OFFSET + channel);
	*current = i16Reg.i16_val;

	return 0;
}

int servo_controller_get_duty_channel(
	struct servo_controller_dev_t *dev,
	enum Servo_controller_servo_id_t channel,
	int16_t *duty)
{
	ALT_DEBUG_ASSERT((dev));
	ALT_DEBUG_ASSERT((channel >= 0));
	ALT_DEBUG_ASSERT((channel < SERVO_CONTROLLER_NUM_SERVO));
	ALT_DEBUG_ASSERT((duty));

	servo_controller_reg_I16 i16Reg;

	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_U0_OFFSET + channel);
	*duty = i16Reg.i16_val;

	return 0;
}

void servo_controller_enable_interrupt(
	struct servo_controller_dev_t *dev,
	uint32_t mask)
{
	ALT_DEBUG_ASSERT((dev));

	servo_controller_reg_IE ie;

	OS_CPU_SR cpu_sr = 0;
	OS_ENTER_CRITICAL();

	ie.val = SERVO_IORD(dev, SERVO_CONTROLLER_IE_OFFSET);
	ie.val |= mask;
	SERVO_IOWR(dev, SERVO_CONTROLLER_IE_OFFSET, ie.val);

	OS_EXIT_CRITICAL();
}

void servo_controller_disable_interrupt(
	struct servo_controller_dev_t *dev,
	uint32_t mask)
{
	ALT_DEBUG_ASSERT((dev));

	servo_controller_reg_IE ie;

	OS_CPU_SR cpu_sr = 0;
	OS_ENTER_CRITICAL();

	ie.val = SERVO_IORD(dev, SERVO_CONTROLLER_IE_OFFSET);
	ie.val &= ~mask;
	SERVO_IOWR(dev, SERVO_CONTROLLER_IE_OFFSET, ie.val);

	OS_EXIT_CRITICAL();
}
