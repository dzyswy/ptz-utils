#include "stereo_director.h"
#include "stream_receiver.h"


static void handle_director(void *arg)
{
	stereo_director *dirt = (stereo_director *)arg;
	dirt->director_process();
}


stereo_director::stereo_director(daemon_stereo *stereo)
{
	stereo_ = stereo;
	
	target_count_ = 0;
	memset(&target_position_, 0, sizeof(target_position_));
	
	going = 1;
	thread_ = new std::thread(handle_director, this);
}

stereo_director::~stereo_director()
{
	going = 0;
	thread_.join();
	delete thread_;
}



void stereo_director::director_process()
{
	
	while(going)
	{
		vector<struct http_output_detect_box> detect_boxes;
		ret = stereo_->get_detect_boxes(detect_boxes, 5);
		if (ret < 0) {
			
		}
			
	}
}

















































