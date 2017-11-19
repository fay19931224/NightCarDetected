#include "DetectedRight.h"
DetectedRight::DetectedRight(string path) :DetectedPosition(path)
{
	position= Rect(0, videoSize.height / 2, videoSize.width / 2, videoSize.height / 2);
}

DetectedRight::~DetectedRight()
{
}

void DetectedRight::SetImageProcessor(ImageProcessor *imageProcessor)
{
	processor = imageProcessor;
}


void DetectedRight::run()
{
	CBrightObjectSegment brightObjectSegment(0.985);
	CBrightObjectSegment brightObjectSegment2(0.985);
	


	while (true)
	{
		capture >> src;
		if (src.empty())
		{
			break;
		}
		//Dividing the video to left and right part and changing image from color to gray		
		Src = src(position);
		cvtColor(Src, Gray, CV_BGR2GRAY);

		//right part
		Rect ROI = Rect(0, Gray.rows * 28 / 100, Gray.cols * 17 / 20, Gray.rows * 5 / 12);
		rectangle(Src, ROI, Scalar(0, 255, 255), 1, 8, 0); // draw ROI
		GrayRect = Gray(ROI);
		GrayRectTemp = GrayRect.clone();

		const int FrontRectX = 0;
		const int FrontRectY = Gray.rows * 28 / 100;
		const int FrontRectWidth = Gray.cols * 2 / 5;
		const int FrontRectHeight = Gray.rows * 5 / 12;
		Rect FrontRect = Rect(FrontRectX, FrontRectY, FrontRectWidth, FrontRectHeight);
		rectangle(Src, FrontRect, Scalar(0, 0, 255), 1, 8, 0); // draw Front ROI

		//imageProcessor.extractEfficientImage(rightGrayRectTemp);

		Rect FrontROI = Rect(0, 0, FrontRectWidth, FrontRectHeight);
		Rect MiddleROI = Rect(FrontRectWidth, 0, Gray.cols * 17 / 20 - FrontRectWidth, FrontRectHeight);

		ROIs.push_back(FrontRect); //detect and track single light 
		ROIs.push_back(MiddleROI);


		Mat Front = GrayRectTemp(FrontROI);
		Mat Middle = GrayRectTemp(MiddleROI);

		brightObjectSegment.getBinaryImage(Front);
		brightObjectSegment2.getBinaryImage(Middle);

		processor->removeNoice(GrayRectTemp, 5, 5, 7, 7);
		processor->detectLight(Src, GrayRectTemp, 0, Gray.rows * 28 / 100, ROIs);

		imshow("Right Result", Src);
		imshow("Right Binary Result", Front);
		imshow("Right Binary Result2", Middle);

		videoWriter << Src;
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
