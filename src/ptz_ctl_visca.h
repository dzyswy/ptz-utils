#ifndef __PTZ_CTL_VISCA_H
#define __PTZ_CTL_VISCA_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct _VISCA_interface;
struct _VISCA_camera;



class ptz_ctl_visca
{
public:	
	ptz_ctl_visca();
	~ptz_ctl_visca();
	
	int open_device(const char *dev_name);
	int close_device();
	
	void set_pan_speed(int value)
	{
		pan_speed_ = (value < min_pan_speed_) ? min_pan_speed_ : ((value > max_pan_speed_) ? max_pan_speed_ : value);
	}
	
	void set_tilt_speed(int value)
	{
		tilt_speed_ = (value < min_tilt_speed_) ? min_tilt_speed_ : ((value > max_tilt_speed_) ? max_tilt_speed_ : value);
	}
	
	void set_zoom_speed(int value)
	{
		zoom_speed_ = (value < min_zoom_speed_) ? min_zoom_speed_ : ((value > max_zoom_speed_) ? max_zoom_speed_ : value);
	}
	
	
	int set_pantilt_left(int pan_speed = -1);
	int set_pantilt_right(int pan_speed = -1);
	int set_pantilt_up(int tilt_speed = -1);
	int set_pantilt_down(int tilt_speed = -1);
	int set_pantilt_upleft(int pan_speed = -1, int tilt_speed = -1);
	int set_pantilt_upright(int pan_speed = -1, int tilt_speed = -1);
	int set_pantilt_downleft(int pan_speed = -1, int tilt_speed = -1);
	int set_pantilt_downright(int pan_speed = -1, int tilt_speed = -1);
	int set_pantilt_stop();
	
	int set_pantilt_absolute_position(int pan_position, int tilt_position, int pan_speed = -1, int tilt_speed = -1);
	int set_pantilt_relative_position(int pan_position, int tilt_position, int pan_speed = -1, int tilt_speed = -1);
	int set_pantilt_home();
	
	int set_zoom_tele(int zoom_speed = -1);
	int set_zoom_wide(int zoom_speed = -1);
	int set_zoom_stop();
	int set_zoom_absolute_position(int zoom_position, int zoom_speed = -1);
	

	
	int get_pan_speed()
	{
		return pan_speed_;
	}
	
	int get_tilt_speed()
	{
		return tilt_speed_;
	}
	
	int get_zoom_speed()
	{
		return zoom_speed_;
	}
	
	int get_min_pan_speed()
	{
		return min_pan_speed_;
	}
	
	int get_max_pan_speed()
	{
		return max_pan_speed_;
	}
	
	int get_min_tilt_speed()
	{
		return min_tilt_speed_;
	}
	
	int get_max_tilt_speed()
	{
		return max_tilt_speed_;
	}
	
	int get_min_zoom_speed()
	{
		return min_zoom_speed_;
	}
	
	int get_max_zoom_speed()
	{
		return max_zoom_speed_;
	}
	
	
	int get_pantilt_position(int *pan_position, int *tilt_position);
	int get_zoom_position(int *zoom_position);

protected:
	inline int gen_valid_pan_speed(int pan_speed) 
	{
		return ((pan_speed >= min_pan_speed_) && (pan_speed <= max_pan_speed_)) ? pan_speed : pan_speed_;
	}
	
	inline int gen_valid_tilt_speed(int tilt_speed) 
	{
		return ((tilt_speed >= min_tilt_speed_) && (tilt_speed <= max_tilt_speed_)) ? tilt_speed : tilt_speed_;
	}
	
	inline int gen_valid_zoom_speed(int zoom_speed) 
	{
		return ((zoom_speed >= min_zoom_speed_) && (zoom_speed <= max_zoom_speed_)) ? zoom_speed : zoom_speed_;
	}
	
	
protected:
	VISCAInterface_t *interface_;
    VISCACamera_t *camera;

	int pan_speed_;
	int tilt_speed_;
	int zoom_speed_;
	
	int min_pan_speed_;
	int max_pan_speed_;
	int min_tilt_speed_;
	int max_tilt_speed_;
	int min_zoom_speed_;
	int max_zoom_speed_;
	
	
};




















































#endif

