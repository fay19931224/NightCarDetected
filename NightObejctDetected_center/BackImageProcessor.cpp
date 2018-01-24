#include "BackImageProcessor.h"
#define ENABLE_TRACKER

BackImageProcessor::BackImageProcessor()
{
}

BackImageProcessor::~BackImageProcessor()
{
}

bool BackImageProcessor::compareDistance(const ObjectDetected &a, const ObjectDetected &b)
{
	return a.centroid.x > b.centroid.x;
}

void BackImageProcessor::setHeadLightManager(HeadLightManager headLightManager)
{
	_headLightManager = headLightManager;
}

void BackImageProcessor::threshold_hist(Mat& src)
{
	/*
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
	*/
}

void BackImageProcessor::removeNoice(Mat &src, int Eheight, int Ewidth, int Dheight, int Dwidth)
{

	Mat kernalELLIPSE = getStructuringElement(MORPH_ELLIPSE, Size(Eheight, Ewidth));
	Mat kernalCIRCLE = getStructuringElement(MORPH_ELLIPSE, Size(Dheight, Dwidth));

	erode(src, src, kernalELLIPSE);
	dilate(src, src, kernalCIRCLE);

}


vector<ObjectDetected> BackImageProcessor::getObjectDetectedVector()
{
	return ObjectDetectedVector;
}



#include<fstream>
void BackImageProcessor::detectLight(Mat& srcImg, Mat binaryImg, int offsetX, int offsetY, vector<Rect> ROIs)
{
	/*char filename[] = "Position.csv";
	fstream fp;
	fp.open(filename, ios::app);//開啟檔案
	if (!fp) {//如果開啟檔案失敗，fp為0；成功，fp為非0
		cout << "Fail to open file: " << filename << endl;
	}*/

	Mat labelImg, stats, centroids;
	const int nLabels = connectedComponentsWithStats(binaryImg, labelImg, stats, centroids, 8, CV_16U);
	ObjectDetectedVector.clear();

	_headLightManager.setBackDetectRegion(ROIs, offsetX, offsetY);



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
		if ((area > 5) && (area < 4000) && HeightWidthRatio < 2) //haitec area > 400
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

				//haitec
				//(-0.0151*pow(carLightDistanse, 2) + 2.9012*carLightDistanse + 111.56 >= carLeftingDistanse - 50)
				//&& (-0.0151*pow(carLightDistanse, 2) + 2.9012*carLightDistanse + 111.56 <= carLeftingDistanse + 50)

				//SJCAM
				//-0.4146*carLightDistanse + 293.7 >= carLeftingDistanse - 50
				//&& -0.4146*carLightDistanse + 293.7 <= carLeftingDistanse + 50

				const double carLightDistanse = ObjectDetectedVector[j].centroid.x - ObjectDetectedVector[i].centroid.x;
				const double carLeftingDistanse = srcImg.rows - ((ObjectDetectedVector[j].centroid.y + ObjectDetectedVector[i].centroid.y) / 2);
				const double carLightheightDiffY = abs(ObjectDetectedVector[j].centroid.y - ObjectDetectedVector[i].centroid.y); //height
				if (carLightheightDiffY < 5 && 
					carLightDistanse > 0 &&
					(-0.0151*pow(carLightDistanse, 2) + 2.9012*carLightDistanse + 111.56 >= carLeftingDistanse - 50)
					&& (-0.0151*pow(carLightDistanse, 2) + 2.9012*carLightDistanse + 111.56 <= carLeftingDistanse + 50))
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
					//Point p1 = Point((ObjectDetectedVector[j].centroid.x + ObjectDetectedVector[i].centroid.x) / 2, (ObjectDetectedVector[j].centroid.y + ObjectDetectedVector[i].centroid.y) / 2);
					//Point p2 = Point(srcImg.cols / 2, srcImg.rows);
					//line(srcImg, p1, p2, Scalar(255, 0, 0), 3);

					ostringstream strs;
					strs << carLightDistanse;
					string str = strs.str();
					ostringstream strs2;
					strs2 << carLeftingDistanse;
					string str2 = strs2.str();
					//putText(srcImg, str, CvPoint(carLeftingDistanse, ObjectDetectedVector[j].region.y), 0, 1, Scalar(0, 0, 255), 2);
					//putText(srcImg, str2, CvPoint(carLeftingDistanse, ObjectDetectedVector[j].region.y - 25), 0, 1, Scalar(0, 255, 0), 2);
					//fp << carLightDistanse << "," << carLeftingDistanse << endl;
				}
			}
		}
	}

	//detect single light in front ROI
	for (int i = 0; i < ObjectDetectedVector.size(); i++)
	{
		if ((ROIs[1].contains(ObjectDetectedVector[i].centroid)) && (ObjectDetectedVector[i].isMatched == false))
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
	//fp.close();
}

int BackImageProcessor::thresholdValue(Mat& src)
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

void BackImageProcessor::extractEfficientImage(Mat& src)
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

bool BackImageProcessor::isCarLightHeightDiffYCorrect(int diffY, int distance)
{
	return false;
}

