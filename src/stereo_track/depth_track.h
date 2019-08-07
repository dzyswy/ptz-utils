#ifndef __DEPTH_TRACK_H
#define __DEPTH_TRACK_H

#include <opencv2/opencv.hpp>



using namespace cv;
using namespace std;



enum _TRACK_STATE
{
	TRACK_INIT = 0,
	TRACK_TRACKED = 1,
	TRACK_LOST = 2,
	TRACK_INACTIVE = 3,
};

enum _DEPTH_TRACK_STATE_TYPE
{
	DEPTH_TRACK_BOX_CX = 0,
	DEPTH_TRACK_BOX_CY = 1,
	DEPTH_TRACK_BOX_AERA = 2,
	DEPTH_TRACK_BOX_RADIO = 3,
	DEPTH_TRACK_XCM = 4,
	DEPTH_TRACK_YCM = 5,
	DEPTH_TRACK_ZCM = 6,
	DEPTH_TRACK_BOX_CX_SPEED = 7,
	DEPTH_TRACK_BOX_CY_SPEED = 8,
	DEPTH_TRACK_BOX_AERA_SPEED = 9,
	DEPTH_TRACK_BOX_RADIO_SPEED = 10,
	DEPTH_TRACK_XCM_SPEED = 11,
	DEPTH_TRACK_YCM_SPEED = 12,
	DEPTH_TRACK_ZCM_SPEED = 13,
};


struct dt_measure
{
	float u;
	float v;
	float s;
	float r;
	float x;
	float y;
	float z;
};

class depth_track
{
public:
	depth_track(struct dt_measure *measure, int id);
	depth_track(struct dt_measure *measure, int id, int min_hit, int max_miss);
	~depth_track();
	void predict(float timestamp);
	void update(struct dt_measure *measure);
	void mark_missed();
	int get_track_id();
	int get_track_state();
	struct dt_measure &get_post_measure();
	
private:
	static constexpr int ksp = 14;
	static constexpr int kmp = 7;
	static constexpr int kcp = 0;
	KalmanFilter *kf;
	float process_noise[ksp];
	float measure_noise[kmp];
	
	int age;
	int hit;
	int miss;
	int track_state;
	
	
	struct dt_measure post_measure;

	int _id;
	int _min_hit;
	int _max_miss;
};



#endif

