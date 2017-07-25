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
	void initialize(Rect2d startPos, Mat& srcImg);
	void update(Mat& srcImg);
	Rect2d getCurrentPos();
private:
	string _trackingAlg = "KCF";
	//MultiTracker _trackers;
	Ptr<Tracker> _tracker;
	Rect2d _headLight;

};