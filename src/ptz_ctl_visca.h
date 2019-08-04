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

	virtual void config_default_parameter()
	{
		min_visca_pan_speed_ = 0.1;
		max_visca_pan_speed_ = 120;
		min_visca_tilt_speed_ = 0.1;
		max_visca_tilt_speed_ = 80;
		min_visca_zoom_speed_ = 0.1;
		max_visca_zoom_speed_ = 20;
		pan_speed_radio_ = ;
		tilt_speed_radio_ = ;
		zoom_speed_radio_ = ;
		focus_speed_radio_ = ;
		

		min_visca_pan_position_ = -170;
		max_visca_pan_position_ = 170;
		min_visca_tilt_position_ = -30;
		max_visca_tilt_position_ = 90;
		min_visca_zoom_position_ = 2.9;
		max_visca_zoom_position_ = 59.5;
		pan_position_radio_ = ;
		tilt_position_radio_ = ;
		zoom_position_radio_ = ;
		focus_position_radio_ = ;
	}
	

	
	
	
protected:
	VISCAInterface_t *interface_;
    VISCACamera_t *camera;
	
	
	float min_visca_pan_speed_;
	float max_visca_pan_speed_;
	float min_visca_tilt_speed_;
	float max_visca_tilt_speed_;
	float min_visca_zoom_speed_;
	float max_visca_zoom_speed_;
	float pan_speed_radio_;
	float tilt_speed_radio_;
	float zoom_speed_radio_;
	float focus_speed_radio_;
	

	float min_visca_pan_position_;
	float max_visca_pan_position_;
	float min_visca_tilt_position_;
	float max_visca_tilt_position_;
	float min_visca_zoom_position_;
	float max_visca_zoom_position_;
	float pan_position_radio_;
	float tilt_position_radio_;
	float zoom_position_radio_;
	float focus_position_radio_;
	
	
	
};
















#endif

