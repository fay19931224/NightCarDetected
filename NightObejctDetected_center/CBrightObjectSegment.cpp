#include "CBrightObjectSegment.h"

using namespace cv;

CBrightObjectSegment::CBrightObjectSegment()
{
}

CBrightObjectSegment::CBrightObjectSegment(double m_SF_Threshold) : m_ThresJD(m_SF_Threshold)
{
}

CBrightObjectSegment::~CBrightObjectSegment()
{
}

void CBrightObjectSegment::getBinaryImage(Mat grayscale_image, int framePos, int buffer)
{

	if (buffer > 0)
	{
		if (framePos % buffer == 0)
		{
			m_vThresSet.clear();
		}
	}

	//Mat mat(grayscale_image, 0);
	int bright_threshold = 255;

	this->Histogram(grayscale_image);
	bright_threshold = this->Optimum_Threshold_Decision();
	//bright_threshold = 210;
	this->Convert_Binary_Plane(grayscale_image, bright_threshold);

}

ThresholdSet CBrightObjectSegment::GetThresholdSet()
{
	return m_vThresSet;
}

void CBrightObjectSegment::Histogram(Mat grayscale_image)
{
	int i, j, image_width, image_height, width_step, image_pixel;
	int histogram[256];
	uchar *image_data = (uchar*)grayscale_image.data;

	memset(histogram, 0, sizeof(histogram));
	image_width = grayscale_image.cols;
	image_height = grayscale_image.rows;
	width_step = grayscale_image.step;
	image_pixel = (image_width * image_height) >> 1;

	for (i = 0; i<image_height; i++)
		for (j = 0; j<image_width; j++)
			histogram[image_data[i*width_step + j]]++;

	for (i = 0; i<256; i++)
		m_Histogram_Probility[i] = (double)histogram[i] / (double)image_pixel;

	Update_MeanT();
	Update_VarianceT();
	m_vPartClasses.resize(1);

	m_vPartClasses[0].LowerBound = 0;
	m_vPartClasses[0].UpperBound = 255;
	UpdateSingleClassInfo(m_vPartClasses[0]);
}

int CBrightObjectSegment::Optimum_Threshold_Decision()
{
	int histogram_space = 0, bright_threshold = 255;

	histogram_space = (m_vPartClasses[0].UpperBound - m_vPartClasses[0].LowerBound);
	if (histogram_space > 2)
		OptiBCVHistoThres(m_ThresJD);

	//Draw Thresholded Bright Bmp
	if (histogram_space > 2)
		bright_threshold = *(max_element(m_vThresSet.begin(), m_vThresSet.end()));	//取最大的threshold
	else
		bright_threshold = 255;

	return bright_threshold;
}
void CBrightObjectSegment::Convert_Binary_Plane(Mat grayscale_image, int bright_threshold)
{
	register int x, y;
	int image_width, image_height, width_step;
	uchar *image_data = (uchar*)grayscale_image.data;

	image_width = grayscale_image.cols;
	image_height = grayscale_image.rows;
	width_step = grayscale_image.step;

	for (y = 0; y<image_height; y++)
	{
		for (x = 0; x<image_width; x++)
		{
			if (y>0)
			{
				if (*(image_data + y*width_step + x) < bright_threshold)
					image_data[y*width_step + x] = 0;	// Non-Bright Object
				else
					image_data[y*width_step + x] = 255;	//Bright Object
			}
			else
			{
				image_data[y*width_step + x] = 0;
			}
		}
	}
}

bool CBrightObjectSegment::UpdateSingleClassInfo(CPartClass& PartClass)
{
	UpdateSingleClass_wn(PartClass);
	UpdateSingleClass_mean_un(PartClass);
	UpdateSingleClass_var(PartClass);

	return true;
}

bool CBrightObjectSegment::UpdateAllClassInfo(void)
{
	int i;

	if (m_vPartClasses.empty())
		return false;
	for (i = 0; i<m_vPartClasses.size(); i++)
		UpdateSingleClassInfo(m_vPartClasses[i]);

	return true;
}

double CBrightObjectSegment::UpdateSingleClass_wn(CPartClass& PartClass)
{
	double wn = 0.0;
	int i;

	for (i = PartClass.LowerBound; i <= PartClass.UpperBound; i++)
		wn += m_Histogram_Probility[i];

	PartClass.m_wn = wn;
	return wn;
}

void CBrightObjectSegment::UpdateSingleClass_mean_un(CPartClass& PartClass)
{
	double mn = 0.0;
	int i;

	for (i = PartClass.LowerBound; i <= PartClass.UpperBound; i++)
		mn += (double)i * m_Histogram_Probility[i];

	PartClass.m_mn = mn;
	if (PartClass.m_wn > 0.0)
		PartClass.m_un = PartClass.m_mn / PartClass.m_wn;
	else
		PartClass.m_un = 0.0;
}


double CBrightObjectSegment::UpdateSingleClass_var(CPartClass& PartClass)
{
	double AccVarN = 0.0;
	int i;

	for (i = PartClass.LowerBound; i <= PartClass.UpperBound; i++)
		AccVarN += (((double)i - PartClass.m_un) * ((double)i - PartClass.m_un) * m_Histogram_Probility[i]) / PartClass.m_wn;

	PartClass.m_var = AccVarN;

	return AccVarN;
}

double CBrightObjectSegment::Update_MeanT(void)
{
	double AccMean = 0.0;
	int i;

	for (i = 0; i<256; i++)
		AccMean += (double)i * m_Histogram_Probility[i];

	m_meanT = AccMean;

	return AccMean;
}

double CBrightObjectSegment::Update_VarianceT(void)
{
	double AccVar = 0.0;
	int i;

	for (i = 0; i<256; i++)
		AccVar += ((double)i - m_meanT) * ((double)i - m_meanT) * m_Histogram_Probility[i];

	m_varT = AccVar;

	return AccVar;
}

// Evaluate the wp0 zeroth moment for binary partition
double CBrightObjectSegment::CalcWp0ForBiThres(CPartClass* pPartClass, int evalThres)
{
	double wp0 = 0.0;
	int i;

	//CalcHistoProb();
	for (i = pPartClass->LowerBound; i <= evalThres; i++)
		wp0 += m_Histogram_Probility[i];

	pPartClass->m_wp0 = wp0;
	pPartClass->m_wp1 = pPartClass->m_wn - pPartClass->m_wp0;

	return wp0;
}

double CBrightObjectSegment::CalcUp0ForBiThres(CPartClass* pPartClass, int evalThres)
{
	double mp0 = 0.0, mp1 = 0.0;
	int i;

	//CalcHistoProb();
	for (i = pPartClass->LowerBound; i <= evalThres; i++)
		mp0 += (double)i * m_Histogram_Probility[i];

	pPartClass->m_mp0 = mp0;
	pPartClass->m_up0 = pPartClass->m_mp0 / pPartClass->m_wp0;

	for (int i = evalThres + 1; i <= pPartClass->UpperBound; i++)
		mp1 += (double)i * m_Histogram_Probility[i];

	pPartClass->m_mp1 = mp1;
	pPartClass->m_up1 = pPartClass->m_mp1 / pPartClass->m_wp1;

	return pPartClass->m_up0;
}


double CBrightObjectSegment::EvalNuForBiThres(CPartClass* pPartClass, int evalThres)
{
	//double GlVar = m_dVariance * m_dVariance;
	double wp0 = CalcWp0ForBiThres(pPartClass, evalThres);
	double wp1 = pPartClass->m_wp1;
	double up0 = CalcUp0ForBiThres(pPartClass, evalThres);
	double up1 = pPartClass->m_up1;
	//double Nu = (wp0 * wp1 * (up1 - up0) * (up1 - up0)) / PartClass.m_var;
	double Nu = (wp0 * (pPartClass->m_un - up0) * (pPartClass->m_un - up0))
		+ (wp1 * (up1 - pPartClass->m_un) * (up1 - pPartClass->m_un));
	Nu = Nu / pPartClass->m_var;

	//pPartClass->m_NuValue = Nu;

	return Nu;
}

int CBrightObjectSegment::CalcOptiThresForBiThres(CPartClass* pPartClass)
{
	int i;
	int j = -1, k = -1, l;
	double Max_Nu = -1.0;
	int Opti_Thres = -1;

	//Using in Determine Not neccessary evaluation thresholds
	for (i = pPartClass->LowerBound; i <= pPartClass->UpperBound; i++)
	{
		if ((j < 0) && (m_Histogram_Probility[i] > 0.0))
			j = i;       /* First index */
		if (m_Histogram_Probility[i] > 0.0)
			k = i;                  /* Last index  */
	}

	//for (int i = PartClass.LowerBound ; i <= PartClass.UpperBound; i++) 
	for (l = j; l <= k; l++)
	{
		double y = EvalNuForBiThres(pPartClass, l);          /* Compute NU */
		if (y > Max_Nu)
		{                     /* Is it the biggest? */
			Max_Nu = y;       /* Yes. Save value and i */
			Opti_Thres = l;
		}
	}

	//t = m;
	pPartClass->m_OptiThres = Opti_Thres;
	pPartClass->m_OptiNuValue = Max_Nu;

	return Opti_Thres;
}

double CBrightObjectSegment::UpdateVBCwithJD(void)
{
	int i;
	double VBC_Acc = 0.0;

	if (m_vPartClasses.size() < 2)
	{
		m_varBC = 0.0;
		m_JD = 0.0;
		return 0.0;
	}

	for (i = 0; i<m_vPartClasses.size(); i++)
	{
		double tmpDiff = (m_vPartClasses[i].m_un - m_meanT);
		VBC_Acc += m_vPartClasses[i].m_wn * tmpDiff * tmpDiff;
	}

	m_varBC = VBC_Acc;
	if (m_varT > 0.0)
		m_JD = m_varBC / m_varT;
	else
		m_JD = 0.0;

	return m_varBC;
}

double CBrightObjectSegment::UpdateVWCwithUM(void)
{
	int i;
	double VWC_Acc = 0.0;

	if (m_vPartClasses.size() < 2)
	{
		m_varWC = 0.0;
		m_UniMeas = 0.0;
		return 0.0;
	}

	for (i = 0; i<m_vPartClasses.size(); i++)
	{
		VWC_Acc += m_vPartClasses[i].m_wn * m_vPartClasses[i].m_var;
	}

	m_varWC = VWC_Acc;
	if (m_varT > 0.0)
		m_UniMeas = 1.0 - (m_varWC / m_varT);
	else
		m_UniMeas = 0.0;

	return m_varWC;
}


int CBrightObjectSegment::PerformOptiBiThres(CPartClass* pPartClass)
{
	int OptiBiThres = CalcOptiThresForBiThres(pPartClass);

	CPartClass* pSplittedClass = new CPartClass;
	pSplittedClass->UpperBound = pPartClass->UpperBound;
	pPartClass->UpperBound = OptiBiThres;
	pSplittedClass->LowerBound = pPartClass->UpperBound + 1;
	UpdateSingleClassInfo(*pSplittedClass);
	UpdateSingleClassInfo(*pPartClass);
	m_vPartClasses.push_back(*pSplittedClass);
	//UpdateSingleClassInfo( m_vPartClasses.back() );
	sort(m_vPartClasses.begin(), m_vPartClasses.end());
	UpdateAllClassInfo();

	m_vThresSet.push_back(OptiBiThres);
	sort(m_vThresSet.begin(), m_vThresSet.end());

	delete pSplittedClass;

	return OptiBiThres;
}

int CBrightObjectSegment::OptiBCVHistoThres(double stop_crit)
{
	int ResultClasses = m_vPartClasses.size();

	UpdateAllClassInfo();
	UpdateVBCwithJD();

	//By JD rather than the New one
	while (m_JD < stop_crit)
	{
		//Find the Class with Maxmal Within Class Contribution
		vector<CPartClass>::iterator it_MaxVarClass = max_element(m_vPartClasses.begin(), m_vPartClasses.end(), FO_MaxVWCPartClass());

		int OptiThres = PerformOptiBiThres(&(*it_MaxVarClass));

		UpdateAllClassInfo();
		UpdateVBCwithJD();
		UpdateVWCwithUM();
	}

	UpdateVBCwithJD();
	UpdateVWCwithUM();

	ResultClasses = m_vPartClasses.size();

	return ResultClasses;
}
