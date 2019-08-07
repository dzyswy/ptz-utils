#ifndef __STEREO_FILTER_H
#define __STEREO_FILTER_H






class daemon_stereo;


struct stereo_filter_detect_box
{
	float box_x;
	float box_y;
	float box_w;
	float box_h;
	float cod_x;
	float cod_y;
	float cod_z;
};


enum STEREO_TARGET_NUMBER_STATE
{
	STEREO_NO_TARGET = 0,
	STEREO_SINGLE_TARGET = 1,
	STEREO_MULTI_TARGET = 2
	
};



class stereo_filter
{
public:
	stereo_filter(daemon_stereo *stereo, int coord_type);
	~stereo_filter();
	

	
	

	void filter_process();


protected:
	
	
	
protected:
	daemon_stereo *stereo_;
	
	int going;
	std::thread *thread_;

	int target_count_;
	struct http_output_detect_box target_position_;

	int target_number_state_;
	
	int now_target_number_state_count_[3];
	
	vector<struct http_output_detect_box> pre_detect_boxes_;
};





#endif















