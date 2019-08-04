#include <boost/math/special_functions/round.hpp>
#include <boost/math/tr1.hpp>
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string.hpp>
#include "ptz_track.h"
#include "ptz_visca.h"
#include "pid_opt.h"


using namespace std;

#define PTZ_PID_TIMEOUT	200

struct run_track_config {
	boost::asio::io_context *io_context_;
	boost::asio::steady_timer *timer_;
	ptz_track *track_;
};

ptz_track::ptz_track(ptz_pos_func cb, void *cb_arg)
{
	track_mode = 0;
	pos_cb = cb;
	pos_cb_arg = cb_arg;
	
	for (int i = 0; i < 3; i++)
	{
		pos_offset[i] = 0;
		pos_scale[i] = 1;
		install_offset[i] = 0;
		pos_limit[i][0] = -16384;
		pos_limit[i][1] = 16384;
	}	
	pair<int, float> item0(0, 1.0);
	pair<int, float> item1(16384, 2000.0);
	zoom_map.push_back(item0);
	zoom_map.push_back(item1);
	
	mptz = new ptz_visca;
	
	mpid[0] = new pid_opt;
	mpid[1] = new pid_opt;
	mpid[2] = new pid_opt;
	
	io_context_ = new boost::asio::io_context;
	timer_ = new boost::asio::steady_timer(*io_context_);
	
	boost::thread t(handle_run_track, this);
	t.detach();
}


ptz_track::~ptz_track()
{
	io_context_->stop();
	
	delete timer_;
	delete io_context_;
	
	delete mpid[0];
	delete mpid[1];
	delete mpid[2];
	delete mptz;
}

void ptz_track::set_track_mode(int value)
{
	track_mode = value;
}

void ptz_track::move_ptz_origin(int panSpeed, int tiltSpeed)
{
	set_pantilt_absolute_position(panSpeed, tiltSpeed, pos_offset[0], pos_offset[1]);
	set_zoom_value(pos_offset[2]);
}

void ptz_track::pan_pos_to_azimuth_angle(int pos, float &angle)
{
	angle = (pos - pos_offset[0]) / pos_scale[0];
}

void ptz_track::tilt_pos_to_zenith_angle(int pos, float &angle)
{
	angle = (pos - pos_offset[1]) / pos_scale[1];
}

void ptz_track::zoom_pos_to_radius(int pos, float &r)
{
	int num = zoom_map.size();
	int n = -1;
	for (int i = 0; i < num; i++)
	{
		int item_pos = zoom_map[i].first;
		
		if (item_pos >  pos) {
			n = i;
			break;
		}
	}	
	
	if (n == -1) {
		r = zoom_map[num - 1].second;
	}
	else if (n == 0) {
		r = zoom_map[0].second;
	}
	else {
		float k = (float)(pos - zoom_map[n - 1].first) / (float)(zoom_map[n].first - zoom_map[n - 1].first);
		float range = zoom_map[n].second - zoom_map[n - 1].second;
		r = zoom_map[n - 1].second + k * range;
	}	
}

void ptz_track::azimuth_angle_to_pan_pos(float angle, int &pos)
{
	pos = angle * pos_scale[0] + pos_offset[0];
}

void ptz_track::zenith_angle_to_tilt_pos(float angle, int &pos)
{
	pos = angle * pos_scale[1] + pos_offset[1];
}

void ptz_track::radius_to_zoom_pos(float r, int &pos)
{
	int num = zoom_map.size();
	int n = -1;
	for (int i = 0; i < num; i++)
	{
		float item_dist = zoom_map[i].second;
		
		if (item_dist > r) {
			n = i;
			break;
		}
	}	
	
	if (n == -1) {
		pos = zoom_map[num - 1].first;
	}
	else if (n == 0) {
		pos = zoom_map[0].first;
	}
	else {
		float k = (float)(r - zoom_map[n - 1].second) / (float)(zoom_map[n].second - zoom_map[n - 1].second);
		float range = zoom_map[n].first - zoom_map[n - 1].first;
		pos = zoom_map[n - 1].first + k * range;			
	}
}


void ptz_track::handle_run_track(void *arg)
{
	ptz_track *mtk = (ptz_track *)arg;
	
//	boost::this_thread::sleep(boost::posix_time::seconds(2)); 
	
	mtk->on_start_timer(PTZ_PID_TIMEOUT);
	mtk->io_context_->run();
}

void ptz_track::on_start_timer(float timeout)
{
	timer_->expires_from_now(boost::chrono::milliseconds((int)timeout));
	timer_->async_wait(boost::bind(&ptz_track::handle_timer_timeout, this, boost::asio::placeholders::error));	
}


void ptz_track::handle_timer_timeout(const boost::system::error_code& err)
{

	if (!err) {
		
		const boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();
		while(track_mode)
		{
			int ret = 0;
			int pan_pos = 0, tilt_pos = 0, zoom_pos = 0;
			ret = get_pantilt_position(&pan_pos, &tilt_pos);
			if (ret < 0) {
				cout << "failed to get_pantilt_position" << endl;
				break;
			}
			
			ret = get_zoom_value(&zoom_pos);
			if (ret < 0) {
				cout << "failed to get_zoom_value" << endl;
				break;
			}
			
			float pan_out = mpid[0]->compute((float)pan_pos);
			float tilt_out = mpid[1]->compute((float)tilt_pos);
			float zoom_out = mpid[2]->compute((float)zoom_pos);
			int panSpeed = boost::math::round((double)fabs(pan_out));
			int tiltSpeed = boost::math::round((double)fabs(tilt_out));
			int zoomSpeed = boost::math::round((double)fabs(zoom_out));
			
			if ((panSpeed == 0) && (tiltSpeed == 0)) 
			{
				mptz->set_pantilt_stop(panSpeed, tiltSpeed);
			}	
			else 
			{
				if ((panSpeed != 0) && (tiltSpeed != 0)) 
				{
					if ((pan_out > 0) && (tilt_out > 0))
					{
						mptz->set_pantilt_upright(panSpeed, tiltSpeed);
					}	
					else if ((pan_out > 0) && (tilt_out < 0))
					{
						mptz->set_pantilt_downright(panSpeed, tiltSpeed);
					}
					else if ((pan_out < 0) && (tilt_out > 0))
					{
						mptz->set_pantilt_upleft(panSpeed, tiltSpeed);
					}	
					else if ((pan_out < 0) && (tilt_out < 0))
					{
						mptz->set_pantilt_downleft(panSpeed, tiltSpeed);
					}	
				}
				else if ((panSpeed != 0) && (tiltSpeed == 0))
				{
					if (pan_out < 0) {
						mptz->set_pantilt_left(panSpeed, tiltSpeed);
					}
					else {
						mptz->set_pantilt_right(panSpeed, tiltSpeed);
					}
				}	
				else if ((panSpeed == 0) && (tiltSpeed != 0))
				{
					if (tilt_out < 0) {
						mptz->set_pantilt_down(panSpeed, tiltSpeed);
					}
					else {
						mptz->set_pantilt_up(panSpeed, tiltSpeed);
					}
				}	
			}
			
			if (zoomSpeed == 0)
			{
				mptz->set_zoom_stop();
			}	
			else
			{
				if (zoom_out > 0)
				{
					mptz->set_zoom_tele_speed(zoomSpeed - 1);
				}	
				else
				{
					mptz->set_zoom_wide_speed(zoomSpeed - 1);
				}		
			}	
		
			if (pos_cb) {
				struct ptz_positon pos;
				pos.pan = pan_pos;
				pos.tilt = tilt_pos;
				pos.zoom = zoom_pos;
				pos_cb(pos_cb_arg, &pos);	
			}
			
			// Get current time from the clock, using microseconds resolution
			const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
			// Get the time offset in current day
			const boost::posix_time::time_duration td = now.time_of_day();
		//	int hh = td.hours();
		//	int mm = td.minutes();
		//	int ss = td.seconds();
		//	int ms = td.total_milliseconds() - ((hh * 3600 + mm * 60 + ss) * 1000);
			cout << "ms: " << td.total_milliseconds();
			cout << " pan: " << pan_pos;
			cout << " tilt: " << tilt_pos;
			cout << " panSpeed: " << panSpeed;
			cout << endl;
			
			break;
		}	
		const boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::millisec_posix_time_system_config::time_duration_type time_elapse;  
		time_elapse = end_time - start_time;
		double spend_time = (double)time_elapse.total_milliseconds();
		double leave_time = PTZ_PID_TIMEOUT - spend_time;
		if (leave_time < 0)
			leave_time = 1;
		on_start_timer(leave_time);
	}
	else
	{
		cout << "ptz track timeout error!" << endl;
	}	
	
}


int ptz_track::open_ptz(const char *dev_name)
{
	return mptz->open_ptz(dev_name);
}

int ptz_track::close_ptz()
{
	return mptz->close_ptz();
}

int ptz_track::load_config(const char *config_name)
{
	boost::property_tree::ptree pt;  
	
	try
	{
		boost::property_tree::ini_parser::read_ini(config_name, pt);
	}
	catch(std::exception e)
	{
		cout << e.what() << endl;
		boost::property_tree::ini_parser::write_ini(config_name, pt);
		return -1;
	}
    
	
	double kp[3], ki[3], kd[3], dead_zone[3], min_limit[3], max_limit[3];
	int pos_offset[3];
	int pos_scale[3];
	string zoom_map_str;
	int install_offset[3];
	
	
	kp[0] = pt.get<double>("pan.kp", 0.00);
	kp[1] = pt.get<double>("tilt.kp", 0.00);
	kp[2] = pt.get<double>("zoom.kp", 0.00);
	
	ki[0] = pt.get<double>("pan.ki", 0.01);
	ki[1] = pt.get<double>("tilt.ki", 0.01);
	ki[2] = pt.get<double>("zoom.ki", 0.01);
	
	kd[0] = pt.get<double>("pan.kd", 0.00);
	kd[1] = pt.get<double>("tilt.kd", 0.00);
	kd[2] = pt.get<double>("zoom.kd", 0.00);
	
	dead_zone[0] = pt.get<double>("pan.dead_zone", 100);
	dead_zone[1] = pt.get<double>("tilt.dead_zone", 100);
	dead_zone[2] = pt.get<double>("zoom.dead_zone", 100);
	
	min_limit[0] = pt.get<double>("pan.min_limit", 0);
	min_limit[1] = pt.get<double>("tilt.min_limit", 0);
	min_limit[2] = pt.get<double>("zoom.min_limit", 0);
	
	max_limit[0] = pt.get<double>("pan.max_limit", 24);
	max_limit[1] = pt.get<double>("tilt.max_limit", 24);
	max_limit[2] = pt.get<double>("zoom.max_limit", 7);
	
	pos_offset[0] = pt.get<int>("pan.pos_offset", 0);
	pos_offset[1] = pt.get<int>("tilt.pos_offset", 0);
	pos_offset[2] = pt.get<int>("zoom.pos_offset", 0);
	
	pos_scale[0] = pt.get<double>("pan.pos_scale", 1);
	pos_scale[1] = pt.get<double>("tilt.pos_scale", 1);
	pos_scale[2] = pt.get<double>("zoom.pos_scale", 1);
	zoom_map_str = pt.get<string>("zoom.zoom_map", "2 0 100 16384 2000");
	
	install_offset[0] = pt.get<int>("pan.install_offset", 0);
	install_offset[1] = pt.get<int>("tilt.install_offset", 0);
	install_offset[2] = pt.get<int>("zoom.install_offset", 0);
	
	for (int i = 0; i < 3; i++) 
	{
		set_kp(i, kp[i]);
		set_ki(i, ki[i]);
		set_kd(i, kd[i]);
		set_dead_zone(i, dead_zone[i]);
		set_min_limit(i, min_limit[i]);
		set_max_limit(i, max_limit[i]);
		set_pos_offset(i, pos_offset[i]);
		set_pos_scale(i, pos_scale[i]);
		set_install_offset(i, install_offset[i]);
	}

	set_zoom_map_str(zoom_map_str);
	
	return 0;
}

int ptz_track::save_config(const char *config_name)
{
	boost::property_tree::ptree pt;  
    boost::property_tree::ini_parser::read_ini(config_name, pt);
	
	
	pt.put<double>("pan.kp", get_kp(0));
	pt.put<double>("pan.ki", get_ki(0));
	pt.put<double>("pan.kd", get_kd(0));
	pt.put<double>("pan.dead_zone", get_dead_zone(0));
	pt.put<double>("pan.min_limit", get_min_limit(0));
	pt.put<double>("pan.max_limit", get_max_limit(0));
	pt.put<int>("pan.pos_offset", get_pos_offset(0));
	pt.put<double>("pan.pos_scale", get_pos_scale(0));
	pt.put<int>("pan.install_offset", get_install_offset(0));
	
	pt.put<double>("tilt.kp", get_kp(1));
	pt.put<double>("tilt.ki", get_ki(1));
	pt.put<double>("tilt.kd", get_kd(1));
	pt.put<double>("tilt.dead_zone", get_dead_zone(1));
	pt.put<double>("tilt.min_limit", get_min_limit(1));
	pt.put<double>("tilt.max_limit", get_max_limit(1));
	pt.put<int>("tilt.pos_offset", get_pos_offset(1));
	pt.put<double>("tilt.pos_scale", get_pos_scale(1));
	pt.put<int>("tilt.install_offset", get_install_offset(1));
	
	pt.put<double>("zoom.kp", get_kp(2));
	pt.put<double>("zoom.ki", get_ki(2));
	pt.put<double>("zoom.kd", get_kd(2));
	pt.put<double>("zoom.dead_zone", get_dead_zone(2));
	pt.put<double>("zoom.min_limit", get_min_limit(2));
	pt.put<double>("zoom.max_limit", get_max_limit(2));
	pt.put<int>("zoom.pos_offset", get_pos_offset(2));
	pt.put<double>("zoom.pos_scale", get_pos_scale(2));
	pt.put<int>("zoom.install_offset", get_install_offset(2));
	
	string zoom_map_str;
	get_zoom_map_str(zoom_map_str);
	pt.put<string>("zoom.zoom_map", zoom_map_str);

	boost::property_tree::ini_parser::write_ini(config_name, pt);
	
	return 0;
}

int ptz_track::set_pantilt_up(int panSpeed, int tiltSpeed)
{
	return mptz->set_pantilt_up(panSpeed, tiltSpeed);
}

int ptz_track::set_pantilt_upleft(int panSpeed, int tiltSpeed)
{
	return mptz->set_pantilt_upleft(panSpeed, tiltSpeed);
}

int ptz_track::set_pantilt_upright(int panSpeed, int tiltSpeed)
{
	return mptz->set_pantilt_upright(panSpeed, tiltSpeed);
}

int ptz_track::set_pantilt_left(int panSpeed, int tiltSpeed)
{
	return mptz->set_pantilt_left(panSpeed, tiltSpeed);
}

int ptz_track::set_pantilt_right(int panSpeed, int tiltSpeed)
{
	return mptz->set_pantilt_right(panSpeed, tiltSpeed);
}

int ptz_track::set_pantilt_down(int panSpeed, int tiltSpeed)
{
	return mptz->set_pantilt_down(panSpeed, tiltSpeed);
}


int ptz_track::set_pantilt_downleft(int panSpeed, int tiltSpeed)
{
	return mptz->set_pantilt_downleft(panSpeed, tiltSpeed);
}

int ptz_track::set_pantilt_downright(int panSpeed, int tiltSpeed)
{
	return mptz->set_pantilt_downright(panSpeed, tiltSpeed);
}


int ptz_track::set_pantilt_stop(int panSpeed, int tiltSpeed)
{
	return mptz->set_pantilt_stop(panSpeed, tiltSpeed);
}

int ptz_track::set_pantilt_absolute_position(int panSpeed, int tiltSpeed, int pan_position, int tilt_position)
{
	return mptz->set_pantilt_absolute_position(panSpeed, tiltSpeed, pan_position, tilt_position);
}

int ptz_track::set_pantilt_relative_position(int panSpeed, int tiltSpeed, int pan_position, int tilt_position)
{
	return mptz->set_pantilt_relative_position(panSpeed, tiltSpeed, pan_position, tilt_position);
}
	

int ptz_track::set_pantilt_home()
{
	return mptz->set_pantilt_home();
}

int ptz_track::set_zoom_tele_speed(int speed)
{
	return mptz->set_zoom_tele_speed(speed);
}

int ptz_track::set_zoom_wide_speed(int speed)
{
	return mptz->set_zoom_wide_speed(speed);
}

int ptz_track::set_zoom_stop()
{
	return mptz->set_zoom_stop();
}

int ptz_track::set_zoom_value(int value)
{
	return mptz->set_zoom_value(value);
}

int ptz_track::set_focus_far_speed(int speed)
{
	return mptz->set_focus_far_speed(speed);
}

int ptz_track::set_focus_near_speed(int speed)
{
	return mptz->set_focus_near_speed(speed);
}

int ptz_track::set_focus_stop()
{
	return mptz->set_focus_stop();
}

int ptz_track::set_focus_value(int value)
{
	return mptz->set_focus_value(value);
}

int ptz_track::set_datascreen_off()
{
	return mptz->set_datascreen_off();
}

int ptz_track::set_datascreen_on()
{
	return mptz->set_datascreen_on();
}


int ptz_track::get_pantilt_position(int *pan_position, int *tilt_position)
{
	return mptz->get_pantilt_position(pan_position, tilt_position);
}

int ptz_track::get_zoom_value(int *value)
{
	return mptz->get_zoom_value(value);
}

int ptz_track::get_focus_value(int *value)
{
	return mptz->get_focus_value(value);
}

void ptz_track::pid_reset(int id)
{
	mpid[id]->pid_reset();
}

void ptz_track::set_target(int id, float value)
{
	mpid[id]->set_target(value);
}

void ptz_track::set_kp(int id, float value)
{
	mpid[id]->set_kp(value);
}

void ptz_track::set_ki(int id, float value)
{
	mpid[id]->set_ki(value);
}

void ptz_track::set_kd(int id, float value)
{
	mpid[id]->set_kd(value);
}

void ptz_track::set_dead_zone(int id, float value)
{
	mpid[id]->set_dead_zone(value);
}

void ptz_track::set_min_limit(int id, float value)
{
	mpid[id]->set_min_limit(value);
}

void ptz_track::set_max_limit(int id, float value)
{
	mpid[id]->set_max_limit(value);
}

float ptz_track::get_kp(int id)
{
	return mpid[id]->get_kp();
}

float ptz_track::get_ki(int id)
{
	return mpid[id]->get_ki();
}

float ptz_track::get_kd(int id)
{
	return mpid[id]->get_kd();
}

float ptz_track::get_dead_zone(int id)
{
	return mpid[id]->get_dead_zone();
}

float ptz_track::get_min_limit(int id)
{
	return mpid[id]->get_min_limit();
}

float ptz_track::get_max_limit(int id)
{
	return mpid[id]->get_max_limit();
}


void ptz_track::set_pos_offset(int id, int value)
{
	pos_offset[id] = value;
}

void ptz_track::set_pos_scale(int id, float value)
{
	pos_scale[id] = value;
}

void ptz_track::set_zoom_map(vector<pair<int, float> > &value)
{
	zoom_map = value;
}

int ptz_track::set_zoom_map_str(string &value)
{
	vector<string> vecs;
	split(vecs, value, boost::is_any_of(" "), boost::token_compress_on);
	int vs = vecs.size();
	int num = (vs > 0) ? boost::lexical_cast<int>(vecs[0]) : 0;
	
	if ((num > 0) && (vs > (num * 2)))
	{
		zoom_map.resize(0);
		int vn = 1;
		for (int i = 0; i < num; i++)
		{
			pair<int, float> item;
			item.first = boost::lexical_cast<int>(vecs[vn++]);
			item.second = boost::lexical_cast<double>(vecs[vn++]);
			zoom_map.push_back(item);
		}	
		return 0;
	}
	else
	{
		cout << "error: zoom map format!" << endl;
		return -1;
	}
}

void ptz_track::set_install_offset(int id, int value)
{
	install_offset[id] = value;
}

void ptz_track::set_min_pos(int id, int value)
{
	pos_limit[id][0] = value;
}

void ptz_track::set_max_pos(int id, int value)
{
	pos_limit[id][1] = value;
}

int ptz_track::get_pos_offset(int id)
{
	return pos_offset[id];
}

float ptz_track::get_pos_scale(int id)
{
	return pos_scale[id];
}

void ptz_track::get_zoom_map(vector<pair<int, float> > &value)
{
	value = zoom_map;
}

void ptz_track::get_zoom_map_str(string &value)
{
	std::stringstream os;
	os << zoom_map.size() << " ";
	for (int i = 0; i < zoom_map.size(); i++) 
	{
		os << zoom_map[i].first << " " << zoom_map[i].second << " ";
	}	
	value = os.str();
}

int ptz_track::get_install_offset(int id)
{
	return install_offset[id];
}

int ptz_track::get_min_pos(int id)
{
	return pos_limit[id][0];
}

int ptz_track::get_max_pos(int id)
{
	return pos_limit[id][1];
}



























