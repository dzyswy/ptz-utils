#ifndef __HUNGARIAN_OPER_H
#define __HUNGARIAN_OPER_H

#include <opencv2/opencv.hpp>
#include "munkres.h"


using namespace std;
using namespace cv;


class HungarianOper
{
public:
	void Solve(Mat &cost, float max_cost);
};





#endif
