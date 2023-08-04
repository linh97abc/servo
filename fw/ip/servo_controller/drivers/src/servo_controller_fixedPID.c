#include <stdint.h>
#include "FixedPID.h"
#include <errno.h>

#define INT32_ALMOST_MAX (INT32_MAX - 1)
#define INT16_ALMOST_MAX (INT16_MAX - 1)

int PID_Init(struct FixedPID *self, struct FixedPIDArgument *arg)
{
	if (arg->dT == 0)
	{
		return -EINVAL;
	}

	if (arg->E_lsb == 0)
	{
		return -EINVAL;
	}

	if (arg->U_lsb == 0)
	{
		return -EINVAL;
	}

	// Caculate Kp, Ki, Kd and convert to fixed(32,16)
	float a = UINT16_MAX * (arg->E_lsb / arg->U_lsb) * (arg->kp + arg->ki * arg->dT + arg->kd / arg->dT);
	float b = -UINT16_MAX * (arg->E_lsb / arg->U_lsb) * (arg->kp - arg->ki * arg->dT + 2 * arg->kd / arg->dT);
	float c = UINT16_MAX * (arg->E_lsb / arg->U_lsb) * (arg->kd / arg->dT);

	float spk1 = arg->sp0 / arg->E_lsb;
	float delta_sp_max = arg->ramp_rate * arg->dT / arg->E_lsb;

	// check overflow
	if ((a > INT32_ALMOST_MAX) || (a < -INT32_ALMOST_MAX))
	{
		return -EINVAL;
	}

	if ((b > INT32_ALMOST_MAX) || (b < -INT32_ALMOST_MAX))
	{
		return -EINVAL;
	}

	if ((c > INT32_ALMOST_MAX) || (c < -INT32_ALMOST_MAX))
	{
		return -EINVAL;
	}

	if ((spk1 > INT16_ALMOST_MAX) || (spk1 < -INT16_ALMOST_MAX))
	{
		return -EINVAL;
	}

	if ((delta_sp_max > INT16_ALMOST_MAX) || (delta_sp_max < -INT16_ALMOST_MAX))
	{
		return -EINVAL;
	}

	self->A = (int32_t)a;
	self->B = (int32_t)b;
	self->C = (int32_t)c;

	self->spk1 = (int16_t)spk1;
	self->delta_sp_max = (int16_t)delta_sp_max;

	self->uk1 = 0;
	self->hek1 = 0;
	self->hek2 = 0;

	return 0;
}

int16_t PID_Step(struct FixedPID *self, int16_t sp, int16_t pv)
{
	int64_t uk;
	int16_t hek;
	int32_t spk = self->spk1;

	if (sp == spk)
	{
		// do nothing
	}
	else if (sp > spk)
	{
		spk += self->delta_sp_max;
		if (spk > sp)
		{
			spk = sp;
		}
	}
	else
	{
		spk -= self->delta_sp_max;
		if (spk < sp)
		{
			spk = sp;
		}
	}

	hek = (int16_t)((spk - pv) >> 1);

	uk =
		(int64_t)self->uk1 +
		(int64_t)self->A * hek +
		(int64_t)self->B * self->hek1 +
		(int64_t)self->C * self->hek2;

	if (uk > INT32_MAX)
	{
		uk = INT32_MAX;
	}
	else if (uk < -INT32_MAX)
	{
		uk = -INT32_MAX;
	}

	self->hek2 = self->hek1;
	self->hek1 = hek;
	self->uk1 = uk;
	self->spk1 = (int16_t)spk;

	return uk >> 15;
}
