#include "HeadLightManager.h"


HeadLightManager::HeadLightManager() 
{
	
}
HeadLightManager::~HeadLightManager() {}

void HeadLightManager::setLightObjects(vector<ObjectDetected> lightObjects)
{
	_lightObjects = lightObjects;
}

void HeadLightManager::setHeadLightPairs(Rect2d headLight, Mat& srcImg)
{
	ObjectTracker objectTracker;

	int intersactionArea = 0;
	int flag = -1;
	for (int i = 0; i < _vectorOfObjectTracker.size(); i++)
	{
		Rect2d currentTrackPos = _vectorOfObjectTracker[i].getCurrentPos();
		//ckeck which object occupuies the most area
		if ((currentTrackPos & headLight).area() > intersactionArea)
		{
			//_vectorOfObjectTracker.erase(_vectorOfObjectTracker.begin() + i);
			flag = i;
			intersactionArea = (currentTrackPos & headLight).area();
			break;
		}
	}
	if (_vectorOfObjectTracker.size() > 0 && (flag != -1))
	{		
		_vectorOfObjectTracker.erase(_vectorOfObjectTracker.begin() + flag);
	}


	objectTracker.initialize(headLight, srcImg);
	_vectorOfObjectTracker.push_back(objectTracker);
	
	//cout << " Size: " << _vectorOfObjectTracker.size() << endl;
	//for (int i = 0; i < _vectorOfObjectTracker.size(); i++)
	//{
	//	Rect2d currentTrackPos = _vectorOfObjectTracker[i].getCurrentPos();
	//	cout << i << " " << currentTrackPos.x << " " << currentTrackPos.y << endl;
	//}
}


void HeadLightManager::updateHeadLightPairs(Mat& srcImg, Mat srcTemp)
{
	//bool isTrack = false;
	//int trackIndex;

	
	//check whether tracker is out of detect window or not
	for (int i = 0; i < _vectorOfObjectTracker.size(); i++)
	{
		Rect2d currentTrackPos = _vectorOfObjectTracker[i].getCurrentPos();

		if (currentTrackPos.x < 10 || 
			currentTrackPos.y < 10 ||
			currentTrackPos.x + currentTrackPos.width > _offsetX + _middle.x + _middle.width ||
			currentTrackPos.y + currentTrackPos.height > _offsetY + _middle.y + _middle.height)
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
				_vectorOfObjectTracker[i].addObjectContain();
			}
		}
	}

	//cout << " Size: " << _vectorOfObjectTracker.size() << endl;

	for (int i = 0; i < _vectorOfObjectTracker.size(); i++)
	{	
		if (_vectorOfObjectTracker[i].getNumberOfObjectContain() == 0)
		{
			_vectorOfObjectTracker.erase(_vectorOfObjectTracker.begin() + i);		
		}							
	}

	//update
	for (int i = 0; i < _vectorOfObjectTracker.size(); i++)
	{
		_vectorOfObjectTracker[i].update(srcImg);
	}
}

void HeadLightManager::setDetectRegion(Rect front, Rect middle, int offsetX, int offsetY)
{	
	_front = front;
	_middle = middle;
	_offsetX = offsetX;
	_offsetY = offsetY;
}