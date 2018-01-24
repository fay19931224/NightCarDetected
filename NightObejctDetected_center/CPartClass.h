#pragma once


#include <vector>

#ifndef  CPARTCLASS_H
#define  CPARTCLASS_H

using namespace std;

typedef vector<unsigned int> ThresholdSet;
typedef vector<double> ClassParameterSet;

class CPartClass
{
public:
	CPartClass(void);
	~CPartClass(void);

	//wn = zeroth moment of class
	double m_wn;
	//un = mean / wn
	double m_un;
	//vairance of the class
	double m_var;
	// mean value of the class
	double m_mn;
	// The Gray Level LowerBound t(n-1)
	unsigned int LowerBound;
	// UpperBound of gary level, tn
	unsigned int UpperBound;
	unsigned int m_OptiThres;
	double m_OptiNuValue;

	double m_wp0;	//For Binary Thresholding	
	double m_mp0;	// mean for calculate up0	
	double m_up0;	// up0 for binary thresholding
	double m_NuValue;

	bool operator<(const CPartClass& _ClassB) const
	{
		return this->LowerBound < _ClassB.LowerBound;
	}

	double m_wp1;
	double m_mp1;
	double m_up1;

	//Multithresholding Usage:	
	ClassParameterSet m_MultiPrSet;	// Class Probality Set for Multithresholding
	ClassParameterSet m_MultiUSet;	// Class Mean Set for Multithresholding	
	ThresholdSet m_OptiThresSet;	// Optimal Threshold Set

};

#endif
