#pragma once
#include "DetectedPosition.h"
class DetectedBack :public DetectedPosition
{
public:
	DetectedBack(string path);
	~DetectedBack();
	void SetImageProcessor(ImageProcessor *imageProcessor);
	Mat getResult();
	void run();
	
private:
	Mat src;
	Mat dst;
	Mat gray;
	Mat grayRect;
	Mat grayRectTemp;
	ImageProcessor *processor;
};

