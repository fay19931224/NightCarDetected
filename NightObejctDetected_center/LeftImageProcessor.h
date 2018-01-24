#include <algorithm>
#include "HeadLightManager.h"
#include "ObjectDetected.h"
#include "ImageProcessor.h"

using namespace cv;
using namespace std;

class LeftImageProcessor : public ImageProcessor
{
public:
	LeftImageProcessor();
	~LeftImageProcessor();
	void threshold_hist(Mat& src);
	void removeNoice(Mat &src, int Eheight, int Ewidth, int Dheight, int Dwidth);
	void detectLight(Mat& srcImg, Mat binaryImg, int offsetX, int offsetY, vector<Rect> ROIs);
	int thresholdValue(Mat& src);
	void extractEfficientImage(Mat& src);
	void setHeadLightManager(HeadLightManager headLightManager);
	vector<ObjectDetected> getObjectDetectedVector();
	bool isCarLightHeightDiffYCorrect(int diffY, int distance);
private:
	vector<ObjectDetected> ObjectDetectedVector;
	static bool compareDistance(const ObjectDetected &a, const ObjectDetected &b);
	int previousThresholdValue = 0;
	HeadLightManager _headLightManager;
};