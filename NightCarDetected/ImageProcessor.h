#pragma once
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <vector>
using namespace cv;
using namespace std;
class ImageProcessor
{
public:
	ImageProcessor();
	~ImageProcessor();
	Mat removeNoiseAndThreshold(Mat src, Rect rect);
	void removeNoice(Mat &ROI);
	void detectLight(Mat srcImg, Mat rightGray, Mat binaryImg, int offsetX, int offsetY, Rect frontRegion, Rect rearRegion);
	int thresholdValue(Mat& src);
private:
	struct ObjectDetected {
		bool isMatched;
		Rect region;
		Point centroid;
	};
	vector<ObjectDetected> ObjectDetectedVector;
	int previousThresholdValue = 0;
};


