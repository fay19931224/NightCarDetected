#include "ObjectTracker.h"


ObjectTracker::ObjectTracker()
{
}

ObjectTracker::~ObjectTracker() {}

void ObjectTracker::initialize(Rect2d startPos, Mat& srcImg)
{
	_headLights.push_back(startPos);
	_trackers.add(_trackingAlg, srcImg, startPos);
}
	

void ObjectTracker::update(Mat& srcImg)
{
	_trackers.update(srcImg);

	// draw the tracked object
	for (unsigned i = 0; i < _trackers.objects.size(); i++)
	{
		rectangle(srcImg, _trackers.objects[i], Scalar(255, 0, 0), 1, 1);
	}
		
	// show image with the tracked object
	imshow("tracker", srcImg);
}

void ObjectTracker::clearObject()
{
	_trackers.~MultiTracker();
}