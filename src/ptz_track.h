#ifndef __PTZ_TRACK_H
#define __PTZ_TRACK_H


#include "pid_inc.h"
#include "fit_calib.h"
#include "stereo_filter.h"
#include "ptz_ctl_visca.h"


enum ptz_track_ptz_channel_type
{
	PTZ_TRACK_PTZ_PAN = 0,
	PTZ_TRACK_PTZ_TILT = 1,
	PTZ_TRACK_PTZ_ZOOM = 2,
	PTZ_TRACK_PTZ_MAX_CHANNEL = 3,
};

enum ptz_track_track_mode_type
{
	PTZ_TRACK_CONTINUOUS_TRACK_MODE = 1,
	PTZ_TRACK_LOCK_TRACK_MODE = 2,
};

#define PTZ_TRACK_PTZ_PAN_MASK		(1<<0)
#define PTZ_TRACK_PTZ_TILT_MASK		(1<<1)
#define PTZ_TRACK_PTZ_ZOOM_MASK		(1<<2)
#define PTZ_TRACK_PTZ_ALL_MASK		(0x7)

class ptz_track
{
public:
	ptz_track(ptz_ctl_visca *ptz, stereo_filter *target, fit_calib *calib, float period = 100);

	void set_track_mode(int value)
	{
		track_mode_ = value;
	}
	
	
	
	
	void track_process();
	
protected:
	ptz_ctl_visca *ptz_;
	stereo_filter *target_;
	fit_calib *calib_;

	float period_;
	int track_mode_;
	int track_mask_;
	int lock_count_;
	
	int going;
	std::mutex mux_;
//	std::condition_variable cond_;
	std::thread *track_thread_;
	
	pid_inc pid_[PTZ_TRACK_PTZ_MAX_CHANNEL];
};
















#endif


















