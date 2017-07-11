#include <string>  
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <sstream>
#include <iostream>
#include <fstream>
#include "OtsuMultiThreshold.h"
#include "ImageProcessor.h"
using namespace std;
using namespace cv;




int main() {
	Mat src;
	Mat rightSrc, leftSrc;
	Mat rightDst, leftDst;
	Mat leftGray, rightGray;
	Mat temp;
	
	//string path = "C:/Users/User/Dropbox/AV1-20170710_193208.avi"; 
	string path = "C:/Users/User/Dropbox/¤w°Å/AV1-20170710_194208(0-1¤À34).avi";

	//string path = "C:/Users/User/Dropbox/AV1-20170627_122941.avi";
	
	ImageProcessor imagepProcessor;
	VideoCapture capture(path);
	if (!capture.isOpened()) {
		cout << "Cannot open video" << endl;
		system("pause");
		return -1;
	}

	Size videoSize = Size((int)capture.get(CV_CAP_PROP_FRAME_WIDTH), (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT));
	capture.set(CV_CAP_PROP_POS_FRAMES, 0);
	while (true)
	{
		
		capture >> src;
		if (src.empty()) {
			break;
		}
		//Dividing the video to left and right part and changing image from color to gray
		Rect right = Rect(0, videoSize.height / 2, videoSize.width / 2, videoSize.height / 2);
		rightSrc = src(right);
		cvtColor(rightSrc, rightGray, CV_BGR2GRAY);


		//right part
		Rect rightRect = Rect(0, rightGray.rows*3/10, rightGray.cols*17/20 , rightGray.rows/3 );
		rectangle(rightSrc, rightRect, Scalar(0, 255, 0), 1, 8, 0); // draw ROI
		temp = rightGray(rightRect);

		/*OtsuMultiThreshold o;
		o.getHistogram(temp);
		int thres = o.otsuThreshold(o.getHistogram());
		vector<int> multiThres = o.multiThreshold(o.getHistogram());
		o.processMultiThreshold(temp);
		
		threshold(temp, temp, multiThres[2], 255, CV_THRESH_BINARY);
		cout << multiThres[0] << endl;
		cout << multiThres[1] << endl;
		cout << multiThres[2] << endl;
		cout <<endl;
		
		/*
		
		Rect rightRectExt = Rect(0, rightGray.rows / 32 * 8, rightGray.cols / 7 * 6, rightGray.rows / 5 * 2 + rightGray.rows / 32 * 1);
		Rect rightFrontRect = Rect(rightRect.width - rightGray.cols / 24 * 3, rightGray.rows / 32 * 9, rightGray.cols / 24 * 3, rightGray.rows / 15 * 2);
		Rect rightFrontRectExt = Rect(rightRect.width - rightGray.cols / 24 * 3, rightGray.rows / 32 * 8, rightGray.cols / 24 * 3 + 20 , rightGray.rows / 15 * 2 + rightGray.rows / 32 * 1);
		Rect rightRearRect = Rect(0, rightGray.rows / 32 * 9, rightGray.cols / 5 * 2, rightGray.rows / 5 *1 );
		Rect rightRearRectExt = Rect(0, rightGray.rows / 32 * 8, rightGray.cols / 5 * 2, rightGray.rows / 5 * 1 + rightGray.rows / 32 * 1);
		
		rightDst = imagepProcessor.removeNoiseAndThreshold(rightGray, rightRect);


		imagepProcessor.detectLight(rightSrc, rightGray, rightDst, 0, rightGray.rows / 32 * 9, rightFrontRect, rightRearRect);

		
		rectangle(rightSrc, rightRectExt, Scalar(0, 255, 0), 1, 8, 0); // draw ROI
		rectangle(rightSrc, rightRect, Scalar(255, 255, 255), 1, 8, 0); // draw ROI
		rectangle(rightSrc, rightFrontRectExt, Scalar(0, 0, 255), 1, 8, 0); // draw ROI
		rectangle(rightSrc, rightFrontRect, Scalar(255, 0, 55), 1, 8, 0); // draw ROI
		rectangle(rightSrc, rightRearRectExt, Scalar(0, 0, 255), 1, 8, 0); // draw ROI
		rectangle(rightSrc, rightRearRect, Scalar(255, 0, 55), 1, 8, 0); // draw ROI*/
		imshow("Right Result", rightSrc);
		imshow("Right Re", rightGray);
		
		imshow("temp", temp);
		//imshow("Right ROI", rightDst);
		
		
		waitKey(1);
	}
	
}