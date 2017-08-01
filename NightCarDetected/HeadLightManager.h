#pragma once
#include "ObjectTracker.h"
#include "ObjectDetected.h"
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include "CBrightObjectSegment.h"

using namespace cv;
using namespace std;

class HeadLightManager {
public:
	HeadLightManager();
	~HeadLightManager();
	void setLightObjects(vector<ObjectDetected> lightObjects);
	void setHeadLightPairs(Rect2d headLight, Mat& srcImg);
	void updateHeadLightPairs(Mat& srcImg,Mat srcTemp);


private:
	vector<Rect2d> _headLightPairs;
	vector<ObjectTracker> _vectorOfObjectTracker;
	vector<ObjectDetected> _lightObjects;
};
