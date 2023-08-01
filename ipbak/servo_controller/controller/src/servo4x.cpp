#include "pid.h"

struct servo_arg_t {
//	D_type umin_pos;
//	D_type umin_neg;
	D_type umax_pos;
//	D_type umax_neg;

	C_type c0;
	C_type c1;
	C_type c2;

	D_type sp;
	D_type pv;
};


struct servo4x_arg_t {
	struct servo_arg_t s[4];
};

struct servo_return_t {
	Duty_type u[4];
};

static void servo1x(
		struct servo_arg_t &s,
		D_type &e1, D_type &e2, U_type &u1);

servo_return_t servo4x(
		struct servo_arg_t &s0,
		struct servo_arg_t &s1,
		struct servo_arg_t &s2,
		struct servo_arg_t &s3

//		struct servo4x_arg_t s
)
{
#pragma HLS INTERFACE mode=ap_none port=s0
#pragma HLS INTERFACE mode=ap_none port=s1
#pragma HLS INTERFACE mode=ap_none port=s2
#pragma HLS INTERFACE mode=ap_none port=s3
//#pragma HLS INTERFACE mode=ap_none port=s
#pragma HLS INTERFACE mode=ap_none port=return

#pragma HLS PIPELINE off

	struct servo_arg_t s[4];

	static D_type e1[4];
	static D_type e2[4];
	static U_type u1[4];

#pragma HLS reset variable=e1
#pragma HLS reset variable=e2
#pragma HLS reset variable=u1

	s[0] = s0;
	s[1] = s1;
	s[2] = s2;
	s[3] = s3;

	servo_1x_loop: for (unsigned i = 0; i < 4; i++) {
		servo1x(s[i], e1[i], e2[i], u1[i]);
	}

	servo_return_t ret = {u1[0], u1[1], u1[2], u1[3]};

	return ret;

}


static void servo1x(
		struct servo_arg_t &s,
		D_type &e1, D_type &e2, U_type &u1)
{
	D_type e0 = s.sp - s.pv;
	U_type u1_next = pid_pipeline(
//					s.umin_pos, s.umin_neg,
					s.umax_pos,
//					s.umax_neg,
					s.c0, s.c1, s.c2,
					e0, e1, e2, u1);
	u1 = u1_next;
	e2 = e1;
	e1 = e0;
}
