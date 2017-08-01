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

	int intersactionArea = 0;
	int flag = -1;
	for (int i = 0; i < _vectorOfObjectTracker.size(); i++)
	{
		Rect2d currentTrackPos = _vectorOfObjectTracker[i].getCurrentPos();
		//ckeck which object occupuies the most area
		if ((currentTrackPos & headLight).area() > intersactionArea)
		{
			_vectorOfObjectTracker.erase(_vectorOfObjectTracker.begin() + i);
			/*flag = i;
			intersactionArea = (currentTrackPos & headLight).area();*/
		}
	}
	/*if (_vectorOfObjectTracker.size() > 0 && (flag != -1))
	{
		_vectorOfObjectTracker.erase(_vectorOfObjectTracker.begin() + flag);
	}*/


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

		if (currentTrackPos.x < 10 || currentTrackPos.y < 10 ||
			currentTrackPos.x > srcImg.cols || currentTrackPos.y > srcImg.rows)
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

	for (int i = 0; i < _vectorOfObjectTracker.size(); i++)
	{
		//cout << i << " " << _vectorOfObjectTracker[i].getNumberOfObjectContain() << endl;		
		if (_vectorOfObjectTracker[i].getNumberOfObjectContain() == 0)
		{
			Mat srcTempGray;
			cvtColor(srcTemp.clone()(_vectorOfObjectTracker[i].getCurrentPos()), srcTempGray, CV_BGR2GRAY);
			CBrightObjectSegment brightObjectSegment(0.98);
			brightObjectSegment.getBinaryImage(srcTempGray);
			erode(srcTempGray, srcTempGray, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
			dilate(srcTempGray, srcTempGray, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));
			Mat labelImg, stats, centroids;
			const int nLabels = connectedComponentsWithStats(srcTempGray, labelImg, stats, centroids, 8, CV_16U);
			cout << nLabels << endl;
			/*switch (nLabels) 
			{
				case 2:
					_vectorOfObjectTracker.erase(_vectorOfObjectTracker.begin() + i);
					break;
				case 3:
					Point centroid = Point(centroids.at<double>(2, 0), centroids.at<double>(2, 1));
					Point centroid2 = Point(centroids.at<double>(3, 0), centroids.at<double>(3, 1));
					
					
					break;
				default:
					_vectorOfObjectTracker.erase(_vectorOfObjectTracker.begin() + i);
					break;
			}*/
			if (!((nLabels ==2)||(nLabels == 3))) {
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