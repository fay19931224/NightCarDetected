#include "RightImageProcessor.h"

#define ENABLE_TRACKER

RightImageProcessor::RightImageProcessor()
{
}

RightImageProcessor::~RightImageProcessor()
{
}

bool RightImageProcessor::compareDistance(const ObjectDetected &a, const ObjectDetected &b)
{
	return a.centroid.x > b.centroid.x;
}

void RightImageProcessor::setHeadLightManager(HeadLightManager headLightManager)
{
	_headLightManager = headLightManager;
}

void RightImageProcessor::threshold_hist(Mat& src)
{

	int ThresholdValue = 0;
	int ThresholdValueAdjust = 0;

	ThresholdValue = thresholdValue(src);

	if (previousThresholdValue == 0)
	{
		previousThresholdValue = ThresholdValue;
	}

	ThresholdValueAdjust = ThresholdValue*0.875 + previousThresholdValue*0.125;
	threshold(src, src, ThresholdValueAdjust, 255, THRESH_BINARY); //OTSU is not necessary to set thres
	previousThresholdValue = ThresholdValueAdjust;
}

void RightImageProcessor::removeNoice(Mat &src, int Eheight, int Ewidth, int Dheight, int Dwidth)
{

	Mat kernalELLIPSE = getStructuringElement(MORPH_ELLIPSE, Size(Eheight, Ewidth));
	Mat kernalCIRCLE = getStructuringElement(MORPH_ELLIPSE, Size(Dheight, Dwidth));
	erode(src, src, kernalELLIPSE);
	dilate(src, src, kernalCIRCLE);
}


vector<ObjectDetected> RightImageProcessor::getObjectDetectedVector()
{
	return ObjectDetectedVector;
}



#include<fstream>
void RightImageProcessor::detectLight(Mat& srcImg, Mat binaryImg, int offsetX, int offsetY, vector<Rect> ROIs)
{
	char filename[] = "Position.txt";
	fstream fp;
	fp.open(filename, ios::app);//開啟檔案
	if (!fp) {//如果開啟檔案失敗，fp為0；成功，fp為非0
		cout << "Fail to open file: " << filename << endl;
	}

	Mat labelImg, stats, centroids;
	const int nLabels = connectedComponentsWithStats(binaryImg, labelImg, stats, centroids, 8, CV_16U);
	ObjectDetectedVector.clear();

	_headLightManager.setSideDetectRegion(ROIs, offsetX, offsetY);



	Mat srcTemp = srcImg.clone();
	//extract meaningful component
	for (int label = 1; label < nLabels; ++label)
	{
		const int width = stats.at<int>(label, CC_STAT_WIDTH);
		const int height = stats.at<int>(label, CC_STAT_HEIGHT);
		const int area = stats.at<int>(label, CC_STAT_AREA);
		const int left = stats.at<int>(label, CC_STAT_LEFT) + offsetX;
		const int top = stats.at<int>(label, CC_STAT_TOP) + offsetY;
		Point centroid = Point(centroids.at<double>(label, 0) + offsetX, centroids.at<double>(label, 1) + offsetY);
		const double HeightWidthRatio = static_cast<double>(height) / static_cast<double>(width);
		if ((area > 40) && (area < 2000) && HeightWidthRatio<2)
		{
			ObjectDetected objectDetected{ false,Rect(left,top,width,height),centroid ,true ,area };
			ObjectDetectedVector.push_back(objectDetected);
		}

	}

	//extract the object in upper postion
	for (int i = 0; i < ObjectDetectedVector.size(); i++)
	{
		for (int j = 0; j < ObjectDetectedVector.size(); j++)
		{
			if ((ObjectDetectedVector[i].centroid.y < ObjectDetectedVector[j].centroid.y) &&
				((ObjectDetectedVector[i].upperPosition != false) || (ObjectDetectedVector[j].upperPosition != false)))
			{
				int widthStart = ObjectDetectedVector[j].region.x;
				int widthEnd = ObjectDetectedVector[j].region.x + ObjectDetectedVector[i].region.width;
				while (widthStart<widthEnd)
				{
					if (ObjectDetectedVector[i].region.contains(CvPoint(widthStart, ObjectDetectedVector[i].centroid.y)))
					{
						ObjectDetectedVector[j].upperPosition = false;
						break;
					}
					widthStart++;
				}
			}
		}
	}

	for (vector<ObjectDetected>::iterator it = ObjectDetectedVector.begin(); it != ObjectDetectedVector.end();)
	{
		ObjectDetected temp = (ObjectDetected)*it;

		if (temp.upperPosition == false)
		{
			it = ObjectDetectedVector.erase(it);
		}
		else
		{
			it++;
		}

	}


	sort(ObjectDetectedVector.begin(), ObjectDetectedVector.end(), compareDistance);

	//match carlight
	for (int i = 0; i < ObjectDetectedVector.size(); i++)
	{
		for (int j = 0; j < ObjectDetectedVector.size(); j++)
		{
			if ((i != j) && (ObjectDetectedVector[i].isMatched == false) && (ObjectDetectedVector[j].isMatched == false))
			{
				// i is on left and  j is on right
				const double carLightDistanse = ObjectDetectedVector[j].centroid.x - ObjectDetectedVector[i].centroid.x;
				const double carLeftingDistanse = ObjectDetectedVector[i].centroid.x + carLightDistanse / 2;
				const double carLightheightDiffY = ObjectDetectedVector[j].centroid.y - ObjectDetectedVector[i].centroid.y;
				if ((carLightheightDiffY < 5 &&
					isCarLightHeightDiffYCorrect(carLightheightDiffY, carLeftingDistanse) &&
					(-0.0005*pow(carLightDistanse, 3) + 0.1379*pow(carLightDistanse, 2) - 14.055*carLightDistanse + 679.14 <= carLeftingDistanse)
					&& (-0.0301*pow(carLightDistanse, 2) + 0.8564*carLightDistanse + 575.29 >= carLeftingDistanse)))
				{
					ObjectDetectedVector[i].isMatched = true;
					ObjectDetectedVector[j].isMatched = true;
					Rect2d carLightRect = Rect2d(ObjectDetectedVector[i].region.x, 
												 ObjectDetectedVector[j].region.y, 
												(ObjectDetectedVector[j].region.x + ObjectDetectedVector[j].region.width) - ObjectDetectedVector[i].region.x, 
												 ObjectDetectedVector[j].region.height);

# ifdef ENABLE_TRACKER

					_headLightManager.setHeadLightPairs(carLightRect, srcImg);
# endif

					rectangle(srcImg, carLightRect, Scalar(0, 0, 255), 2);
					rectangle(srcImg, ObjectDetectedVector[i].region, Scalar(255, 255, 0), 2);
					rectangle(srcImg, ObjectDetectedVector[j].region, Scalar(255, 255, 0), 2);

					ostringstream strs;
					strs << carLightDistanse;
					string str = strs.str();
					ostringstream strs2;
					strs2 << carLeftingDistanse;
					string str2 = strs2.str();
					putText(srcImg, str, CvPoint(carLeftingDistanse, ObjectDetectedVector[j].region.y), 0, 1, Scalar(0, 0, 255), 2);
					putText(srcImg, str2, CvPoint(carLeftingDistanse, ObjectDetectedVector[j].region.y - 25), 0, 1, Scalar(0, 0, 255), 2);
					fp << carLightDistanse << "," << carLeftingDistanse << endl;
				}
			}
		}
	}

	//detect single light in front ROI
	for (int i = 0; i < ObjectDetectedVector.size(); i++)
	{
		if ((ROIs[0].contains(ObjectDetectedVector[i].centroid)) && (ObjectDetectedVector[i].isMatched == false))
		{
			/*ostringstream strs;
			strs << ObjectDetectedVector[i].area;
			string str = strs.str();
			putText(srcImg, str, CvPoint(ObjectDetectedVector[i].region.x, ObjectDetectedVector[i].region.y - 25), 0, 1, Scalar(0, 0, 255), 2);*/
			_headLightManager.setHeadLightPairs(ObjectDetectedVector[i].region, srcImg);
			rectangle(srcImg, ObjectDetectedVector[i].region, Scalar(0, 97, 255), 2);
			//rectangle(srcImg, ObjectDetectedVector[i].region, Scalar(255, 255, 255), 2);
		}
		else if (ObjectDetectedVector[i].isMatched == false)
		{
			/*_headLightManager.setHeadLightPairs(ObjectDetectedVector[i].region, srcImg);
			rectangle(srcImg, ObjectDetectedVector[i].region, Scalar(255, 255, 255), 2);*/
		}

	}


# ifdef ENABLE_TRACKER
	_headLightManager.setLightObjects(ObjectDetectedVector);
	_headLightManager.updateHeadLightPairs(srcImg, srcTemp);
# endif
	fp.close();
}

int RightImageProcessor::thresholdValue(Mat& src)
{
	int* grayLevel = new int[256];
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
		if (sumOfGrayLevel>NumberOfPixel*0.98)
		{
			if (i<20)
				return 20;
			return i;
		}
	}
}

void RightImageProcessor::extractEfficientImage(Mat& src)
{
	for (unsigned int col = src.cols / 3; col < src.cols; col++)//寬
	{
		for (unsigned int row = 0; row < src.rows; row++)//高
		{
			if ((double(col) - double(src.cols / 3)>row * 8) || (double(col) - double(src.cols / 3)>double(row*(-1) + src.rows) * 4))
				src.at<uchar>(row, col) = 0;
		}
	}
}


bool RightImageProcessor::isCarLightHeightDiffYCorrect(int diffY, int distance)
{
	switch (diffY)
	{
	case 3:
		if (distance <= 450 && distance >= 215)
			return true;
		return false;
		break;
	case 2:
		if (distance <= 505 && distance >= 240)
			return true;
		return false;
		break;
	case 1:
		if (distance <= 570 && distance >= 235)
			return true;
		return false;
		break;
	case 0:
		if (distance <= 580 && distance >= 160)
			return true;
		return false;
		break;
	case -1:
		if (distance <= 575 && distance >= 335)
			return true;
		return false;
		break;
	case -2:
		if (distance <= 575 && distance >= 240)
			return true;
		return false;
		break;
	case -3:
		if (distance <= 535 && distance >= 380)
			return true;
		return false;
		break;
	case -4:
		if (distance <= 470 && distance >= 370)
			return true;
		return false;
		break;
	case -5:
		if (distance <= 420 && distance >= 380)
			return true;
		return false;
		break;
	case -6:
		if (distance <= 335 && distance >= 310)
			return true;
		return false;
		break;
	case -7:
		if (distance <= 315 && distance >= 280)
			return true;
		return false;
		break;
	case -8:
		if (distance <= 300 && distance >= 245)
			return true;
		return false;
		break;
	case -9:
		if (distance <= 245 && distance >= 220)
			return true;
		return false;
		break;
	case -10:
		if (distance <= 225 && distance >= 205)
			return true;
		return false;
	case -11:
		if (distance <= 210 && distance >= 190)
			return true;
		return false;
	case -12:
		if (distance <= 210 && distance >= 190)
			return true;
		return false;
	case -13:
		if (distance <= 190 && distance >= 165)
			return true;
		return false;
	case -14:
		if (distance <= 165 && distance >= 150)
			return true;
		return false;
	default:
		return false;
		break;
	}
}
