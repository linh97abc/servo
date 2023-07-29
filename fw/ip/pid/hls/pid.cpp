#include "pid.h"

U_type
//servo4x
pid_pipeline
(
//		D_type umin_pos,
//		D_type umin_neg,
		Duty_type umax_pos,
//		D_type umax_neg,

		C_type c0, C_type c1, C_type c2,

		D_type e0, D_type e1, D_type e2,
		U_type u1
		) {

#pragma HLS INTERFACE mode=ap_none port=c0
#pragma HLS INTERFACE mode=ap_none port=c1
#pragma HLS INTERFACE mode=ap_none port=c2
#pragma HLS INTERFACE mode=ap_none port=e0
#pragma HLS INTERFACE mode=ap_none port=e1
#pragma HLS INTERFACE mode=ap_none port=e2
#pragma HLS INTERFACE mode=ap_none port=u1
#pragma HLS INTERFACE mode=ap_none port=return
//#pragma HLS PIPELINE II=1
#pragma HLS LATENCY min=8



//	U_type c0e0 = c0*e0;
//	U_type c1e1 = c1*e1;
//	U_type c2e2 = c2*e2;
//
//	U_type u_part1 = u1 + c0e0;
//	U_type u_part2 = c1e1 + c2e2;
//
//	U_type u0 = u_part1 + u_part2;
	U_type u0 = u1 + c0*e0 + c1*e1 + c2*e2;
	U_type u_ret = u0;


//	if (u0 > 0) {
//		if (u0 > umax_pos) {
//			u_ret = umax_pos;
//		} else if (u0 < umin_pos) {
//			u_ret = umin_pos;
//		}
//	} else {
//		if (u0 < umax_neg) {
//			u_ret = umax_neg;
//		} else if (u0 > umin_neg) {
//			u_ret = umin_neg;
//		}
//	}


	if (u0 > umax_pos) {
		u_ret = umax_pos;
	} else if (u0 < 0) {
		u_ret = 0;
	}


	return u_ret;
}
