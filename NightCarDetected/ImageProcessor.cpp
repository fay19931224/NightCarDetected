#include "ImageProcessor.h"


ImageProcessor::ImageProcessor()
{
}

ImageProcessor::~ImageProcessor()
{
}

void ImageProcessor::threshold_hist(Mat& src)
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

void ImageProcessor::removeNoice(Mat &src, int Eheight, int Ewidth,int Dheight,int Dwidth)
{
	
	Mat kernalELLIPSE = getStructuringElement(MORPH_ELLIPSE, Size(Eheight, Ewidth));
	Mat kernalCIRCLE = getStructuringElement(MORPH_ELLIPSE, Size(Dheight, Dwidth));
	erode(src, src, kernalELLIPSE);
	dilate(src, src, kernalCIRCLE);	
}

vector<Rect2d> ImageProcessor::getHeadLightPairs()
{
	return headLightPairs;
}

vector<ObjectDetected> ImageProcessor::getObjectDetectedVector() 
{
	return ObjectDetectedVector;
}

void ImageProcessor::setHeadLightPairs(Rect2d headLight, Mat& srcImg)
{
	ObjectTracker objectTracker;
	if (vectorOfObjectTracker.size() > 0)
	{
		for (int i = 0; i < vectorOfObjectTracker.size(); i++)
		{
			Rect2d currentTrackPos = vectorOfObjectTracker[i].getCurrentPos();

			if ((currentTrackPos & headLight).area() > 0)
			{
				vectorOfObjectTracker.erase(vectorOfObjectTracker.begin() + i);
			}
		}
	}

	objectTracker.initialize(headLight, srcImg);
	vectorOfObjectTracker.push_back(objectTracker);
		

	cout << " Size: " << vectorOfObjectTracker.size() << endl;
	for (int i = 0; i < vectorOfObjectTracker.size(); i++)
	{
		Rect2d currentTrackPos = vectorOfObjectTracker[i].getCurrentPos();
		cout << i << " " << currentTrackPos.x << " " << currentTrackPos.y << endl;
	}


	
}


#include<fstream>
void ImageProcessor::detectLight(Mat& srcImg, Mat binaryImg, int offsetX, int offsetY, Rect frontRegion) 
{
	char filename[] = "Position.txt";
	fstream fp;
	fp.open(filename, ios::app);//開啟檔案
	if (!fp) {//如果開啟檔案失敗，fp為0；成功，fp為非0
		cout << "Fail to open file: " << filename << endl;
	}
	
	Mat labelImg, stats, centroids;
	int nLabels = connectedComponentsWithStats(binaryImg, labelImg, stats, centroids, 8, CV_16U);
	ObjectDetectedVector.clear();

	for (int label = 1; label < nLabels; ++label)
	{
		int width = stats.at<int>(label, CC_STAT_WIDTH);
		int height = stats.at<int>(label, CC_STAT_HEIGHT);
		int area = stats.at<int>(label, CC_STAT_AREA);
		int left = stats.at<int>(label, CC_STAT_LEFT) + offsetX;
		int top = stats.at<int>(label, CC_STAT_TOP) + offsetY;
		Point centroid = Point(centroids.at<double>(label, 0) + offsetX, centroids.at<double>(label, 1) + offsetY);
		if (area < 2000) 
		{
			ObjectDetected objectDetected{ false,Rect(left,top,width,height),centroid ,true ,area };
			ObjectDetectedVector.push_back(objectDetected);
			//	if (ObjectDetectedVector.size() == 0)
			//	{
			//		ObjectDetected objectDetected{ false,Rect(left,top,width,height),centroid };
			//		ObjectDetectedVector.push_back(objectDetected);
			//	}
			//	else if (ObjectDetectedVector.size() > 0)
			//	{
			//		bool isReflection = false;
			//		for (int i = 0; i < ObjectDetectedVector.size(); i++)
			//		{
			//			if ((abs(ObjectDetectedVector[i].centroid.y - centroid.y) >= 10) ||
			//				(abs(ObjectDetectedVector[i].centroid.x - centroid.x) < 5))
			//				{
			//					//cout << "111111" << endl;
			//					Mat lightObject = rightGray(Rect(left, top, width, height));
			//					int sumOfGreyIntensity = 0;
			//					int sumOfGreyIntensityOfVariance = 0;
			//					double mean = 0;
			//					double variance = 0;
			//					for (int row = 0; row < lightObject.rows; row++)
			//					{
			//						for (int col = 0; col < lightObject.cols; col++)
			//						{
			//							sumOfGreyIntensity += lightObject.at<uchar>(row, col);
			//							sumOfGreyIntensityOfVariance += (lightObject.at<uchar>(row, col) * lightObject.at<uchar>(row, col));
			//						}
			//					}
			//					mean = sumOfGreyIntensity / (lightObject.rows + lightObject.cols);
			//					variance = (sumOfGreyIntensityOfVariance / (lightObject.rows + lightObject.cols)) - (mean * mean);
			//				//	cout << "mean : " << mean << endl;
			//				//	cout << "vari{ance : " << variance << endl;							
			//					if (mean < 1200 || variance > -8e06)
			//						isReflection = true;
			//				}
			//		}
			//
			//		if (!isReflection)
			//		{
			//			ObjectDetected objectDetected{ false,Rect(left,top,width,height),centroid };
			//			ObjectDetectedVector.push_back(objectDetected);
			//		}
			//	}
		}
		
	}
	

	//whether the objective is in upper postion
	for (int i = 0; i < ObjectDetectedVector.size(); i++)
	{
		for (int j = 0; j < ObjectDetectedVector.size(); j++) 
		{	
			if((abs(ObjectDetectedVector[i].centroid.x- ObjectDetectedVector[j].centroid.x)<15)
				&&(ObjectDetectedVector[i].centroid.y < ObjectDetectedVector[j].centroid.y)
				&&((ObjectDetectedVector[i].upperPosition!=false)||(ObjectDetectedVector[j].upperPosition != false)))
			{
				ObjectDetectedVector[j].upperPosition = false;
			}
		}
	}

	for (vector<ObjectDetected>::iterator it =ObjectDetectedVector.begin(); it!=ObjectDetectedVector.end();)
	{
		ObjectDetected temp=(ObjectDetected)*it;
		if (temp.upperPosition == false) 
		{
			it = ObjectDetectedVector.erase(it);
		}
		else
		{
			it++;
		}
	}	
	
	for (int i = 0; i < ObjectDetectedVector.size(); i++)
	{		
		for (int j = 0; j < ObjectDetectedVector.size(); j++)
		{
			if ((i != j) && (ObjectDetectedVector[i].isMatched == false) && (ObjectDetectedVector[j].isMatched == false))
			{
				// i is on left and  j is on right
				const double carLightDistanse= ObjectDetectedVector[j].centroid.x - ObjectDetectedVector[i].centroid.x;
				const double carLeftingDistanse = ObjectDetectedVector[i].centroid.x + carLightDistanse / 2;
				const double carLightheightDiffY = ObjectDetectedVector[j].centroid.y - ObjectDetectedVector[i].centroid.y;
				if ((abs(ObjectDetectedVector[i].centroid.y - ObjectDetectedVector[j].centroid.y) < 10) &&
					(ObjectDetectedVector[i].region.area() <= ObjectDetectedVector[j].region.area()) &&
					(carLightDistanse>1) &&(-0.0301*carLightDistanse*carLightDistanse+0.8564*carLightDistanse+575.29>=carLeftingDistanse))
				{
					ObjectDetectedVector[i].isMatched = true;
					ObjectDetectedVector[j].isMatched = true;
					Rect2d rect = Rect2d(ObjectDetectedVector[i].region.x, ObjectDetectedVector[j].region.y, (ObjectDetectedVector[j].region.x + ObjectDetectedVector[j].region.width) - ObjectDetectedVector[i].region.x, ObjectDetectedVector[j].region.height);
					
					
				
					setHeadLightPairs(rect, srcImg);

					rectangle(srcImg, rect, Scalar(0, 0, 255), 2);
					rectangle(srcImg, ObjectDetectedVector[i].region, Scalar(255, 255, 0), 2);
					rectangle(srcImg, ObjectDetectedVector[j].region, Scalar(255, 255, 0), 2);

					ostringstream strs;
					strs << carLightheightDiffY;
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
		//determine isn't carlight from far position
		if ((frontRegion.contains(ObjectDetectedVector[i].centroid)) && (ObjectDetectedVector[i].isMatched == false))
		{
			/*ostringstream strs;
			strs << ObjectDetectedVector[i].area;
			string str = strs.str();
			putText(srcImg, str, CvPoint(ObjectDetectedVector[i].region.x, ObjectDetectedVector[i].region.y - 25), 0, 1, Scalar(0, 0, 255), 2);*/

			rectangle(srcImg, ObjectDetectedVector[i].region, Scalar(0, 97, 255), 2);
		}		
	}

	for (int i = 0; i < vectorOfObjectTracker.size(); i++)
	{
		Rect2d currentTrackPos = vectorOfObjectTracker[i].getCurrentPos();

		if (currentTrackPos.x < 10)
		{
			vectorOfObjectTracker.erase(vectorOfObjectTracker.begin() + i);
		}
	}

	for (int i = 0; i < vectorOfObjectTracker.size(); i++)
	{
		vectorOfObjectTracker[i].update(srcImg);
	}
	
	fp.close();
}

int ImageProcessor::thresholdValue(Mat& src)
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

void ImageProcessor::extractEfficientImage(Mat& src)
{
	for (unsigned int col = src.cols / 3; col < src.cols; col++)//寬
	{
		for (unsigned int row = 0; row < src.rows; row++)//高
		{
			if((double(col)- double(src.cols/3)>row*8)|| (double(col) - double(src.cols / 3)>double(row*(-1) + src.rows) * 4))
				src.at<uchar>(row, col) = 0;
		}
	}	
}