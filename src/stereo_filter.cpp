#include "stereo_filter.h"
#include "stream_receiver.h"


static void handle_filter(void *arg)
{
	stereo_filter *dirt = (stereo_filter *)arg;
	dirt->filter_process();
}


stereo_filter::stereo_filter(daemon_stereo *stereo)
{
	stereo_ = stereo;
	
	target_count_ = 0;
	memset(&target_position_, 0, sizeof(target_position_));
	
	going = 1;
	thread_ = new std::thread(handle_filter, this);
}

stereo_filter::~stereo_filter()
{
	going = 0;
	thread_.join();
	delete thread_;
}



void stereo_filter::filter_process()
{
	
	while(going)
	{
		vector<struct http_output_detect_box> now_detect_boxes;
		ret = stereo_->get_detect_boxes(now_detect_boxes, 5);
		if (ret < 0) {
			now_detect_boxes.clear();
		}
		
		int now_count = now_detect_boxes.size();
		if (now_count == 0) {
			now_target_number_state_count_[STEREO_NO_TARGET]++;
			now_target_number_state_count_[STEREO_SINGLE_TARGET] = 0;
			now_target_number_state_count_[STEREO_MULTI_TARGET] = 0;
		} else if (now_count == 1) {
			now_target_number_state_count_[STEREO_NO_TARGET] = 0;
			now_target_number_state_count_[STEREO_SINGLE_TARGET]++;
			now_target_number_state_count_[STEREO_MULTI_TARGET] = 0;
		} else {
			now_target_number_state_count_[STEREO_NO_TARGET] = 0;
			now_target_number_state_count_[STEREO_SINGLE_TARGET] = 0;
			now_target_number_state_count_[STEREO_MULTI_TARGET]++;
		}
		
		switch(target_number_state_)
		{
			case STEREO_NO_TARGET:
			{
				if (now_target_number_state_count_[STEREO_SINGLE_TARGET] > single_count[]) {
					target_number_state_ = STEREO_SINGLE_TARGET;
				} else if (now_target_number_state_count_[STEREO_SINGLE_TARGET] > min_single_count) {
					target_number_state_ = STEREO_MULTI_TARGET;
				}	
			}break;
			case STEREO_SINGLE_TARGET:
			{
				if (now_target_number_state_count_[STEREO_SINGLE_TARGET] > min_single_count) {
					target_number_state_ = STEREO_SINGLE_TARGET;
				} else if (now_target_number_state_count_[STEREO_SINGLE_TARGET] > min_single_count) {
					target_number_state_ = STEREO_MULTI_TARGET;
				}	
			}break;
			case STEREO_MULTI_TARGET:
			{
				if (now_target_number_state_count_[STEREO_SINGLE_TARGET] > min_single_count) {
					target_number_state_ = STEREO_SINGLE_TARGET;
				} else if (now_target_number_state_count_[STEREO_SINGLE_TARGET] > min_single_count) {
					target_number_state_ = STEREO_MULTI_TARGET;
				}	
			}break;
			default:
				break;
			
		}
		
		
		if (target_position_.d == 0)
		{
			memcpy();
		}
		else 
		{
			
		}	
			
	}
}

















































