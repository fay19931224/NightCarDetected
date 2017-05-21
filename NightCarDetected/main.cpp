#include <string>  
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
using namespace cv;

struct ObjectDetected {
	bool isMatched;
	Rect region;
	Point centroid;
};

int previousThresholdValue = 0;
vector<ObjectDetected> ObjectDetectedVector;


void removeNoice(Mat &ROI)
{
	Mat kernalELLIPSE = getStructuringElement(MORPH_ELLIPSE, Size(8, 6));
	Mat kernalCIRCLE = getStructuringElement(MORPH_ELLIPSE, Size(6, 6));
	erode(ROI, ROI, kernalELLIPSE);
	dilate(ROI, ROI, kernalCIRCLE);
}

void detectLight(Mat srcImg, Mat rightGray, Mat binaryImg, int offsetX, int offsetY, Rect frontRegion, Rect rearRegion) {

	Mat labelImg, stats, centroids;
	int nLabels = connectedComponentsWithStats(binaryImg, labelImg, stats, centroids, 8, CV_16U);
	ObjectDetectedVector.clear();

	for (int label = 1; label < nLabels; ++label)
	{
		int width = stats.at<int>(label, CC_STAT_WIDTH);
		int height = stats.at<int>(label, CC_STAT_HEIGHT);
		int area = stats.at<int>(label, CC_STAT_AREA);
		int left = stats.at<int>(label, CC_STAT_LEFT) + offsetX;
		int top = stats.at<int>(label, CC_STAT_TOP) + offsetY;
		Point centroid = Point(centroids.at<double>(label, 0) + offsetX, centroids.at<double>(label, 1) + offsetY);

		if (area > 50)
		{						
			if (ObjectDetectedVector.size() == 0)
			{
				ObjectDetected objectDetected{ false,Rect(left,top,width,height),centroid };
				ObjectDetectedVector.push_back(objectDetected);
			}
			else if (ObjectDetectedVector.size() > 0)
			{
				bool isReflection = false;
				for (int i = 0; i < ObjectDetectedVector.size(); i++)
				{
					if ((abs(ObjectDetectedVector[i].centroid.y - centroid.y) >= 10) ||
						(abs(ObjectDetectedVector[i].centroid.x - centroid.x) < 5))
						{
							//cout << "111111" << endl;
							Mat lightObject = rightGray(Rect(left, top, width, height));
							int sumOfGreyIntensity = 0;
							int sumOfGreyIntensityOfVariance = 0;
							double mean = 0;
							double variance = 0;
							for (int row = 0; row < lightObject.rows; row++)
							{
								for (int col = 0; col < lightObject.cols; col++)
								{
									sumOfGreyIntensity += lightObject.at<uchar>(row, col);
									sumOfGreyIntensityOfVariance += (lightObject.at<uchar>(row, col) * lightObject.at<uchar>(row, col));
								}
							}
							mean = sumOfGreyIntensity / (lightObject.rows + lightObject.cols);
							variance = (sumOfGreyIntensityOfVariance / (lightObject.rows + lightObject.cols)) - (mean * mean);
						//	cout << "mean : " << mean << endl;
						//	cout << "vari{ance : " << variance << endl;							
							if (mean < 1100 || variance > -7e05)
								isReflection = true;
						}
				}

				if (!isReflection)
				{
					ObjectDetected objectDetected{ false,Rect(left,top,width,height),centroid };
					ObjectDetectedVector.push_back(objectDetected);
				}
			}
		}
	}
	
	for (int i = 0; i < ObjectDetectedVector.size(); i++)
	{		
		for (int j = 0; j < ObjectDetectedVector.size(); j++)
		{
			if ((i != j) && (ObjectDetectedVector[i].isMatched == false) && (ObjectDetectedVector[j].isMatched == false))
			{
				//i is on left and j is on right
				if ((abs(ObjectDetectedVector[i].centroid.y - ObjectDetectedVector[j].centroid.y) < 20) && 
					(ObjectDetectedVector[i].region.area() <= ObjectDetectedVector[j].region.area()) &&
					(ObjectDetectedVector[j].centroid.x - ObjectDetectedVector[i].centroid.x > 0) && 
					(ObjectDetectedVector[j].centroid.x - ObjectDetectedVector[i].centroid.x < (binaryImg.cols / 3*2)))
				{
					ObjectDetectedVector[i].isMatched = true;
					ObjectDetectedVector[j].isMatched = true;					
					Rect rect = Rect(ObjectDetectedVector[i].region.x, ObjectDetectedVector[j].region.y, (ObjectDetectedVector[j].region.x + ObjectDetectedVector[j].region.width)- ObjectDetectedVector[i].region.x, ObjectDetectedVector[j].region.height);
					rectangle(srcImg, rect, Scalar(0, 0, 255), 3);
					rectangle(srcImg, ObjectDetectedVector[i].region, Scalar(255, 0, 255), 2);
					rectangle(srcImg, ObjectDetectedVector[j].region, Scalar(255, 0, 255), 2);
				}
			}
		}
		//determine isn't carlight from far position
		if ((frontRegion.contains(ObjectDetectedVector[i].centroid)) && (ObjectDetectedVector[i].isMatched == false))
		{
			rectangle(srcImg, ObjectDetectedVector[i].region, Scalar(0, 97, 255), 2);
		}
		//determine isn't carlight from near position
		if ((rearRegion.contains(ObjectDetectedVector[i].centroid)) && (ObjectDetectedVector[i].isMatched == false)&&ObjectDetectedVector[i].region.area()>200)
		{
			rectangle(srcImg, ObjectDetectedVector[i].region, Scalar(0, 255, 255), 2);
		}
	}
}


int thresholdValue(Mat& src)
{
	int* grayLevel = new int[256];
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
		if (sumOfGrayLevel>NumberOfPixel*0.98)
		{
			if (i<20)
				return 20;
			return i;
		}
	}
}

Mat removeNoiseAndThreshold(Mat src, Rect rect)
{
	Mat ROI, ROITemp;

	ROITemp = src(rect);
	ROI = ROITemp.clone();

	
	int ThresholdValue = 0;
	int ThresholdValueAdjust = 0;
	double avgThresholdValueOfLight = 0;

	ThresholdValue = thresholdValue(ROI);

	if (previousThresholdValue == 0)
	{
		previousThresholdValue = ThresholdValue;
	}

	ThresholdValueAdjust = ThresholdValue*0.875 + previousThresholdValue*0.125;
	threshold(ROI, ROI, ThresholdValueAdjust, 255, THRESH_BINARY); //OTSU is not necessary to set thres
	previousThresholdValue = ThresholdValueAdjust;

	removeNoice(ROI);

	return ROI;
}

int main() {
	Mat src;
	Mat rightSrc, leftSrc;
	Mat rightDst, leftDst;
	Mat leftGray, rightGray;

	//string path = "C:/Users/HenryLiang/Documents/video/freeway_with_filter.mp4";
	string path = "E:/Dropbox/freeway_with_filter.mp4";
	

	VideoCapture capture(path);
	if (!capture.isOpened()) {
		cout << "Cannot open video" << endl;
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

		/*
		leftSrc = src(Rect(videoSize.width / 2, videoSize.height / 2, videoSize.width / 2, videoSize.height / 2));
		cvtColor(leftSrc, leftGray, CV_BGR2GRAY);
		Rect leftRect = Rect(100, leftGray.rows / 7 * 2, leftGray.cols / 7 * 6, leftGray.rows / 4 * 2);
		leftDst=detectLight(leftGray, leftRect);
		*/
		Rect right = Rect(0, videoSize.height / 2, videoSize.width / 2, videoSize.height / 2);
		rightSrc = src(right);
		cvtColor(rightSrc, rightGray, CV_BGR2GRAY);
		Rect rightRect = Rect(0, rightGray.rows / 32 * 9, rightGray.cols / 6 * 5, rightGray.rows / 5 * 2);
		Rect rightFrontRect = Rect(rightRect.width - rightGray.cols / 24 * 3, rightGray.rows / 32 * 9, rightGray.cols / 24 * 3, rightGray.rows / 15 * 2);
		Rect rightRearRect = Rect(0, rightGray.rows / 32 * 9, rightGray.cols / 5 * 2, rightGray.rows / 5 *1 );
		
		rightDst = removeNoiseAndThreshold(rightGray, rightRect);


		detectLight(rightSrc, rightGray, rightDst, 0, rightGray.rows / 32 * 9, rightFrontRect, rightRearRect);

		
		rectangle(rightSrc, rightRect, Scalar(255, 255, 255), 1, 8, 0); // draw ROI
		rectangle(rightSrc, rightFrontRect, Scalar(255, 0, 55), 1, 8, 0); // draw ROI
		rectangle(rightSrc, rightRearRect, Scalar(255, 0, 55), 1, 8, 0); // draw ROI
		imshow("Right Result", rightSrc);
		imshow("Right ROI", rightDst);

		waitKey(1);
	}



}