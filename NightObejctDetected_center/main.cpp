#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include "RightImageProcessor.h"
#include "BackImageProcessor.h"
#include "LeftImageProcessor.h"
#include "DetectedRight.h"
#include "DetectedBack.h"
#include "DetectedPano.h"

using namespace std;
using namespace cv;

int main() {
	int mode = 2;
	string carBackPath1 = "D:/UserFolder/Desktop/haitec/video/car_back/華創鏡頭/car-str.mp4";
	string carBackPath2 = "D:/UserFolder/Desktop/haitec/video/car_back/華創鏡頭/car-curve.mp4";
	string carBackPath3 = "D:/UserFolder/Desktop/haitec/video/car_back/華創鏡頭/car-curve2.mp4";
	string carBackPath4 = "D:/UserFolder/Desktop/haitec/video/car_back/華創鏡頭/motor-str.mp4";
	string carBackPath5 = "D:/UserFolder/Desktop/haitec/video/car_back/華創鏡頭/motor-curve.mp4";
	string carBackPath6 = "D:/UserFolder/Desktop/haitec/video/car_back/華創鏡頭/distance-10-5.mp4";
	string carBackPath7 = "D:/UserFolder/Desktop/haitec/video/car_back/華創鏡頭/SJCAM-car-distance40-30.mp4";
	string carBackPath8 = "D:/UserFolder/Desktop/haitec/video/car_back/華創鏡頭/SJCAM-car-distance20-10-5.mp4";
	string carBackPath9 = "D:/UserFolder/Desktop/haitec/video/car_back/華創鏡頭/FILE0268.mp4";
	string carRightPath = "D:/UserFolder/Desktop/haitec/video/cut/highway1.avi";

	string carPanoPath = "D:/UserFolder/Desktop/haitec/video/car_pano_sunny/AV1-20180101_022850.avi"; //frame 0 ~ 1000
	string carPanoPath1 = "D:/UserFolder/Desktop/haitec/video/car_pano_sunny/AV1-20180101_022350.avi"; //frame 1410

	if (mode == 0)
	{
		DetectedPosition *detectedPosition = new DetectedBack(carBackPath1);
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

	else if (mode == 2)
	{
		DetectedPosition *detectedPosition = new DetectedPano(carPanoPath1);
		ImageProcessor *backImageProcessor = new BackImageProcessor();
		ImageProcessor *rightImageProcessor = new RightImageProcessor();
		ImageProcessor *leftImageProcessor = new LeftImageProcessor();
		detectedPosition->SetImageProcessor(backImageProcessor);
		detectedPosition->SetImageProcessor(rightImageProcessor);
		detectedPosition->SetImageProcessor(leftImageProcessor);
		detectedPosition->run();
	}
	return 0;
}