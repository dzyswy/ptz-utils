


static void handle_stream(void *arg)
{
	daemon_stereo *cam = (daemon_stereo *)arg;
	printf("create daemon stereo thread!\n");
	cam->stream_process();
	printf("leave daemon stereo thread!\n");
}


daemon_stereo::daemon_stereo()
{
	going = 0;
	sensor_ = NULL;
	reconnect_count_ = 0;
	
}


int daemon_stereo::connect_device(const char *ip, int stream_port, int cmd_port, int stream_mode)
{
	
	std::unique_lock<std::mutex> lock(mux_);

	if (going)
		return -1;
	
	ip_ = ip;
	stream_port_ = stream_port;
	cmd_port_ = cmd_port;
	stream_mode_ = stream_mode;
	
	going = 1;
	stream_thread_ = new std::thread(handle_stream, this);

	return 0;
}


int daemon_stereo::disconnect_device()
{
	if (!going)
		return -1;
	
	std::unique_lock<std::mutex> lock(mux_);
	going = 0;
	if (stream_thread_) {
		stream_thread_->join();
		delete stream_thread_;
		stream_thread_ = NULL;
	}
	
	return 0;
}


int daemon_stereo::set_detect_mode(int value);
{
	std::unique_lock<std::mutex> lock(mux_);
	if (!sensor_)
		return -1;
	
	return sensor_->set_detect_mode(value);
}

int daemon_stereo::get_frame(vector<unsigned char> &image, int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	
	if (cond_.wait_for(lck, std::chrono::seconds(timeout)) == std::cv_status::timeout)
		return -1;
	
	image = frame_buffer_;
	return 0;
}

int daemon_stereo::get_detect_boxes(vector<struct http_output_detect_box> &detect_boxes, int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	
	if (cond_.wait_for(lck, std::chrono::seconds(timeout)) == std::cv_status::timeout)
		return -1;
	
	detect_boxes = detect_boxes_;
	return 0;
}


int daemon_stereo::get_connect_state()
{
	std::unique_lock<std::mutex> lock(mux_);
	if (sensor_)
		return 1;
	else
		return 0;
}

int daemon_stereo::get_reconnect_count()
{
	std::unique_lock<std::mutex> lock(mux_);
	return reconnect_count_;
}


void daemon_stereo::stream_process()
{
	int ret;
	int rebuild_flag = 0;
	while(1)
	{
		std::unique_lock<std::mutex> lock(mux_);
		
		if (rebuild_flag == 1) 
		{
			sensor_->close_device();
			delete sensor_;
			sensor_ = NULL;
		}	
		if (sensor_ == NULL)
		{
			sensor_ = new stereo_sensor(ip_, stream_port_, cmd_port_, stream_mode_);
			reconnect_count_++;
		}	
		
		vector<struct http_output_detect_box> detect_boxes;
		struct gyro_status status;
		ret = sensor_->query_frame(frame_buffer_, detect_boxes, status, 5);
		if (ret < 0) {
			rebuild_flag = 1;
			std::this_thread::sleep_for(std::chrono::seconds(3));
		} else {
			cond_.notify_all();
			reconnect_count_ = 0;
		}
			
		if (!going)	
		{
			sensor_->close_device();
			delete sensor_;
			sensor_ = NULL;
			break;
		}	
	
	}	
}




















