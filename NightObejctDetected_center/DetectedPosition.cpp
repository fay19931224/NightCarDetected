#include "DetectedPosition.h"

DetectedPosition::DetectedPosition(string path)
{
	capture.open(path);
	if (!capture.isOpened()) {
		cout << "Cannot open video" << endl;
		system("pause");
	}
	videoSize = Size((int)capture.get(CV_CAP_PROP_FRAME_WIDTH), (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT));
	videoWriter.open("test.avi", CV_FOURCC('M', 'P', '4', '2'), capture.get(CV_CAP_PROP_FPS), Size(videoSize.width / 2, videoSize.height / 2), true);
	if (!videoWriter.isOpened())
	{
		cout << "Cannot write video" << endl;
		system("pause");
	}
	frame = 0;
	capture.set(CV_CAP_PROP_POS_FRAMES, frame);
}

DetectedPosition::~DetectedPosition()
{

}


void DetectedPosition::run()
{
	cout << "test" << endl;
	system("PAUSE");
}

Mat DetectedPosition::getResult()
{
	return Mat();
}

void DetectedPosition::SetImageProcessor(ImageProcessor *imageProcessor)
{
}
