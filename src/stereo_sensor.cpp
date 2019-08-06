#include "stereo_sensor.h"
#include "stream_receiver.h"



stereo_sensor::stereo_sensor()
{
	stream_ = NULL;
}


int stereo_sensor::open_device(const char *ip, int stream_port, int cmd_port, int mode)
{
	if (stream_) {
		printf("already open stereo sensor!\n");
		return -1;
	}
	
	stream_ = new stream_receiver(ip, stream_port, mode);
	
	cmd_ = new camera_command(ip, cmd_port);
	
	return 0;
	
}

int stereo_sensor::close_device()
{
	delete cmd_;
	delete stream_;
	return 0;
}



int stereo_sensor::set_detect_mode(int value)
{
	int ret;
	int detect_mode;
	int track_mode;
	if (value)
	{
		detect_mode = 1;
		ret = cmd_->set_value("set_detect_mode", detect_mode);
		if (ret < 0)
			return -1;
		
		track_mode = 1;
		ret = cmd_->set_value("set_track_mode", track_mode);
		if (ret < 0)
			return -1;
		
	}
	else
	{
		detect_mode = 0;
		ret = cmd_->set_value("set_detect_mode", detect_mode);
		if (ret < 0)
			return -1;
		
		track_mode = 0;
		ret = cmd_->set_value("set_track_mode", track_mode);
		if (ret < 0)
			return -1;	
	}

	return 0;
}


int stereo_sensor::get_point_space_status(int x, int y, struct point_space_status &status)
{
	return cmd_->get_point_space_status(x, y, status);
}


















