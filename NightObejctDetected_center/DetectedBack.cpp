#include "DetectedBack.h"
#include <fstream>

DetectedBack::DetectedBack(string path) :DetectedPosition(path)
{
	position = Rect(videoSize.width / 2, 0, videoSize.width / 2, videoSize.height / 2);
	//position = Rect(0, 0, videoSize.width, videoSize.height);
}

DetectedBack::~DetectedBack()
{
}

void DetectedBack::SetImageProcessor(ImageProcessor *imageProcessor)
{
	processor = imageProcessor;
}

Mat DetectedBack::getResult()
{
	return src;
}

void DetectedBack::run()
{
	
	CBrightObjectSegment brightObjectSegment(0.99);
	char filename[] = "FPS.csv";
	fstream fp;
	fp.open(filename, ios::app);//開啟檔案
	if (!fp) {//如果開啟檔案失敗，fp為0；成功，fp為非0
		cout << "Fail to open file: " << filename << endl;
	}


	capture.set(CV_CAP_PROP_POS_FRAMES, 0);
	
	for (;;)
	{
		
		capture >> src;
		if (src.empty())
		{
			break;
		}
		
		//Dividing the video to left and right part and changing image from color to gray				
		src = src(position);	
		//resize(src, src, Size(728, 484));
		cvtColor(src, gray, CV_BGR2GRAY);

		//back part
		Rect backROI = Rect(0, gray.rows / 5 * 2, gray.cols , gray.rows / 5 * 3);
		//rectangle(src, backROI, Scalar(0, 255, 255), 1, 8, 0); // draw ROI
		grayRect = gray(backROI);
		grayRectTemp = grayRect.clone();


		// src left ROI
		const int leftRectX = 0;
		const int leftRectY = gray.rows / 5 * 2;
		const int leftRectWidth = gray.cols / 3;
		const int leftRectHeight = gray.rows / 5 * 3;
		Rect leftRect = Rect(leftRectX, leftRectY, leftRectWidth, leftRectHeight);

		// src center ROI
		const int centerRectX = 0;
		const int centerRectY = gray.rows / 5 * 2;
		const int centerRectWidth = gray.cols;
		const int centerRectHeight = gray.rows / 5 * 3;
		Rect centerRect = Rect(centerRectX, centerRectY, centerRectWidth, centerRectHeight);

		// src left ROI
		const int rightRectX = gray.cols / 3 * 2;
		const int rightRectY = gray.rows / 5 * 2;
		const int rightRectWidth = gray.cols / 3;
		const int rightRectHeight = gray.rows / 5 * 3;
		Rect rightRect = Rect(rightRectX, rightRectY, rightRectWidth, rightRectHeight);

		
		// grayRectTemp ROI
		Rect leftROI = Rect(0, 0, leftRectWidth, leftRectHeight);
		Rect centerROI = Rect(0, 0, gray.cols, gray.rows / 5 * 3);
		Rect rightROI = Rect(gray.cols / 3 * 2, 0, rightRectWidth, rightRectHeight);


		ROIs.push_back(leftROI);
		ROIs.push_back(centerROI);
		ROIs.push_back(rightROI);

		//Mat left = grayRectTemp(leftROI);
		Mat center = grayRectTemp(centerROI);
		//Mat right = grayRectTemp(rightROI);

		//rectangle(src, leftRect, Scalar(255, 0, 0), 1, 8, 0); // draw Front ROI
		rectangle(src, centerRect, Scalar(0, 255, 0), 1, 8, 0); // draw Front ROI
		//rectangle(src, rightRect, Scalar(0, 0, 255), 1, 8, 0); // draw Front ROI

		


		//imshow("Right gray", gray);

		//ThresholdSet thres = brightObjectSegment.GetThresholdSet();

		//for (int i = 0; i < thres.size(); i++)
		//{
		//cout << "Thres:"  << thres[0] << endl;
		/*for (int j = 0; j < gray.cols; j++)
		{
			for (int k = 0; k < gray.rows; k++)
			{
				//cout << "pixel: " << (int)gray.at<uchar>(k, j) << endl;
				if ((int)gray.at<uchar>(k, j) <= thres[thres.size() - 4])
				{
					gray.at<uchar>(k, j) = 0;
				}
				else if ((int)gray.at<uchar>(k, j) >= thres[thres.size() - 4] && (int)gray.at<uchar>(k, j) < thres[thres.size() - 3])
				{
					gray.at<uchar>(k, j) = 32;
				}
				else if ((int)gray.at<uchar>(k, j) >= thres[thres.size() - 3] && (int)gray.at<uchar>(k, j) < thres[thres.size() - 2])
				{
					gray.at<uchar>(k, j) = 64;
				}
				else if ((int)gray.at<uchar>(k, j) >= thres[thres.size() - 2] && (int)gray.at<uchar>(k, j) < thres[thres.size() - 1])
				{
					gray.at<uchar>(k, j) = 128;
				}
				else if ((int)gray.at<uchar>(k, j) >= thres[thres.size() - 1])
				{
					gray.at<uchar>(k, j) = 255;
				}
			}
		}*/


		//}

		//imshow("Right gray thres", gray);
		//imshow("Right Binary", center);

		time_t start, finish;
		double duration = 0;
		double FPS = 0;
		start = clock();
		brightObjectSegment.getBinaryImage(center);
		processor->removeNoice(center, 8, 8, 5, 5);
		processor->detectLight(src, center, 0, gray.rows / 5 * 2, ROIs);
		
		finish = clock();
		duration = (double)(finish - start) / CLOCKS_PER_SEC;
		FPS = 1 / duration;
		cout << "time: " << duration << endl;

		stringstream ss;
		ostringstream ss1;
		ss << "FPS: " << FPS;
		ss1 << FPS;
		putText(src, ss.str(), Point(50, 50), 0, 1, Scalar(0, 0, 255), 2);
		fp << ss1.str() << endl;
		imshow("Right Result", src);
		imshow("Right Binary Result", center);

		if (capture.get(CV_CAP_PROP_POS_FRAMES) > 8880)
		{
			break;
		}
		

		

		videoWriter << src;
	
		switch (1) {
		case 1:
			cout << "frame:" << capture.get(CV_CAP_PROP_POS_FRAMES) << endl;
			waitKey(1);
			break;
		case 0:
			int key = waitKey(-1);
			if (key == 120)
			{

				frame++;
				cout << "frame:" << frame << endl;
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
