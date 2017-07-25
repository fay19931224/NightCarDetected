#include "ObjectTracker.h"


ObjectTracker::ObjectTracker() {}

ObjectTracker::~ObjectTracker() {}


void ObjectTracker::initialize(Rect2d startPos, Mat& srcImg)
{
	_headLight = startPos;
	_tracker = Tracker::create(_trackingAlg);
	_tracker->init(srcImg, _headLight);
	
	//cout << _headLight.x << " " << _headLight.y << endl;
	//_trackers.add(_trackingAlg, srcImg, startPos);
}
	

void ObjectTracker::update(Mat& srcImg)
{
	_tracker->update(srcImg, _headLight);

	// draw the tracked object
	rectangle(srcImg, _headLight, Scalar(255, 0, 0), 1, 1);
		
}

Rect2d ObjectTracker::getCurrentPos()
{
	return _headLight;
}

