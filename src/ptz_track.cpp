#include "ptz_track.h"


static void handle_track(void *arg)
{
	ptz_track *tk = (ptz_track *)arg;
	tk->track_process();	
}



ptz_track::ptz_track(ptz_ctl_visca *ptz, stereo_filter *target, fit_calib *calib, float period)
{
	ptz_ = ptz;
	target_ = target;
	calib_ = calib;
	period_ = period;
	
	
	going = 1;
	track_thread_ = new std::thread(handle_track, this);
}



void ptz_track::track_process()
{
	int ret;
	int track_mode = 0;
	while(1)
	{
		if ((track_mode != 0) && (track_mode_ == 0)) {
			ptz_->set_pantilt_stop();
			ptz_->set_zoom_stop();
		}
		track_mode = track_mode_;
		
		struct http_output_detect_box target_position = traget_->get_target_position();
		int position_stable = target_->get_position_stable();
		
		int ptz_pos[PTZ_TRACK_PTZ_MAX_CHANNEL];
		ret = ptz_->get_pantilt_position(&ptz_pos[PTZ_TRACK_PTZ_PAN], &ptz_pos[PTZ_TRACK_PTZ_TILT]);
		if (ret < 0) {
			printf("Failed to get pan tilt position!\n");
			std::this_thread::sleep_for(std::chrono::seconds(3));
			continue;
		}
		ret = ptz_->get_zoom_position(&ptz_pos[PTZ_TRACK_PTZ_ZOOM]);
		if (ret < 0) {
			printf("Failed to get zoom position!\n");
			std::this_thread::sleep_for(std::chrono::seconds(3));
			continue;
		}
		
		float target_pos[PTZ_TRACK_PTZ_MAX_CHANNEL] = {0};
		switch(coord_type_)
		{
			case FIT_CALIB_GRAPH_COORD:
			{
				target_pos[PTZ_TRACK_PTZ_PAN] = calib_->curve_change(target_position.x, coord_type_, FIT_CALIB_PTZ_PAN, 0);
				target_pos[PTZ_TRACK_PTZ_TILT] = calib_->curve_change(target_position.y, coord_type_, FIT_CALIB_PTZ_TILT, 0);
				target_pos[PTZ_TRACK_PTZ_ZOOM] = calib_->curve_change(target_position.d, coord_type_, FIT_CALIB_PTZ_ZOOM, 0);
			}break;
			case FIT_CALIB_CAMEAR_COORD:
			{
				target_pos[PTZ_TRACK_PTZ_PAN] = calib_->curve_change(target_position.xcm, coord_type_, FIT_CALIB_PTZ_PAN, 0);
				target_pos[PTZ_TRACK_PTZ_TILT] = calib_->curve_change(target_position.ycm, coord_type_, FIT_CALIB_PTZ_TILT, 0);
				target_pos[PTZ_TRACK_PTZ_ZOOM] = calib_->curve_change(target_position.zcm, coord_type_, FIT_CALIB_PTZ_ZOOM, 0);
			}break;
			case FIT_CALIB_ROOM_COORD:
			{
				target_pos[PTZ_TRACK_PTZ_PAN] = calib_->curve_change(target_position.xtcm, coord_type_, FIT_CALIB_PTZ_PAN, 0);
				target_pos[PTZ_TRACK_PTZ_TILT] = calib_->curve_change(target_position.ytcm, coord_type_, FIT_CALIB_PTZ_TILT, 0);
				target_pos[PTZ_TRACK_PTZ_ZOOM] = calib_->curve_change(target_position.ztcm, coord_type_, FIT_CALIB_PTZ_ZOOM, 0);
			}break;
			case FIT_CALIB_BALL_COORD:
			{
				target_pos[PTZ_TRACK_PTZ_PAN] = calib_->curve_change(target_position.xa, coord_type_, FIT_CALIB_PTZ_PAN, 0);
				target_pos[PTZ_TRACK_PTZ_TILT] = calib_->curve_change(target_position.ya, coord_type_, FIT_CALIB_PTZ_TILT, 0);
				target_pos[PTZ_TRACK_PTZ_ZOOM] = calib_->curve_change(target_position.r, coord_type_, FIT_CALIB_PTZ_ZOOM, 0);
			}break;
			default:
				break;
		}
		
		float err[PTZ_TRACK_PTZ_MAX_CHANNEL];
		for (int i = 0; i < PTZ_TRACK_PTZ_MAX_CHANNEL; i++)
		{
			err[i] = target_pos[i] - ptz_pos[i];
		}	

		/*
		 * 1.连续跟踪模式
		 * 2.跟踪锁定模式
		*/
		
		if ((track_mode) && (target_position.d))
		{
			if (track_mode == PTZ_TRACK_CONTINUOUS_TRACK_MODE)
			{
				
			}	
			else if (track_mode == PTZ_TRACK_LOCK_TRACK_MODE)
			{
				switch(lock_track_state)
				{
					case PTZ_TRACK_TRACK_LOCKED:
					{
						
					}break;
					
					case PTZ_TRACK_TRACK_UNLOCKED:
					{
						if (position_stable) 
						{
							if ((track_mask_ & PTZ_TRACK_PTZ_PAN_MASK) || (track_mask_ & PTZ_TRACK_PTZ_TILT_MASK))
							{
								int pan_position = (track_mask_ & PTZ_TRACK_PTZ_PAN_MASK) ? target_pos[PTZ_TRACK_PTZ_PAN] : ptz_pos[PTZ_TRACK_PTZ_PAN];
								int tilt_position = (track_mask_ & PTZ_TRACK_PTZ_TILT_MASK) ? target_pos[PTZ_TRACK_PTZ_TILT] : ptz_pos[PTZ_TRACK_PTZ_TILT];
								ptz_->set_pantilt_absolute_position(&pan_position, &tilt_position, ptz_->get_max_pan_speed(), ptz_->get_max_tilt_speed());
							}	
							
							if (track_mask_ & PTZ_TRACK_PTZ_ZOOM_MASK)
							{
								int zoom_position = target_pos[PTZ_TRACK_PTZ_ZOOM];
								ptz_->set_zoom_absolute_position(&zoom_position, ptz_->get_max_zoom_speed());
							}	
							
							lock_track_state = PTZ_TRACK_TRACK_LOCKED;
						}
							
					}break;
					default:
						break;
					
				}
			}
		}	
		
	}	
}









