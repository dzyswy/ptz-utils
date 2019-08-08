#include "fit_calib.h"


fit_calib::fit_calib(ptz_ctl_visca *ptz, daemon_stereo *sensor)
{
	set_degree(FIT_CALIB_GRAPH_COORD, FIT_CALIB_PTZ_PAN, 3);
	set_degree(FIT_CALIB_GRAPH_COORD, FIT_CALIB_PTZ_TILT, 3);
	set_degree(FIT_CALIB_GRAPH_COORD, FIT_CALIB_PTZ_ZOOM, 3);
	
	set_degree(FIT_CALIB_CAMEAR_COORD, FIT_CALIB_PTZ_PAN, 3);
	set_degree(FIT_CALIB_CAMEAR_COORD, FIT_CALIB_PTZ_TILT, 3);
	set_degree(FIT_CALIB_CAMEAR_COORD, FIT_CALIB_PTZ_ZOOM, 3);
	
	set_degree(FIT_CALIB_ROOM_COORD, FIT_CALIB_PTZ_PAN, 3);
	set_degree(FIT_CALIB_ROOM_COORD, FIT_CALIB_PTZ_TILT, 3);
	set_degree(FIT_CALIB_ROOM_COORD, FIT_CALIB_PTZ_ZOOM, 3);
	
	set_degree(FIT_CALIB_BALL_COORD, FIT_CALIB_PTZ_PAN, 3);
	set_degree(FIT_CALIB_BALL_COORD, FIT_CALIB_PTZ_TILT, 3);
	set_degree(FIT_CALIB_BALL_COORD, FIT_CALIB_PTZ_ZOOM, 3);
	
	ptz_ = ptz;
	sensor_ = sensor;
}

int fit_calib::set_degree(int coord_type, int channel_type, int value) 
{
	if ((coord_type < 0) || (coord_type >= FIT_CALIB_MAX_COORD))
		return -1;
	if ((channel_type < 0) || (channel_type >= FIT_CALIB_PTZ_MAX_CHANNEL))
		return -1;
	
	fit_degree_[coord_type][channel_type] = value;
	fit_para_[coord_type][channel_type][0].resize(value);
	fit_para_[coord_type][channel_type][1].resize(value);
}

int fit_calib::get_degree(int coord_type, int channel_type, int &value)
{
	if ((coord_type < 0) || (coord_type >= FIT_CALIB_MAX_COORD))
		return -1;
	if ((channel_type < 0) || (channel_type >= FIT_CALIB_PTZ_MAX_CHANNEL))
		return -1;
	
	value = fit_degree_[coord_type][channel_type];
	return 0;
}


int fit_calib::add_sample(int x, int y)
{
	int ret;
	
	int pan_position, tilt_position, zoom_position;
	
	ret = ptz_->get_pantilt_position(&pan_position, &tilt_position);
	if (ret < 0)
		return -1;
	
	ret = ptz_->get_zoom_position(&zoom_position);
	if (ret < 0)
		return -1;
	
	struct fit_calib_ptz_pos ptz_pos;
	ptz_pos.pos[FIT_CALIB_PTZ_PAN] = (float)pan_position;
	ptz_pos.pos[FIT_CALIB_PTZ_TILT] = (float)tilt_position;
	ptz_pos.pos[FIT_CALIB_PTZ_ZOOM] = (float)zoom_position;
	
	
	struct point_space_status space_status;
	ret = sensor_->get_point_space_status(x, y, space_status);
	if (ret < 0)
		return -1;
	
	if (space_status.d == 0)
		return -2;
	
	struct fit_calib_sensor_pos sensor_pos;
	sensor_pos.pos[FIT_CALIB_GRAPH_COORD][FIT_CALIB_PTZ_PAN] = space_status.x;
	sensor_pos.pos[FIT_CALIB_GRAPH_COORD][FIT_CALIB_PTZ_TILT] = space_status.y;
	sensor_pos.pos[FIT_CALIB_GRAPH_COORD][FIT_CALIB_PTZ_ZOOM] = space_status.d;
	
	sensor_pos.pos[FIT_CALIB_CAMEAR_COORD][FIT_CALIB_PTZ_PAN] = space_status.xcm;
	sensor_pos.pos[FIT_CALIB_CAMEAR_COORD][FIT_CALIB_PTZ_TILT] = space_status.ycm;
	sensor_pos.pos[FIT_CALIB_CAMEAR_COORD][FIT_CALIB_PTZ_ZOOM] = space_status.zcm;
	
	sensor_pos.pos[FIT_CALIB_ROOM_COORD][FIT_CALIB_PTZ_PAN] = space_status.xtcm;
	sensor_pos.pos[FIT_CALIB_ROOM_COORD][FIT_CALIB_PTZ_TILT] = space_status.ytcm;
	sensor_pos.pos[FIT_CALIB_ROOM_COORD][FIT_CALIB_PTZ_ZOOM] = space_status.ztcm;
	
	sensor_pos.pos[FIT_CALIB_BALL_COORD][FIT_CALIB_PTZ_PAN] = space_status.xa;
	sensor_pos.pos[FIT_CALIB_BALL_COORD][FIT_CALIB_PTZ_TILT] = space_status.ya;
	sensor_pos.pos[FIT_CALIB_BALL_COORD][FIT_CALIB_PTZ_ZOOM] = space_status.r;
	
	fit_samples_.push_back(make_pair(ptz_pos, sensor_pos));
	return 0;
}


int fit_calib::fit_samples()
{
	int ret = 0;
	int sample_num = fit_samples_.size();
	float *first = new float [sample_num];
	float *second = new float [sample_num];
	
	for (int i = 0; i < FIT_CALIB_MAX_COORD; i++)
	{
		for (int j = 0; j < FIT_CALIB_PTZ_MAX_CHANNEL; j++)
		{
			for (int k = 0; k < sample_num; k++)
			{
				first[k] = fit_samples_.first.pos[j];
				second[k] = fit_samples_.second.pos[i][j];
			}	
			ret = calc_fit_para(first, second, sample_num, fit_para_[i][j][0]);
			if (ret < 0)
				break;
			ret = calc_fit_para(second, first, sample_num, fit_para_[i][j][1]);
			if (ret < 0)
				break;
		}
		if (ret < 0)
			break;		
	}

	delete first;
	delete second;
	return 0;
}


int fit_calib::calc_fit_para(float *first, float *second, int count, vector<double> &para)
{
	int degree = para.size() - 1;
	Polynomial pf;
	pf.setAttribute(degree, false, 1.0);
	pf.setSample(first, second, count, false, NULL);
	if (!pf.process()) {
		return -1;
	}
	pf.print();
	for (int i = 0; i < para.size(); i++)
	{
		para[i] = para.getResult(i);
	}
	return 0;
}



int fit_calib::set_fit_para(vector<double> value, int coord_type, int channel_type, int pn)
{
	if ((coord_type < 0) || (coord_type >= FIT_CALIB_MAX_COORD))
		return -1;
	if ((channel_type < 0) || (channel_type >= FIT_CALIB_PTZ_MAX_CHANNEL))
		return -1;
	if ((pn < 0) ||(pn > 1))
		return -1;
	
	if (value.size() != fit_degree_[coord_type][channel_type].size())
		return -1;
	
	fit_para_[coord_type][channel_type][pn] = value;
	
	return 0;
}

int fit_calib::get_fit_para(vector<double> &value, int coord_type, int channel_type, int pn)
{
	if ((coord_type < 0) || (coord_type >= FIT_CALIB_MAX_COORD))
		return -1;
	if ((channel_type < 0) || (channel_type >= FIT_CALIB_PTZ_MAX_CHANNEL))
		return -1;
	if ((pn < 0) ||(pn > 1))
		return -1;
	
//	value.resize(fit_degree_[coord_type][channel_type].size());
	value = fit_para_[coord_type][channel_type][pn];
	return 0;
}


float fit_calib::curve_change(float value, int coord_type, int channel_type, int pn)
{
	int degree = fit_degree_[coord_type][channel_type];
	double sum = fit_para_[coord_type][channel_type][pn][degree];
	for (int i = 0; i < degree; i++)
	{
		sum += pow(value, degree - i) * fit_para_[coord_type][channel_type][pn][i];
	}	
	return (float)sum;
}










