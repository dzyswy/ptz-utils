#include "depth_track.h"


depth_track::depth_track(struct dt_measure *measure, int id, int min_hit, int max_miss)
{
	_id = id;
	_min_hit = min_hit;
	_max_miss = max_miss;
	
	for (int i = 0; i < kmp; i++)
	{
		process_noise[i] = 0.1;
		process_noise[i + kmp] = 10;
	}	
//	process_noise[DEPTH_TRACK_BOX_AERA_SPEED] = 50;
//	process_noise[DEPTH_TRACK_BOX_RADIO_SPEED] = 50;
	
	for (int i = 0; i < kmp; i++)
	{
		measure_noise[i] = 10;
	}
	measure_noise[DEPTH_TRACK_BOX_CY] = 50;
	measure_noise[DEPTH_TRACK_BOX_AERA] = 1000;
	measure_noise[DEPTH_TRACK_BOX_RADIO] = 1000;


	kf = new KalmanFilter(ksp, kmp, kcp, CV_32F);
	
	//state: 	[u v s r x y z U V S R X Y Z]
	//measure: 	[u v s r x y z]
	//[1 0 0 0 0 0 0 t 0 0 0 0 0 0]	u
	//[0 1 0 0 0 0 0 0 t 0 0 0 0 0]	v
	//[0 0 1 0 0 0 0 0 0 t 0 0 0 0]	s
	//[0 0 0 1 0 0 0 0 0 0 t 0 0 0]	r
	//[0 0 0 0 1 0 0 0 0 0 0 t 0 0]	x
	//[0 0 0 0 0 1 0 0 0 0 0 0 t 0]	y
	//[0 0 0 0 0 0 1 0 0 0 0 0 0 t]	z
	//[0 0 0 0 0 0 0 1 0 0 0 0 0 0]	U
	//[0 0 0 0 0 0 0 0 1 0 0 0 0 0]	V	
	//[0 0 0 0 0 0 0 0 0 1 0 0 0 0]	S
	//[0 0 0 0 0 0 0 0 0 0 1 0 0 0]	R
	//[0 0 0 0 0 0 0 0 0 0 0 1 0 0]	X
	//[0 0 0 0 0 0 0 0 0 0 0 0 1 0]	Y
	//[0 0 0 0 0 0 0 0 0 0 0 0 0 1]	Z
	setIdentity(kf->transitionMatrix, Scalar(1.0f));
	
	//[1 0 0 0 0 0 0 0 0 0 0 0 0 0]	u
	//[0 1 0 0 0 0 0 0 0 0 0 0 0 0]	v
	//[0 0 1 0 0 0 0 0 0 0 0 0 0 0]	s
	//[0 0 0 1 0 0 0 0 0 0 0 0 0 0]	r
	//[0 0 0 0 1 0 0 0 0 0 0 0 0 0]	x
	//[0 0 0 0 0 1 0 0 0 0 0 0 0 0]	y
	//[0 0 0 0 0 0 1 0 0 0 0 0 0 0]	z
	//								U
	//								V
	//								S
	//								R
	//								X
	//								Y
	//								Z
	kf->measurementMatrix = Mat::zeros(kmp, ksp, CV_32F);
	kf->measurementMatrix.at<float>(0, 0) = 1.0f;
	kf->measurementMatrix.at<float>(1, 1) = 1.0f;
	kf->measurementMatrix.at<float>(2, 2) = 1.0f;
	kf->measurementMatrix.at<float>(3, 3) = 1.0f;
	kf->measurementMatrix.at<float>(4, 4) = 1.0f;
	kf->measurementMatrix.at<float>(5, 5) = 1.0f;
	kf->measurementMatrix.at<float>(6, 6) = 1.0f;
	
	kf->processNoiseCov = Mat::zeros(ksp, ksp, CV_32F);
	for (int i = 0; i < ksp; i++)
	{
		kf->processNoiseCov.at<float>(i, i) = process_noise[i];
	}	
	
	
	setIdentity(kf->measurementNoiseCov, Scalar(1.0f));
	for (int i = 0; i < kmp; i++)
	{
		kf->measurementNoiseCov.at<float>(i, i) = measure_noise[i];
	}	
	
	setIdentity(kf->errorCovPost, Scalar(1000.0f));//give high uncertainty to the unobservable initial velocities
	
	kf->statePost.at<float>(0) = measure->u;
	kf->statePost.at<float>(1) = measure->v;
	kf->statePost.at<float>(2) = measure->s;
	kf->statePost.at<float>(3) = measure->r;
	kf->statePost.at<float>(4) = measure->x;
	kf->statePost.at<float>(5) = measure->y;
	kf->statePost.at<float>(6) = measure->z;
	kf->statePost.at<float>(7) = 0;
	kf->statePost.at<float>(8) = 0;
	kf->statePost.at<float>(9) = 0;
	kf->statePost.at<float>(10) = 0;
	kf->statePost.at<float>(11) = 0;
	kf->statePost.at<float>(12) = 0;
	kf->statePost.at<float>(13) = 0;
	
	age = 0;
	hit = 0;
	miss = 0;
	track_state = TRACK_INIT;
}

depth_track::~depth_track()
{
	delete kf;
}

void depth_track::predict(float timestamp)
{
//	cout << "last:" << endl;
//	cout << kf->statePre << endl;
	
	kf->transitionMatrix.at<float>(0, 7) = timestamp;
	kf->transitionMatrix.at<float>(1, 8) = timestamp;
	kf->transitionMatrix.at<float>(2, 9) = timestamp;
	kf->transitionMatrix.at<float>(3, 10) = timestamp;
	kf->transitionMatrix.at<float>(4, 11) = timestamp;
	kf->transitionMatrix.at<float>(5, 12) = timestamp;
	kf->transitionMatrix.at<float>(6, 13) = timestamp;
	Mat st = kf->predict();
	post_measure.u = st.at<float>(0);
	post_measure.v = st.at<float>(1);
	post_measure.s = st.at<float>(2);
	post_measure.r = st.at<float>(3);
	post_measure.x = st.at<float>(4);
	post_measure.y = st.at<float>(5);
	post_measure.z = st.at<float>(6);
	
//	cout << "A:" << endl;
//	cout << kf->transitionMatrix << endl;
	
//	cout << "predict:" << endl;
//	cout << st << endl;
	
	age++;
	miss++;
}

void depth_track::update(struct dt_measure *measure)
{
	Mat ms(kmp, 1, CV_32F);
	ms.at<float>(0) = measure->u;
	ms.at<float>(1) = measure->v;
	ms.at<float>(2) = measure->s;
	ms.at<float>(3) = measure->r;
	ms.at<float>(4) = measure->x;
	ms.at<float>(5) = measure->y;
	ms.at<float>(6) = measure->z;
	
	Mat st = kf->correct(ms);
	post_measure.u = st.at<float>(0);
	post_measure.v = st.at<float>(1);
	post_measure.s = st.at<float>(2);
	post_measure.r = st.at<float>(3);
	post_measure.x = st.at<float>(4);
	post_measure.y = st.at<float>(5);
	post_measure.z = st.at<float>(6);
	
//	cout << "measure:" << endl;
//	cout << ms << endl;
	
//	cout << "post:" << endl;
//	cout << st << endl;
	
	hit++;
	miss = 0;
	
	if ((track_state == TRACK_INIT) && (hit >= _min_hit)) {
		track_state = TRACK_TRACKED;
	}
}

void depth_track::mark_missed()
{
	if (track_state == TRACK_INIT) {
		track_state = TRACK_INACTIVE;
	} else if (miss > _max_miss) {
		track_state = TRACK_INACTIVE;
	}
}

int depth_track::get_track_id()
{
	return _id;
}

int depth_track::get_track_state()
{
	return track_state;
}

struct dt_measure& depth_track::get_post_measure()
{
	return post_measure;
}
