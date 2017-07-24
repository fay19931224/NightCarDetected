#include "ObjectTracker.h"


ObjectTracker::ObjectTracker()
{
}

ObjectTracker::~ObjectTracker() {}

void ObjectTracker::initialize(Rect2d startPos, Mat& srcImg)
{
	bool isDuplicate = false;
	if (_trackers.objects.size() == 0)
	{
		_headLights.push_back(startPos);
		_trackers.add(_trackingAlg, srcImg, startPos);
	}
	else
	{
		for (unsigned i = 0; i < _trackers.objects.size(); i++)
		{
			cout << "d : " << i << " " << startPos.x << " " << startPos.y << endl;
			cout << "t : " << i << " " << _trackers.objects[i].x << " " << _trackers.objects[i].y << endl;
			if ((abs(_trackers.objects[_trackers.objects.size() - 1].x - startPos.x) <= 10) &&
				(abs(_trackers.objects[_trackers.objects.size() - 1].y - startPos.y) <= 5))
			{
				isDuplicate = true;
				//clearObject();
				break;
			}


		}
		if (!isDuplicate)
		{
			_headLights.push_back(startPos);
			_trackers.add(_trackingAlg, srcImg, startPos);
		}

	}
	
	
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