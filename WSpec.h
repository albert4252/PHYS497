// WSpec.h: interface for the WSpec class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WSPEC_H__04E905F2_4983_11D3_AC94_00400565639E__INCLUDED_)
#define AFX_WSPEC_H__04E905F2_4983_11D3_AC94_00400565639E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum FileType {UNEXIST=-10,RWPERM,UNKNOWN,WRITERR,READERR,SEEKERR,OPENERR=-1,SP90,SP92,SP97};

#pragma pack( push, before_this_struct)		// Push present packing scheme
#pragma pack( 1 )		// pack as dense as possible, now  sizeof(WSHeader)=29
							// default align is for largest struct member i.e. 8 
class WSpec  
{							// Lets allow everyone access to our DATA 
public:						// 
	BOOL	m_ext;			// TRUE for external databuffers; public because we must be able to toggle 
protected:
	long	type;			// 1-SP90, 2-SP92, 3-SP97 Y, 4-SP97 XY	-> Sergei style
	double	start;
	double	step;
	long	length;
	long	time;
	char	m_yreclen,m_xreclen;
	void	*m_D,*m_E;					// Data pointers		!! VOID !!
	char	*m_C;						// Comment pointer
	long	m_Clen,m_Dlen,m_Elen;		// The absolute bytelengths of Comment & Data buffers
	long	m_spno,m_type,m_length;		// specno, filetype, filelength after read
	double	m_xmin,m_xmax,m_ymin,m_ymax;// real min and max
	long	*m_Fat,m_Flen;				// Spectra FAT pointer
	CFile	F;
	BOOL	m_valid;					// Is the data valid in WSpec
public:						
	BOOL	WSpec::Valid(int force=-1);	// if Force=0/1 set m_valid; else calculate it
	// data is elsewhere; If SetHeader returns TRUE -> Header and databuffers are valid m_ext is true
	BOOL	SetHeader(void *Y,void *X,long len,char ytype=0,char xtype=0);
	BOOL	SetHeader(float *Y,float *X,long len)	{return SetHeader(Y,X,len,4,4);}
	BOOL	SetHeader(double *Y,double *X,long len) {return SetHeader(Y,X,len,8,8);}
	BOOL	SetHeader(double *Y,float *X,long len)  {return SetHeader(Y,X,len,8,4);}
	BOOL	SetHeader(float *Y,double *X,long len)  {return SetHeader(Y,X,len,4,8);}
	BOOL	SetHeader(void *Y,double Start,double Step,long len,char ytype);
	BOOL	SetHeader(double *Y,double Start,double Step,long len) {return SetHeader(Y,Start,Step,len,8);}
	BOOL	SetHeader(float *Y,double Start,double Step,long len)  {return SetHeader(Y,Start,Step,len,4);}
	BOOL	SetComment(CString S);
	BOOL	SetComment(char *C);
	char* 	GetComment() {return m_C;}
	// buffers are created or modified inside WSpec
	BOOL	GrowBy(long add);				// Infrequently: create temp, move data, free m_D, malloc m_D+, move data, free temp
	BOOL	CleanCreate(double Start,double Step,long len,char ytype);		// create with clean
	BOOL	CleanCreate(double start, double step, long len,char ytype,char xtype);					// --""--
	BOOL	CleanCreate(long len,char ytype,char xtype,double xn=0, double xm=0);					// --""--
	BOOL	Create(long len,char ytype,char xtype);
	BOOL	Create(double Start,double Step,long len,char ytype);
	BOOL	CreateComment(char *C);			// Copies string into comment too
	BOOL	CreateComment(long len);		// Creates comment buffer of length
	// Get/set parameters
	void	SetTime(long t=0); 
	long	GetTime() {return time;}
	long	GetLength() {return length;}
	long	GetType()	{return type;}
	long	GetDataType(bool YT=TRUE);
	BOOL	SetSS(double Start, double Step);
	double	GetStart();
	double	GetStep();
	double	GetStop();
	long	GetPos(double X);							// Get channel from X, works only for monotone
	BOOL	TransferData(double *dest,long leng);		// Transfer data from m_D to destination
	BOOL	TransferData(float  *dest,long leng);
	BOOL	GetAt(double &x,double &y);					// Linear interpolation
	// Change type
	void	Swap(int i,int j);		// Swap points i,j by X
	void	Linearize();			// Sort, and linearly interpolate
	void	Pair();					// Make XY pair spec, easy job
	// Do actual File I/O
	void	Unlink();		// Use this if data is allocated externally, also deallocating is external concern
	void	KillData();		// Use this if data is allocated by WSpec
	long	GetSpecs();
	long	GetSpecs(const wchar_t *Fname);
	BOOL	Read(const wchar_t *Fname,long no);
	BOOL	Verify(const wchar_t *Fname);
	long	Check(const wchar_t *Fname, long *nosp=NULL);	// Returns detected (FileType), does not change a local thing
	virtual	BOOL	Append(const wchar_t *Fname, int Ftype=SP97);
	BOOL	GetPoint(double &x,double &y,long i);
	BOOL	SetPoint(double x,double y,long i);
	void	FindMinMax(int len=0);						// Finds both X,Y-axis minmax, if (len) only first len points are used for Yminmax
	BOOL	GetRealMM(double &m_xmin,double &m_xmax,double &m_ymin,double &m_ymax);
	BOOL	Create(const wchar_t *Fname, int Ftype=SP97);
	WSpec();
	virtual ~WSpec();
	virtual void Serialize(CArchive& ar);			//
	WSpec&	operator=(const WSpec& U);
	void	Vordub(const WSpec &U);						// Dummy helper to oper =

protected:
	void	FindMM(float *p, double &n, double &m, int len=0);		// A macro, finds real min max of an axis
	void	FindMM(double *p, double &n, double &m, int len=0);
	BOOL	SetReclen(char &ytype);
	BOOL	Append92();
	BOOL	Append97();
	BOOL	Read92();
	BOOL	Read97();
	BOOL	ReadFAT();
	void	KillFAT();
	long	GetPointer(long no);
};
#pragma pack( pop, before_this_struct )

#endif // !defined(AFX_WSPEC_H__04E905F2_4983_11D3_AC94_00400565639E__INCLUDED_)
