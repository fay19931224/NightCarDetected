#include <algorithm>
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\core\core.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "CPartClass.h"

using namespace cv;

#ifndef  CBRIGHTOBJECTSEGMENT_H
#define  CBRIGHTOBJECTSEGMENT_H

class CBrightObjectSegment
{
public:
	CBrightObjectSegment(void);
	CBrightObjectSegment(double m_SF_Threshold);
	~CBrightObjectSegment(void);

	struct FO_MaxVarPartClass {
		bool operator() (const CPartClass& classA, const CPartClass& classB) const
		{
			return (classA.m_var < classB.m_var);
		}
	};

	struct FO_MaxVWCPartClass {
		bool operator() (const CPartClass& classA, const CPartClass& classB) const
		{
			return ((classA.m_wn * classA.m_var) < (classB.m_wn * classB.m_var));
		}
	};

	void getBinaryImage(Mat grayscale_image, int frmaePos = -1, int buffer = 0);
	ThresholdSet GetThresholdSet();

protected:
	double m_ThresJD;	// Setting JD threshold

private:
	void Histogram(Mat grayscale_image);
	void Convert_Binary_Plane(Mat grayscale_image, int bright_threshold);
	int Optimum_Threshold_Decision(void);

	vector<CPartClass> m_vPartClasses;	// Pi, Probality distribution of histogram
	double m_meanT;		// Total mean
	double m_varT;		// Total variance
	double m_varBC;		// Between class variance	
	double m_varWC;		// within class variance
	double m_JD;		// JD = VBC / VT Value for judge the discriminant	
	double m_UniMeas;	// Uniformity Measure  = 1 - Vwc / VT
						//UINT m_PixCntN;	 // Total Valid number of Pixels, N	
	int m_ImagePixel;	// Total Valid number of Pixels, N	
	int m_Image_Height, m_Image_Width;
	ThresholdSet m_vThresSet;
	//ClassParameterSet m_HistoProb;
	double m_Histogram_Probility[256];

	double Update_MeanT(void);
	double Update_VarianceT(void);

	bool UpdateSingleClassInfo(CPartClass& PartClass);
	double UpdateSingleClass_wn(CPartClass& PartClass);
	void UpdateSingleClass_mean_un(CPartClass& PartClass);
	double UpdateSingleClass_var(CPartClass& PartClass);
	bool UpdateAllClassInfo(void);

	double UpdateVBCwithJD(void);
	double UpdateVWCwithUM(void);

	// Evaluate the wp0 zeroth moment for binary partition
	double CalcWp0ForBiThres(CPartClass* pPartClass, int evalThres);
	double CalcUp0ForBiThres(CPartClass* pPartClass, int evalThres);
	double EvalNuForBiThres(CPartClass* pPartClass, int evalThres);
	int CalcOptiThresForBiThres(CPartClass* pPartClass);

	int OptiBCVHistoThres(double stop_crit);
	int PerformOptiBiThres(CPartClass* pPartClass);
};

#endif
