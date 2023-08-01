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

	if (u0 > umax_pos) {
		u_ret = umax_pos;
	} else if (u0 < 0) {
		u_ret = 0;
	}


	return u_ret;
}
