#pragma once

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/tracking.hpp"



using namespace cv;
using namespace std;


class MotionEstimate 
{
public:
	MotionEstimate();
	void initialize(Point startPos);
	void update(Point nextPos, Mat& srcImg);


	~MotionEstimate();

private:
	KalmanFilter _KF;
	vector<Point> _headlight, _kalman;
	Mat_<float> _measurement;
};