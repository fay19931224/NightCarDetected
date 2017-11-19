#pragma once
#include <opencv2\imgproc\imgproc.hpp>

using namespace cv;
using namespace std;

struct ObjectDetected {
	bool isMatched;
	Rect region;
	Point centroid;
	bool upperPosition;
	int area;
};