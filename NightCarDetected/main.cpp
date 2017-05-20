#include <string>  
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;


/*void removeNoice(Mat binaryImg) {
	Mat erodeStruct = getStructuringElement(MORPH_RECT, Size(5, 5));
	erode(binaryImg, binaryImg, erodeStruct);
	imshow("Erode", binaryImg);
}*/

void detectLight(Mat srcImg, Mat binaryImg, int offsetX, int offsetY) {
	
	Mat labelImg, stats, centroids;
	int nLabels = connectedComponentsWithStats(binaryImg, labelImg, stats, centroids, 8, CV_32S);


	std::vector<cv::Vec3b> colors(nLabels);
	colors[0] = cv::Vec3b(0, 0, 0);
	//std::cout << "Number of connected components = " << nLabels << std::endl << std::endl;

	for (int label = 1; label < nLabels; ++label) 
	{		
		int width = stats.at<int>(label, CC_STAT_WIDTH);
		int height = stats.at<int>(label, CC_STAT_HEIGHT);
		int area = stats.at<int>(label, CC_STAT_AREA);
		Point centroid = Point(centroids.at<double>(label, 0), centroids.at<double>(label, 1));

		if (area > 100 && (binaryImg.rows/80<height) && (binaryImg.rows / 4>height) && (binaryImg.cols / 60<width) && (binaryImg.cols / 4>width)&&(width>height))
		{
			colors[label] = cv::Vec3b((std::rand() & 255), (std::rand() & 255), (std::rand() & 255));
			int left = stats.at<int>(label, CC_STAT_LEFT) + offsetX;
			int top = stats.at<int>(label, CC_STAT_TOP) + offsetY;
			rectangle(srcImg, Rect(left, top, width, height), Scalar(255, 255, 255), 2);
		}


	}

	//show labeling color
	/*Mat dst(binaryImg.size(), CV_8UC3);
	for (int r = 0; r < dst.rows; ++r) {
		for (int c = 0; c < dst.cols; ++c) {
			int label = labelImg.at<int>(r, c);
			Vec3b &pixel = dst.at<Vec3b>(r, c); //get the index of the pixel
			pixel = colors[label]; //set new pixel
		}
	}*/
	//imshow("Labeling", dst);
}


int thresholdValue(Mat& src)
{
	int* grayLevel=new int[256];
	int sumOfGrayLevel = 0;
	int NumberOfPixel = src.rows*src.cols;

	for (int i = 0; i < 256; i++)
	{
		grayLevel[i] = 0;
	}

	for (unsigned int row = 0; row < src.rows; row++)
	{
		for (unsigned int col = 0; col < src.cols; col++)
		{						
			grayLevel[int(src.at<uchar>(row, col))]++;	
		}
	}

	for (int i = 0; i < 256; i++)
	{
		sumOfGrayLevel += grayLevel[i];		
		if (sumOfGrayLevel>NumberOfPixel*0.99)
		{
			if(i<20)
				return 255;			
			return i;
		}
	}		
}

Mat detectLight(Mat src, Rect rect)
{
	Mat ROI,ROITemp;
	
	ROITemp = src(rect);
	ROI = ROITemp.clone();


	int previousThresholdValue = NULL;
	int ThresholdValue = 0;
	int ThresholdValueAdjust = 0;
	ThresholdValue = thresholdValue(ROI);
	if (previousThresholdValue == NULL)
	{
		previousThresholdValue = ThresholdValue;
	}
	ThresholdValueAdjust = ThresholdValue*0.875 + previousThresholdValue*0.125;
	threshold(ROI, ROI, ThresholdValueAdjust, 255, THRESH_BINARY); //OTSU is not necessary to set thres
	previousThresholdValue = ThresholdValueAdjust;

	erode(ROI, ROI, 2);
	dilate(ROI, ROI, 3);

	Mat kernalELLIPSE = getStructuringElement(MORPH_ELLIPSE, Size(4, 3));
	Mat kernalCIRCLE = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
	erode(ROI, ROI, kernalELLIPSE);
	erode(ROI, ROI, kernalELLIPSE);
	dilate(ROI, ROI,kernalCIRCLE);


	//removeNoice(rightROI);
	//detectLight(rightSrc, rightROI, 100, rightSrc.rows / 7 * 2);

	//imshow("Left Src", leftSrc);
	//imshow("Left Grey", leftGrey);
	//imshow("Left ROI", leftROI);
	
	return ROI;
}

int main() {	
	Mat src;
	Mat rightSrc, leftSrc;
	Mat rightDst, leftDst;	
	Mat leftGray, rightGray;
	
	//string path = "C://Users//¹ù«a³Í//Desktop//NightVehicleDetection//NightVehicleDetection//AV1-20170510_213352.avi";//µLÂo
	string path = "C:/Users/Henry/Documents/NightCarDetected/NightCarDetected/video/Produce.mp4";//Âo
	


	VideoCapture capture(path);
	if (!capture.isOpened()) {
		cout << "Cannot open video" << endl;
		return -1;
	}

	Size videoSize = Size((int)capture.get(CV_CAP_PROP_FRAME_WIDTH), (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT));	

	while (true)
	{
		capture >> src;
		if (src.empty()) {
			break;
		}


	/*	leftSrc = src(Rect(videoSize.width / 2, videoSize.height / 2, videoSize.width / 2, videoSize.height / 2));
		cvtColor(leftSrc, leftGray, CV_BGR2GRAY);
		Rect leftRect = Rect(100, leftGray.rows / 7 * 2, leftGray.cols / 7 * 6, leftGray.rows / 4 * 2);
		leftDst=detectLight(leftGray, leftRect);
		*/
		
		rightSrc = src(Rect(0, videoSize.height / 2, videoSize.width / 2, videoSize.height / 2));
		cvtColor(rightSrc, rightGray, CV_BGR2GRAY);
		Rect rightRect = Rect(0, rightGray.rows / 32*9, rightGray.cols/5*4 , rightGray.rows /5 * 2);
		rectangle(rightGray,rightRect,Scalar(0,0,255),5,8,0);
		rightDst = detectLight(rightGray, rightRect);


		detectLight(rightGray, rightDst, 0, rightGray.rows / 32 * 9);
		//imshow("Src", src);
		/*imshow("Left Grey", leftGray);
		imshow("Left ROI", leftDst);*/

		
		imshow("Right Grey", rightGray);
		imshow("Right ROI", rightDst);

		waitKey(1);
	}



}