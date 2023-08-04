#include <stdint.h>
#include "FixedPID.h"
#include <errno.h>

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

	int64_t a64 = (int64_t)a;
	int64_t b64 = (int64_t)b;
	int64_t c64 = (int64_t)c;

	// check overflow
	if ((a64 > INT32_MAX) || (a64 < -INT32_MAX))
	{
		return -EINVAL;
	}

	if ((b64 > INT32_MAX) || (b64 < -INT32_MAX))
	{
		return -EINVAL;
	}

	if ((c64 > INT32_MAX) || (c64 < -INT32_MAX))
	{
		return -EINVAL;
	}

	self->A = (int32_t)a64;
	self->B = (int32_t)b64;
	self->C = (int32_t)c64;

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
