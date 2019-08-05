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






int ptz_ctl_visca::add_sample(int dim, int ch, float first, float second)
{
	if ((dim >= PTZ_MAX_DIM) || (ch >= PTZ_MAX_CHANNEL))
		return -1;
	
	fit_samples_[dim][ch].push_back(make_pair(first, second));
	return 0;
}



int ptz_ctl_visca::calc_fit_para(vector<pair<float, float> > &samples, vector<float> &para_p, vector<float> &para_n, int degree)
{
	int sample_num = samples.size();
	float *first = new float [sample_num];
	float *second = new float [sample_num];
	
	for (int i = 0; i < sample_num; i++)
	{
		fisrt[i] = samples[i].first;
		second[i] = samples[i].second;
	}	
	
	Polynomial pf_p;
	pf_p.setAttribute(degree, false, 1.0);
	pf_p.setSample(first, second, sample_num, false, NULL);
	if (!pf_p.process()) {
		delete first;
		delete second;
		return -1;
	}
	pf_p.print();
	
	Polynomial pf_n;
	pf_n.setAttribute(degree, false, 1.0);
	pf_n.setSample(second, first, sample_num, false, NULL);
	if (!pf_n.process()) {
		delete first;
		delete second;
		return -1;
	}
	pf_n.print();
	
	para_p.clear();
	para_n.clear();
	for (int i = 0; i < degree; i++)
	{
		para_p.push_back(pf_p.getResult(i));
		para_n.push_back(pf_n.getResult(i));
	}	
	
	return 0;
}

float ptz_ctl_visca::calc_fit_value(vector<float> &para, float x)
{
	int degree = para.size() - 1;
	double sum = para[para.size() - 1];
	for (int i = 0; i < degree; i++)
	{
		sum += pow(x, degree - i) * para[i];
	}
	return (float)sum;
}










