#ifndef __PTZ_CTL_VISCA_H
#define __PTZ_CTL_VISCA_H


#include <iostream>


enum PTZ_CHANNEL_SELECT
{
	PTZ_PAN = 0,
	PTZ_TILT = 1,
	PTZ_ZOOM = 2,
	PTZ_MAX_CHANNEL = 3,
	
};

enum PTZ_DIM_SELECT
{
	PTZ_POSITION_DIM = 0,
	PTZ_SPEED_DIM = 1,
	PTZ_MAX_DIM = 2,
};


class ptz_ctl_base
{
public:
	ptz_ctl_base 
	{
		pan_speed_ = 30;
		tilt_speed_ = 20;
		zoom_speed_ = 10;
	}
	
	virtual int open_device(const char *dev_name) = NULL;
	virtual int close_device() = NULL;
	
	
	virtual int set_pan_speed(float value)
	{
		pan_speed_ = value;
		return 0;
	}
	virtual int set_tilt_speed(float value)
	{
		tilt_speed_ = value;
		return 0;
	}
	virtual int set_zoom_speed(float value)
	{
		zoom_speed_ = value;
		return 0;
	}

	
	virtual int set_pantilt_left(float pan_speed = -1) = NULL;
	virtual int set_pantilt_right(float pan_speed = -1) = NULL;
	virtual int set_pantilt_up(float tilt_speed = -1) = NULL;
	virtual int set_pantilt_down(float tilt_speed = -1) = NULL;
	virtual int set_pantilt_upleft(float pan_speed = -1, float tilt_speed = -1) = NULL;
	virtual int set_pantilt_upright(float pan_speed = -1, float tilt_speed = -1) = NULL;
	virtual int set_pantilt_downleft(float pan_speed = -1, float tilt_speed = -1) = NULL;
	virtual int set_pantilt_downright(float pan_speed = -1, float tilt_speed = -1) = NULL;
	virtual int set_pantilt_stop() = NULL;
	
	virtual int set_pantilt_absolute_position(float pan_position, float tilt_position, float pan_speed = -1, float tilt_speed = -1) = NULL;
	virtual int set_pantilt_relative_position(float pan_position, float tilt_position, float pan_speed = -1, float tilt_speed = -1) = NULL;
	virtual int set_pantilt_home() = NULL;
	
	virtual int set_zoom_tele(float zoom_speed = -1) = NULL;
	virtual int set_zoom_wide(float zoom_speed = -1) = NULL;
	virtual int set_zoom_absolute_position(float zoom_position, float zoom_speed = -1) = NULL;
	virtual int set_zoom_stop() = NULL;
	
	
	virtual int get_pan_position(float *pan_position) = NULL;
	virtual int get_tilt_position(float *tilt_position) = NULL;
	virtual int get_pantilt_position(float *pan_position, float *tilt_position) = NULL;
	virtual int get_zoom_position(float *zoom_position) = NULL;
	
	
protected:
	float pan_speed_;
	float tilt_speed_;
	float zoom_speed_;
	
	
};



#endif


















