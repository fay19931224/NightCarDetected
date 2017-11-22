#include "ObjectTracker.h"


ObjectTracker::ObjectTracker() {}

ObjectTracker::~ObjectTracker() {}


void ObjectTracker::initialize(Rect2d startPos, Mat& srcImg)
{
	_headLight = startPos;
	_tracker = Tracker::create(_trackingAlg);
	_tracker->init(srcImg, _headLight);
}
	

void ObjectTracker::update(Mat& srcImg)
{
	_tracker->update(srcImg, _headLight);

	// draw the tracked object
	Rect2d extHeadLight;

	if (_headLight.x - 5 >= 0 && _headLight.y - 5 >= 0 && _headLight.width + 10 <= srcImg.cols && _headLight.height + 10 <= srcImg.rows)
	{
		extHeadLight.x = _headLight.x - 5;
		extHeadLight.y = _headLight.y - 5;
		extHeadLight.width = _headLight.width + 10;
		extHeadLight.height = _headLight.height + 10;
		rectangle(srcImg, extHeadLight, Scalar(255, 0, 0), 2, 1);
	}
	else
	{
		rectangle(srcImg, _headLight, Scalar(255, 0, 0), 2, 1);
	}
	
		
}

Rect2d ObjectTracker::getCurrentPos()
{
	return _headLight;
}

void ObjectTracker::addObjectContain()
{
	_objectContainCount++;
}

int ObjectTracker::getNumberOfObjectContain()
{
	return _objectContainCount;
}

void ObjectTracker::clearObjectContain()
{
	_objectContainCount = 0;
}

void ObjectTracker::setFrameCount(int count)
{
	_frameCount = count;
}

int ObjectTracker::getFrameCount() 
{
	return  _frameCount;
}