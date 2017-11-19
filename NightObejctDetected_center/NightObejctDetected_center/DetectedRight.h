#pragma once
#include "DetectedPosition.h"
class DetectedRight: public DetectedPosition
{
public:
	DetectedRight(string path);
	void SetImageProcessor(ImageProcessor *imageProcessor);
	~DetectedRight();
	void run();
private:
	Mat Src;
	Mat Dst;
	Mat Gray;
	Mat GrayRect;
	Mat GrayRectTemp;
	ImageProcessor *processor;
};

