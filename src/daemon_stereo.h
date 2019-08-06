
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

class stream_receiver;
class camera_command;



class daemon_stereo
{
public:	
	daemon_stereo();
	int connect_device(const char *ip, int stream_port, int cmd_port, int stream_mode);
	int disconnect_device();
	
	

	int set_detect_mode(int value);
	
	int get_frame(vector<unsigned char> &image, int timeout = -5);
	int get_detect_boxes(vector<struct http_output_detect_box> &detect_boxes, int timeout = -5);
	
	int get_connect_state();
	int get_reconnect_count();

	void stream_process();
	
protected:
	stereo_sensor *sensor_;

	
	string ip_;
	int stream_port_;
	int cmd_port_;
	int stream_mode_;
	

	int reconnect_count_;
	
	int detect_mode_;

	vector<unsigned char> frame_buffer_;
	vector<struct http_output_detect_box> detect_boxes_;
	
	int going;
	std::mutex mux_;
	std::condition_variable cond_;
	std::thread *stream_thread_;
};























