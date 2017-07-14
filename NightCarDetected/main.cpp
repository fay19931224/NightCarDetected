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
	Mat leftGrayRect, rightGrayRect;
	Mat temp;
	//string path = "C:/Users/User/Dropbox/AV1-20170710_193208.avi"; 
	string path = "C:/Users/User/Dropbox/¤w°Å/AV1-20170710_194208(0-1¤À34).avi";

	//string path = "C:/Users/User/Dropbox/AV1-20170627_122941.avi";	
	ImageProcessor imageProcessor;
	VideoWriter videoWriter;
	VideoCapture capture(path);
	if (!capture.isOpened()) {
		cout << "Cannot open video" << endl;
		system("pause");
		
		return -1;
	}
	Size videoSize = Size((int)capture.get(CV_CAP_PROP_FRAME_WIDTH), (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT));
	//videoWriter.open("test.avi", capture.get(CV_CAP_PROP_FOURCC), capture.get(CV_CAP_PROP_FPS), CvSize(videoSize.width / 2, videoSize.height / 2));
	videoWriter.open("test.avi", CV_FOURCC('M', 'J', 'P', 'G'), capture.get(CV_CAP_PROP_FPS), CvSize(videoSize.width / 2, videoSize.height / 2));
	if (!videoWriter.isOpened()) 
	{
		system("pause");

		return -1;
	}
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
		Rect rightRect = Rect(0, rightGray.rows*28/100, rightGray.cols*17/20 , rightGray.rows*5/12 );
		rectangle(rightSrc, rightRect, Scalar(0, 255, 255), 1, 8, 0); // draw ROI
		rightGrayRect = rightGray(rightRect);
		temp=rightGrayRect.clone();
		

		Rect rightFrontRect = Rect(0, rightGray.rows * 28 / 100, rightGray.cols / 5, rightGray.rows * 5 / 12);
		rectangle(rightSrc, rightFrontRect, Scalar(0, 0, 255), 1, 8, 0); // draw ROI
		
		Rect rightRearRect = Rect(rightGray.cols*2/3 , rightGray.rows * 28 / 100, rightGrayRect.cols-rightGray.cols * 2 / 3, rightGray.rows * 5 / 12);
		rectangle(rightSrc, rightRearRect, Scalar(255, 0, 55), 1, 8, 0); // draw ROI

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
		*/
		
		/*
		Rect rightRectExt = Rect(0, rightGray.rows / 32 * 8, rightGray.cols / 7 * 6, rightGray.rows / 5 * 2 + rightGray.rows / 32 * 1);
		Rect rightFrontRect = Rect(right.width - rightGray.cols / 24 * 3, rightGray.rows / 32 * 9, rightGray.cols / 24 * 3, rightGray.rows / 15 * 2);
		Rect rightFrontRectExt = Rect(rightRect.width - rightGray.cols / 24 * 3, rightGray.rows / 32 * 8, rightGray.cols / 24 * 3 + 20 , rightGray.rows / 15 * 2 + rightGray.rows / 32 * 1);
		Rect rightRearRect = Rect(0, rightGray.rows / 32 * 9, rightGray.cols / 5 * 2, rightGray.rows / 5 *1 );
		Rect rightRearRectExt = Rect(0, rightGray.rows / 32 * 8, rightGray.cols / 5 * 2, rightGray.rows / 5 * 1 + rightGray.rows / 32 * 1);
		*/

		imageProcessor.extractEfficientImage(temp);
		imageProcessor.threshold_hist(temp);
		imageProcessor.removeNoice(temp);
		imageProcessor.detectLight(rightSrc, temp,0, rightGray.rows * 28 / 100, rightFrontRect, rightFrontRect);
		

		//imageProcessor.detectLight(rightSrc, rightGray, rightDst, 0, rightGray.rows / 32 * 9, rightFrontRect, rightRearRect);

		
		
		//rectangle(rightSrc, rightFrontRectExt, Scalar(0, 0, 255), 1, 8, 0); // draw ROI
		//rectangle(rightSrc, rightFrontRect, Scalar(255, 0, 55), 1, 8, 0); // draw ROI
		//rectangle(rightSrc, rightRearRectExt, Scalar(0, 0, 255), 1, 8, 0); // draw ROI
		//rectangle(rightSrc, rightRearRect, Scalar(255, 0, 55), 1, 8, 0); // draw ROI

		imshow("Right Result", rightSrc);
		imshow("Right Binary", temp);
		
		
		videoWriter << rightSrc;
		
		
		waitKey(1);
	}
	
}
