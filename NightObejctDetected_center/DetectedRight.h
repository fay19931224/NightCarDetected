#pragma once
#include "DetectedPosition.h"
class DetectedRight: public DetectedPosition
{
public:
	DetectedRight(string path);
	void SetImageProcessor(ImageProcessor *imageProcessor);
	Mat getResult();
	~DetectedRight();
	void run();
private:
	Mat src;
	Mat Dst;
	Mat Gray;
	Mat GrayRect;
	Mat GrayRectTemp;
	ImageProcessor *processor;
};

