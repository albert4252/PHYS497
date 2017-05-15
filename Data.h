// Data.h: interface for the Data class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATA_H__60BEAB13_3FB2_439F_BEA2_718DB932FA36__INCLUDED_)
#define AFX_DATA_H__60BEAB13_3FB2_439F_BEA2_718DB932FA36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Data  
{
public:
	Data();
	virtual ~Data();
	void	FindMinMax();

	Carry	X,Y;
	PointF	*PFH;
	int Length;
	int	iYmin,iYmax;
	double Ymin,Ymax;
	int	iXmin,iXmax;
	double Xmin,Xmax;
	int	Y_value;		// For comment
};

#endif // !defined(AFX_DATA_H__60BEAB13_3FB2_439F_BEA2_718DB932FA36__INCLUDED_)
