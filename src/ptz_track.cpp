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
	
	track_mode_ = 0;
	track_mask_ = PTZ_TRACK_PTZ_ALL_MASK;
	track_coord_ = FIT_CALIB_BALL_COORD;
	lock_time_ = 10;
	lock_track_state_ = 
	
	
	going = 1;
	track_thread_ = new std::thread(handle_track, this);
}

ptz_track::~ptz_track()
{
	going = 0;
	if (track_thread_)
	{
		track_thread_.join();
		delete track_thread_;
		track_thread_ = NULL;
	}	
}

void ptz_track::track_process()
{
	int ret;
	int track_mode = 0;
	while(going)
	{
		struct timeval tv[2];
		gettimeofday(&tv[0], NULL);
		
		if ((track_mode != 0) && (track_mode_ == 0)) {
			ptz_->set_pantilt_stop();
			ptz_->set_zoom_stop();
		}
		track_mode = track_mode_;
 
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
		
		
		struct http_output_detect_box target_position = traget_->get_target_position();
		int position_statble_count = target_->get_position_statble_count();
		 
		/*
		 * 1.连续跟踪模式
		 * 2.跟踪锁定模式
		*/
		
		if ((track_mode) && (target_position.d))
		{
			float target_pos[PTZ_TRACK_PTZ_MAX_CHANNEL] = {0};
			switch(track_coord_)
			{
				case FIT_CALIB_GRAPH_COORD:
				{
					target_pos[PTZ_TRACK_PTZ_PAN] = calib_->curve_change(target_position.x, track_coord_, FIT_CALIB_PTZ_PAN, 0);
					target_pos[PTZ_TRACK_PTZ_TILT] = calib_->curve_change(target_position.y, track_coord_, FIT_CALIB_PTZ_TILT, 0);
					target_pos[PTZ_TRACK_PTZ_ZOOM] = calib_->curve_change(target_position.d, track_coord_, FIT_CALIB_PTZ_ZOOM, 0);
				}break;
				case FIT_CALIB_CAMEAR_COORD:
				{
					target_pos[PTZ_TRACK_PTZ_PAN] = calib_->curve_change(target_position.xcm, track_coord_, FIT_CALIB_PTZ_PAN, 0);
					target_pos[PTZ_TRACK_PTZ_TILT] = calib_->curve_change(target_position.ycm, track_coord_, FIT_CALIB_PTZ_TILT, 0);
					target_pos[PTZ_TRACK_PTZ_ZOOM] = calib_->curve_change(target_position.zcm, track_coord_, FIT_CALIB_PTZ_ZOOM, 0);
				}break;
				case FIT_CALIB_ROOM_COORD:
				{
					target_pos[PTZ_TRACK_PTZ_PAN] = calib_->curve_change(target_position.xtcm, track_coord_, FIT_CALIB_PTZ_PAN, 0);
					target_pos[PTZ_TRACK_PTZ_TILT] = calib_->curve_change(target_position.ytcm, track_coord_, FIT_CALIB_PTZ_TILT, 0);
					target_pos[PTZ_TRACK_PTZ_ZOOM] = calib_->curve_change(target_position.ztcm, track_coord_, FIT_CALIB_PTZ_ZOOM, 0);
				}break;
				case FIT_CALIB_BALL_COORD:
				{
					target_pos[PTZ_TRACK_PTZ_PAN] = calib_->curve_change(target_position.xa, track_coord_, FIT_CALIB_PTZ_PAN, 0);
					target_pos[PTZ_TRACK_PTZ_TILT] = calib_->curve_change(target_position.ya, track_coord_, FIT_CALIB_PTZ_TILT, 0);
					target_pos[PTZ_TRACK_PTZ_ZOOM] = calib_->curve_change(target_position.r, track_coord_, FIT_CALIB_PTZ_ZOOM, 0);
				}break;
				default:
					break;
			}
			
			bool pan_mask = (track_mask_ & PTZ_TRACK_PTZ_PAN_MASK) ? true : false;
			bool tilt_mask = (track_mask_ & PTZ_TRACK_PTZ_TILT_MASK) ? true : false;
			bool zoom_mask = (track_mask_ & PTZ_TRACK_PTZ_ZOOM_MASK) ? true : false;
			bool pan_move = (fabs(target_pos[PTZ_TRACK_PTZ_PAN] - ptz_pos[PTZ_TRACK_PTZ_PAN]) < dead_zone_[PTZ_TRACK_PTZ_PAN]) ? false : true;
			bool tilt_move = (fabs(target_pos[PTZ_TRACK_PTZ_TILT] - ptz_pos[PTZ_TRACK_PTZ_TILT]) < dead_zone_[PTZ_TRACK_PTZ_TILT]) ? false : true;
			bool zoom_move = (fabs(target_pos[PTZ_TRACK_PTZ_ZOOM] - ptz_pos[PTZ_TRACK_PTZ_ZOOM]) < dead_zone_[PTZ_TRACK_PTZ_ZOOM]) ? false : true;
			bool move_pan = (pan_mask && pan_move) ? true : false;
			bool move_tilt = (tilt_mask && tilt_move) ? true : false;
			bool move_zoom = (zoom_mask && zoom_move) ? true : false;
			
			
			if (track_mode == PTZ_TRACK_CONTINUOUS_TRACK_MODE)
			{
				float err[PTZ_TRACK_PTZ_MAX_CHANNEL];
				for (int i = 0; i < PTZ_TRACK_PTZ_MAX_CHANNEL; i++)
				{
					err[i] = target_pos[i] - ptz_pos[i];
				}
				
				float pan_speed = pid_[PTZ_TRACK_PTZ_PAN].compute(err[PTZ_TRACK_PTZ_PAN]);
				float tilt_speed = pid_[PTZ_TRACK_PTZ_TILT].compute(err[PTZ_TRACK_PTZ_TILT]);
				float zoom_speed = pid_[PTZ_TRACK_PTZ_ZOOM].compute(err[PTZ_TRACK_PTZ_ZOOM]);
				
				int panSpeed = pan_mask ? std::round(pan_speed) : 0;
				int tiltSpeed = tilt_mask ? std::round(tilt_speed) : 0;
				int zoomSpeed = zoom_mask ? std::round(zoom_speed) : 0;
				
				if ((panSpeed == 0) && (tiltSpeed == 0)) 
				{
					ptz_->set_pantilt_stop(panSpeed, tiltSpeed);
				}	
				else 
				{
					if ((panSpeed != 0) && (tiltSpeed != 0)) 
					{
						if ((pan_out > 0) && (tilt_out > 0))
						{
							ptz_->set_pantilt_upright(panSpeed, tiltSpeed);
						}	
						else if ((pan_out > 0) && (tilt_out < 0))
						{
							ptz_->set_pantilt_downright(panSpeed, tiltSpeed);
						}
						else if ((pan_out < 0) && (tilt_out > 0))
						{
							ptz_->set_pantilt_upleft(panSpeed, tiltSpeed);
						}	
						else if ((pan_out < 0) && (tilt_out < 0))
						{
							ptz_->set_pantilt_downleft(panSpeed, tiltSpeed);
						}	
					}
					else if ((panSpeed != 0) && (tiltSpeed == 0))
					{
						if (pan_out < 0) {
							ptz_->set_pantilt_left(panSpeed, tiltSpeed);
						}
						else {
							ptz_->set_pantilt_right(panSpeed, tiltSpeed);
						}
					}	
					else if ((panSpeed == 0) && (tiltSpeed != 0))
					{
						if (tilt_out < 0) {
							ptz_->set_pantilt_down(panSpeed, tiltSpeed);
						}
						else {
							ptz_->set_pantilt_up(panSpeed, tiltSpeed);
						}
					}	
				}
				
				if (zoomSpeed == 0)
				{
					ptz_->set_zoom_stop();
				}	
				else
				{
					if (zoom_out > 0)
					{
						ptz_->set_zoom_tele_speed(zoomSpeed - 1);
					}	
					else
					{
						ptz_->set_zoom_wide_speed(zoomSpeed - 1);
					}		
				}	
				
			}	
			else if (track_mode == PTZ_TRACK_LOCK_TRACK_MODE)
			{
				switch(lock_track_state_)
				{
					case PTZ_TRACK_TRACK_UNLOCKED:
					{
						if (position_statble_count) 
						{
							if (move_pan || move_tilt)
							{
								int pan_position = move_pan ? target_pos[PTZ_TRACK_PTZ_PAN] : ptz_pos[PTZ_TRACK_PTZ_PAN];
								int tilt_position = move_tilt ? target_pos[PTZ_TRACK_PTZ_TILT] : ptz_pos[PTZ_TRACK_PTZ_TILT];
								ptz_->set_pantilt_absolute_position(&pan_position, &tilt_position, ptz_->get_max_pan_speed(), ptz_->get_max_tilt_speed());
							}	
							
							if (move_zoom)
							{
								int zoom_position = target_pos[PTZ_TRACK_PTZ_ZOOM];
								ptz_->set_zoom_absolute_position(&zoom_position, ptz_->get_max_zoom_speed());
							}	
							
							lock_track_state_ = PTZ_TRACK_TRACK_LOCKED;
							lock_timeout_ = time(0) + lock_time_;
						}
							
					}break;
					case PTZ_TRACK_TRACK_LOCKED:
					{
						if (time(0) > lock_timeout_)
						{
							lock_track_state_ = PTZ_TRACK_TRACK_UNLOCKED;
						}	
					}break;
					default:
						break;
					
				}
			}
		}	
		
		gettimeofday(&tv[1], NULL);
		
		float time_used = (tv[1].tv_sec - tv[0].tv_sec) * 1000 + (tv[1].tv_usec - tv[0].tv_usec) / 1000;
		float time_leave = period_ - time_used;
		if (time_leave > 1)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(time_leave));
		}	 
	}	
}









