#ifndef __PID_OPT_H
#define __PID_OPT_H




class pid_opt
{
public:
	pid_opt();
	float compute(float measure);
	void pid_reset();
	void set_target(float value);
	void set_kp(float value);
	void set_ki(float value);
	void set_kd(float value);
	void set_dead_zone(float value);
	void set_min_limit(float value);
	void set_max_limit(float value);
	
	float get_kp();
	float get_ki();
	float get_kd();
	float get_dead_zone();
	float get_min_limit();
	float get_max_limit();
	
	
private:
	int isReset;
	float kp;
	float ki;
	float kd;
	float dead_zone;
	float min_limit;
	float max_limit;
	
	
	float target;
	float err_1;
	float err_2;
};












#endif
