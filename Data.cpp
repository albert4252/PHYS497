// Data.cpp: implementation of the Data class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Data.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Data::Data()
{
	PFH=NULL;
}

Data::~Data()
{
	if (PFH!=NULL) free(PFH); PFH=NULL;
}

void Data::FindMinMax()
{	int i,k,ni,mi; 
	double d,n=0,m=0;
	k=Y.GetSize();
	if (k>0)
	{	ni=mi=0;
		n=m=Y[0];
		for (i=1;i<k;i++)
		{	
			d=Y[i]; 
			if (d<n) { n=d; ni=i;}
			if (d>m) { m=d; mi=i;}
		}
	}
	Ymin=n; iYmin=ni;
	Ymax=m; iYmax=mi;
	k=X.GetSize();
	if (k>0)
	{	ni=mi=0;
	n=m=X[0];
	for (i=1;i<k;i++)
	{	
		d=X[i]; 
		if (d<n) { n=d; ni=i;}
		if (d>m) { m=d; mi=i;}
	}
	}
	Xmin=n; iXmin=ni;
	Xmax=m; iXmax=mi;
}

