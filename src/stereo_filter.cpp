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
	
	fast_change_count_ = 2;
	slow_change_count_ = 4;
	
	target_state_ = STEREO_FILTER_NO_TARGET;
	memset(&target_position_, 0, sizeof(target_position_));
	
	memset(&now_target_state_count_, 0, sizeof(now_target_state_count_));
	
	position_stable_ = 0;
	stable_dist_ = 100;
	target_position_buffer_.clear();
	
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
			now_target_state_count_[STEREO_FILTER_NO_TARGET]++;
			now_target_state_count_[STEREO_FILTER_SINGLE_TARGET] = 0;
			now_target_state_count_[STEREO_FILTER_MULTI_TARGET] = 0;
		} else if (now_count == 1) {
			now_target_state_count_[STEREO_FILTER_NO_TARGET] = 0;
			now_target_state_count_[STEREO_FILTER_SINGLE_TARGET]++;
			now_target_state_count_[STEREO_FILTER_MULTI_TARGET] = 0;
		} else {
			now_target_state_count_[STEREO_FILTER_NO_TARGET] = 0;
			now_target_state_count_[STEREO_FILTER_SINGLE_TARGET] = 0;
			now_target_state_count_[STEREO_FILTER_MULTI_TARGET]++;
		}
		
		
		
		/*
		 * 1.目标状态判断
		 * 	|-根据目标计数的持续状态和当前状态进行切换。
		 *	|-避免状态的频繁切换。
		 * 2.目标位置确定
		 *	|-跟踪上一帧的相同ID目标。
		 * 	|-当没有检测到上一帧ID目标时，选择与上个ID目标直线距离最近的目标。
		 *	|-当前没有新目标出现时，维持上一帧的目标不变。
		 *	|-避免镜头大浮动摆动。
		 * 3.目标位置是否稳定
		 *	|-连续3帧之间的直线距离小于阈值，认为目标位置稳定。
		*/
		{
			std::unique_lock<std::mutex> lock(mux_);
			
			switch(target_state_)
			{
				case STEREO_FILTER_NO_TARGET:
				{
					if (now_target_state_count_[STEREO_FILTER_SINGLE_TARGET] >= fast_change_count_) {
						target_state_ = STEREO_FILTER_SINGLE_TARGET;
					} else if (now_target_state_count_[STEREO_FILTER_MULTI_TARGET] >= fast_change_count_) {
						target_state_ = STEREO_FILTER_MULTI_TARGET;
					}	
				}break;
				case STEREO_FILTER_SINGLE_TARGET:
				{
					if (now_target_state_count_[STEREO_FILTER_NO_TARGET] >= slow_change_count_) {
						target_state_ = STEREO_FILTER_NO_TARGET;
					} else if (now_target_state_count_[STEREO_FILTER_MULTI_TARGET] >= slow_change_count_) {
						target_state_ = STEREO_FILTER_MULTI_TARGET;
					}	
				}break;
				case STEREO_FILTER_MULTI_TARGET:
				{
					if (now_target_state_count_[STEREO_FILTER_SINGLE_TARGET] >= slow_change_count_) {
						target_state_ = STEREO_FILTER_SINGLE_TARGET;
					} else if (now_target_state_count_[STEREO_FILTER_NO_TARGET] >= slow_change_count_) {
						target_state_ = STEREO_FILTER_NO_TARGET;
					}	
				}break;
				default:
					break;
				
			}
			
			if (now_detect_boxes.size())
			{
				int pre_id = target_position_.id;
				int min_id = -1;
				int pre_offset = -1, min_offset = 0;
				for (int i = 0; i < now_count; i++)
				{
					if (pre_id == now_detect_boxes[i].id) {
						pre_offset = i;
						break;
					}
						
					if ((min_id == -1) || (now_detect_boxes[i].id < min_id)) {
						min_offset = i;
					}
				}
				
				int offset = (pre_offset >= 0) ? pre_offset : min_offset;
				memcpy(&target_position_, &now_detect_boxes[offset], sizeof(target_position_));
			}
			
			if (target_position_buffer_.size() == STEREO_FILTER_TARGET_POSITION_NUMBER)
				target_position_buffer_.pop_front();
			
			target_position_buffer_.push_back(target_position_);
			
			float dist = -1;
			if (target_position_buffer_.size() == STEREO_FILTER_TARGET_POSITION_NUMBER)
			{
				float x[STEREO_FILTER_TARGET_POSITION_NUMBER], y[STEREO_FILTER_TARGET_POSITION_NUMBER], z[STEREO_FILTER_TARGET_POSITION_NUMBER];
				for (int i = 0; i < STEREO_FILTER_TARGET_POSITION_NUMBER; i++)
				{
					x[i] = target_position_buffer_[i].xcm;
					y[i] = target_position_buffer_[i].ycm;
					z[i] = target_position_buffer_[i].zcm;
				}	
				float dist0 = sqrt((x[2] - x[0]) * (x[2] - x[0]) + (y[2] - y[0]) * (y[2] - y[0]) + (z[2] - z[0]) * (z[2] - z[0]));
				float dist1 = sqrt((x[2] - x[1]) * (x[2] - x[1]) + (y[2] - y[1]) * (y[2] - y[1]) + (z[2] - z[1]) * (z[2] - z[1]));
				dist = (dist0 + dist1) / 2;
			}	
			
			if ((dist > 0) && (dist < stable_dist_))
				position_stable_ = 1;
			else
				position_stable_ = 0;
			
		}
				
	}
}

















































