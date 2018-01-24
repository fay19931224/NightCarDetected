#pragma once
#include "DetectedPosition.h"
class DetectedPano : public DetectedPosition
{
public:
	DetectedPano(string path);
	//void SetBackImageProcessor(ImageProcessor *imageProcessor);
	//void SetSideImageProcessor(ImageProcessor *imageProcessor);
	void SetImageProcessor(ImageProcessor *imageProcessor);
	~DetectedPano();
	void run();
private:
	Mat gray;
	Mat backResult;
	Mat rightResult;
	Rect backPosition;
	Rect rightPosition;
	Rect leftPosition;
	ImageProcessor *backImageProcessor;
	ImageProcessor *rightImageProcessor;
	ImageProcessor *leftImageProcessor;
	vector<Rect> backROIs;
	vector<Rect> rightROIs;
	vector<Rect> leftROIs;
};
