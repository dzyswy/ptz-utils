#ifndef __STEREO_DIRECTOR_H
#define __STEREO_DIRECTOR_H






class daemon_stereo;



struct stereo_director_detect_box {
	int id;
	int box_x;
	int box_y;
	int box_w;
	int box_h;
	int x;
	int y;
	int d;
	int xcm;
	int ycm;
	int zcm;
	int xtcm;
	int ytcm;
	int ztcm;
	float xa;
	float ya;
	float r;
	
};


class stereo_director
{
public:
	stereo_director(daemon_stereo *stereo);
	~stereo_director();
	

	int get_target_count();
	struct stereo_director_detect_box get_target_position();

	void director_process();


protected:
	
	
	
protected:
	daemon_stereo *stereo_;
	
	int going;
	std::thread *thread_;

	int target_count_;
	struct stereo_director_detect_box target_position_;

	vector<struct stereo_director_detect_box> detect_boxes_;
};





#endif















