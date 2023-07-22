#include "servo_controller.h"
#include "servo_controller_reg.h"

#include <ucos_ii.h>
#include <io.h>
#include <priv/alt_file.h>
#include <sys/alt_irq.h>
#include <sys/alt_dev.h>
#include <sys/alt_debug.h>

#define SERVO_IOWR(dev, reg, data) IOWR((uintptr_t)dev->BASE, reg, data)
#define SERVO_IORD(dev, reg) IORD((uintptr_t)dev->BASE, reg)

struct servo_controller_dev_t *servo_controller_open_dev(const char *name)
{
	extern alt_llist alt_dev_list;
	struct servo_controller_dev_t *dev = (struct servo_controller_dev_t *)alt_find_dev(name, &alt_dev_list);

	return dev;
}

int servo_controller_apply_configure(struct servo_controller_dev_t *dev)
{
	servo_controller_reg_CR crReg;
	uint32_t prescale;
	servo_controller_reg_IE ieReg;

	ALT_DEBUG_ASSERT((dev));

	ALT_DEBUG_ASSERT((dev->cfg->i_max[0] > 0));
	ALT_DEBUG_ASSERT((dev->cfg->i_max[1] > 0));
	ALT_DEBUG_ASSERT((dev->cfg->i_max[2] > 0));
	ALT_DEBUG_ASSERT((dev->cfg->i_max[3] > 0));

	crReg.val = 0;
	crReg.field.en = 0;
	crReg.field.filter_level = dev->cfg->filter_level;
	crReg.field.protected_0_en = dev->cfg->protected_en[0];
	crReg.field.protected_1_en = dev->cfg->protected_en[1];
	crReg.field.protected_2_en = dev->cfg->protected_en[2];
	crReg.field.protected_3_en = dev->cfg->protected_en[3];
	crReg.field.drv_0_en = dev->cfg->drv_en[0];
	crReg.field.drv_1_en = dev->cfg->drv_en[1];
	crReg.field.drv_2_en = dev->cfg->drv_en[2];
	crReg.field.drv_3_en = dev->cfg->drv_en[3];

	OS_CPU_SR cpu_sr = 0;
	OS_ENTER_CRITICAL();
	SERVO_IOWR(dev, SERVO_CONTROLLER_CR_OFFSET, crReg.val);

	prescale = dev->CORE_FREQ / dev->cfg->spi_speed - 1;
	SERVO_IOWR(dev, SERVO_CONTROLLER_SPI_PRES_OFFSET, prescale);

	prescale = dev->CORE_FREQ / dev->cfg->pwm_base_freq - 1;
	SERVO_IOWR(dev, SERVO_CONTROLLER_PWM_PRES_OFFSET, prescale);

	prescale = dev->CORE_FREQ / (dev->cfg->pwm_freq << 1) - 1;
	SERVO_IOWR(dev, SERVO_CONTROLLER_PWM_HPERIOD_OFFSET, prescale);

	SERVO_IOWR(dev, SERVO_CONTROLLER_PULSE_MODE0_OFFSET, dev->cfg->drv_mode[0]);
	SERVO_IOWR(dev, SERVO_CONTROLLER_PULSE_MODE1_OFFSET, dev->cfg->drv_mode[1]);
	SERVO_IOWR(dev, SERVO_CONTROLLER_PULSE_MODE2_OFFSET, dev->cfg->drv_mode[2]);
	SERVO_IOWR(dev, SERVO_CONTROLLER_PULSE_MODE3_OFFSET, dev->cfg->drv_mode[3]);

	SERVO_IOWR(dev, SERVO_CONTROLLER_I0_MAX_OFFSET, dev->cfg->i_max[0]);
	SERVO_IOWR(dev, SERVO_CONTROLLER_I1_MAX_OFFSET, dev->cfg->i_max[1]);
	SERVO_IOWR(dev, SERVO_CONTROLLER_I2_MAX_OFFSET, dev->cfg->i_max[2]);
	SERVO_IOWR(dev, SERVO_CONTROLLER_I3_MAX_OFFSET, dev->cfg->i_max[3]);

	// clear flag
	SERVO_IOWR(dev, SERVO_CONTROLLER_FLAG_OFFSET, 0xFFFFFFFFu);

	ieReg.val = 0;
	ieReg.field.adc_valid = dev->cfg->on_adc_valid ? 1 : 0;
	ieReg.field.mea_trig = dev->cfg->on_new_process ? 1 : 0;
	ieReg.field.realtime_err = dev->cfg->on_realtime_err ? 1 : 0;

	if (dev->cfg->on_stop_err)
	{
		ieReg.field.drv_0_stop = 1;
		ieReg.field.drv_1_stop = 1;
		ieReg.field.drv_2_stop = 1;
		ieReg.field.drv_3_stop = 1;
	}

	if (dev->cfg->on_drv_fault)
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
	OS_ENTER_CRITICAL();

	servo_controller_reg_CR crReg;
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

	OS_CPU_SR cpu_sr = 0;
	OS_ENTER_CRITICAL();

	SERVO_IOWR(dev, SERVO_CONTROLLER_U0_OFFSET, duty[0]);
	SERVO_IOWR(dev, SERVO_CONTROLLER_U1_OFFSET, duty[1]);
	SERVO_IOWR(dev, SERVO_CONTROLLER_U2_OFFSET, duty[2]);
	SERVO_IOWR(dev, SERVO_CONTROLLER_U3_OFFSET, duty[3]);

	SERVO_IOWR(dev, SERVO_CONTROLLER_TR_OFFSET,
			   SERVO_CONTROLLER_TR_U_VALID_BIT);
	OS_EXIT_CRITICAL();

	return 0;
}

int servo_controller_get_position(
	struct servo_controller_dev_t *dev,
	int16_t position[SERVO_CONTROLLER_NUM_SERVO])
{
	ALT_DEBUG_ASSERT((dev));
	ALT_DEBUG_ASSERT((position));

	OS_CPU_SR cpu_sr = 0;
	servo_controller_reg_I16 i16Reg;
	OS_ENTER_CRITICAL();

	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_POS0_OFFSET);
	position[0] = i16Reg.i16_val;
	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_POS1_OFFSET);
	position[1] = i16Reg.i16_val;
	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_POS2_OFFSET);
	position[2] = i16Reg.i16_val;
	i16Reg.u32_val = SERVO_IORD(dev, SERVO_CONTROLLER_POS3_OFFSET);
	position[3] = i16Reg.i16_val;

	OS_EXIT_CRITICAL();

	return 0;
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
		dev->cfg->on_new_process(dev, dev->cfg->on_new_process_arg);
	}

	if (flag.val & ~SERVO_CONTROLLER_FLAG_MEA_TRIG_BIT)
	{
		if (flag.val & SERVO_CONTROLLER_FLAG_ADC_VALID_BIT)
		{
			dev->cfg->on_adc_valid(dev, dev->cfg->on_adc_valid_arg);
			ie.field.adc_valid = 0;
			SERVO_IOWR(dev, SERVO_CONTROLLER_IE_OFFSET, ie.val);
		}

		if (flag.val & SERVO_CONTROLLER_FLAG_REALTIME_ERR_BIT)
		{
			dev->cfg->on_realtime_err(dev, dev->cfg->on_realtime_err_arg);
		}

		if (flag.val & SERVO_CONTROLLER_FLAG_STOP0_BIT)
		{
			dev->cfg->on_stop_err(dev, 0, dev->cfg->on_stop_err_arg);
		}

		if (flag.val & SERVO_CONTROLLER_FLAG_STOP1_BIT)
		{
			dev->cfg->on_stop_err(dev, 1, dev->cfg->on_stop_err_arg);
		}

		if (flag.val & SERVO_CONTROLLER_FLAG_STOP2_BIT)
		{
			dev->cfg->on_stop_err(dev, 2, dev->cfg->on_stop_err_arg);
		}

		if (flag.val & SERVO_CONTROLLER_FLAG_STOP3_BIT)
		{
			dev->cfg->on_stop_err(dev, 3, dev->cfg->on_stop_err_arg);
		}

		if (flag.val & SERVO_CONTROLLER_FLAG_DRV8320_FAULT0_BIT)
		{
			dev->cfg->on_drv_fault(dev, 0, dev->cfg->on_drv_fault_arg);
		}

		if (flag.val & SERVO_CONTROLLER_FLAG_DRV8320_FAULT1_BIT)
		{
			dev->cfg->on_drv_fault(dev, 1, dev->cfg->on_drv_fault_arg);
		}

		if (flag.val & SERVO_CONTROLLER_FLAG_DRV8320_FAULT2_BIT)
		{
			dev->cfg->on_drv_fault(dev, 2, dev->cfg->on_drv_fault_arg);
		}

		if (flag.val & SERVO_CONTROLLER_FLAG_DRV8320_FAULT3_BIT)
		{
			dev->cfg->on_drv_fault(dev, 3, dev->cfg->on_drv_fault_arg);
		}

		SERVO_IOWR(dev, SERVO_CONTROLLER_FLAG_OFFSET, 0xFFFFFFFFu);
	}
}

void servo_controller_init(struct servo_controller_dev_t *dev)
{
	SERVO_IOWR(dev, SERVO_CONTROLLER_CR_OFFSET, 0);

	SERVO_IOWR(dev, SERVO_CONTROLLER_TR_OFFSET, SERVO_CONTROLLER_TR_RESET_BIT);

	// disable irq
	SERVO_IOWR(dev, SERVO_CONTROLLER_IE_OFFSET, 0);

	// clear flag
	SERVO_IOWR(dev, SERVO_CONTROLLER_FLAG_OFFSET, 0xFFFFFFFFu);

	alt_ic_isr_register(dev->IC_ID, dev->IRQ, servo_controller_irq_handler, dev, 0);
	alt_dev_reg(&dev->dev);
}