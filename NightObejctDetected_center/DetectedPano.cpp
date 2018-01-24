#include "DetectedPano.h"
#include "BackImageProcessor.h"
#include "RightImageProcessor.h"
#include "LeftImageProcessor.h"
#include <fstream>

DetectedPano::DetectedPano(string path) :DetectedPosition(path)
{
	backPosition = Rect(videoSize.width / 2 + 10, 0, videoSize.width / 2 - 10, videoSize.height / 2);
	rightPosition = Rect(videoSize.width / 2, videoSize.height / 2, videoSize.width / 2 , videoSize.height / 2);
	leftPosition = Rect(0, videoSize.height / 2, videoSize.width / 2, videoSize.height / 2);
	//position = backPosition;
}

DetectedPano::~DetectedPano()
{

}

void DetectedPano::SetImageProcessor(ImageProcessor *imageProcessor)
{
	if (BackImageProcessor *backImgPro = dynamic_cast<BackImageProcessor*>(imageProcessor))
	{
		backImageProcessor = backImgPro;
	}
	else if (RightImageProcessor *rightImgPro = dynamic_cast<RightImageProcessor*>(imageProcessor))
	{
		rightImageProcessor = rightImgPro;
	}
	else if (LeftImageProcessor *leftImgPro = dynamic_cast<LeftImageProcessor*>(imageProcessor))
	{
		leftImageProcessor = leftImgPro;
	}
}

void DetectedPano::run()
{

	CBrightObjectSegment backBrightObjectSegment(0.995);
	CBrightObjectSegment rightBrightObjectSegment(0.995);
	CBrightObjectSegment leftBrightObjectSegment(0.995);
	char filename[] = "FPS.csv";
	fstream fp;
	fp.open(filename, ios::app);//開啟檔案
	if (!fp) {//如果開啟檔案失敗，fp為0；成功，fp為非0
		cout << "Fail to open file: " << filename << endl;
	}

	VideoWriter writer;
	writer.open("test.avi", CV_FOURCC('M', 'P', '4', '2'), capture.get(CV_CAP_PROP_FPS), Size(1747, 387), true);
	if (!writer.isOpened())
	{
		cout << "Cannot write video" << endl;
		system("pause");
	}

	capture.set(CV_CAP_PROP_POS_FRAMES, 1410);

	for (;;)
	{
		int framePosition = capture.get(CV_CAP_PROP_POS_FRAMES);
		capture >> src;
		if (src.empty())
		{
			break;
		}

		cvtColor(src, gray, CV_BGR2GRAY);
		Mat back = src(backPosition);
		Mat right = src(rightPosition);
		Mat left = src(leftPosition);

		Mat grayBack = gray(backPosition);
		Rect grayCenterROI = Rect(0, grayBack.rows / 2, grayBack.cols, grayBack.rows / 2);
		Mat grayCenter = grayBack(grayCenterROI).clone();
		backROIs.push_back(grayCenterROI);
		backROIs.push_back(grayCenterROI);
		backROIs.push_back(grayCenterROI);
		backBrightObjectSegment.getBinaryImage(grayCenter);
		backImageProcessor->removeNoice(grayCenter, 5, 5, 7, 7);
		backImageProcessor->detectLight(back, grayCenter, 0, grayBack.rows / 2, backROIs);
		//imshow("binaryCenter", grayCenter);

		Mat grayRight = gray(rightPosition);
		Rect grayRightROI = Rect(0, grayRight.rows * 28 / 100, grayRight.cols * 17 / 20, grayRight.rows * 5 / 12);
		Mat grayRightFront = grayRight(grayRightROI).clone();
		rightROIs.push_back(grayRightROI);
		rightROIs.push_back(grayRightROI);
		//rightROIs.push_back(grayRightROI);
		rightBrightObjectSegment.getBinaryImage(grayRightFront);
		rightImageProcessor->removeNoice(grayRightFront, 5, 5, 7, 7);
		rightImageProcessor->detectLight(right, grayRightFront, 0, grayRight.rows * 28 / 100, rightROIs);
		//imshow("binaryRightFront", grayRightFront);

		Mat grayLeft = gray(leftPosition);
		Rect grayLeftROI = Rect(grayLeft.cols * 3 / 20, grayLeft.rows * 28 / 100, grayLeft.cols * 17 / 20, grayLeft.rows * 5 / 12);
		Mat grayLeftFront = grayLeft(grayLeftROI).clone();
		leftROIs.push_back(grayLeftROI);
		leftROIs.push_back(grayLeftROI);
		leftROIs.push_back(grayRightROI);
		leftBrightObjectSegment.getBinaryImage(grayLeftFront);
		leftImageProcessor->removeNoice(grayLeftFront, 5, 5, 7, 7);
		leftImageProcessor->detectLight(left, grayLeftFront, grayLeft.cols * 3 / 20, grayLeft.rows * 28 / 100, leftROIs);
		//imshow("binaryLeftFront", grayLeftFront);
		

		// show result image
		Mat pano = cv::Mat::ones(videoSize.height / 2, videoSize.width / 2 * 3, CV_8UC3);
		Mat binary = cv::Mat::ones(videoSize.height / 2, videoSize.width / 2 * 3, CV_8UC1);

		rectangle(back, grayCenterROI, Scalar(0, 255, 0), 1, 8, 0); // draw Front ROI
		rectangle(right, grayRightROI, Scalar(0, 255, 0), 1, 8, 0); // draw Front ROI
		rectangle(left, grayLeftROI, Scalar(0, 255, 0), 1, 8, 0); // draw Front ROI

		//draw pano
		right.copyTo(pano(Rect(0, 0, right.cols, right.rows)));
		back.copyTo(pano(Rect(right.cols, 0, back.cols, back.rows)));
		left.copyTo(pano(Rect(right.cols + back.cols, 0, left.cols, left.rows)));
		resize(pano, pano, Size(), 0.8, 0.8, INTER_LINEAR);

		//draw binary
		grayRightFront.copyTo(binary(Rect(0, 0, grayRightFront.cols, grayRightFront.rows)));
		line(binary, Point(grayRightFront.cols, 0), Point(grayRightFront.cols, grayRightFront.rows), Scalar(255, 0, 0), 3);
		grayCenter.copyTo(binary(Rect(grayRightFront.cols, 0, grayCenter.cols, grayCenter.rows)));
		line(binary, Point(grayRightFront.cols + grayCenter.cols, 0), Point(grayRightFront.cols + grayCenter.cols, grayCenter.rows), Scalar(255, 0, 0), 3);
		grayLeftFront.copyTo(binary(Rect(grayRightFront.cols + grayCenter.cols, 0, grayLeftFront.cols, grayLeftFront.rows)));
		resize(binary, binary, Size(), 0.8, 0.8, INTER_LINEAR);

		imshow("pano", pano);
		imshow("binary", binary);
		
		
		cout << "VW: " << videoSize.width / 2 * 3 << "H: " << videoSize.height / 2 << endl;
		cout << "PW: " << pano.cols << "H: " << pano.rows << endl;
		writer << pano;
		waitKey(1);
	}
}
