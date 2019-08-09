#ifndef __STEREO_FILTER_H
#define __STEREO_FILTER_H





#include "daemon_stereo.h"



enum stereo_filter_target_state_type
{
	STEREO_FILTER_NO_TARGET = 0,
	STEREO_FILTER_SINGLE_TARGET = 1,
	STEREO_FILTER_MULTI_TARGET = 2,
	STEREO_FILTER_TARGET_MAX_STATE = 3,
};


#define STEREO_FILTER_TARGET_POSITION_NUMBER	3


class stereo_filter
{
public:
	stereo_filter(daemon_stereo *stereo);
	~stereo_filter();
	
	
	void set_fast_change_count(int value) 
	{
		fast_change_count_ = value;
	}
	
	void set_slow_change_count(int value)
	{
		slow_change_count_ = value;
	}
	
	void set_stable_dist(float value)
	{
		stable_dist_ = value;
	}
	
	int get_fast_change_count()
	{
		return fast_change_count_;
	}
	
	int get_slow_change_count()
	{
		return slow_change_count;
	}
	
	int get_target_state()
	{
		std::unique_lock<std::mutex> lock(mux_);
		return target_state_;
	}
	
	struct http_output_detect_box get_target_position()
	{
		std::unique_lock<std::mutex> lock(mux_);
		return target_position_;
	}
	
	int get_position_statble_count()
	{
		std::unique_lock<std::mutex> lock(mux_);
		return position_statble_count_;
	}
	
	float get_stable_dist()
	{
		return stable_dist_;
	}
	

	void filter_process();

 
	
protected:
	daemon_stereo *stereo_;
	
	int fast_change_count_;
	int slow_change_count_;
	
	
	int going;
	std::thread *thread_;
	
	std::mutex mux_;

	int now_target_state_count_[STEREO_FILTER_TARGET_MAX_STATE];

	int target_state_;
	struct http_output_detect_box target_position_;
	
	int position_statble_count_;
	float stable_dist_;
	list<struct http_output_detect_box> target_position_buffer_;
};





#endif















