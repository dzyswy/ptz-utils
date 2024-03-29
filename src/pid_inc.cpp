#include <math.h>
#include <iostream>
#include "pid_inc.h"


using namespace std;


pid_inc::pid_inc()
{
	kp = 0;
	ki = 0;
	kd = 0;
	dead_zone = 0;
	max_limit = 0;
	pid_reset(0);
}

float pid_inc::compute(float err)
{
	if (fabs(err) < dead_zone) {
		err = 0;
	}

	float out = kp * (err - err_1) + ki * err + kd * (err - 2 * err_1 + err_2);
	
	if ((max_limit > 0) && (fabs(out) > max_limit))
	{
		if (out > 0)
			out = max_limit;
		else
			out = -max_limit;
	}	
	err_2 = err_1;
	err_1 = err;
	
	return out;
}












