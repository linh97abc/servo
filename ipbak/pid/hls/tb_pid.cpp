#include "pid.h"
#include <stdio.h>
#include <math.h>

float cal_pid(

		float umin_pos,
		float umin_neg,
		float umax_pos,
		float umax_neg,

		float c0, float c1, float c2,
		float e0, float e1, float e2,
		float u1
		) {

	float u0 = u1 + c0*e0 + c1*e1 + c2*e2;

	if (u0 > 0) {
		if (u0 > umax_pos) {
			u0 = umax_pos;
		} else if (u0 < umin_pos) {
			u0 = umin_pos;
		}
	} else {
		if (u0 < umax_neg) {
			u0 = umax_neg;
		} else if (u0 > umin_neg) {
			u0 = umin_neg;
		}
	}


	return u0;
}


int main()
{
	pid_pipeline(1, -1, 25, -25, 0.1, 1.2, 2.3, 3, 4, 5, 6.2);
	pid_pipeline(1, -1, 25, -25, 1.1, 2.2, 3.3, 4, 5, 6, 7.1);
	pid_pipeline(1, -1, 25, -25, 2.2, 3.4, 4.4, -1, -2, -3, -4.2);
	pid_pipeline(1, -1, 25, -25, 3.1, 4.1, 5.1, 6, 7, 8, 9.1);
	pid_pipeline(1, -1, 25, -25, 1.1, 2.2, 3.3, -1, -2, -3, -4.2);

	pid_pipeline(1, -1, 25, -25, 0.1, 1.2, 2.3, 3, 4, 5, 6.2);
	pid_pipeline(1, -1, 25, -25, 1.1, 2.2, 3.3, 4, 5, 6, 7.1);
	pid_pipeline(1, -1, 25, -25, 2.2, 3.4, 4.4, -1, -2, -3, -4.2);
	pid_pipeline(1, -1, 25, -25, 3.1, 4.1, 5.1, 6, 7, 8, 9.1);
	pid_pipeline(1, -1, 25, -25, 1.1, 2.2, 3.3, -1, -2, -3, -4.2);

	pid_pipeline(1, -1, 25, -25, 0.1, 1.2, 2.3, 3, 4, 5, 6.2);
	pid_pipeline(1, -1, 25, -25, 1.1, 2.2, 3.3, 4, 5, 6, 7.1);
	pid_pipeline(1, -1, 25, -25, 2.2, 3.4, 4.4, -1, -2, -3, -4.2);
	pid_pipeline(1, -1, 25, -25, 3.1, 4.1, 5.1, 6, 7, 8, 9.1);
	pid_pipeline(1, -1, 25, -25, 1.1, 2.2, 3.3, -1, -2, -3, -4.2);

	pid_pipeline(1, -1, 25, -25, 0.1, 1.2, 2.3, 3, 4, 5, 6.2);
	pid_pipeline(1, -1, 25, -25, 1.1, 2.2, 3.3, 4, 5, 6, 7.1);
	pid_pipeline(1, -1, 25, -25, 2.2, 3.4, 4.4, -1, -2, -3, -4.2);
	pid_pipeline(1, -1, 25, -25, 3.1, 4.1, 5.1, 6, 7, 8, 9.1);
	pid_pipeline(1, -1, 25, -25, 1.1, 2.2, 3.3, -1, -2, -3, -4.2);

	//


	U_type actual[4];
	float expected[4];



	expected[0] = cal_pid(1, -1, 25, -25, 0.1, 1.2, 2.3, 3, 4, 5, 6.2);
	expected[1] = cal_pid(1, -1, 25, -25, 1.1, 2.2, 3.3, 4, 5, 6, 7.1);
	expected[2] = cal_pid(1, -1, 25, -25, 2.2, 3.4, 4.4, -1, -2, -3, -4.2);
	expected[3] = cal_pid(1, -1, 25, -25, 3.1, 4.1, 5.1, 6, 7, 8, 9.1);

	actual[0] = pid_pipeline(1, -1, 25, -25, 0.1, 1.2, 2.3, 3, 4, 5, 6.2);
	actual[1] = pid_pipeline(1, -1, 25, -25, 1.1, 2.2, 3.3, 4, 5, 6, 7.1);
	actual[2] = pid_pipeline(1, -1, 25, -25, 2.2, 3.4, 4.4, -1, -2, -3, -4.2);
	actual[3] = pid_pipeline(1, -1, 25, -25, 3.1, 4.1, 5.1, 6, 7, 8, 9.1);

	for (unsigned i = 0; i < 4; i++)
	{
		float actual_val = actual[i].to_float();
		float err = expected[i] - actual_val;

		if (abs(err) > 0.01) {
			printf("Test Failed");
		}
	}

	printf("Test End");
	return 0;
}
