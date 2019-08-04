#ifndef __PTZ_TRACK_H
#define __PTZ_TRACK_H

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <exception>
#include <boost/bind.hpp>
#include <iostream>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;


class ptz_visca;
class pid_opt;


enum PTZ_PID_TYPE {
	PTZ_PID_PAN = 0,
	PTZ_PID_TILT = 1,
	PTZ_PID_ZOOM = 2
};

struct ptz_positon {
	int pan;
	int tilt;
	int zoom;
};

typedef void (*ptz_pos_func)(void *, void *);

class ptz_track
{
public:	
	ptz_track(ptz_pos_func cb = NULL, void *cb_arg = NULL);
	~ptz_track();
	int open_ptz(const char *dev_name);
	int close_ptz();
	
	int load_config(const char *config_name);
	int save_config(const char *config_name);
	
	void set_track_mode(int value);
	void move_ptz_origin(int panSpeed, int tiltSpeed);
	
	void pan_pos_to_azimuth_angle(int pos, float &angle);
	void tilt_pos_to_zenith_angle(int pos, float &angle);
	void zoom_pos_to_radius(int pos, float &r);
	
	void azimuth_angle_to_pan_pos(float angle, int &pos);
	void zenith_angle_to_tilt_pos(float angle, int &pos);
	void radius_to_zoom_pos(float r, int &pos);
	
	int set_pantilt_up(int panSpeed, int tiltSpeed);
	int set_pantilt_upleft(int panSpeed, int tiltSpeed);
	int set_pantilt_upright(int panSpeed, int tiltSpeed);
	int set_pantilt_left(int panSpeed, int tiltSpeed);
	int set_pantilt_right(int panSpeed, int tiltSpeed);
	int set_pantilt_down(int panSpeed, int tiltSpeed);
	int set_pantilt_downleft(int panSpeed, int tiltSpeed);
	int set_pantilt_downright(int panSpeed, int tiltSpeed);
	int set_pantilt_stop(int panSpeed, int tiltSpeed);
	int set_pantilt_absolute_position(int panSpeed, int tiltSpeed, int pan_position, int tilt_position);
	int set_pantilt_relative_position(int panSpeed, int tiltSpeed, int pan_position, int tilt_position);
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
	
	void pid_reset(int id);
	void set_target(int id, float value);
	void set_kp(int id, float value);
	void set_ki(int id, float value);
	void set_kd(int id, float value);
	void set_dead_zone(int id, float value);
	void set_min_limit(int id, float value);
	void set_max_limit(int id, float value);
	
	float get_kp(int id);
	float get_ki(int id);
	float get_kd(int id);
	float get_dead_zone(int id);
	float get_min_limit(int id);
	float get_max_limit(int id);
	
	void set_pos_offset(int id, int value);
	void set_pos_scale(int id, float value);
	void set_zoom_map(vector<pair<int, float> > &value);
	int set_zoom_map_str(string &value);
	void set_install_offset(int id, int value);
	void set_min_pos(int id, int value);
	void set_max_pos(int id, int value);
	
	int get_pos_offset(int id);
	float get_pos_scale(int id);
	void get_zoom_map(vector<pair<int, float> > &value);
	void get_zoom_map_str(string &value);
	int get_install_offset(int id);
	int get_min_pos(int id);
	int get_max_pos(int id);
	
private:
	static void handle_run_track(void *arg);
	void on_start_timer(float timeout); 
	void handle_timer_timeout(const boost::system::error_code& err);
	
	
private:
	ptz_pos_func pos_cb;
	void *pos_cb_arg;
	ptz_visca *mptz;
	pid_opt *mpid[3];
	int track_mode;
	boost::asio::io_context *io_context_;
	boost::asio::steady_timer *timer_;
	
	int pos_offset[3];
	float pos_scale[3];
	vector<pair<int, float> > zoom_map;
	int install_offset[3];
	int pos_limit[3][2];
	
};

















#endif


