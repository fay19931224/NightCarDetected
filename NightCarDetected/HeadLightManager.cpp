#include "HeadLightManager.h"


HeadLightManager::HeadLightManager() {}
HeadLightManager::~HeadLightManager() {}

void HeadLightManager::setLightObjects(vector<ObjectDetected> lightObjects)
{
	_lightObjects = lightObjects;
}

void HeadLightManager::setHeadLightPairs(Rect2d headLight, Mat& srcImg)
{
	ObjectTracker objectTracker;
	//if (_vectorOfObjectTracker.size() > 0)
	//{
		for (int i = 0; i < _vectorOfObjectTracker.size(); i++)
		{
			Rect2d currentTrackPos = _vectorOfObjectTracker[i].getCurrentPos();

			if ((currentTrackPos & headLight).area() > 0)
			{
				_vectorOfObjectTracker.erase(_vectorOfObjectTracker.begin() + i);
			}
		}
	//}

	objectTracker.initialize(headLight, srcImg);
	_vectorOfObjectTracker.push_back(objectTracker);


	//cout << " Size: " << _vectorOfObjectTracker.size() << endl;
	//for (int i = 0; i < _vectorOfObjectTracker.size(); i++)
	//{
	//	Rect2d currentTrackPos = _vectorOfObjectTracker[i].getCurrentPos();
	//	cout << i << " " << currentTrackPos.x << " " << currentTrackPos.y << endl;
	//}
}


void HeadLightManager::updateHeadLightPairs(Mat& srcImg)
{
	//bool isTrack = false;
	//int trackIndex;
	//check whether tracker is out of detect window or not
	for (int i = 0; i < _vectorOfObjectTracker.size(); i++)
	{
		Rect2d currentTrackPos = _vectorOfObjectTracker[i].getCurrentPos();

		if (currentTrackPos.x < 10)
		{
			_vectorOfObjectTracker.erase(_vectorOfObjectTracker.begin() + i);
		}
	}

	//check whether tracker is contain light object or not
	for (int i = 0; i < _vectorOfObjectTracker.size(); i++)
	{
		Rect2d currentTrackPos = _vectorOfObjectTracker[i].getCurrentPos();
		_vectorOfObjectTracker[i].clearObjectContain();

		for (int j = 0; j < _lightObjects.size(); j++)
		{
			if (currentTrackPos.contains(Point(_lightObjects[j].centroid.x, _lightObjects[j].centroid.y)))
			{
				//cout << "curent : " << trackIndex << " " << currentTrackPos.x << " " << currentTrackPos.y << endl;
				//cout << "object : " << j << " " << _lightObjects[j].centroid.x << " " << _lightObjects[j].centroid.y << endl;
				//isTrack = true;

				_vectorOfObjectTracker[i].addObjectContain();
			}
		}

	}

	//cout << " Size: " << _vectorOfObjectTracker.size() << endl;
	if (_vectorOfObjectTracker.size() > 0)
	{
		//cout << trackIndex << endl;

		for (int i = 0; i < _vectorOfObjectTracker.size(); i++)
		{
			cout << i << " " << _vectorOfObjectTracker[i].getNumberOfObjectContain() << endl;
			if (_vectorOfObjectTracker[i].getNumberOfObjectContain() == 0)
			{
				_vectorOfObjectTracker.erase(_vectorOfObjectTracker.begin() + i);
			}
			
		}

		
	}
	



	//update
	for (int i = 0; i < _vectorOfObjectTracker.size(); i++)
	{
		_vectorOfObjectTracker[i].update(srcImg);
	}
}