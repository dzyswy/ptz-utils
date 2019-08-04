#include "ptz_ctl_visca.h"
#include <libvisca/libvisca.h>

ptz_ctl_visca::ptz_ctl_visca()
{
	
	config_default_parameter();
	
	interface_ = new VISCACamera_t;
	camera = new VISCACamera_t;
	
}

ptz_ctl_visca::~ptz_ctl_visca()
{
	delete camera;
	delete interface_;
	
}

int ptz_ctl_visca::open_device(const char *dev_name) 
{
	int camera_num;
    if (VISCA_open_serial(interface_, dev_name)!=VISCA_SUCCESS) {
        fprintf(stderr,"camera_ui: unable to open serial device %s\n",dev_name);
        return -1;
    }

    interface_->broadcast=0;
    VISCA_set_address(interface_, camera_num);
    camera.address=1;
    VISCA_clear(interface_, camera);
    VISCA_get_camera_info(interface_, camera);
	
	const unsigned char VISCA_POWER_ON = 2;
	const unsigned char VISCA_POWER_OFF = 3;
	VISCA_set_power(interface_, camera, VISCA_POWER_ON);
	
	return 0;
} 

int ptz_ctl_visca::close_device() 
{
	VISCA_close_serial(interface_);
	return 0;
} 
	
	

	
int ptz_ctl_visca::set_pantilt_left(float pan_speed = -1) 
{
	if (pan_speed > 0)
		set_pan_speed(pan_speed);
	if (VISCA_set_pantilt_left(interface_, camera, panSpeed, tiltSpeed) != VISCA_SUCCESS)
		return -1;
	return 0;
} 

int ptz_ctl_visca::set_pantilt_right(float pan_speed = -1) 
{
	
} 

int ptz_ctl_visca::set_pantilt_up(float tilt_speed = -1) 
{
	
} 

int ptz_ctl_visca::set_pantilt_down(float tilt_speed = -1) 
{
	
} 

int ptz_ctl_visca::set_pantilt_upleft(float pan_speed = -1, float tilt_speed = -1) 
{
	
} 

int ptz_ctl_visca::set_pantilt_upright(float pan_speed = -1, float tilt_speed = -1) 
{
	
} 

int ptz_ctl_visca::set_pantilt_downleft(float pan_speed = -1, float tilt_speed = -1) 
{
	
}
 
int ptz_ctl_visca::set_pantilt_downright(float pan_speed = -1, float tilt_speed = -1) 
{
	
} 

int ptz_ctl_visca::set_pantilt_stop() 
{
	
} 
	
int ptz_ctl_visca::set_pantilt_absolute_position(float pan_position, float tilt_position, float pan_speed = -1, float tilt_speed = -1) 
{
	
} 

int ptz_ctl_visca::set_pantilt_relative_position(float pan_position, float tilt_position, float pan_speed = -1, float tilt_speed = -1) 
{
	
} 

int ptz_ctl_visca::set_pantilt_home() 
{
	
} 
	
int ptz_ctl_visca::set_zoom_tele(float zoom_speed = -1) 
{
	
} 

int ptz_ctl_visca::set_zoom_wide(float zoom_speed = -1) 
{
	
} 

int ptz_ctl_visca::set_zoom_absolute_position(float zoom_position, float zoom_speed = -1) 
{
	
} 

int ptz_ctl_visca::set_zoom_stop() 
{
	
} 
	
	
int ptz_ctl_visca::set_focus_near(float focus_speed) 
{
	
} 

int ptz_ctl_visca::set_focus_far(float focus_speed) 
{
	
}
 
int ptz_ctl_visca::set_focus_absolute_position(float focus_position, float focus_speed = -1) 
{
	
} 

int ptz_ctl_visca::set_focus_stop() 
{
	
} 
	
int ptz_ctl_visca::get_pan_position(float *pan_position) 
{
	
}
 
int ptz_ctl_visca::get_tilt_position(float *tilt_position) 
{
	
} 

int ptz_ctl_visca::get_pantilt_position(float *pan_position, float *tilt_position) 
{
	
} 

int ptz_ctl_visca::get_zoom_position(float *zoom_position) 
{
	
} 

int ptz_ctl_visca::get_focus_position(float *focus_position) 
{
	
} 























