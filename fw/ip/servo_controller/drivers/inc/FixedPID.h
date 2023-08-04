#ifndef __FIXEDPID_H__
#define __FIXEDPID_H__

#include <stdint.h>
#include <errno.h>

struct FixedPID
{
	// fixed(32,16)
	int32_t A;
	int32_t B;
	int32_t C;

	int32_t uk1;

	int16_t delta_sp_max;
	int16_t spk1;

	int16_t hek1;
	int16_t hek2;
};

struct FixedPIDArgument
{
	float kp;
	float ki;
	float kd;
	float dT;
	float U_lsb; // u = u_fix(16,0) * U_lsb; control value
	float E_lsb; // e = e_fix(16,0) * E_lsb; error value
	float ramp_rate; // dsp/dt max
	float sp0; // Setpoint initial value
};

int PID_Init(struct FixedPID *self, struct FixedPIDArgument *arg);

int16_t PID_Step(struct FixedPID *self, int16_t sp, int16_t pv) __attribute__((section(".exceptions")));

#endif // __FIXEDPID_H__