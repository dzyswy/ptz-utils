#ifndef __DEPTH_TRACKS_H
#define __DEPTH_TRACKS_H



#include<iostream>
#include<string>
#include<vector>
#include<utility>

#include "depth_track.h"

using namespace std;

struct depth_tracks_state {
	vector<pair<int, int> > matches;
	vector<int> unmatched_detects;
	vector<int> unmatched_tracks;
};

struct depth_track_box {
	struct dt_measure measure;
	int id;
	int state;
};

class depth_tracks
{
public:
	depth_tracks();
	int compute(vector<struct dt_measure> &detect_boxes, float timestamp);
	void clear_track();
	void get_track_boxes(vector<struct depth_track_box> &track_boxes);

private:
	void tracks_predict(float timestamp);
	void tracks_match(vector<struct dt_measure> &detect_boxes, float max_cost);
	void update_match_track(vector<struct dt_measure> &detect_boxes);
	void mark_unmatch_track();
	void create_new_track(vector<struct dt_measure> &detect_boxes);
	void delete_inactive_track();
	void delete_track(int type);
	void gen_tracks_state(Mat &cost, float max_cost);
	void gen_match_cost(vector<struct dt_measure> &detect_boxes, vector<depth_track *> &tracks, Mat &cost);
	float calc_match_cost(struct dt_measure &dm, struct dt_measure &tm);
	
	
private:
	float _max_cost;
	

	int track_id;
	vector<depth_track *> tracks;
	struct depth_tracks_state tracks_state;
	
public:
	void set_track_max_cost(float value) {
		_max_cost = value;
	}
	

	float get_track_max_cost() {
		return _max_cost;
	}

};











#endif

