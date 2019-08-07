#include "depth_tracks.h"
#include "HungarianOper.h"




depth_tracks::depth_tracks()
{
	_max_cost = 150;
	track_id = 0;
	
}



int depth_tracks::compute(vector<struct dt_measure> &detect_boxes, float timestamp)
{
	tracks_predict(timestamp);
	tracks_match(detect_boxes, _max_cost);
	update_match_track(detect_boxes);
	mark_unmatch_track();
	create_new_track(detect_boxes);
	delete_inactive_track();
	return 0;
}

void depth_tracks::clear_track()
{
	vector<depth_track *>::iterator it;
    for(it = tracks.begin(); it != tracks.end();) 
	{
        if((*it)) {
			depth_track *tk = *it;
			delete tk;
			it = tracks.erase(it);
		} else {
			++it;
		}
    }
	track_id = 0;
}


void depth_tracks::get_track_boxes(vector<struct depth_track_box> &track_boxes)
{
	track_boxes.resize(0);
	for (int i = 0; i < tracks.size(); i++)
	{
		depth_track *tk = tracks[i];
		struct depth_track_box track_box;
		track_box.id = tk->get_track_id();
		track_box.state = tk->get_track_state();
		track_box.measure = tk->get_post_measure();
		track_boxes.push_back(track_box);
	}
}


void depth_tracks::tracks_predict(float timestamp)
{
	for (int i = 0; i < tracks.size(); i++)
	{
		tracks[i]->predict(timestamp);
	}	
}

void depth_tracks::tracks_match(vector<struct dt_measure> &detect_boxes, float max_cost)
{
	int h = tracks.size();
	int w = detect_boxes.size();
	
	tracks_state.matches.resize(0);
	tracks_state.unmatched_detects.resize(0);
	tracks_state.unmatched_tracks.resize(0);
	
	if (h == 0)
	{
		for (int i = 0; i < w; i++)
			tracks_state.unmatched_detects.push_back(i);
		return;
	}	
	if (w == 0)
	{
		for (int i = 0; i < h; i++)
			tracks_state.unmatched_tracks.push_back(i);
		return;
	}	
	
	
	Mat cost = Mat(h, w, CV_32F);
	gen_match_cost(detect_boxes, tracks, cost);
//	cout << "cost = " << endl << " " << cost << endl << endl;
	
	HungarianOper hung;
	hung.Solve(cost, _max_cost);
//	cout << "cost post = " << endl << " " << cost << endl << endl;
	gen_tracks_state(cost, _max_cost);
}

void depth_tracks::update_match_track(vector<struct dt_measure> &detect_boxes)
{
	vector<pair<int, int> > &matches = tracks_state.matches;
	for (int i = 0; i < matches.size(); i++)
	{
		pair<int, int> &match = matches[i];
		tracks[match.first]->update(&detect_boxes[match.second]);
	}	
}

void depth_tracks::mark_unmatch_track()
{
	vector<int> &unmatched_tracks = tracks_state.unmatched_tracks;
	for (int i = 0; i < unmatched_tracks.size(); i++)
	{
		tracks[unmatched_tracks[i]]->mark_missed();
	}	
}

void depth_tracks::create_new_track(vector<struct dt_measure> &detect_boxes)
{
	vector<int> &unmatched_detects = tracks_state.unmatched_detects;
	for (int i = 0; i < unmatched_detects.size(); i++)
	{
		struct dt_measure measure = detect_boxes[unmatched_detects[i]];
		depth_track *tk = new depth_track(&measure, track_id, 3, 1);
		tracks.push_back(tk);
		track_id++;
	}	
}

void depth_tracks::delete_inactive_track()
{
	vector<depth_track *>::iterator it;
    for(it = tracks.begin(); it != tracks.end();) 
	{
        if((*it)->get_track_state() == TRACK_INACTIVE) {
			depth_track *tk = *it;
			delete tk;
			it = tracks.erase(it);
		} else {
			++it;
		}
    }
}


void depth_tracks::gen_tracks_state(Mat &cost, float max_cost)
{
	int rows = cost.size().height;	//h y track
    int cols = cost.size().width;	//w x detect
	
	tracks_state.matches.resize(0);
	for (int y = 0; y < rows; y++)
	{
		float *pc = cost.ptr<float>(y);
		for (int x = 0; x < cols; x++)
		{
			float c = pc[x];
			if (c > 0)
			{
				pair<int, int> match;
				match.first = y;
				match.second = x;
				tracks_state.matches.push_back(match);
			}
		}	
	}	
	
	tracks_state.unmatched_tracks.resize(0);
	for (int y = 0; y < rows; y++)
	{
		bool flag = false;
		for (int i = 0; i < tracks_state.matches.size(); i++)
		{
			if (y == tracks_state.matches[i].first)
			{
				flag = true;
				break;
			}	
		}	
		if (flag == false)
			tracks_state.unmatched_tracks.push_back(y);
	}	
	
	tracks_state.unmatched_detects.resize(0);
	for (int x = 0; x < cols; x++)
	{
		bool flag = false;
		for (int i = 0; i < tracks_state.matches.size(); i++)
		{
			if (x == tracks_state.matches[i].second)
			{
				flag = true;
				break;
			}	
		}
		if (flag == false)
			tracks_state.unmatched_detects.push_back(x);
	}	
}


void depth_tracks::gen_match_cost(vector<struct dt_measure> &detect_boxes, vector<depth_track *> &tracks, Mat &cost)
{
	for (int y = 0; y < tracks.size(); y++)
	{
		for (int x = 0; x < detect_boxes.size(); x++)
		{
			cost.at<float>(y, x) = calc_match_cost(detect_boxes[x], tracks[y]->get_post_measure());
		}	
	}	
}

float depth_tracks::calc_match_cost(struct dt_measure &dm, struct dt_measure &tm)
{
	float dx = dm.x - tm.x;
	float dy = dm.y - tm.y;
	float dz = dm.z - tm.z;
	float dist = sqrt(dx * dx + dy * dy + dz * dz);
	return dist;
}


