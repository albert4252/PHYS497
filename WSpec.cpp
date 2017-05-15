// WSpec.cpp: implementation of the WSpec class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WSpec.h"
#include "math.h"
#include "io.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WSpec::WSpec()
{
	m_D=NULL; m_E=NULL; m_C=NULL; m_Fat=NULL;
	m_Clen=m_Dlen=m_Elen=m_Flen=0;
	m_valid=m_ext=FALSE;	
	m_type=UNKNOWN;
}

WSpec::~WSpec()
{
	KillData();
	KillFAT();
}

WSpec& WSpec::operator=(const WSpec& U)
{
	if (&U!=this) Vordub(U);
	return *(WSpec *)this; 
}

void WSpec::Vordub(const WSpec &U)					// This construction enables to use this 
{													// also as a subroutine
		// Copy constructor changes these
	m_ext=FALSE;			// We unlink thus we know
	m_Flen=0;				// Spectra FAT length is temporary
	if (m_Fat!=NULL) free(m_Fat); m_Fat=NULL;
		//	CFile F;		// Remains undetermined, is temporary			
		// The real copy
	type=U.type;			
	start=U.start;
	step=U.step;
	length=U.length;
	time=U.time;
	m_yreclen=U.m_yreclen;
	m_xreclen=U.m_xreclen;
	m_spno=U.m_spno;
	m_type=U.m_type;
	m_length=U.m_length;		
	m_xmin=U.m_xmin;
	m_xmax=U.m_xmax;
	m_ymin=U.m_ymin;
	m_ymax=U.m_ymax;
	m_valid=U.m_valid;			
		// Copy & unlink Comment & Data buffers
	if (m_C!=NULL) free(m_C); m_C=NULL;	m_Clen=U.m_Clen;
	if (m_D!=NULL) free(m_D); m_D=NULL;	m_Dlen=U.m_Dlen;
	if (m_E!=NULL) free(m_E); m_E=NULL;	m_Elen=U.m_Elen;
	if (m_Clen>0)
	{	m_C=(char *)malloc(m_Clen);
		memmove(m_C,U.m_C,m_Clen);
	}
	if (m_Dlen>0)
	{	m_D=(char *)malloc(m_Dlen);
		memmove(m_D,U.m_D,m_Dlen);
	}
	if (m_Elen>0)
	{	m_E=(char *)malloc(m_Elen);
		memmove(m_E,U.m_E,m_Elen);
	}
}

void WSpec::Serialize(CArchive& ar)
{	int i,j,l,*ip;
	j=sizeof(int);	
	F.Serialize(ar);				// Serialize CFile
	if (ar.IsStoring())
	{
		if (m_ext) m_ext=FALSE;		// Change for archival purposes spectrum into internally allocated one 
		ar << length << m_length << m_spno << m_type << m_ext;
		ar << m_valid << m_xmax << m_xmin << m_ymax << m_ymin << m_xreclen << m_yreclen;
		ar << start << step << time << type;
		if (! m_valid)				// Data in spectrum is not valid anyway, so why bother saving data, save zero lengths
		{	long L=0; ar << L << L << L;
		}
		else  		  ar << m_Clen << m_Dlen << m_Elen;
		if (m_C != NULL && m_Clen>0 && m_valid)
		{	l=(m_Clen+j)/j; 
			ip=(int *)m_C;
			ar << l;
			for (i=0; i<l; i++) ar << *(ip+i);
		}
		if (m_D != NULL && m_Dlen>0 && m_valid)
		{	
			l=m_Dlen/j; 
			ip=(int *)m_D;
			ar << l;
			for (i=0; i<l; i++) ar << *(ip+i);
		}
		if (m_E != NULL && m_Elen>0 && m_valid)
		{	
			l=m_Elen/j; 
			ip=(int *)m_E;
			ar << l;
			for (i=0; i<l; i++) ar << *(ip+i);
		}
	}
	else
	{	if (m_C != NULL) { free(m_C); m_C=NULL;} 	// even if it is externally allocated spectrum, we claim it now
		if (m_D != NULL) { free(m_D); m_D=NULL;}						
		if (m_E != NULL) { free(m_E); m_E=NULL;}
		if (m_Fat != NULL) { free(m_Fat); m_Fat=NULL;}
		m_Flen=0;									// Purge SFAT completely  and we did not even store it
		ar >> length >> m_length >> m_spno >> m_type >> m_ext;
		ar >> m_valid >> m_xmax >> m_xmin >> m_ymax >> m_ymin >> m_xreclen >> m_yreclen;
		ar >> start >> step >> time >> type;
		ar >> m_Clen >> m_Dlen >> m_Elen;
		if (m_valid && m_Clen>0)
		{	ar >> l;
			m_C=(char *)malloc(l*j);
			if (m_C != NULL)
			{
				ip=(int *)m_C;
				for (i=0; i<l; i++) ar >> *(ip+i);
			}
		}
		if (m_valid && m_Dlen>0)
		{	
			ar >> l;
			m_D=malloc(l*sizeof(int));
			if (m_D != NULL)
			{
				ip=(int *)m_D;
				for (i=0; i<l; i++) ar >> *(ip+i);
			}
		}
		if (m_valid && m_Elen>0)
		{	
			ar >> l;
			m_E=malloc(l*sizeof(int));
			if (m_E != NULL)
			{
				ip=(int *)m_E;
				for (i=0; i<l; i++) ar >> *(ip+i);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
//			SpectraSolve Formats									//
//////////////////////////////////////////////////////////////////////
//	Type 1: spectrum structure (SP90)
//  Start(D), Step(D), Length(I2), Time(L), Comment[30]
//  {Length * 4 YData} 
//////////////////////////////////////////////////////////////////////
//	Type 2: spectrum structure (SP92)
//  Start(D), Step(D), Length(I2), Time(L), 
//  {Length * 4 YData}, zlpComment, [padding] 
//////////////////////////////////////////////////////////////////////
//	Type 3: spectrum structure
//  Type(L), Start(D), Step(D), Length(L), Time(L), Yreclen(C)
//  {Length * Yreclen YData}, ComLen(L), zlpComment, [padding] 
//////////////////////////////////////////////////////////////////////
//	Type 4: spectrum structure
//	Type(L), Length(L), Time(L), Yreclen(C), {Length * Yreclen YData},
//  Xreclen(C), {Length * Xreclen XData},
//  ComLen(L), zlpComment, [padding] 
//////////////////////////////////////////////////////////////////////

// Get the type(record length) of X or Y axis
long	WSpec::GetDataType(bool YT)
{
	if (!YT && type==4) return (long)m_xreclen; 
	else				return (long)m_yreclen;
}

double	WSpec::GetStart()
{	
	if (type==4) 
	{	if (m_E==NULL) return -1;
		else 
		{	if (m_xreclen==4) return (double)*(float *)m_E; 
			else return *(double *)m_E;
		}
	}
	else		 return start;
}

double	WSpec::GetStep()
{	
	long l=length-1;
	if (type==4) 
	{	if (m_E==NULL || l<=1) return -1;
		else 
		{	if (m_xreclen==8) 
			{	step=(*((double *)m_E+1) - *(double *)m_E);	// 1st - 2nd point
			}
			else 
			{	step=(*((float *)m_E+1) - *(float *)m_E); 
			}
		}
	}
	return step;
}

double	WSpec::GetStop()
{	
	long l=length-1;
	if (type==4) 
	{	if (m_E==NULL) return -1;
		else 
		{	if (m_xreclen==4) return (double)*((float *)m_E+l); 
			else return *((double *)m_E+l);
		}
	}
	else		 return start+(double)l*step;
}

long WSpec::GetPos(double X)									// Works only for monotone
{
	long i,j,k,l;
	double x,n,m,s;
	if (type==4) 
	{	if (m_E==NULL || length<1) return -1;					// error, do not use
		if (length>10000)										// Huge arrays, use binary iteration  (works only for monotone X)
		{
			k=0; j=length-1;
			if (m_xreclen==4)	{n=*((float *)m_E+k); m=*((float *)m_E+j);}			// Starting & End point X values
			else				{n=*((double *)m_E+k); m=*((double *)m_E+j);}
			s=(m-n)/fabs(m-n);									// the sign
			if (s>0) { if (X<n) return k; if (X>m) return j; }  // Out of the range		
			else	 { if (X<n) return j; if (X>m) return k; }
			i=length/2;								
			while (i>0)
			{
				i=(j-k)/2;
				l=k+i;											// position in the center		
				if (m_xreclen==4)	x=*((float *)m_E+l);		// centerpos X value
				else				x=*((double *)m_E+l);
				if (fabs(x-X)<1e-20) return l;					// almost Equal
				if (s>0)						// X increases
				{
					if (X>x) {n=x; k=l;}
					else	 {m=x; j=l;}
				}
				else							// X decreases
				{
					if (X<x) {n=x; k=l;}
					else	 {m=x; j=l;}
				}
			}
			if (fabs(n-X)>fabs(m-X)) return j; else return k;
		}
		else
		{
			if (m_xreclen==4)	n=*((float *)m_E); 
			else				n=*((double *)m_E);
			m=fabs(X-n);
			for (i=1,k=0;i<length;i++)	
			{	
				if (m_xreclen==4)	x=*((float *)m_E+i); 
				else				x=*((double *)m_E+i);
				if (m>fabs(X-x)) {n=x; m=fabs(X-n); k=i;} 
			}
			return k;
		}
	}
	else return (long)((X-start)/step+.5);						// Linear, we can calculate
}

	// Macro type checking
BOOL	WSpec::SetReclen(char &ytype)
{	
	switch(ytype)
	{
	default:	ytype=4; return FALSE;
	case   10:
	case	8:
	case	4:	return TRUE;
	}
}

//	SetHeader Does not allocate data -> data is assumed to be external
//	returns with TRUE if header seems to be valid
BOOL	WSpec::SetHeader(void *Y,void *X,long len,char ytype,char xtype)
{	
	BOOL retcode=FALSE;
	if (Y!=NULL) 
	{	if (m_D!=NULL) 
		{	if (!m_ext) free(m_D);
			m_D=NULL;
		}
		m_D=Y;
	}
	if (X!=NULL) 
	{	if (m_E!=NULL) 
		{	if (!m_ext) free(m_E);
			m_E=NULL;
		}
		m_E=X;
	}
	if (len>1) length=len;
	SetReclen(ytype);
	m_yreclen=ytype;
	if (SetReclen(xtype))	m_xreclen=xtype;
	else					m_xreclen=ytype;
	type=4;
	m_ext=TRUE;
	return Valid();
}

//	returns with TRUE if header seems to be valid
BOOL	WSpec::SetHeader(void *Y,double Start,double Step,long len,char ytype)
{	
	BOOL retcode=FALSE;
	if (Y!=NULL) 
	{	if (m_D!=NULL) 
		{	if (!m_ext) free(m_D);
			m_D=NULL;
		}
		m_D=Y;
	}
	if (len>1) length=len;
	SetReclen(ytype);
	m_yreclen=ytype;
	if (Start!=-1) start=Start;
	if (Step!=0) step=Step;
	type=3;
	m_ext=TRUE;
	Valid();
	return Valid();
}

BOOL	WSpec::Valid(int force)
{	
	if (force==0) m_valid=FALSE;		
	else if (force==1) m_valid=TRUE;	// Be careful with this!
	else
	{	if (type>0 && type<5 && length<2) return m_valid=FALSE;
		if (m_D==NULL || m_C==NULL || (type==4 && m_E==NULL)) return m_valid=FALSE;
		else m_valid=TRUE;
	}
	return m_valid;
}

BOOL	WSpec::SetComment(CString S) 
{	BOOL r;
	r=SetComment((char *)LPCTSTR(S));
	return r;
} 
// SetComment sets m_C pointer to the comment C
// In file the comment is written with the trailing zero
BOOL	WSpec::SetComment(char *C)
{	
	int l=strlen(C);
	if (C==NULL || l<0) return FALSE;		// Nothing to set
	if (m_ext)								// WSpec does not allocate memory
	{	m_C=C;
	}
	else
	{
		if (m_C!=NULL) 						// WSpec allocates memory
		{	free(m_C); m_C=NULL;			// 
		}
//		l=l*2+10;
		//ll++;
		m_C=(char *)malloc(l+1);
		memset(m_C,0,l+1);
		if (m_C!=NULL)
		{	memmove(m_C,C,l);				// Actually copy comment here
		}
	}
	m_Clen=strlen(m_C);						// Calculate the real length	 
//	*(m_C+m_Clen)=0;
	m_Clen++;
	return Valid();
}

// SetSS Type 3 is self-explanatory
// Type 4: calculates X-axis from start & step 
BOOL	WSpec::SetSS(double Start, double Step)
{	
	long i;
	double x;
	float f;
	if (fabs(Step)<1e-38  || fabs(Step)>1e38)  return FALSE;
	if (fabs(Start)<-1e38 || fabs(Start)>1e38) return FALSE;
	if (type==4)	// In case we have XY pairs we make X-axis out of it
	{	if (m_E==NULL) return FALSE;
		switch(m_xreclen)
		{default:	return FALSE;
		 case 8:	x=Start;
					for (i=0;i<length;i++,x+=Step) 
					*(double *)((double *)m_E+i)=x; 
					break;
		 case 4:	f=(float)Start;
					for (i=0;i<length;i++,f+=(float)Step) 
					*(float *)((float *)m_E+i)=f; 
					break;
		}
	}
	else
	{	start=Start; step=Step;						// We must aquit
	}
	return TRUE;
}

void	WSpec::SetTime(long t)
{	
	if (t) time=t;
	else    
	{
		SYSTEMTIME st;				// Here, this takes into account the local DST correction too
		GetLocalTime(&st);
		CTime theTime(st);			
		time=theTime.GetTime();		// Oldtime compatibility
	}
}

BOOL	WSpec::Create(long len,char ytype,char xtype)
{	
	if (len<=1) return FALSE;
	if (ytype<=4 || ytype>10) ytype=4;
	else if (ytype<=8) ytype=8;
	else ytype=10;
	if (xtype<1 || xtype>10) xtype=ytype;
	KillData();	
	type=4;
	length=len;
	m_yreclen=ytype;
	m_xreclen=xtype;
	m_Dlen=m_yreclen*len;
	m_Elen=m_xreclen*len;
	m_ext=FALSE;		// internal Data
	SetTime();
	m_D=malloc(ytype*len);
	m_E=malloc(xtype*len);
	if (m_D==NULL || m_E==NULL) 
	{	if (m_E != NULL) free(m_E); 
		if (m_D != NULL) free(m_D); 
		m_D=m_E=NULL;
		m_valid=FALSE;
	}
	m_valid=TRUE;
	return m_valid;
}

BOOL	WSpec::CleanCreate(double start, double step, long len,char ytype,char xtype)
{
	Create( len, ytype, xtype);
	long i,j; 
	if (m_valid) 
	{	j=len*(long)ytype/4L;
		for (i=0;i<j;i++) 	*((long *)m_D+i)=0;
			 if (xtype==8) for (i=0;i<len;i++)	*((double *)m_E+i)=start+(double)i*step;
		else if (xtype==4) for (i=0;i<len;i++)	*((float *)m_E+i) =(float)(start+(float)i*step);
	}
	return m_valid;
}

BOOL	WSpec::CleanCreate(long len,char ytype,char xtype,double xn,double xm)
{
	Create( len, ytype, xtype);
	long i; 
	double s;
	if (m_valid) 
	{	if (len>1)	s=(xm-xn)/(double)(len-1);
		else		s=0;
		if (ytype==8) for (i=0;i<len;i++)		*((double *)m_D+i)=0;		// Zero           y coordinate
		else		  for (i=0;i<len;i++)		*((float *)m_D+i)=0;
		if (xtype==8) for (i=0;i<len;i++,xn+=s) *((double *)m_E+i)=xn;		// Create desired x coordinate
		else		  for (i=0;i<len;i++,xn+=s) *((float *)m_E+i)=(float)xn;			
	}
	return m_valid;
}

BOOL	WSpec::GrowBy(long add)				// Leave old data in buffer and grow it by add rec_len units
{											
	if (add<1)								// Here we actually shrink, no need to reallocate 
	{	
		if (length>=add) {length+=add; return TRUE;}
		else return FALSE;
	}
	else
	{
		long len;
		void *y;
		len=length*m_yreclen;
		y=NULL; 
		y=malloc(len); 
		if (y==NULL) return FALSE;
		memmove(y,m_D,len);
		free(m_D); 
		m_valid=FALSE;						// so far we actually screwed up nothing, but now, the spectrum is gone
		m_D=NULL;
		m_D=malloc(len+add*m_yreclen);
		if (m_D==NULL) return FALSE;
		memmove(m_D,y,len);
		free(y);
		if (type==4)
		{	len=length*m_xreclen;
			y=NULL;
			y=malloc(len);
			if (y==NULL) return FALSE;
			memmove(y,m_E,len);
			free(m_E);
			m_E=NULL;
			m_E=malloc(len+add*m_xreclen);
			if (m_E==NULL) return FALSE;
			memmove(m_E,y,len);
			free(y);
		}
		length+=add;
	}
	m_valid=TRUE;
	return m_valid;
}

BOOL	WSpec::CleanCreate(double Start,double Step,long len,char ytype)
{
	Create( Start, Step, len, ytype);
	long i,j; 
	j=len*(long)ytype/4L;
	if (m_valid) 
		for (i=0;i<j;i++) 
			*((long *)m_D+i)=0;
	return m_valid;
}

BOOL	WSpec::Create(double Start,double Step,long len,char ytype)
{	
	if (len<=1) return FALSE;
	if (ytype<1 || ytype>10) ytype=4;
	if (ytype<=4) ytype=4;
	else if (ytype<=8) ytype=8;
	else ytype=10;
	KillData();	
	type=3;		// Type can be set later to 1 or 2 -> SP90 or SP92
	length=len;
	m_yreclen=ytype;
	start=Start;
	step=Step;
	m_ext=FALSE;		// internal Data
	SetTime();
	m_Dlen=ytype*len;
	m_D=malloc(m_Dlen);
	if (m_D==NULL)	{m_valid=FALSE; m_Dlen=0;}
	else			 m_valid=TRUE;
	return m_valid;
}

BOOL	WSpec::CreateComment(char *C)
{	
	long len;
	if (m_ext) return SetComment(C);		// If databuffers are external just use the pointer 
	if (C!=NULL) len=strlen(C);
	if (len>0)
	{	if (m_C!=NULL) {free(m_C); m_C=NULL;}	
		m_C=(char *)malloc(len+1);
		m_Clen=len;
		strcpy(m_C,C);
	}
	return TRUE;
}

BOOL	WSpec::CreateComment(long len)
{	
	if (m_ext) return FALSE;	// Cannot make comment buffer inside now
	if (len>0)
	{	if (m_C!=NULL) {free(m_C); m_C=NULL;}	
		m_C=(char *)malloc(len);
		m_Clen=len;
	}
	return TRUE;
}

void WSpec::Unlink()  
{	
	if (m_ext==TRUE) 
	{	m_ext=FALSE; m_D=NULL; m_E=NULL; m_C=NULL;
		m_valid=FALSE;
	}
} 

void WSpec::KillData()
{	
	Unlink();	// This just unlinks data allocated elsewhere
	if (m_Dlen>0 && m_D != NULL)			free(m_D);  
	if (type==4 && m_Elen>0 && m_E != NULL)	free(m_E); 
	if (m_C != NULL)						free(m_C);  
	m_D=NULL; m_E=NULL; m_C=NULL;
	m_Elen=m_Dlen=m_Clen=0; 
	m_valid=m_ext=FALSE;
}

//---------------------------
//	Data Writing Subroutines	
void WSpec::KillFAT()
{	
	free(m_Fat); m_Fat=NULL; m_Flen=0;
}

BOOL WSpec::ReadFAT()
{TRY
{	
	if (GetSpecs()<0L) return FALSE;
	KillFAT();
	m_Flen=m_spno*sizeof(long);
	m_Fat=(long *) malloc (m_Flen+2*sizeof(long)); 
	memset(m_Fat,0,m_Flen);				// Clear FAT for Safety
	F.Seek(-m_Flen,CFile::end);
	if (m_Flen>0)
	{	F.Read(m_Fat,m_Flen);
		F.Seek(-m_Flen,CFile::end);
	}
	return TRUE;
}
CATCH( CFileException, e ) 
{	return FALSE;
}   
END_CATCH
}

BOOL WSpec::Append97()
{
TRY
{
	F.Write(&type,4);
	if (type==3)
	{	F.Write(&start,8); 
		F.Write(&step,8); 
	}
	F.Write(&length,4);
	F.Write(&time,4);
	F.Write(&m_yreclen,1);
	m_Dlen=length*(long)m_yreclen;
	F.Write(m_D,m_Dlen);
	if (type == 4)
	{	F.Write(&m_xreclen,1);
		m_Elen=length*(long)m_xreclen;
		F.Write(m_E,m_Elen);
	}
	m_Clen=strlen(m_C)+1;				// write also 0 into the comment
	F.Write(&m_Clen,4);
	F.Write(m_C,m_Clen);
	F.Write(m_Fat,m_Flen);
	F.Seek(28,CFile::begin);
	m_spno++;
	F.Write(&m_spno,4);
	return TRUE;
}
CATCH( CFileException, e ) 
{	return FALSE;
}   
END_CATCH
}

BOOL WSpec::Append92()
{TRY
{	
	F.Write(&start,8); 
	F.Write(&step,8); 
	F.Write(&length,2);
	F.Write(&time,4);
	if (m_type==SP90) 
	{	m_C[29]=0;
		F.Write(m_C,30);
	}
	m_Dlen=length*4L;
	F.Write(m_D,m_Dlen);
	m_Clen=strlen(m_C)+1;					// write also 0 into the comment
	if (m_type==SP92) F.Write(m_C,m_Clen);
	F.Write(m_Fat,m_Flen);
	F.Seek(30,CFile::begin);
	m_spno++;
	F.Write(&m_spno,2);
	return TRUE;
}
CATCH( CFileException, e ) 
{	return FALSE;
}   
END_CATCH
}

BOOL WSpec::Append(const wchar_t *Fname, int Ftype)
{
TRY
{	
	long ptr,tyyp;
	BOOL retcod;
	if (m_valid==FALSE) return FALSE;		// Will even not try to write invalid data
	tyyp = Check((const wchar_t *)Fname);
	if (tyyp==UNEXIST)	
	{
		if (Create((const wchar_t *)Fname, Ftype) == FALSE)
			return FALSE; 
		else tyyp=Ftype;
	}
	if (tyyp < SP90 || tyyp > SP97) return FALSE; 
	F.Open((const wchar_t *)Fname, CFile::modeReadWrite | CFile::typeBinary | CFile::shareDenyWrite);
	m_type=tyyp;
	m_length=F.GetLength();
	if (!ReadFAT()) return FALSE;
	ptr=F.Seek(-m_Flen,SEEK_END);		// Seek to the beginning of sfat
	*(m_Fat+m_spno)=ptr; m_Flen+=4;			// Update Sfat in memory
	switch(m_type)
	{case SP90: 
	 case SP92: retcod=Append92(); break;
	 case SP97: retcod=Append97(); break;
	}
	KillFAT();
	F.SeekToEnd();
	F.Close();
	return retcod;
}
CATCH( CFileException, e ) 
{	return FALSE;
}   
END_CATCH
}

//----------------------------------
//	File Checking and Creation Subroutines 
BOOL WSpec::Verify(const wchar_t *Fname)
{	int ty=Check(Fname);
	if (ty==SP90 || ty== SP92 || ty==SP97) return TRUE;
	return FALSE;
}

long WSpec::Check(const wchar_t *Fname, long *nosp)
{
TRY
{	
	CFile C;
	long l,type=UNKNOWN; 
	char wsh[32],*title="SPECTRA ";
	if (_waccess(Fname,0) != 0)			return UNEXIST; 
	C.Open(Fname,CFile::typeBinary|CFile::modeRead);
	l=C.Read(wsh,32);
	C.Close();
	if (l==32 && strnicmp(wsh,title,8) == 0) 
	{	if (wsh[8]==':') 
		{	type=SP97; if (nosp!=NULL) *nosp=*(long *)(wsh+28);
		} 
		else 
		{	if ((wsh[0x15] > 0x2f) && (wsh[0x15] < 0x3a)) type=SP92; 
			else type=SP90;
			if (nosp!=NULL) *nosp=*(short *)(wsh+30); 

		}
	}
	return type;	
}
CATCH( CFileException, e ) 
{	return READERR;
}   
END_CATCH
}

BOOL WSpec::Create(const wchar_t *Fname, int Ftype)
{
TRY
{	
	char *Hd[3]={"SPECTRA - S.Savikhin Software\0\0\0","SPECTRA - S.Savikhin 67\0tware\0\0\0",
		         "SPECTRA : S.Savikhin Softwar\0\0\0\0"};
	CFile C(Fname,CFile::typeBinary|CFile::modeWrite|CFile::modeCreate);	
	C.Write(Hd[Ftype],32);
	C.Close();
	return TRUE;
}
CATCH( CFileException, e ) 
{	return FALSE;
}   
END_CATCH
}

long WSpec::GetSpecs(const wchar_t *Fname)
{	
	char wsh[32];
	CFile C(Fname,CFile::typeBinary|CFile::modeRead);	
	C.SeekToBegin();	
	*(long *)(wsh+28)=0L;
	C.Read(wsh,32);
	C.Close();
	if (*(wsh+8)=='-')	m_spno=*(short *)(wsh+30);
	else				m_spno= *(long *)(wsh+28); 
	return m_spno;	
}

long WSpec::GetSpecs()
{	
	char wsh[32];
	F.SeekToBegin();	
	*(long *)(wsh+28)=0L;
	F.Read(wsh,32);
	if (m_type==SP97)	m_spno= *(long *)(wsh+28);
	else				m_spno=*(short *)(wsh+30); 
	return m_spno;	
}

	// Gets a pointer to spec in file, On error returns -1
long WSpec::GetPointer(long no)
{
TRY
{	long ptr;
	if (GetSpecs()<=0) return -1;
	ptr=(no-1-m_spno)*4L; 
	F.Seek(ptr,CFile::end);
	F.Read(&ptr,4);
	return ptr;
}
CATCH( CFileException, e ) 
{	return -1;
}   
END_CATCH
}

//--------------------------
//	Spectrum read subroutines
BOOL WSpec::Read97()
{
TRY
{
	int i;
	F.Read(&type,4);
	if (type==3)
	{	F.Read(&start,8);
		F.Read(&step,8);
	}
	F.Read(&length,4);
	F.Read(&time,4);
	F.Read(&m_yreclen,1);
	m_Dlen=(long)m_yreclen*length;
	if (m_D==NULL)
	{	if ((m_D=malloc(m_Dlen))==NULL)	return FALSE;
	}
	if (F.Read(m_D,m_Dlen) != (unsigned)m_Dlen)	return FALSE;
	if (type==4)
	{	F.Read(&m_xreclen,1);
		m_Elen=(long)m_xreclen*length;
		if (m_E==NULL)
		{	if ((m_E=malloc(m_Elen))==NULL)	return FALSE;
		}
		if (F.Read(m_E,m_Elen) != (unsigned)m_Elen)	return FALSE;
	}
	if ((i=F.Read(&m_Clen,4)) != 4)  m_Clen=0;
	if (m_Clen>0)
	{	if (m_C==NULL)
		{	if ((m_C=(char *) malloc (m_Clen+1))==NULL) return FALSE;
		}
		i=F.Read(m_C,m_Clen);
		*(m_C+i)=0;
	}
	return TRUE;
}
CATCH( CFileException, e ) 
{	return FALSE;
}   
END_CATCH
}

BOOL WSpec::Read92()
{
TRY
{
	int i; 
	length=0;		// We will read only 2 bytes, so clear it first 
	type=3;
	m_yreclen=4;	// Always
	m_Clen=256;
	m_C=(char *) malloc (m_Clen);
	F.Read(&start,8);
	F.Read(&step,8);
	F.Read(&length,2);
	F.Read(&time,4);
	if (m_type==SP90)
	{	if (F.Read(m_C,30) != 30)  return FALSE;
	}
	m_Dlen=(long)m_yreclen*length;
	if (m_D==NULL)
	{	if ((m_D=malloc(m_Dlen))==NULL)	return FALSE;
	}
	if (F.Read(m_D,m_Dlen) != (unsigned)m_Dlen)	return FALSE;

	if (m_type==SP92)		// i do not care about comment here, so i will just read more than enough in
	{	i=F.Read(m_C,m_Clen);
		if (i>0) i--; 
		if (i>=0) m_C[i]=0;		// 0 the last character inside the buffer
	}
	return TRUE;
}
CATCH( CFileException, e ) 
{	return FALSE;
}   
END_CATCH
}

// General read request, filetype
BOOL WSpec::Read(const wchar_t *Fname,long no)
{	
TRY
{
	long ptr, tyyp;
	BOOL retcod;
	tyyp = Check((const wchar_t *)Fname);							// Is it Spectra type file
	if (tyyp < SP90 || tyyp > SP97) return FALSE;
	m_type = tyyp;
	F.Open(Fname, CFile::modeRead | CFile::typeBinary | CFile::shareDenyWrite);
	if ((ptr = GetPointer(no)) < 0) return FALSE;	// Pointer to Spectrum
	F.Seek(ptr, CFile::begin);
	if (!m_ext) KillData();						// If internal buffers, kill them
	switch (m_type)
	{
	case SP97: retcod = Read97(); break;		// Internal read of SP97 
	case SP90:
	case SP92:	retcod = Read92(); break;
	default:	retcod = FALSE; break;
	}
	F.Close();
	if (!retcod && !m_ext) KillData();
	else FindMinMax();							// Find real minimum and maximum
	m_valid = retcod;
	return retcod;
}
CATCH( CFileException, e ) 
{	m_valid=FALSE;
	return FALSE;
}   
END_CATCH
}

////////////////////////////////////////////////////////////////////////////
// Manipulate data in buffers
void WSpec::FindMM(double *p, double &n, double &m,int len)
{	
	double f;
	n=m=*p;
	if (!len && len<=length) len=length;			// If (0<len<length), use only first len points for minmax
	for (long i=1;i<len;i++)						// This option is used at load time when latter points are 0-s
	{	f=*(p+i);
		if (f>m) m=f;
		if (f<n) n=f;
	}
}

void WSpec::FindMM(float *p, double &n, double &m,int len)
{	
	float f;
	n=m=*p;
	if (!len && len<=length) len=length;			// If (0<len<length), use only first len points for minmax
	for (long i=1;i<len;i++)						// This option is used at load time when latter points are 0-s		
	{	f=*(p+i);
		if (f>m) m=f;
		if (f<n) n=f;
	}
}

void WSpec::FindMinMax(int len)
{	double u;
	if (type==3) 
	{	m_xmin=start; m_xmax=start+step*((double)length-1.);
		if (m_xmin>m_xmax) { u=m_xmin; m_xmin=m_xmax; m_xmax=u; }
	}
	else
	{	if (m_E!=NULL)
		{	if (m_xreclen==4)		FindMM((float *)m_E,m_xmin,m_xmax);
			else if (m_xreclen==8)	FindMM((double *)m_E,m_xmin,m_xmax);
		}
	}
	if (m_D!=NULL)
	{	if (m_yreclen==4)		FindMM((float *)m_D,m_ymin,m_ymax,len);
		else if (m_yreclen==8)	FindMM((double *)m_D,m_ymin,m_ymax,len);
	}
}

BOOL WSpec::GetPoint(double &x,double &y,long i)
{	
	if (m_D==NULL || (type==4 && m_E==NULL) || i>=length || i<0) return FALSE;
	if (m_yreclen==4) y=*(float *)((float *)m_D+i);
	else if (m_yreclen==8) y=*(double *)((double *)m_D+i);
	if (type==4) 
	{	if (m_xreclen==4) x=*(float *)((float *)m_E+i);
		else if (m_xreclen==8) x=*(double *)((double *)m_E+i);
	}
	else x=start+step*i;
	return TRUE;
}

BOOL WSpec::SetPoint(double x,double y,long i)
{	

	if (m_D==NULL || (type==4 && m_E==NULL) || i>=length || i<0) return FALSE;
	if (m_yreclen==4) *(float *)((float *)m_D+i)=(float)y;
	else if (m_yreclen==8) *(double *)((double *)m_D+i)=y;
	if (type==4) 
	{	if (m_xreclen==4) *(float *)((float *)m_E+i)=(float)x;
		else if (m_xreclen==8) *(double *)((double *)m_E+i)=x;
	}
	return TRUE;
}

BOOL WSpec::GetRealMM(double &xmin,double &xmax,double &ymin,double &ymax)
{	
	if (m_D==NULL || (type==4 && m_E==NULL)) return FALSE;
	xmin=m_xmin; xmax=m_xmax; ymin=m_ymin; ymax=m_ymax;	
	return TRUE;
}

BOOL WSpec::TransferData(double *Y,long l)
{	
	long i,len; float *p;
	if (m_D==NULL || Y==NULL || l<2) return FALSE;
	len=l;
	if (length<l) len=length;
	if (m_yreclen==8)
	{	memmove(Y,m_D,len*sizeof(double));
		return TRUE;
	}
	if (m_yreclen==4)
	{	p=(float *)m_D;
		for (i=0;i<len;i++) *(Y+i)=(double)(*(p+i));	
		return TRUE;
	}
	return FALSE;
}

BOOL WSpec::TransferData(float *Y,long l)
{	
	long i,len; double *p;
	if (m_D==NULL || Y==NULL || l<2) return FALSE;
	len=l;
	if (length<l) len=length;
	if (m_yreclen==4)
	{	memmove(Y,m_D,len*sizeof(float));
		return TRUE;
	}
	if (m_yreclen==8)
	{	p=(double *)m_D;
		for (i=0;i<len;i++) *(Y+i)=(float)(*(p+i));	
		return TRUE;
	}
	return FALSE;
}

BOOL WSpec::GetAt(double &x,double &y)		// Linear interpolation
{	BOOL retcod=TRUE;
	double sto,X,Y,xm,xn,ym,yn,d; 
	long i,j;
	if (m_D==NULL || (type==4 && m_E==NULL)) {y=0; return FALSE;} 
	if (type<4)						// All linear spectra types
	{	
		sto=GetStop();
		if ((x<start && step>0) || (x>start && step<0))	// Both way extrapolation
		{	X=(start-x)/step;		// Positive	
			i=0; j=1;
		}
		else if ((x>sto && step>0) || (x<sto && step<0))
		{	X=(x-sto)/step;			// Positive
			i=length-1; j=length-2;
		}
		else if (step>0)			// Interpolation
		{
			X=(x-start)/step; 
			i=(long)floor(X); 
			j=i+1; 
			X=(double)i-X;			// Negative X
		}
		else
		{
			X=(x-start)/step;		// X>0
			i=(long)ceil(X); 
			j=i-1; 
			X=(double)i-X;			// Negative
		}
		GetPoint(d,yn,i);		
		ym=yn;						// Safety
		GetPoint(d,ym,j);		
		y=yn+(yn-ym)*X;		
	}
	else if (type==4) 
	{	
		xn=-1e38; xm=1e38;
		for (i=0;i<length;i++)
		{	
			GetPoint(X,Y,i);
			if (X>=x && X<xm) { xm=X; ym=Y; }	// if x matches xm=xn=x
			if (X<=x && X>xn) { xn=X; yn=Y; }	// also			ym=yn=y
		}
		if (xn==-1e38 && xm==1e38) {y=Y; return FALSE;} 
		if (xn==-1e38)			// All {X} were bigger than x
		{	y=ym; return TRUE;} 
		if (xm==1e38)			// All {X} were smaller than x
		{	y=yn; return TRUE;} 
		if (fabs(xm-xn)>1e-38)  d=(x-xn)/(xm-xn);	// Normally
		else					d=1;				// ideal match
		y=ym*d+yn*(1-d);		// linear interpolation
	}
	else return FALSE;
	return TRUE;
}

// Exchange points if Xj > Xi
void	WSpec::Swap(int i,int j)
{	
	double x1,y1,x2,y2;
	if (m_xreclen==4)
	{	
		x1=*(float *)((float *)m_E+i);
		x2=*(float *)((float *)m_E+j);
		if (x1<x2) return;
		y1=*(float *)((float *)m_D+i);
		y2=*(float *)((float *)m_D+j);
		*(float *)((float *)m_E+j)=(float)x1; 
		*(float *)((float *)m_E+i)=(float)x2; 
		*(float *)((float *)m_D+j)=(float)y1; 
		*(float *)((float *)m_D+i)=(float)y2; 
	}
	else
	{
		x1=*(double *)((double *)m_E+i);
		x2=*(double *)((double *)m_E+j);
		if (x1<x2) return;
		y1=*(double *)((double *)m_D+i);
		y2=*(double *)((double *)m_D+j);
		*(double *)((double *)m_E+j)=x1; 
		*(double *)((double *)m_E+i)=x2; 
		*(double *)((double *)m_D+j)=y1; 
		*(double *)((double *)m_D+i)=y2; 
	}
}
// Make from XY data a linear spectrum, deallocate
void	WSpec::Linearize()
{	
	int i,j;
	double x,y;
	if (length<2) return;
	for (j=0;j<length-1;j++)
		for (i=j+1;i<length;i++)
			Swap(j,i);					
	// Eliminate same pairs 
	if (m_yreclen==4) 
	{	start=*((float *)m_E);
		step=(*((float *)m_E+length-1)-start)/(float)(length-1);
		for (i=0;i<length;i++) 
		{	x=start+step*(double)i;
			GetAt(x,y);
			*((float *)m_D+i)=(float)y;
		}
	}
	else			
	{	start=*((double *)m_E);
		step=(*((double *)m_E+length-1)-start)/(double)(length-1);
		for (i=0;i<length;i++) 
		{	x=start+step*(double)i;
			GetAt(x,y);
			*((double *)m_D+i)=y;
		}
	}
	free(m_E); m_E=NULL; m_Elen=0;
	type=3;
}

// Make from Y data a XY spectrum, allocate
void	WSpec::Pair()
{	int i;
	if (m_E!=NULL) {free(m_E);m_E=NULL;}
	m_E=(char *)malloc(m_Dlen); 
	if (m_E==NULL) return; 	
	type=4;
	if (m_yreclen<=4) m_xreclen=4;
	else m_xreclen=8;
	m_Elen=m_Dlen;
	if (m_xreclen==4)	for (i=0;i<length;i++) *((float *)m_E+i)=(float)(start+i*step);
	else				for (i=0;i<length;i++) *((double *)m_E+i)=start+i*step;
}
