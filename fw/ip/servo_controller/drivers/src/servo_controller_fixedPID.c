#include <stdint.h>
#include "FixedPID.h"


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

	self->A = (int32_t)(arg->E_lsb / arg->U_lsb) * (arg->kp + arg->ki * arg->dT + arg->kd / arg->dT);
	self->B = (int32_t) - (arg->E_lsb / arg->U_lsb) * (arg->kp - arg->ki * arg->dT + 2 * arg->kd / arg->dT);
	self->C = (int32_t)(arg->E_lsb / arg->U_lsb) * (arg->kd / arg->dT);

	// convert to fixed(32,16)
	self->A <<= 16;
	self->B <<= 16;
	self->C <<= 16;

	self->uk1 = 0;
	self->ek1 = 0;
	self->ek2 = 0;

	return 0;
}

int16_t PID_Step(struct FixedPID *self, int16_t sp, int16_t pv)
{
	int64_t uk;
	int16_t ek = sp - pv;

	uk =
		(int64_t)self->uk1 +
		(int64_t)self->A * ek +
		(int64_t)self->B * self->ek1 +
		(int64_t)self->C * self->ek2;

	self->ek2 = self->ek1;
	self->ek1 = ek;

	if (uk > INT32_MAX)
	{
		uk = INT32_MAX;
	}
	else if (uk < -INT32_MAX)
	{
		uk = -INT32_MAX;
	}

	self->uk1 = uk;

	return uk >> 16;
}
