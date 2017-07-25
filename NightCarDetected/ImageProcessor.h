#pragma once
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <vector>
#include <algorithm>
#include "ObjectTracker.h"


using namespace cv;
using namespace std;


struct ObjectDetected {
	bool isMatched;
	Rect region;
	Point centroid;
	bool upperPosition;
	int area;
};

class ImageProcessor
{
public:
	ImageProcessor();
	~ImageProcessor();


	void threshold_hist(Mat& src);
	void removeNoice(Mat &src, int Eheight, int Ewidth, int Dheight, int Dwidth);
	void detectLight(Mat& srcImg, Mat binaryImg, int offsetX, int offsetY, Rect frontRegion);
	int thresholdValue(Mat& src);
	void extractEfficientImage(Mat& src);	
	vector<Rect2d> getHeadLightPairs();
	void setHeadLightPairs(Rect2d headLight, Mat& srcImg);
	vector<ObjectDetected> getObjectDetectedVector();

private:	
	vector<ObjectDetected> ObjectDetectedVector;
	vector<Rect2d> headLightPairs;
	int previousThresholdValue = 0;
	vector<ObjectTracker> vectorOfObjectTracker;;
};


