#include "DetectedBack.h"
DetectedBack::DetectedBack(string path) :DetectedPosition(path)
{
	position = Rect(videoSize.width / 2, 0, videoSize.width / 2, videoSize.height / 2);
}

DetectedBack::~DetectedBack()
{
}

void DetectedBack::SetImageProcessor(ImageProcessor *imageProcessor)
{
	processor = imageProcessor;
}

void DetectedBack::run()
{
	CBrightObjectSegment brightObjectSegment(0.999);
	CBrightObjectSegment brightObjectSegment2(0.985);

	//capture.set(CV_CAP_PROP_POS_FRAMES, 10000);
	while (true)
	{
		capture >> src;
		if (src.empty())
		{
			break;
		}
		
		//Dividing the video to left and right part and changing image from color to gray				
		src = src(position);				
		cvtColor(src, gray, CV_BGR2GRAY);

		//back part
		Rect backROI = Rect(0, gray.rows / 5 * 2, gray.cols , gray.rows / 5 * 3);
		rectangle(src, backROI, Scalar(0, 255, 255), 1, 8, 0); // draw ROI
		grayRect = gray(backROI);
		grayRectTemp = grayRect.clone();


		// src left ROI
		const int leftRectX = 0;
		const int leftRectY = gray.rows / 5 * 2;
		const int leftRectWidth = gray.cols / 3;
		const int leftRectHeight = gray.rows / 5 * 3;
		Rect leftRect = Rect(leftRectX, leftRectY, leftRectWidth, leftRectHeight);

		// src center ROI
		const int centerRectX = gray.cols / 3;
		const int centerRectY = gray.rows / 5 * 2;
		const int centerRectWidth = gray.cols / 3;
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
		Rect centerROI = Rect(gray.cols / 3, 0, gray.cols / 3, gray.rows / 5 * 3);
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

		brightObjectSegment.getBinaryImage(center);


		processor->removeNoice(grayRectTemp, 5, 5, 7, 7);
		processor->detectLight(src, grayRectTemp, 0, gray.rows / 5 * 2, ROIs);

		imshow("Right Result", src);
		imshow("Right Binary Result", center);

		videoWriter << src;
		switch (1) {
		case 1:
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
