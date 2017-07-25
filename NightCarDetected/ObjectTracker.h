#pragma once
#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <cstring>
#include <ctime>

using namespace cv;
using namespace std;
class ObjectTracker
{
public:
	ObjectTracker();
	~ObjectTracker();
	void addObject(Rect2d startPos, Mat& srcImg);
	void update(Mat& srcImg);
	void clearObject();
private:
	string _trackingAlg = "KCF";
	MultiTracker _trackers;
	vector<Ptr<Tracker>> _algorithms;
	vector<Rect2d> _headLights;
};