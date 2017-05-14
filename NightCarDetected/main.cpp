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


void removeNoice(Mat binaryImg) {
	Mat erodeStruct = getStructuringElement(MORPH_RECT, Size(5, 5));
	erode(binaryImg, binaryImg, erodeStruct);

	imshow("Erode", binaryImg);

}


/*void detectLight(Mat srcImg, Mat binaryImg, int offsetX, int offsetY) {
	int threshold = 100;
	Mat labelImg, stats, centroids;
	int nLabels = connectedComponentsWithStats(binaryImg, labelImg, stats, centroids, 8, CV_32S);


	std::vector<cv::Vec3b> colors(nLabels);
	colors[0] = cv::Vec3b(0, 0, 0);
	//std::cout << "Number of connected components = " << nLabels << std::endl << std::endl;

	for (int label = 1; label < nLabels; ++label) {
		colors[label] = cv::Vec3b((std::rand() & 255), (std::rand() & 255), (std::rand() & 255));

		int left = stats.at<int>(label, CC_STAT_LEFT) + offsetX;
		int top = stats.at<int>(label, CC_STAT_TOP) + offsetY;
		int width = stats.at<int>(label, CC_STAT_WIDTH);
		int height = stats.at<int>(label, CC_STAT_HEIGHT);
		int area = stats.at<int>(label, CC_STAT_AREA);
		Point centroid = Point(centroids.at<double>(label, 0), centroids.at<double>(label, 1));

		if (area > 100 && abs(width / height) < 2) {
			rectangle(srcImg, Rect(left, top, width, height), Scalar(0, 0, 255), 2);

		}


	}

	//show labeling color
	Mat dst(binaryImg.size(), CV_8UC3);
	for (int r = 0; r < dst.rows; ++r) {
		for (int c = 0; c < dst.cols; ++c) {
			int label = labelImg.at<int>(r, c);
			Vec3b &pixel = dst.at<Vec3b>(r, c); //get the index of the pixel
			pixel = colors[label]; //set new pixel
		}
	}
	imshow("Labeling", dst);
}
*/

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
		if (sumOfGrayLevel>NumberOfPixel*0.95){
			//cout << i << endl;
			return i;
		}
	}		
}

int main() {	

	int previousThresholdValue = NULL;
	int ThresholdValue = 0;
	int ThresholdValueAdjust = 0;
	Mat src;
	Mat leftSrc, rightSrc;
	Mat leftROI, rightROI, rightROITemp;
	Mat leftGrey, rightGrey;	
	string path = "C://Users//¹ù«a³Í//Desktop//NightVehicleDetection//NightVehicleDetection//AV1-20170510_213352.avi";
	//string path = "C://Users//¹ù«a³Í//Desktop//allvideos//car1.avi";

	VideoCapture capture(path);
	if (!capture.isOpened()) {
		cout << "Cannot open video" << endl;
		return -1;
	}

	Size videoSize = Size((int)capture.get(CV_CAP_PROP_FRAME_WIDTH), (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT));	

	while (true) {
		capture >> src;
		if (src.empty()) {
			break;
		}

		rightSrc = src(Rect(videoSize.width / 2, videoSize.height / 2, videoSize.width / 2, videoSize.height / 2));
		cvtColor(rightSrc, rightGrey, CV_BGR2GRAY);
		Rect ROI = Rect(100, rightGrey.rows / 7 * 2, rightGrey.cols / 7 * 6, rightGrey.rows / 4 * 2);				
		rightROITemp = rightGrey(ROI);
		rightROI=rightROITemp.clone();
				
		ThresholdValue = thresholdValue(rightROI);
		if (previousThresholdValue == NULL)
		{
			previousThresholdValue = ThresholdValue;
		}
		ThresholdValueAdjust = ThresholdValue*0.875 + previousThresholdValue*0.125;
		threshold(rightROI, rightROI, ThresholdValueAdjust, 255, THRESH_BINARY); //OTSU is not necessary to set thres
		previousThresholdValue = ThresholdValueAdjust;
		erode(rightROI, rightROI,3);
		
		//removeNoice(rightROI);
		//detectLight(rightSrc, rightROI, 100, rightSrc.rows / 7 * 2);

		//imshow("Left Src", leftSrc);
		//imshow("Left Grey", leftGrey);
		//imshow("Left ROI", leftROI);

		imshow("Right Src", rightSrc);
		imshow("Right Grey", rightGrey);
		imshow("Right ROI", rightROI);

		waitKey(1);
	}



}