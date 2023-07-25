#include <ap_int.h>
#include <ap_fixed.h>
#include <math.h>

struct input_type {
	ap_fixed<16, 0> u[4];
};

struct return_type {
	ap_uint<15> duty[4];
	ap_uint<1> direction[4];
};



ap_uint<1> servo_u_to_duty_1x (
		ap_uint<15> half_period,
		ap_fixed<12, 0> u0,
//		ap_uint<4> limit_time,
		ap_uint<15> &duty)
{
	ap_uint<1> direction;

	if (u0 < 0) {
		u0 = -u0;
		direction = 0;
	} else {
		direction = 1;
	}

	duty = u0 * half_period;

//	if (duty < limit_time) {
//		duty = 0;
//	} else if (duty >  (half_period - limit_time)) {
//		duty = half_period;
//	}

	return direction;
}

return_type servo_u_to_duty(
		ap_uint<16> half_period,
//		ap_uint<4> limit_time,
		input_type in
		)
{
#pragma HLS INTERFACE mode=ap_none port=return
#pragma HLS INTERFACE mode=ap_none port=half_period
//#pragma HLS INTERFACE mode=ap_none port=limit_time
#pragma HLS INTERFACE mode=ap_none port=in
#pragma HLS PIPELINE  off
	return_type ret;


	u_2_duty_1x_loop: for(unsigned i = 0; i < 4; i++) {
		ret.direction[i] = servo_u_to_duty_1x(half_period, in.u[i],
//				limit_time,
				ret.duty[i]);
	}

	return ret;
}
