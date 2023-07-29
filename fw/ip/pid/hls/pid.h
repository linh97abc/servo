#ifndef __PID_H__
#define __PID_H__

#include <ap_fixed.h>
#include <ap_int.h>

#define C_WIDTH 32
#define D_WIDTH 16
#define DUTY_WIDTH 12
#define U_MAX_NEG 0

using C_type = ap_fixed<C_WIDTH, 16> ;
//using D_type = ap_fixed<D_WIDTH, D_WIDTH> ;
using D_type = ap_int<D_WIDTH> ;
using U_type = ap_fixed<C_WIDTH+D_WIDTH, 16> ;
using Duty_type = ap_int<DUTY_WIDTH>;

U_type pid_pipeline(
//		D_type umin_pos,
//		D_type umin_neg,
		Duty_type umax_pos,
//		D_type umax_neg,

		C_type c0, C_type c1, C_type c2,

		D_type e0, D_type e1, D_type e2,
		U_type u1
		);

#endif
