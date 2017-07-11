// OtsuMultiThreshold.cpp : Defines the entry point for the console application.
//
#include "OtsuMultiThreshold.h"

OtsuMultiThreshold::OtsuMultiThreshold()
{
}

OtsuMultiThreshold::~OtsuMultiThreshold()
{
}

void OtsuMultiThreshold::getHistogram(Mat grayImg) {
	//total number of pixels
	int N = grayImg.cols * grayImg.rows;

	//accumulate image histogram and total number of pixels
	for (int i = 0; i < grayImg.rows; i++) {
		for (int j = 0; j < grayImg.cols; j++) {
			int intensity = grayImg.at<uchar>(i, j);
			if (intensity != 0) {
				histogram[intensity]++;
			}
		}
	}


	for (int i = 0; i < 256; i++) {
		histogram[i] /= N;
		//cout << i << ":" << histogram[i] << endl;
	}
}

double OtsuMultiThreshold::calcSumOfHist(double * histogram, int limit, int indexOfMean) {
	double result = 0.0;

	if (indexOfMean == 1) {
		for (int i = 0; i < limit; i++) {
			result = result + histogram[i];
		}
	}
	else if (indexOfMean == 2) {
		for (int j = limit + 1; j < 256; j++) {
			result = result + histogram[j];
		}
	}

	return result;
}

double OtsuMultiThreshold::calcSumOfHistAndIntensity(double * histogram, int limit, int indexOfMean) {
	double result = 0.0;

	if (indexOfMean == 1) {
		for (int i = 0; i < limit; i++) {
			result = result + histogram[i] * i;
		}
	}
	else if (indexOfMean == 2) {
		for (int j = limit + 1; j < 256; j++) {
			result = result + histogram[j] * j;
		}
	}

	return result;
}

double OtsuMultiThreshold::calcVariance(double * histogram, int limit, int indexOfMean, double mean) {
	double result = 0.0;

	if (indexOfMean == 1) {
		for (int i = 0; i < limit; i++) {
			result = result + (mean - i) * (mean - i) * histogram[i];
		}
	}
	else if (indexOfMean == 2) {
		for (int j = limit + 1; j < 256; j++) {
			result = result + (mean - j) * (mean - j) * histogram[j];
		}
	}

	return result;

}


int OtsuMultiThreshold::otsuThreshold(double * histogram) {
	int minVariance = INT_MAX;
	int threshold = 0;
	/*for (int i = 0; i < 256; i++) {
		cout << i << ":" << histogram[i] << endl;
	}*/

	for (int t = 0; t < 255; t++) {
		double sumOfHistAndIntensity1 = 0.0;
		double sumOfHist1 = 0.0;
		double sumOfHistAndIntensity2 = 0.0;
		double sumOfHist2 = 0.0;
		double sumOfVariance = 0.0;


		double mean1 = 0.0, mean2 = 0.0;
		double variance1 = 0.0, variance2 = 0.0;

		sumOfHistAndIntensity1 = calcSumOfHistAndIntensity(histogram, t, 1);
		sumOfHist1 = calcSumOfHist(histogram, t, 1);
		sumOfHistAndIntensity2 = calcSumOfHistAndIntensity(histogram, t, 2);
		sumOfHist2 = calcSumOfHist(histogram, t, 2);

		mean1 = sumOfHistAndIntensity1 / sumOfHist1;
		mean2 = sumOfHistAndIntensity2 / sumOfHist2;

		variance1 = calcVariance(histogram, t, 1, mean1);
		variance2 = calcVariance(histogram, t, 2, mean2);
		sumOfVariance = variance1 + variance2;

		if (sumOfVariance < minVariance) {
			threshold = t;
			minVariance = sumOfVariance;
		}

		//cout << mean1 << " : " << mean2 << endl;
		//cout << variance1 << " : " << variance2 << endl;
	}

	return threshold;
}


vector<int> OtsuMultiThreshold::multiThreshold(double * histogram) {
	double W0K, W1K, W2K, W3K, M0, M1, M2, M3, currVarB, maxBetweenVar, M0K, M1K, M2K, M3K, MT;
	int optimalThreshold1 = 0, optimalThreshold2 = 0, optimalThreshold3 = 0;
	vector<int> multiThres;

	W0K = 0;
	W1K = 0;
	M0K = 0;
	M1K = 0;
	MT = 0;
	maxBetweenVar = 0;

	for (int k = 0; k <= 255; k++) {
		MT += k * histogram[k];
	}

	for (int t1 = 0; t1 <= 255; t1++)
	{
		W0K += histogram[t1]; //Pi
		M0K += t1 * histogram[t1]; //i * Pi
		M0 = M0K / W0K; //(i * Pi)/Pi

		W1K = 0;
		M1K = 0;

		for (int t2 = t1 + 1; t2 <= 255; t2++)
		{
			W1K += histogram[t2]; //Pi
			M1K += t2 * histogram[t2]; //i * Pi
			M1 = M1K / W1K; //(i * Pi)/Pi
			W2K = 1 - (W0K + W1K);
			M2K = MT - (M0K + M1K);

			if (W2K <= 0) break;

			M2 = M2K / W2K;

			W2K = 0;
			M2K = 0;

			for (int t3 = t2 + 1; t3 <= 255; t3++)
			{
				W2K += histogram[t3]; //Pi
				M2K += t3 * histogram[t3]; // i*Pi
				M2 = M2K / W2K; //(i*Pi)/Pi
				W3K = 1 - (W1K + W2K);
				M3K = MT - (M1K + M2K);

				M3 = M3K / W3K;
				currVarB = W0K * (M0 - MT) * (M0 - MT) + W1K * (M1 - MT) * (M1 - MT) + W2K * (M2 - MT) * (M2 - MT) + W3K * (M3 - MT) * (M3 - MT);

				if (maxBetweenVar < currVarB)
				{
					maxBetweenVar = currVarB;
					optimalThreshold1 = t1;
					optimalThreshold2 = t2;
					optimalThreshold3 = t3;
					
				}
			}
		}
	}
	//cout << "multiThres " << optimalThreshold1 << " " << optimalThreshold2 << " " << optimalThreshold3 << endl;
	multiThres.push_back(optimalThreshold1);
	multiThres.push_back(optimalThreshold2);
	multiThres.push_back(optimalThreshold3);

	return multiThres;
}


void OtsuMultiThreshold::processMultiThreshold(Mat grayImg) {
	vector<int> multiThres;
	Mat TupImg = grayImg.clone();
	multiThres = multiThreshold(histogram);
	/*for (int i = 0; i < multiThres.size(); i++) {
		cout << "multiThres " << i << " : " << multiThres[i] << endl;
	}*/

	for (int r = 0; r < grayImg.rows; r++) {
		for (int c = 0; c < grayImg.cols; c++) {
			int intensity = grayImg.at<uchar>(r, c);
			if (intensity <= multiThres[0]) {
				TupImg.at<uchar>(r, c) = 0;
			}
			else if (intensity <= multiThres[1] && intensity > multiThres[0]) {
				TupImg.at<uchar>(r, c) = 64;
			}
			else if (intensity <= multiThres[2] && intensity > multiThres[1]) {
				TupImg.at<uchar>(r, c) = 128;
			}
			else if (intensity > multiThres[2]) {
				TupImg.at<uchar>(r, c) = 255;
			}

		}
	}

	imshow("Multi", TupImg);


}

void OtsuMultiThreshold::calcKmeans(Mat srcImg) {
	//Mat src = imread("test.jpg");
	Mat samples(srcImg.rows * srcImg.cols, 3, CV_32F);
	for (int y = 0; y < srcImg.rows; y++)
		for (int x = 0; x < srcImg.cols; x++)
			for (int z = 0; z < 3; z++)
				samples.at<float>(y + x*srcImg.rows, z) = srcImg.at<Vec3b>(y, x)[z];


	int clusterCount = 4;
	Mat labels;
	int attempts = 5;
	Mat centers;
	kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers);


	Mat new_image(srcImg.size(), srcImg.type());
	for (int y = 0; y < srcImg.rows; y++)
		for (int x = 0; x < srcImg.cols; x++)
		{
			int cluster_idx = labels.at<int>(y + x*srcImg.rows, 0);
			new_image.at<Vec3b>(y, x)[0] = centers.at<float>(cluster_idx, 0);
			new_image.at<Vec3b>(y, x)[1] = centers.at<float>(cluster_idx, 1);
			new_image.at<Vec3b>(y, x)[2] = centers.at<float>(cluster_idx, 2);
		}
	imshow("clustered image", new_image);

}

double* OtsuMultiThreshold::getHistogram() {
	return histogram;
}
/*
int main()
{
	Mat srcImg;
	Mat grayImg;
	Mat binImg;
	Mat leftSrcImg;
	int thres;
	vector<int> multiThres;

	//string path = "C:/Users/Henry/Desktop/ตุณะ/video/freeway_with_filter.mp4";
	string path = "C:/Users/User/Desktop/2017-05-27 08.40.04.mp4";

	//"C:/Users/Henry/Documents/video/freeway_with_filter.mp4";
	//string path = "C:/Users/User/Dropbox/freeway_with_filter.mp4";


	VideoCapture capture(path);
	if (!capture.isOpened()) {
		cout << "Cannot open video" << endl;
		system("pause");
		return -1;
	}

	Size videoSize = Size((int)capture.get(CV_CAP_PROP_FRAME_WIDTH), (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT));
	capture.set(CV_CAP_PROP_POS_FRAMES, 0);
	while (true) {
		capture >> srcImg;
		if (srcImg.empty()) {
			break;
		}
		Rect left = Rect(0, videoSize.height / 2, videoSize.width / 2, videoSize.height / 2);
		cout << leftSrcImg.cols << " : " << leftSrcImg.rows << endl;
		leftSrcImg = srcImg(left);
		resize(leftSrcImg, leftSrcImg, Size(800, 600));
		//Rect rightRect = Rect(0, rightGray.rows / 32 * 9, rightGray.cols / 6 * 5, rightGray.rows / 5 * 2);

		cvtColor(leftSrcImg, grayImg, CV_BGR2GRAY, CV_8UC1);

		//calcKmeans(leftSrcImg);

		getHistogram(grayImg);
		thres = otsuThreshold(histogram);
		multiThres = multiThreshold(histogram);
		processMultiThreshold(grayImg);


		//cout << "Threshold 0 = " << multiThres[0] << endl;
		//cout << "Threshold 1 = " << multiThres[1] << endl;
		//cout << "Threshold 2 = " << multiThres[2] << endl;
		threshold(grayImg, binImg, multiThres[1], 255, CV_THRESH_BINARY);

		imshow("Src", leftSrcImg);
		imshow("Gray", grayImg);
		imshow("Bin", binImg);

		waitKey(1);
	}
	

	//srcImg = imread("nightCar.jpg");
	

	

	return 0;
}*/

