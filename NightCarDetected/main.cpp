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

int previousThresholdValue = NULL;
/*void removeNoice(Mat binaryImg) {
Mat erodeStruct = getStructuringElement(MORPH_RECT, Size(5, 5));
erode(binaryImg, binaryImg, erodeStruct);
imshow("Erode", binaryImg);
}*/
struct ObjectDetected {
	bool isMatched;
	Rect region;
	Point centroid;
};

vector<ObjectDetected> ObjectDetectedVector;

void detectLight(Mat srcImg, Mat rightGray, Mat binaryImg, int offsetX, int offsetY, Rect region) {

	Mat labelImg, stats, centroids;
	int nLabels = connectedComponentsWithStats(binaryImg, labelImg, stats, centroids, 8, CV_16U);
	ObjectDetectedVector.clear();
	//std::vector<cv::Vec3b> colors(nLabels);
	//colors[0] = cv::Vec3b(0, 0, 0);

	for (int label = 1; label < nLabels; ++label)
	{
		int width = stats.at<int>(label, CC_STAT_WIDTH);
		int height = stats.at<int>(label, CC_STAT_HEIGHT);
		int area = stats.at<int>(label, CC_STAT_AREA);
		//colors[label] = cv::Vec3b((std::rand() & 255), (std::rand() & 255), (std::rand() & 255));
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
							cout << "111111" << endl;
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
							cout << "mean : " << mean << endl;
							cout << "vari{ance : " << variance << endl;
							
							if (mean < 1100 || variance > -7e05) //variance not calibrate
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

		//line(srcImg, centroid, centroid, Scalar(255, 255, 255), 5, 8, 0);
		//rectangle(srcImg, Rect(left, top, width, height), Scalar(0, 170, 255), 2);	
	}
	
	for (int i = 0; i < ObjectDetectedVector.size(); i++)
	{
		rectangle(srcImg, ObjectDetectedVector[i].region, Scalar(0, 170, 255), 2);
		for (int j = 0; j < ObjectDetectedVector.size(); j++)
		{
			if ((i != j) && (ObjectDetectedVector[i].isMatched == false) && (ObjectDetectedVector[j].isMatched == false))
			{
				//i is on left and j is on right
				if ((abs(ObjectDetectedVector[i].centroid.y - ObjectDetectedVector[j].centroid.y) < 10) && 
					(ObjectDetectedVector[i].region.area() <= ObjectDetectedVector[j].region.area()) &&
					(ObjectDetectedVector[j].centroid.x - ObjectDetectedVector[i].centroid.x > 0) && 
					(ObjectDetectedVector[j].centroid.x - ObjectDetectedVector[i].centroid.x < (binaryImg.cols / 3)))
				{
					ObjectDetectedVector[i].isMatched = true;
					ObjectDetectedVector[j].isMatched = true;
					//int top = (ObjectDetectedVector[j].region.y + ObjectDetectedVector[i].region.y) / 2;					
					Rect rect = Rect(ObjectDetectedVector[i].region.x, ObjectDetectedVector[j].region.y, (ObjectDetectedVector[j].region.x + ObjectDetectedVector[j].region.width)- ObjectDetectedVector[i].region.x, ObjectDetectedVector[j].region.height);
					rectangle(srcImg, rect, Scalar(0, 0, 255), 3);
					//rectangle(srcImg, ObjectDetectedVector[i].region, Scalar(0, 255, 0), 2);
					//rectangle(srcImg, ObjectDetectedVector[j].region, Scalar(0, 255, 0), 2);
					//rectangle(srcImg, ObjectDetectedVector[j].region, Scalar(255, 0, 0), 2);
				}
			}
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
		if (sumOfGrayLevel>NumberOfPixel*0.99)
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

	//if (ObjectDetectedVector.size() == 0)
	//{
		ThresholdValue = thresholdValue(ROI);
	//}
	//else if (ObjectDetectedVector.size() > 0) 
	//{
	//	cout << "ObjectDetectedVector.size() : " << ObjectDetectedVector.size() << endl;
	//	for (int i = 0; i < ObjectDetectedVector.size(); i++)
	//	{
	//		Mat lightObject = src(ObjectDetectedVector[i].region);
	//		avgThresholdValueOfLight += thresholdValue(lightObject);
	//	}

	//	avgThresholdValueOfLight = avgThresholdValueOfLight / ObjectDetectedVector.size();
	//}

	//cout << "avgThresholdValueOfLight : " << avgThresholdValueOfLight << endl;
	
	if (previousThresholdValue == 0)
	{
		//ThresholdValue = ObjectDetectedVector.size() > 0 ? avgThresholdValueOfLight : ThresholdValue;
		previousThresholdValue = ThresholdValue;
	}

	ThresholdValueAdjust = ThresholdValue*0.875 + previousThresholdValue*0.125;
	threshold(ROI, ROI, ThresholdValueAdjust, 255, THRESH_BINARY); //OTSU is not necessary to set thres
	previousThresholdValue = ThresholdValueAdjust;
	Mat kernalELLIPSE = getStructuringElement(MORPH_ELLIPSE, Size(8, 6));
	Mat kernalCIRCLE = getStructuringElement(MORPH_ELLIPSE, Size(6, 6));
	erode(ROI, ROI, kernalELLIPSE);

	dilate(ROI, ROI, kernalCIRCLE);
	//dilate(ROI, ROI, kernalCIRCLE);

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

	string path = "C:/Users/HenryLiang/Documents/video/freeway_with_filter.mp4";
	

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
		Rect rightRect = Rect(0, rightGray.rows / 32 * 9, rightGray.cols / 5 * 4, rightGray.rows / 5 * 2);
		rightDst = removeNoiseAndThreshold(rightGray, rightRect);


		detectLight(rightSrc, rightGray, rightDst, 0, rightGray.rows / 32 * 9, right);



		rectangle(rightSrc, rightRect, Scalar(255, 255, 255), 1, 8, 0); // draw ROI
		imshow("Right Result", rightSrc);
		imshow("Right ROI", rightDst);

		waitKey(1);
	}



}