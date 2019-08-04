#include <math.h>
#include <iostream>
#include "pid_opt.h"


using namespace std;


pid_opt::pid_opt()
{
	target = 0;
	kp = 0;
	ki = 0;
	kd = 0;
	dead_zone = 0;
	min_limit = 0;
	max_limit = 100;
	pid_reset();
}

float pid_opt::compute(float measure)
{
	float err = target - measure;
	
	
	if (fabs(err) < dead_zone) {
		err = 0;
	}

	if (isReset) {
		isReset = 0;
		err_1 = err;
		err_2 = err;
	}
		
	float out = kp * (err - err_1) + ki * err + kd * (err - 2 * err_1 + err_2);
	
	cout << "kp: " << kp;
	cout << " ki: " << ki;
	cout << " kd: " << kd;
	cout << " target: " << target << " measure: " << measure << " err: " << err;
	cout << " err_1: " << err_1;
	cout << " err_2: " << err_2;
	cout << " out: " << out;
	cout << endl;
	
	if (fabs(out) > max_limit)
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

void pid_opt::pid_reset()
{
	isReset = 1;
}

void pid_opt::set_target(float value)
{
	target = value;
}

void pid_opt::set_kp(float value)
{
	kp = value;
}

void pid_opt::set_ki(float value)
{
	ki = value;
}

void pid_opt::set_kd(float value)
{
	kd = value;
}

void pid_opt::set_dead_zone(float value)
{
	dead_zone = value;
}

void pid_opt::set_min_limit(float value)
{
	min_limit = value;
}

void pid_opt::set_max_limit(float value)
{
	max_limit = value;
}


float pid_opt::get_kp()
{
	return kp;
}

float pid_opt::get_ki()
{
	return ki;
}

float pid_opt::get_kd()
{
	return kd;
}

float pid_opt::get_dead_zone()
{
	return dead_zone;
}

float pid_opt::get_min_limit()
{
	return min_limit;
}

float pid_opt::get_max_limit()
{
	return max_limit;
}











