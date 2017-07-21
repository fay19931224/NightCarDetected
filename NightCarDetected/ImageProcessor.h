#pragma once
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <vector>
#include <algorithm>
using namespace cv;
using namespace std;
class ImageProcessor
{
public:
	ImageProcessor();
	~ImageProcessor();
	struct ObjectDetected {
		bool isMatched;
		Rect region;
		Point centroid;
		bool upperPosition;
		int area;
	};
	void threshold_hist(Mat& src);
	void removeNoice(Mat &src, int Eheight, int Ewidth, int Dheight, int Dwidth);
	void detectLight(Mat& srcImg, Mat binaryImg, int offsetX, int offsetY, Rect frontRegion);
	int thresholdValue(Mat& src);
	void extractEfficientImage(Mat& src);	
private:	
	vector<ObjectDetected> ObjectDetectedVector;
	int previousThresholdValue = 0;
};


