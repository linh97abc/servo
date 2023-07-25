#include "avl_drv8320.h"
#include "LL/avl_drv8320_reg.h"

#include <ucos_ii.h>
#include <io.h>
#include <priv/alt_file.h>
#include <sys/alt_irq.h>
#include <sys/alt_dev.h>
#include <sys/alt_debug.h>

#define SERVO_IOWR(dev, reg, data) IOWR((uintptr_t)dev->BASE, reg, data)
#define SERVO_IORD(dev, reg) IORD((uintptr_t)dev->BASE, reg)

struct Drv8320_dev_t *Drv8320_open_dev(const char *name)
{
	extern alt_llist alt_dev_list;
	struct Drv8320_dev_t *dev = (struct Drv8320_dev_t *)alt_find_dev(name, &alt_dev_list);

	return dev;
}

int Drv8320_apply_configure(struct Drv8320_dev_t *dev)
{
	Drv8320_reg_CR crReg;
	uint32_t prescale;
	Drv8320_reg_IE ieReg;

	ALT_DEBUG_ASSERT((dev));

	crReg.val = 0;
	crReg.field.en = 0;

	OS_CPU_SR cpu_sr = 0;
	OS_ENTER_CRITICAL();
	SERVO_IOWR(dev, DRV8320_CR_OFFSET, crReg.val);

	prescale = dev->CORE_FREQ / dev->cfg->pwm_base_freq - 1;
	SERVO_IOWR(dev, DRV8320_PWM_PRES_OFFSET, prescale);

	prescale = dev->CORE_FREQ / (dev->cfg->pwm_freq << 1) - 1;
	SERVO_IOWR(dev, DRV8320_PWM_HPERIOD_OFFSET, prescale);

	SERVO_IOWR(dev, DRV8320_PULSE_MODE0_OFFSET, dev->cfg->drv_mode[0]);
	SERVO_IOWR(dev, DRV8320_PULSE_MODE1_OFFSET, dev->cfg->drv_mode[1]);
	SERVO_IOWR(dev, DRV8320_PULSE_MODE2_OFFSET, dev->cfg->drv_mode[2]);
	SERVO_IOWR(dev, DRV8320_PULSE_MODE3_OFFSET, dev->cfg->drv_mode[3]);


	// clear flag
	SERVO_IOWR(dev, DRV8320_FLAG_OFFSET, 0xFFFFFFFFu);

	ieReg.val = 0;
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

	SERVO_IOWR(dev, DRV8320_IE_OFFSET, ieReg.val);

	SERVO_IOWR(dev, DRV8320_TR_OFFSET, DRV8320_TR_RESET_BIT);

	crReg.field.en = 1;
	SERVO_IOWR(dev, DRV8320_CR_OFFSET, crReg.val);

	OS_EXIT_CRITICAL();

	return 0;
}

int Drv8320_start(struct Drv8320_dev_t *dev)
{
	ALT_DEBUG_ASSERT((dev));

	OS_CPU_SR cpu_sr = 0;
	OS_ENTER_CRITICAL();

	Drv8320_reg_CR crReg;
	crReg.field.en = 1;
	crReg.field.drv_0_en = 1;
	crReg.field.drv_1_en = 1;
	crReg.field.drv_2_en = 1;
	crReg.field.drv_3_en = 1;

	SERVO_IOWR(dev, DRV8320_CR_OFFSET, crReg.val);

	OS_EXIT_CRITICAL();

	return 0;
}

int Drv8320_stop(struct Drv8320_dev_t *dev)
{
	ALT_DEBUG_ASSERT((dev));

	OS_CPU_SR cpu_sr = 0;
	OS_ENTER_CRITICAL();

	Drv8320_reg_CR crReg;
	crReg.val = 0;
	crReg.field.en = 1;

	SERVO_IOWR(dev, DRV8320_CR_OFFSET, crReg.val);

	OS_EXIT_CRITICAL();

	return 0;
}

int Drv8320_update_duty(
	struct Drv8320_dev_t *dev,
	int16_t duty[DRV8320_NUM_CHANNEL])
{
	ALT_DEBUG_ASSERT((dev));
	ALT_DEBUG_ASSERT((duty));

	OS_CPU_SR cpu_sr = 0;
	OS_ENTER_CRITICAL();

	SERVO_IOWR(dev, DRV8320_U0_OFFSET, duty[0]);
	SERVO_IOWR(dev, DRV8320_U1_OFFSET, duty[1]);
	SERVO_IOWR(dev, DRV8320_U2_OFFSET, duty[2]);
	SERVO_IOWR(dev, DRV8320_U3_OFFSET, duty[3]);

	SERVO_IOWR(dev, DRV8320_TR_OFFSET,
			   DRV8320_TR_U_VALID_BIT);
	OS_EXIT_CRITICAL();

	return 0;
}



int Drv8320_get_duty(
	struct Drv8320_dev_t *dev,
	int16_t duty[DRV8320_NUM_CHANNEL])
{
	ALT_DEBUG_ASSERT((dev));
	ALT_DEBUG_ASSERT((duty));

	OS_CPU_SR cpu_sr = 0;
	Drv8320_reg_I16 i16Reg;
	OS_ENTER_CRITICAL();

	i16Reg.u32_val = SERVO_IORD(dev, DRV8320_U0_OFFSET);
	duty[0] = i16Reg.i16_val;
	i16Reg.u32_val = SERVO_IORD(dev, DRV8320_U1_OFFSET);
	duty[1] = i16Reg.i16_val;
	i16Reg.u32_val = SERVO_IORD(dev, DRV8320_U2_OFFSET);
	duty[2] = i16Reg.i16_val;
	i16Reg.u32_val = SERVO_IORD(dev, DRV8320_U3_OFFSET);
	duty[3] = i16Reg.i16_val;

	OS_EXIT_CRITICAL();

	return 0;
}

static void Drv8320_irq_handler(void *arg)
{
	Drv8320_reg_FLAG flag;
	Drv8320_reg_IE ie;
	struct Drv8320_dev_t *dev = (struct Drv8320_dev_t *)arg;

	ie.val = SERVO_IORD(dev, DRV8320_IE_OFFSET);
	flag.val = SERVO_IORD(dev, DRV8320_FLAG_OFFSET);

	flag.val &= ie.val;

	if (flag.val & DRV8320_FLAG_MEA_TRIG_BIT)
	{
		dev->cfg->on_new_process(dev, dev->cfg->on_new_process_arg);
	}

	if (flag.val & ~DRV8320_FLAG_MEA_TRIG_BIT)
	{

		if (flag.val & DRV8320_FLAG_REALTIME_ERR_BIT)
		{
			dev->cfg->on_realtime_err(dev, dev->cfg->on_realtime_err_arg);
		}

		if (flag.val & DRV8320_FLAG_STOP0_BIT)
		{
			dev->cfg->on_stop_err(dev, 0, dev->cfg->on_stop_err_arg);
		}

		if (flag.val & DRV8320_FLAG_STOP1_BIT)
		{
			dev->cfg->on_stop_err(dev, 1, dev->cfg->on_stop_err_arg);
		}

		if (flag.val & DRV8320_FLAG_STOP2_BIT)
		{
			dev->cfg->on_stop_err(dev, 2, dev->cfg->on_stop_err_arg);
		}

		if (flag.val & DRV8320_FLAG_STOP3_BIT)
		{
			dev->cfg->on_stop_err(dev, 3, dev->cfg->on_stop_err_arg);
		}

		if (flag.val & DRV8320_FLAG_DRV8320_FAULT0_BIT)
		{
			dev->cfg->on_drv_fault(dev, 0, dev->cfg->on_drv_fault_arg);
		}

		if (flag.val & DRV8320_FLAG_DRV8320_FAULT1_BIT)
		{
			dev->cfg->on_drv_fault(dev, 1, dev->cfg->on_drv_fault_arg);
		}

		if (flag.val & DRV8320_FLAG_DRV8320_FAULT2_BIT)
		{
			dev->cfg->on_drv_fault(dev, 2, dev->cfg->on_drv_fault_arg);
		}

		if (flag.val & DRV8320_FLAG_DRV8320_FAULT3_BIT)
		{
			dev->cfg->on_drv_fault(dev, 3, dev->cfg->on_drv_fault_arg);
		}

		SERVO_IOWR(dev, DRV8320_FLAG_OFFSET, 0xFFFFFFFFu);
	}
}

void Drv8320_init(struct Drv8320_dev_t *dev)
{
	SERVO_IOWR(dev, DRV8320_CR_OFFSET, 0);

	SERVO_IOWR(dev, DRV8320_TR_OFFSET, DRV8320_TR_RESET_BIT);

	// disable irq
	SERVO_IOWR(dev, DRV8320_IE_OFFSET, 0);

	// clear flag
	SERVO_IOWR(dev, DRV8320_FLAG_OFFSET, 0xFFFFFFFFu);

	alt_ic_isr_register(dev->IC_ID, dev->IRQ, Drv8320_irq_handler, dev, 0);
	alt_dev_reg(&dev->dev);
}