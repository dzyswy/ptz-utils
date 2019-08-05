#ifndef __PTZ_CTL_VISCA_H
#define __PTZ_CTL_VISCA_H



#include "ptz_ctl_base.h"



struct _VISCA_interface;
struct _VISCA_camera;



class ptz_ctl_visca : public ptz_ctl_base
{
public:
	ptz_ctl_visca();
	~ptz_ctl_visca();
	virtual int open_device(const char *dev_name);
	virtual int close_device();
	
	virtual int add_sample(int dim, int ch, float first, float second);
	virtual int clear_samples(int dim, int ch);
	virtual int gen_fit_para(int dim, int ch);

	virtual int set_pantilt_left(float pan_speed = -1);
	virtual int set_pantilt_right(float pan_speed = -1);
	virtual int set_pantilt_up(float tilt_speed = -1);
	virtual int set_pantilt_down(float tilt_speed = -1);
	virtual int set_pantilt_upleft(float pan_speed = -1, float tilt_speed = -1);
	virtual int set_pantilt_upright(float pan_speed = -1, float tilt_speed = -1);
	virtual int set_pantilt_downleft(float pan_speed = -1, float tilt_speed = -1);
	virtual int set_pantilt_downright(float pan_speed = -1, float tilt_speed = -1);
	virtual int set_pantilt_stop();
	
	virtual int set_pantilt_absolute_position(float pan_position, float tilt_position, float pan_speed = -1, float tilt_speed = -1);
	virtual int set_pantilt_relative_position(float pan_position, float tilt_position, float pan_speed = -1, float tilt_speed = -1);
	virtual int set_pantilt_home();
	
	virtual int set_zoom_tele(float zoom_speed = -1);
	virtual int set_zoom_wide(float zoom_speed = -1);
	virtual int set_zoom_absolute_position(float zoom_position, float zoom_speed = -1);
	virtual int set_zoom_stop();
	
	
	virtual int get_pan_position(float *pan_position);
	virtual int get_tilt_position(float *tilt_position);
	virtual int get_pantilt_position(float *pan_position, float *tilt_position);
	virtual int get_zoom_position(float *zoom_position);

protected:

	virtual int calc_fit_para(vector<pair<float, float> > &samples, vector<float> &para_p, vector<float> &para_n, int degree);
	virtual float calc_fit_value(vector<float> &para, float x);
	
	virtual float sensor_from_visca(int dim, int ch, int value);
	virtual int sensor_to_visca(int dim, int ch, float value);
	
	
protected:

	virtual void config_default_parameter()
	{
		fit_degree_[PTZ_POSITION_DIM][PTZ_PAN] = 1;
		fit_degree_[PTZ_POSITION_DIM][PTZ_TILT] = 1;
		fit_degree_[PTZ_POSITION_DIM][PTZ_ZOOM] = 3;
		fit_degree_[PTZ_SPEED_DIM][PTZ_PAN] = 3;
		fit_degree_[PTZ_SPEED_DIM][PTZ_TILT] = 3;
		fit_degree_[PTZ_SPEED_DIM][PTZ_ZOOM] = 3;
		
		min_default_pan_position_ = -170;
		max_default_pan_position_ = 170;
		min_default_tilt_position_ = -30;
		max_default_tilt_position_ = 90;
		min_default_zoom_position_ = 2.9;
		max_default_zoom_position_ = 59.5;
		
		min_default_pan_speed_ = 0.1;
		max_default_pan_speed_ = 120;
		min_default_tilt_speed_ = 0.1;
		max_default_tilt_speed_ = 80;
		min_default_zoom_speed_ = 0.1;
		max_default_zoom_speed_ = 20;
		
		
		add_sample(PTZ_POSITION_DIM, PTZ_PAN, 0, 0);
		add_sample(PTZ_POSITION_DIM, PTZ_PAN, -2400, -170);
		add_sample(PTZ_POSITION_DIM, PTZ_PAN, 2400, 170);
		gen_fit_para(PTZ_POSITION_DIM, PTZ_PAN);
		
		add_sample(PTZ_POSITION_DIM, PTZ_TILT, 0, 0);
		add_sample(PTZ_POSITION_DIM, PTZ_TILT, -2400, -30);
		add_sample(PTZ_POSITION_DIM, PTZ_TILT, 2400, 90);
		gen_fit_para(PTZ_POSITION_DIM, PTZ_TILT);
		
		add_sample(PTZ_POSITION_DIM, PTZ_ZOOM, 0, 0);
		add_sample(PTZ_POSITION_DIM, PTZ_ZOOM, -2400, -30);
		add_sample(PTZ_POSITION_DIM, PTZ_ZOOM, 2400, 90);
		gen_fit_para(PTZ_POSITION_DIM, PTZ_ZOOM);
		
		add_sample(PTZ_SPEED_DIM, PTZ_PAN, 0, 0);
		add_sample(PTZ_SPEED_DIM, PTZ_PAN, -2400, -170);
		add_sample(PTZ_SPEED_DIM, PTZ_PAN, 2400, 170);
		gen_fit_para(PTZ_SPEED_DIM, PTZ_PAN);
		
		add_sample(PTZ_SPEED_DIM, PTZ_TILT, 0, 0);
		add_sample(PTZ_SPEED_DIM, PTZ_TILT, -2400, -30);
		add_sample(PTZ_SPEED_DIM, PTZ_TILT, 2400, 90);
		gen_fit_para(PTZ_SPEED_DIM, PTZ_TILT);
		
		add_sample(PTZ_SPEED_DIM, PTZ_ZOOM, 0, 0);
		add_sample(PTZ_SPEED_DIM, PTZ_ZOOM, -2400, -30);
		add_sample(PTZ_SPEED_DIM, PTZ_ZOOM, 2400, 90);
		gen_fit_para(PTZ_SPEED_DIM, PTZ_ZOOM);
		
		
	}
	

	
	
	
protected:
	VISCAInterface_t *interface_;
    VISCACamera_t *camera;
	
	float min_default_pan_position_;
	float max_default_pan_position_;
	float min_default_tilt_position_;
	float max_default_tilt_position_;
	float min_default_zoom_position_;
	float max_default_zoom_position_;
	
	float min_default_pan_speed_;
	float max_default_pan_speed_;
	float min_default_tilt_speed_;
	float max_default_tilt_speed_;
	float min_default_zoom_speed_;
	float max_default_zoom_speed_;

	
	
	int fit_degree_[PTZ_MAX_DIM][PTZ_MAX_CHANNEL];
	vector<pair<float, float> > fit_samples_[PTZ_MAX_DIM][PTZ_MAX_CHANNEL];
	vector<double> fit_para_[PTZ_MAX_DIM][PTZ_MAX_CHANNEL][2];
	
};
















#endif

