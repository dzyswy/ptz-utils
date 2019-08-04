#include "ptz_visca.h"




int ptz_visca::open_ptz(const char *dev_name)
{
	int camera_num;
    if (VISCA_open_serial(&interface_, dev_name)!=VISCA_SUCCESS) {
        fprintf(stderr,"camera_ui: unable to open serial device %s\n",dev_name);
        return -1;
    }

    interface_.broadcast=0;
    VISCA_set_address(&interface_, &camera_num);
    camera.address=1;
    VISCA_clear(&interface_, &camera);
    VISCA_get_camera_info(&interface_, &camera);
	
	const unsigned char VISCA_POWER_ON = 2;
	const unsigned char VISCA_POWER_OFF = 3;
	VISCA_set_power(&interface_, &camera, VISCA_POWER_ON);
	
	return 0;
}


int ptz_visca::close_ptz()
{
/*	unsigned char packet[3000];
    int i, bytes;

    read the rest of the data: (should be empty)
    ioctl(interface_.port_fd, FIONREAD, &bytes);
    if (bytes>0) {
        fprintf(stderr, "ERROR: %d bytes not processed: ", bytes);
        read(interface_.port_fd, &packet, bytes);
        for (i=0;i<bytes;i++) {
            fprintf(stderr,"%2x ",packet[i]);
        }
        fprintf(stderr,"\n");
    }*/
    VISCA_close_serial(&interface_);
	return 0;
}


int ptz_visca::set_pantilt_up(int panSpeed, int tiltSpeed)
{
	if (VISCA_set_pantilt_up(&interface_, &camera, panSpeed, tiltSpeed) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::set_pantilt_upleft(int panSpeed, int tiltSpeed)
{
	if (VISCA_set_pantilt_upleft(&interface_, &camera, panSpeed, tiltSpeed) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::set_pantilt_upright(int panSpeed, int tiltSpeed)
{
	if (VISCA_set_pantilt_upright(&interface_, &camera, panSpeed, tiltSpeed) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::set_pantilt_left(int panSpeed, int tiltSpeed)
{
	if (VISCA_set_pantilt_left(&interface_, &camera, panSpeed, tiltSpeed) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::set_pantilt_right(int panSpeed, int tiltSpeed)
{
	if (VISCA_set_pantilt_right(&interface_, &camera, panSpeed, tiltSpeed) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::set_pantilt_down(int panSpeed, int tiltSpeed)
{
	if (VISCA_set_pantilt_down(&interface_, &camera, panSpeed, tiltSpeed) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::set_pantilt_downleft(int panSpeed, int tiltSpeed)
{
	if (VISCA_set_pantilt_downleft(&interface_, &camera, panSpeed, tiltSpeed) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::set_pantilt_downright(int panSpeed, int tiltSpeed)
{
	if (VISCA_set_pantilt_downright(&interface_, &camera, panSpeed, tiltSpeed) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::set_pantilt_stop(int panSpeed, int tiltSpeed)
{
	if (VISCA_set_pantilt_stop(&interface_, &camera, panSpeed, tiltSpeed) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::set_pantilt_absolute_position(int panSpeed, int tiltSpeed, int pan_position, int tilt_position)
{
	if (VISCA_set_pantilt_absolute_position(&interface_, &camera, panSpeed, tiltSpeed, pan_position, tilt_position) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::set_pantilt_relative_position(int panSpeed, int tiltSpeed, int pan_position, int tilt_position)
{
	if (VISCA_set_pantilt_relative_position(&interface_, &camera, panSpeed, tiltSpeed, pan_position, tilt_position) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::set_pantilt_home()
{
	if (VISCA_set_pantilt_home(&interface_, &camera) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::set_zoom_tele_speed(int speed)
{
	if (VISCA_set_zoom_tele_speed(&interface_, &camera, speed) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::set_zoom_wide_speed(int speed)
{
	if (VISCA_set_zoom_wide_speed(&interface_, &camera, speed) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::set_zoom_stop()
{
	if (VISCA_set_zoom_stop(&interface_, &camera) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::set_zoom_value(int value)
{
	if (VISCA_set_zoom_value(&interface_, &camera, value) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::set_focus_far_speed(int speed)
{
	if (VISCA_set_focus_far_speed(&interface_, &camera, speed) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::set_focus_near_speed(int speed)
{
	if (VISCA_set_focus_near_speed(&interface_, &camera, speed) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::set_focus_stop()
{
	if (VISCA_set_focus_stop(&interface_, &camera) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::set_focus_value(int value)
{
	if (VISCA_set_focus_value(&interface_, &camera, value) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::set_datascreen_off()
{
	if (VISCA_set_datascreen_off(&interface_, &camera) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::set_datascreen_on()
{
	if (VISCA_set_datascreen_on(&interface_, &camera) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::get_pantilt_position(int *pan_position, int *tilt_position)
{
	short pan_pos = 0, tilt_pos = 0;
	if (VISCA_get_pantilt_position(&interface_, &camera, (int16_t *)&pan_pos, (int16_t *)&tilt_pos) != VISCA_SUCCESS)
		return -1;
	*pan_position = pan_pos;
	*tilt_position = tilt_pos;
	return 0;
}

int ptz_visca::get_zoom_value(int *value)
{
	if (VISCA_get_zoom_value(&interface_, &camera, (uint16_t *)value) != VISCA_SUCCESS)
		return -1;
	return 0;
}

int ptz_visca::get_focus_value(int *value)
{
	if (VISCA_get_focus_value(&interface_, &camera, (uint16_t *)value) != VISCA_SUCCESS)
		return -1;
	return 0;
}













