#pragma once
#include <iostream>
#include <vector>
#include "opencv2\core\core.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\opencv.hpp"

using namespace std;
using namespace cv;

class OtsuMultiThreshold
{
public:
	OtsuMultiThreshold();
	~OtsuMultiThreshold();
	void getHistogram(Mat grayImg);
	double calcSumOfHist(double * histogram, int limit, int indexOfMean);
	double calcSumOfHistAndIntensity(double * histogram, int limit, int indexOfMean);
	double calcVariance(double * histogram, int limit, int indexOfMean, double mean);
	int otsuThreshold(double * histogram);
	vector<int> multiThreshold(double * histogram);
	void processMultiThreshold(Mat grayImg);
	void calcKmeans(Mat srcImg);
	double* getHistogram();
private:
	double histogram[256] = { 0.0 };
};

