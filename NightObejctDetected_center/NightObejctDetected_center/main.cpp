#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include "RightImageProcessor.h"
#include "BackImageProcessor.h"
#include "DetectedRight.h"
#include "DetectedBack.h"

using namespace std;
using namespace cv;

int main() {
	int mode = 0;
	string carBackPath = "D:/UserFolder/Desktop/haitec/video/car_back/µØ³ÐÃèÀY/AV1-move2.avi";
	string carRightPath = "D:/UserFolder/Desktop/haitec/video/cut/highway1.avi";

	if (mode == 0)
	{
		DetectedPosition *detectedPosition = new DetectedBack(carBackPath);
		BackImageProcessor *backImageProcessor = new BackImageProcessor();
		detectedPosition->SetImageProcessor(backImageProcessor);
		detectedPosition->run();
	}
	else if (mode == 1)
	{
		DetectedPosition *detectedPosition = new DetectedRight(carRightPath);
		RightImageProcessor *rightImageProcessor = new RightImageProcessor();
		detectedPosition->SetImageProcessor(rightImageProcessor);
		detectedPosition->run();
	}
}