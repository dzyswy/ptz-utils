#ifndef __PTZ_VISCA_H
#define __PTZ_VISCA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libvisca/libvisca.h>


class ptz_visca
{
public:
	int open_ptz(const char *dev_name);
	int close_ptz();
	
	int set_pantilt_up(int panSpeed, int tiltSpeed);
	int set_pantilt_upleft(int panSpeed, int tiltSpeed);
	int set_pantilt_upright(int panSpeed, int tiltSpeed);
	int set_pantilt_left(int panSpeed, int tiltSpeed);
	int set_pantilt_right(int panSpeed, int tiltSpeed);
	int set_pantilt_down(int panSpeed, int tiltSpeed);
	int set_pantilt_downleft(int panSpeed, int tiltSpeed);
	int set_pantilt_downright(int panSpeed, int tiltSpeed);
	int set_pantilt_stop(int panSpeed, int tiltSpeed);
	int set_pantilt_absolute_position(int pan_speed, int tilt_speed, int pan_position, int tilt_position);
	int set_pantilt_relative_position(int pan_speed, int tilt_speed, int pan_position, int tilt_position);
	int set_pantilt_home();
	int set_zoom_tele_speed(int speed);
	int set_zoom_wide_speed(int speed);
	int set_zoom_stop();
	int set_zoom_value(int value);
	int set_focus_far_speed(int speed);
	int set_focus_near_speed(int speed);
	int set_focus_stop();
	int set_focus_value(int value);
	int set_datascreen_off();
	int set_datascreen_on();
	int get_pantilt_position(int *pan_position, int *tilt_position);
	int get_zoom_value(int *value);
	int get_focus_value(int *value);
	
private:	
	VISCAInterface_t interface_;
    VISCACamera_t camera;
};












#endif

