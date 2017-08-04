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
			_vectorOfObjectTracker.erase(_vectorOfObjectTracker.begin() + i);
			/*_vectorOfObjectTracker[i].setFrameCount(_vectorOfObjectTracker[i].getFrameCount() + 1);
			if (_vectorOfObjectTracker[i].getFrameCount() > 10) 
			{
				_vectorOfObjectTracker.erase(_vectorOfObjectTracker.begin() + i);
			}*/			
		}

		//Mat srcTempGray;
		//cvtColor(srcTemp.clone()(_vectorOfObjectTracker[i].getCurrentPos()), srcTempGray, CV_BGR2GRAY);
		//CBrightObjectSegment brightObjectSegment(0.98);
		//brightObjectSegment.getBinaryImage(srcTempGray);
		//erode(srcTempGray, srcTempGray, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		//dilate(srcTempGray, srcTempGray, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));
		//Mat labelImg, stats, centroids;
		//const int nLabels = connectedComponentsWithStats(srcTempGray, labelImg, stats, centroids, 8, CV_16U);
		//cout << nLabels << endl;
		//vector<ObjectDetected> reMultThreshodObjecte;
		//for (int label = 1; label < nLabels; ++label)
		//{
		//	const int width = stats.at<int>(label, CC_STAT_WIDTH);
		//	const int height = stats.at<int>(label, CC_STAT_HEIGHT);
		//	const int area = stats.at<int>(label, CC_STAT_AREA);
		//	const int left = stats.at<int>(label, CC_STAT_LEFT) ;
		//	const int top = stats.at<int>(label, CC_STAT_TOP);
		//	Point centroid = Point(centroids.at<double>(label, 0) , centroids.at<double>(label, 1));
		//	const double HeightWidthRatio = static_cast<double>(height) / static_cast<double>(width);
		//	if (area < 2000 && HeightWidthRatio <= 2)
		//	{
		//		ObjectDetected objectDetected{ false,Rect(left,top,width,height),centroid ,true ,area };
		//		reMultThreshodObjecte.push_back(objectDetected);
		//	}
		//}
		//for (int i = 0; i < reMultThreshodObjecte.size(); i++)
		//{
		//	for (int j = 0; j < reMultThreshodObjecte.size(); j++)
		//	{
		//		if ((i != j) && (reMultThreshodObjecte[i].isMatched == false) && (reMultThreshodObjecte[j].isMatched == false))
		//		{
		//			// i is on left and  j is on right
		//			const double carLightDistanse = reMultThreshodObjecte[j].centroid.x - reMultThreshodObjecte[i].centroid.x;
		//			const double carLeftingDistanse = reMultThreshodObjecte[i].centroid.x + carLightDistanse / 2;
		//			const double carLightheightDiffY = reMultThreshodObjecte[j].centroid.y - reMultThreshodObjecte[i].centroid.y;
		//			if ((reMultThreshodObjecte[i].region.area() <= reMultThreshodObjecte[j].region.area()) && carLightDistanse>0)
		//			{
		//				cout << "dddddddddddddddddddd" << endl;
		//				reMultThreshodObjecte[i].isMatched = true;
		//				reMultThreshodObjecte[j].isMatched = true;
		//				break;
		//			}
		//		}
		//	}
		//}		
		//bool temp =true;
		//for (int i = 0; i < reMultThreshodObjecte.size(); i++) 
		//{				
		//	if (reMultThreshodObjecte[i].isMatched == true) 
		//	{
		//		temp = false;
		//		break;
		//	}
		//}
		//if (temp) 
		//{
		//	//cout << "dddddddddddddddddddd" << endl;
		//	_vectorOfObjectTracker.erase(_vectorOfObjectTracker.begin() + i);
		//}										
	}

	//update
	for (int i = 0; i < _vectorOfObjectTracker.size(); i++)
	{
		_vectorOfObjectTracker[i].update(srcImg);
	}
}

void HeadLightManager::setDetectRegion(Rect middle, Rect front, int offsetX, int offsetY)
{	
	_front = front;
	_middle = middle;
	_offsetX = offsetX;
	_offsetY = offsetY;	
}