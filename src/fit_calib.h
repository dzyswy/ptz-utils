#ifndef __FIT_CALIB_H
#define __FIT_CALIB_H



#include "ptz_ctl_visca.h"
#include "daemon_stereo.h"




enum fit_calib_coord_type
{
	FIT_CALIB_GRAPH_COORD = 0,
	FIT_CALIB_CAMEAR_COORD = 1,
	FIT_CALIB_ROOM_COORD = 2,
	FIT_CALIB_BALL_COORD = 3,
	FIT_CALIB_MAX_COORD = 4,
};

enum fit_calib_ptz_channel_type
{
	FIT_CALIB_PTZ_PAN = 0,
	FIT_CALIB_PTZ_TILT = 1,
	FIT_CALIB_PTZ_ZOOM = 2,
	FIT_CALIB_PTZ_MAX_CHANNEL = 3,
};


struct fit_calib_ptz_pos
{
	float pos[FIT_CALIB_PTZ_MAX_CHANNEL];
};

struct fit_calib_sensor_pos
{
	float pos[FIT_CALIB_MAX_COORD][FIT_CALIB_PTZ_MAX_CHANNEL];
};


class fit_calib
{
public:
	fit_calib(ptz_ctl_visca *ptz, daemon_stereo *sensor);

	int set_degree(int coord_type, int channel_type, int value);
	int get_degree(int coord_type, int channel_type, int &value);
	
	
	int add_sample(int x, int y);
	void clear_samples()
	{
		fit_samples_.clear();
	}
	int fit_samples();
	int get_sample_number() 
	{
		return fit_samples_.size();
	}
	int get_sample(pair<struct fit_calib_ptz_pos, struct fit_calib_sensor_pos> &value, int id)
	{
		if ((id < 0) || (id >= fit_samples_.size()))
			return -1;
		value = fit_samples_[id];
		return 0;
	}
	
	int set_fit_para(vector<double> value, int coord_type, int channel_type, int pn);
	int get_fit_para(vector<double> &value, int coord_type, int channel_type, int pn);
	
	float curve_change(float value, int coord_type, int channel_type, int pn);
		

protected:
	int calc_fit_para(float *first, float *second, int count, vector<double> &para);
	
	
protected:	
	ptz_ctl_visca *ptz_;
	daemon_stereo *sensor_;

	
	vector<pair<struct fit_calib_ptz_pos, struct fit_calib_sensor_pos> > fit_samples_;
	
	int fit_degree_[FIT_CALIB_MAX_COORD][FIT_CALIB_PTZ_MAX_CHANNEL];
	vector<double> fit_para_[FIT_CALIB_MAX_COORD][FIT_CALIB_PTZ_MAX_CHANNEL][2];
	
	
};















#endif

