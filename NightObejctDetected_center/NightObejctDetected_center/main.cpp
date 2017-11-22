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
	string carBackPath1 = "D:/UserFolder/Desktop/haitec/video/car_back/�س����Y/car-str.mp4";
	string carBackPath2 = "D:/UserFolder/Desktop/haitec/video/car_back/�س����Y/car-curve.mp4";
	string carBackPath3 = "D:/UserFolder/Desktop/haitec/video/car_back/�س����Y/car-curve2.mp4";
	string carBackPath4 = "D:/UserFolder/Desktop/haitec/video/car_back/�س����Y/motor-str.mp4";
	string carBackPath5 = "D:/UserFolder/Desktop/haitec/video/car_back/�س����Y/motor-curve.mp4";
	string carBackPath6 = "D:/UserFolder/Desktop/haitec/video/car_back/�س����Y/distance-10-5.mp4";
	string carBackPath7 = "D:/UserFolder/Desktop/haitec/video/car_back/�س����Y/SJCAM-car-distance40-30.mp4";
	string carBackPath8 = "D:/UserFolder/Desktop/haitec/video/car_back/�س����Y/SJCAM-car-distance20-10-5.mp4";
	string carRightPath = "D:/UserFolder/Desktop/haitec/video/cut/highway1.avi";

	if (mode == 0)
	{
		DetectedPosition *detectedPosition = new DetectedBack(carBackPath6);
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