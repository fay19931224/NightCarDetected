#pragma once
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <vector>
#include <algorithm>

#include "HeadLightManager.h"
#include "ObjectDetected.h"


using namespace cv;
using namespace std;

class ImageProcessor
{
public:
	//ImageProcessor();
	//~ImageProcessor();
	virtual void threshold_hist(Mat& src) = 0;
	virtual void removeNoice(Mat &src, int Eheight, int Ewidth, int Dheight, int Dwidth) = 0;
	virtual void detectLight(Mat& srcImg, Mat binaryImg, int offsetX, int offsetY, vector<Rect> ROIs) = 0;
	virtual void extractEfficientImage(Mat& src) = 0;
	virtual void setHeadLightManager(HeadLightManager headLightManager) = 0;
	virtual vector<ObjectDetected> getObjectDetectedVector() = 0;
	virtual int thresholdValue(Mat& src) = 0;

	bool isCarLightHeightDiffYCorrect(int diffY, int distance);
	

};


