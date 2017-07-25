#include "ObjectTracker.h"


ObjectTracker::ObjectTracker()
{
}

ObjectTracker::~ObjectTracker() {}

void ObjectTracker::addObject(Rect2d startPos, Mat& srcImg)
{
	_headLights.push_back(startPos);
	_trackers.add(_trackingAlg, srcImg, startPos);
}
	
void ObjectTracker::update(Mat& srcImg)
{
	_trackers.update(srcImg);

	// draw the tracked object
	
	//for (unsigned i = 0; i < _trackers.objects.size(); i++)
	//{
	//	if (_trackers.objects[i].x < 160)
	//	{
	//		_trackers.objects.erase(_trackers.objects.begin() + i);
	//	}
	//}


	for (unsigned i = 0; i < _trackers.objects.size(); i++)
	{
		//cout << i << " " << _trackers.objects[i].x << " " << _trackers.objects[i].y << endl;
		rectangle(srcImg, _trackers.objects[i], Scalar(255, 0, 0), 1, 1);
	}
		
	// show image with the tracked object
	imshow("tracker", srcImg);
}

void ObjectTracker::clearObject()
{
	_trackers.~MultiTracker();
}