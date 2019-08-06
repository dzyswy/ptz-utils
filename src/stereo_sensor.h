#ifndef __STEREO_SENSOR_H
#define __STEREO_SENSOR_H



class stream_receiver;
class camera_command;

class stereo_sensor
{
public:
	stereo_sensor();
	int open_device(const char *ip, int stream_port, int cmd_port, int mode);
	int close_device();
	int set_detect_mode(int value);
	
	int query_frame(vector<unsigned char> &image, int timeout = 5);
	int query_frame(vector<unsigned char> &image, vector<struct http_output_detect_box> &detect_boxes, struct gyro_status &status, int timeout = 5);
	
	
	int get_point_space_status(int x, int y, struct point_space_status &status);
	
	
protected:
	stream_receiver *stream_;
	camera_command *cmd_;

	
};


















#endif

