#pragma once
#include "DetectedPosition.h"
class DetectedBack :public DetectedPosition
{
public:
	DetectedBack(string path);
	void SetImageProcessor(ImageProcessor *imageProcessor);
	~DetectedBack();
	void run();
private:
	Mat src;
	Mat dst;
	Mat gray;
	Mat grayRect;
	Mat grayRectTemp;
	ImageProcessor *processor;
};

