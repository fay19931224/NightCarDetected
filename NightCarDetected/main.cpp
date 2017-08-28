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
#include "CBrightObjectSegment.h"

using namespace std;
using namespace cv;


int main() {	
	Mat src;
	Mat rightSrc, leftSrc;
	Mat rightDst, leftDst;
	Mat leftGray, rightGray;
	Mat leftGrayRect, rightGrayRect;
	Mat rightGrayRectTemp;
	

	//冠凱
	//string path = "C:/Users/User/Dropbox/已剪/city1.avi"; 
	string path = "C:/Users/User/Desktop/car_speed_OK/70km.avi";
	//string path = "C:/Users/User/Dropbox/已剪/city2.avi"; 
	//string path = "C:/Users/User/Dropbox/已剪/city3.avi"; 
	//string path = "C:/Users/User/Dropbox/已剪/highway1.avi";	
	//string path = "C:/Users/User/Dropbox/已剪/highway2.avi";
	//string path = "C:/Users/User/Dropbox/已剪/lowbrightness1.avi";
	//string path = "C:/Users/User/Dropbox/已剪/lowbrightness2.avi";
	//string path = "C:/Users/User/Dropbox/已剪/lowbrightness3.avi";
	//string path = "C:/Users/User/Dropbox/已剪/motor1.avi";
	//string path = "C:/Users/User/Desktop/大直橋/AV1-20170807_214856_20_10m.avi";
	//string path = "C:/Users/User/Desktop/大直橋/AV1-20170807_214356_40_30_20m.avi";

	//紅衣
	//string path = "C:/Users/Henry/Desktop/華創/video/AV1-20170710_194208(0-1分34).avi";
	
	
	ImageProcessor imageProcessor;

	CBrightObjectSegment brightObjectSegment(0.985);
	CBrightObjectSegment brightObjectSegment2(0.985);
	//CBrightObjectSegment brightObjectSegment3(0.90);
	
	VideoWriter videoWriter;
	VideoCapture capture(path);
	if (!capture.isOpened()) {
		cout << "Cannot open video" << endl;
		system("pause");		
		return -1;
	}
	Size videoSize = Size((int)capture.get(CV_CAP_PROP_FRAME_WIDTH), (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT));	
	videoWriter.open("test.avi", CV_FOURCC('M', 'P', '4', '2'), capture.get(CV_CAP_PROP_FPS), CvSize(videoSize.width / 2, videoSize.height / 2), true);		
	if (!videoWriter.isOpened()) 
	{
		system("pause");

		return -1;
	}

	
	int frame = 0;
	capture.set(CV_CAP_PROP_POS_FRAMES, frame);

	while (true)
	{		
		capture >> src;
		if (src.empty()) 
		{
			break;
		}
		//Dividing the video to left and right part and changing image from color to gray
		Rect right = Rect(0, videoSize.height / 2, videoSize.width / 2, videoSize.height / 2);
		rightSrc = src(right);		
		cvtColor(rightSrc, rightGray, CV_BGR2GRAY);

		//right part
		Rect rightROI = Rect(0, rightGray.rows*28/100, rightGray.cols*17/20 , rightGray.rows*5/12 );
		rectangle(rightSrc, rightROI, Scalar(0, 255, 255), 1, 8, 0); // draw ROI
		rightGrayRect = rightGray(rightROI);
		rightGrayRectTemp = rightGrayRect.clone();
		
		const int rightFrontRectX = 0;
		const int rightFrontRectY = rightGray.rows * 28 / 100; 
		const int rightFrontRectWidth = rightGray.cols * 2 / 5;
		const int rightFrontRectHeight = rightGray.rows * 5 / 12;		
		Rect rightFrontRect = Rect(rightFrontRectX, rightFrontRectY, rightFrontRectWidth, rightFrontRectHeight);		
		rectangle(rightSrc, rightFrontRect, Scalar(0, 0, 255), 1, 8, 0); // draw Front ROI
				
		//imageProcessor.extractEfficientImage(rightGrayRectTemp);

		Rect rightFrontROI = Rect(0,0, rightFrontRectWidth, rightFrontRectHeight);		
		Rect rightMiddleROI = Rect(rightFrontRectWidth, 0, rightGray.cols * 17 / 20 - rightFrontRectWidth, rightFrontRectHeight);
		/*Rect rightMiddleROI = Rect(rightFrontRectWidth, 0, rightFrontRectWidth- rightFrontRect.width/5, rightFrontRectHeight);		
		Rect rightFarROI = Rect(rightMiddleROI.x+ rightMiddleROI.width, 0, rightROI.width- (rightMiddleROI.x + rightMiddleROI.width), rightFrontRectHeight);*/

		Mat rightFront = rightGrayRectTemp(rightFrontROI);
		Mat rightMiddle = rightGrayRectTemp(rightMiddleROI);		
		//Mat rightFar = rightGrayRectTemp(rightFarROI);

		//rectangle(rightGrayRectTemp, rightFrontROI, Scalar(255, 255, 255), 1, 8, 0); // draw  ROI
		//rectangle(rightGrayRectTemp, rightMiddleROI, Scalar(255, 255, 255), 1, 8, 0); // draw  ROI
		//rectangle(rightGrayRectTemp, rightFarROI, Scalar(255, 255, 255), 1, 8, 0); // draw  ROI



		brightObjectSegment.getBinaryImage(rightFront);
		brightObjectSegment2.getBinaryImage(rightMiddle);
		//brightObjectSegment3.getBinaryImage(rightFar);
		imageProcessor.removeNoice(rightGrayRectTemp,5,5,7,7);
		imageProcessor.detectLight(rightSrc, rightGrayRectTemp,0, rightGray.rows * 28 / 100, rightFrontRect, rightMiddleROI);

		/*		
		Mat rightHSV;
		cvtColor(rightSrc, rightHSV, CV_BGR2HSV);
		Mat rightHSVRect = rightHSV(rightRect);		
		
		Mat mask = Mat::zeros(rightHSVRect.rows, rightHSVRect.cols, CV_8U); //為了濾掉其他顏色
		Mat orange;
		Mat yellow;
		Mat white;
		Mat dst;

		inRange(rightHSVRect, Scalar(11, 0, 46), Scalar(25, 70, 255), orange);
		inRange(rightHSVRect, Scalar(26, 0, 46), Scalar(34, 70, 255), yellow);
		inRange(rightHSVRect, Scalar(0, 0, 221), Scalar(180, 100, 255), white);
		
		mask = white.clone();// +yellow + orange;
		imageProcessor.removeNoice(mask, 4, 4, 10, 10);
		imshow("orange", orange);		
		imshow("yellow", yellow);
		imshow("white", white);
		imshow("Rightesult", mask);
		
		//rightHSVRect.copyTo(dst, mask);

		imshow("HSV", rightHSVRect);
		*/
				
		imshow("Right Result", rightSrc);
		imshow("Right Binary Result", rightGrayRectTemp);
		
		videoWriter << rightSrc;
		switch (1) {
			case 1:
				waitKey(1);
				break;
			case 0:
				int key = waitKey(-1);

				if (key == 120)
				{

					frame++;
					cout <<"frame:" <<frame << endl;
					continue;
				}
				else if (key == 122)
				{
					frame--;
					if (frame < 0)
					{
						frame = 0;
					}
					else
					{
						capture.set(CV_CAP_PROP_POS_FRAMES, frame);
						cout << "frame:" << frame << endl;
					}
				}
				break;
		}		
	}
	
	
}
