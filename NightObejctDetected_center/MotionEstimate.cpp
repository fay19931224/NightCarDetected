#include "MotionEstimate.h"

#define drawCross( center, color, d )                                 \
line( srcImg, Point( center.x - d, center.y - d ), Point( center.x + d, center.y + d ), color, 2, CV_AA, 0); \
line( srcImg, Point( center.x + d, center.y - d ), Point( center.x - d, center.y + d ), color, 2, CV_AA, 0 )

MotionEstimate::MotionEstimate() {}

MotionEstimate::~MotionEstimate() {}

void MotionEstimate::initialize(Point startPos) 
{
	_KF = KalmanFilter(4, 2, 0);

	// intialization of KF...
	_KF.transitionMatrix = (Mat_<float>(4, 4) << 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1);
	_measurement = Mat_<float>(2, 1);
	_measurement.setTo(Scalar(0));

	_KF.statePre.at<float>(0) = startPos.x;
	_KF.statePre.at<float>(1) = startPos.y;
	_KF.statePre.at<float>(2) = 0;
	_KF.statePre.at<float>(3) = 0;
	setIdentity(_KF.measurementMatrix);
	setIdentity(_KF.processNoiseCov, Scalar::all(1e-4));
	setIdentity(_KF.measurementNoiseCov, Scalar::all(10));
	setIdentity(_KF.errorCovPost, Scalar::all(.1));



	_headlight.clear();
	_kalman.clear();
}

void MotionEstimate::update(Point nextPos, Mat& srcImg)
{
	// First predict, to update the internal statePre variable
	Mat prediction = _KF.predict();
	Point predictPt(prediction.at<float>(0), prediction.at<float>(1));

	// Get mouse point
	_measurement(0) = nextPos.x;
	_measurement(1) = nextPos.y;

	// The update phase 
	Mat estimated = _KF.correct(_measurement);

	Point statePt(estimated.at<float>(0), estimated.at<float>(1));
	Point measPt(_measurement(0), _measurement(1));
	// plot points
	//imshow("mouse kalman", srcImg);
	//srcImg = Scalar::all(0);

	_headlight.push_back(measPt);
	_kalman.push_back(statePt);
	drawCross(statePt, Scalar(255, 255, 255), 10);
	drawCross(measPt, Scalar(0, 0, 255), 10);

	for (int i = 0; i < _headlight.size() - 1; i++)
		line(srcImg, _headlight[i], _headlight[i + 1], Scalar(255, 255, 0), 1);

	for (int i = 0; i < _kalman.size() - 1; i++)
		line(srcImg, _kalman[i], _kalman[i + 1], Scalar(0, 155, 255), 1);
}