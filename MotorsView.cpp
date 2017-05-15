// MotorsView.cpp : implementation of the CMotorsView class
//

#include "stdafx.h"
#include "math.h"
#include "Motors.h"
#include "MotorsDoc.h"
#include "MotorsView.h"
#include "CommandView.h"
#include "WSpec.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMotorsView

IMPLEMENT_DYNCREATE(CMotorsView, CView)

BEGIN_MESSAGE_MAP(CMotorsView, CView)
	//{{AFX_MSG_MAP(CMotorsView)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER_LOAD,OnDrawPoint)
	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMotorsView construction/destruction

CMotorsView::CMotorsView()
{
	SetDefaultValues();
	MarkerOn=FALSE;
	MX=MY=-1;
}

CMotorsView::~CMotorsView()
{
}

BOOL CMotorsView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CMotorsView::AssertValid() const{CView::AssertValid();}
void CMotorsView::Dump(CDumpContext& dc) const{	CView::Dump(dc);}

CMotorsDoc* CMotorsView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMotorsDoc)));
	return (CMotorsDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMotorsView drawing

void CMotorsView::OnDraw(CDC* pDC)
{
	pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	int amount;
	amount=pDoc->Spc.GetSize();
	ActSpec=pDoc->ActSpec;
	if (!amount)
	{	CString S="No data";
		pDC->TextOut(10,10,S);
		return;
	}
	if (ActSpec>=amount) ActSpec=amount-1;
	if (ActSpec<0) ActSpec=0;
	Draw(ActSpec);
}

void CMotorsView::Draw(int what)
{	
	hW=this->m_hWnd;
	InitSize();
	Graphics G(hW);
	G.FillRectangle(LGbrush,BRuutf);
	G.FillRectangle(Tbrush,Ruutf);
	PrepareData(what);
	MakeTicks();
	G.DrawPath(Gpen,&GridPath);			// Grid and ticks are drawn into 
	G.DrawPath(Tpen,&TickPath);
	DrawTexts(G);
	DrawData(pDoc->Spc[what],G); 
}

LRESULT CMotorsView::OnDrawPoint(WPARAM wrd, LPARAM lp)	
{
	if (wrd>=0 && lp>=0)
	{
		DrawPoint(wrd,lp);
	}
	return 0;
}

void CMotorsView::DrawPoint(int what,int point)
{	
	hW=this->m_hWnd;
	Graphics G(hW);
	if (!point) 
	{	InitSize();
		G.FillRectangle(LGbrush,BRuutf);
		G.FillRectangle(Tbrush,Ruutf);
		PrepareData(what);
		MakeTicks();
		G.DrawPath(Gpen,&GridPath);			// Grid and ticks are drawn into 
		G.DrawPath(Tpen,&TickPath);
		DrawTexts(G);
	}
	else
		DrawData(pDoc->Spc[what],G,point); 
}

/////////////////////////////////////////////////////////////////////////////

void CMotorsView::SetDefaultValues()
{
	//	Transferable data
	Xtlen=Ytlen=3; Xbtlen=Ybtlen=7;					// Ticklengths
	black=0xff000000; white=0xffffffff;				// ARGB colors
//	trbkg=0xc0e0fff0;								// Transparent aqua
	trbkg=0xffe0f0f0;								// aqua
	Font_Size=12;
	grad1=0xffc0c0c0; grad2=0xffa0a0a0;				// ARGB gradiendi jaoks
	StaticEdge=0xc;									// Top & Left edge of BRuut are static (ltrb)
	//	StaticEdge=0xf;									// All edges are static (ltrb)
	LeftGutter=5; RightGutter=5;					// Gutters for dynamic dialog or view
	TopGutter=5; BottomGutter=5;
	//	To initialize to known values
	PFP=NULL;
	x_min=x_max=x_mul=y_mul=y_min=y_max=0;			// To ask for autonorm, (registrysaved)
	//	Non-Transferable data
	m_bXgrid=m_bYgrid=FALSE;
}

void CMotorsView::InitSize()
{
	CRect B,R,rect;
	OffsetX=80;							// For left texts & ticks
	OffsetY=15;
	GetClientRect(&BRuut);
	BRuutf.X=(float)BRuut.left;
	BRuutf.Y=(float)BRuut.top;
	BRuutf.Width=(float)BRuut.Width();
	BRuutf.Height=(float)BRuut.Height();
	Ruutf.X=BRuutf.X+LeftGutter+OffsetX; 
	Ruutf.Y=BRuutf.Y+TopGutter;
	height=BRuutf.Height-TopGutter-OffsetY-BottomGutter;
	width=BRuutf.Width-LeftGutter-OffsetX-RightGutter;
	Ruutf.Width=width;
	Ruutf.Height=height;
	CRuutf.X=(float)OffsetX;				// CRuutf is black rectangle around Ruutf
	CRuutf.Y=(float)OffsetY;
	CRuutf.Width=width;
	CRuutf.Height=height;
	//	sformat.SetAlignment(StringAlignmentCenter);
	FontFamily  fontFamily(L"Times New Roman");
	ftnr=fontFamily.Clone();
	Font fo(&fontFamily, Font_Size, FontStyleRegular, UnitPixel);
	tnr12=fo.Clone();
	Pen pen(black);	Tpen=pen.Clone(); 
	Pen ben(0xffa0a0a0); Gpen=ben.Clone();		// Clone pen for grid
	Pen cen(0xff111155); Cpen=cen.Clone();		// Clone pen for curve
	LinearGradientBrush gB(BRuutf,grad1,grad2,LinearGradientModeHorizontal);
	LGbrush=(LinearGradientBrush *)gB.Clone();
	SolidBrush br(black),wr(white),tr(trbkg);
	Bbrush=(SolidBrush *)br.Clone();
	Wbrush=(SolidBrush *)wr.Clone();
	Tbrush=(SolidBrush *)tr.Clone();

}

////////////////////  Curve  ////////////////////////////////////////////////////////////////////////////////
//	Data is drawn and ticks are calculated using xy_mul/min/max
//	We need to do that at least once for chosen Data * before 
//	ticks are calculated, it is called from DrawData later as well

void CMotorsView::PrepareData(double &xmin,double &ymin,double &xmax,double &ymax,
							  double &xmul,double &ymul) 
{
	double	dx;
	m_bXgrid=pComm->m_bXgrid;
	m_bYgrid=pComm->m_bYgrid;
	dx=xmax-xmin;									// If dx==0 it is sign that these values are invalid
	if (fabs(dx)<1e-10 || fabs(dx)>1e10)			// Check validity
	{	dx=(x_max-x_min); xmin=x_min; xmax=x_max;}	
	if (fabs(dx)<1e-10 || fabs(dx)>1e10) 
	{	dx=1; xmin=0; xmax=1;}						// Use defaults if invalid
	xmul=width/dx;
	dx=ymax-ymin;
	if (fabs(dx)<1e-10 || fabs(dx)>1e10) 
	{	dx=(y_max-y_min); ymin=y_min; ymax=y_max;}
	if (fabs(dx)<1e-10 || fabs(dx)>1e10) 
	{	dx=1; ymin=0; ymax=1;}
	ymul=height/dx;
}

//	Ticks and labels are calculated according to xy_min/max
//	We either do this at least once to know xy_min/max 
//	Or we need to download absolute settings from dialog edits 
void CMotorsView::PrepareData(int what)
{
	double	xn, xm, yn, ym, xx, yx;
	yn = _wtof(pComm->m_sYmin); ym = _wtof(pComm->m_sYmax);	// Defaults are edit winlet values
	xn = _wtof(pComm->m_sXmin); xm = _wtof(pComm->m_sXmax);
	if (pComm->m_bAutonorm) bXnorm = bYnorm = TRUE;
	if (bXnorm || bYnorm)								// If we force from CCommandView
	{
		double xnm, xmm, ynm, ymm;
		pDoc->Spc[what].FindMinMax();
		pDoc->Spc[what].GetRealMM(xnm, xmm, ynm, ymm);
		if (bXnorm)
		{
			xn = xnm; xm = xmm;
		}
		if (bYnorm)
		{
			yn = ynm; ym = ymm;
		}
	}
	if (pComm->m_bZeronorm) yn = 0;
	PrepareData(xn, yn, xm, ym, xx, yx);
	y_mul = yx; x_mul = xx;
	y_min = yn; x_min = xn;
	y_max = ym; x_max = xm;
	if (bXnorm)
	{
		pComm->m_sXmin.Format(L"%-5.1f", xn);
		pComm->m_sXmax.Format(L"%-5.1f", xm);
	}
	if (bYnorm)
	{
		pComm->m_sYmin.Format(L"%-5.1f", yn);
		pComm->m_sYmax.Format(L"%-5.1f", ym);
	}
	pComm->UpdateData(FALSE);
	bXnorm = bYnorm = FALSE;
}

////////////////////  Ticks  /////////////////////////////////////////////////////////////////////////////////////
//	Calculate label units automatically
//	yd - is max-min; d - is labeled unit; sd - is small unit; 
//	ord - is the number of small units in bigger one
void CMotorsView::GetTicks(double yd, double &d, double &sd)
{
	double e,f; 
	int ord;							
	e=log10(yd); 
	f=floor(e);
	d=pow(10.,e-f);
	if (d>5)		{d=1.;	ord=10;}
	else if (d>2)	{d=.5;  ord=5;}
	else			{d=.2;  ord=10;}
	e=log10(d)+f;
	d=pow(10.,e);
	sd=d/(double)ord;
}

//	A helper which sets loop variables for (possibly) reversed axis
BOOL CMotorsView::GetOrder(double xn, double xm, double d, int &i0, int &i1, int &i2)
{	int k;
	i0=(int)ceil(xn*.99999/d);					// unit
	k=(int)floor(xm*1.00001/d);
	if (i0==k) return FALSE;			// reversed axis					
	if (i0>k) 
	{	i0=(int)floor(xn/d);
		k=(int)ceil(xm/d);
		i1=i0-k; i2=-1; 
	}	
	else	 {i1=k-i0; i2=1;}
	return TRUE;
}

//	This needs to know x/y_min/max/mul & Ruut 
void CMotorsView::MakeTicks() 
{	double d,sd,xd; 
//	const int stick=3, btick=7;
	int i,j,x,y,i0,i1,i2;
	int	sbb,stb,sbe,ste,bbe,bte;	// small/big; bottom/top; & begin/end
	int	srb,slb,sre,sle,bre,ble;	// small/big; left/right; & begin/end
	TickPath.Reset();
	GridPath.Reset();
	TexX.RemoveAll();
	TexY.RemoveAll();
	TexV.RemoveAll();
//	Calculate the actual tick end & beg positions	
//	Ticks are drawn in BRuut but around the Ruut
	sbb=(int)(Ruutf.Y+Ruutf.Height);		
	stb=(int)Ruutf.Y; 
	sbe=sbb+Ytlen;					
	bbe=sbb+Ybtlen;
	ste=stb-Ytlen; 
	bte=stb-Ybtlen;
	srb=(int)(Ruutf.X+Ruutf.Width); 
	slb=(int)Ruutf.X; 
	sre=srb+Xtlen;					
	bre=srb+Xbtlen;
	sle=slb-Xtlen; 
	ble=slb-Xbtlen;
// x ticks	
	xd=fabs(x_max-x_min);
	GetTicks(xd,d,sd);
	//	Bick ticks or Grid
	if (GetOrder(x_min,x_max,d,i0,i1,i2))
		for (i=0;i<=i1;i++)			// Change of variables
		{	j=i0+i*i2;				// axis can run both directions
			x=(int)((j*d-x_min)*x_mul)+OffsetX+LeftGutter;
			TickPath.StartFigure(); TickPath.AddLine(x,sbb,x,bbe);
			TickPath.StartFigure(); TickPath.AddLine(x,stb,x,bte);
			TexY.Add(sbb+Ybtlen+1);
			if (m_bXgrid)			// Grid
			{	
				GridPath.StartFigure(); GridPath.AddLine(x,sbb-1,x,stb+1);
			}
			TexX.Add(x);
			TexV.Add((double)j*d);
		}
	//	Small Ticks
	if (GetOrder(x_min,x_max,sd,i0,i1,i2))
		for (i=0;i<=i1;i++)			// Change of variables
		{	j=i0+i*i2;				// axis can run both directions
			x=(int)((j*sd-x_min)*x_mul)+OffsetX+LeftGutter;
			TickPath.StartFigure(); TickPath.AddLine(x,sbb,x,sbe);
			TickPath.StartFigure(); TickPath.AddLine(x,stb,x,ste);
		}
	//	Marker for the beginning of Y axis ticks
	Yaxis=TexX.GetSize();
// y ticks	
	xd=fabs(y_max-y_min);
	GetTicks(xd,d,sd);
	//	Bick ticks or Grid
	if (GetOrder(y_min,y_max,d,i0,i1,i2))
		for (i=0;i<=i1;i++)			// Change of variables
		{	j=i0+i*i2;				// axis can run both directions
			y=(int)(height-(j*d-y_min)*y_mul)+TopGutter;
			TickPath.StartFigure(); 
			TickPath.AddLine(slb,y,ble,y);
			TickPath.StartFigure(); 
			TickPath.AddLine(srb,y,bre,y);
			TexX.Add(slb-Xbtlen-1);
			if (m_bYgrid)			// Grid
			{	GridPath.StartFigure(); 
				GridPath.AddLine(srb-1,y,slb+1,y);
			}
			TexY.Add(y-8);
			TexV.Add((double)j*d);
		}
	//	Small Ticks
	if (GetOrder(y_min,y_max,sd,i0,i1,i2))
		for (i=0;i<=i1;i++)			// Change of variables
		{	j=i0+i*i2;				// i can run both directions
			y=(int)(height-(j*sd-y_min)*y_mul)+TopGutter;
			TickPath.StartFigure(); 
			TickPath.AddLine(slb,y,sle,y);
			TickPath.StartFigure(); 
			TickPath.AddLine(srb,y,sre,y);
		}
}

////////////////  Draw  /////////////////////////////////////////////////////////////////////////////////////////
//	Here is how we interface with Data structure, note that this is NOT Matrix type Data but a lot smaller class
//	
void CMotorsView::DrawData(WSpec &D,Graphics &G,int points) 
{
	PointF	*PFH;
	double	x,y;
	int		i,leng;
	if (points<=0)  leng=D.GetLength();
	else			leng=points;		
	PFH=(PointF *)malloc(sizeof(PointF)*leng);
	for (i=0;i<leng;i++)							
	{
		D.GetPoint(x,y,i);
		x=(x-x_min)*x_mul;
		y=(y-y_min)*y_mul;
		if (x<0) x=0; if (y<0) y=0;
		if (x>(double)width) x=width;
		if (y>(double)height) y=height;
		PFH[i].X=(float)(x+OffsetX+LeftGutter);
		PFH[i].Y=(float)(height-y+TopGutter);
	}
	
	G.DrawLines(Cpen,PFH,leng);
	free(PFH);
}

//	Draw labels and make ticks
void CMotorsView::DrawTexts(Graphics &g) 
{
	wchar_t buffer[128];
	int i,l,k,m;
	PointF Pf;
	l=TexV.GetSize();
	k=TexV.GetSize();
	m=TexV.GetSize();
	if (l<k) l=k; if (l<m) l=m;
	sformat.SetAlignment(StringAlignmentCenter);
	for (i=0;i<l;i++)
	{
		PointF pointF((float)TexX[i],(float)TexY[i]);
		swprintf(buffer,L"%-g",TexV[i]);
		if (i==Yaxis) sformat.SetAlignment(StringAlignmentFar);
		g.DrawString(buffer, -1, tnr12, pointF, &sformat, Bbrush);
	}
}

//////////////////////////////////////////////////////////////////////////
void CMotorsView::DrawMarker()							
{	int k;
	CPen *OP,MarkerPen(0,1,0x00a0a0);
	CDC *pDC=GetDC();
	OP=pDC->SelectObject(&MarkerPen);				// Save old pen
	k=pDC->SetROP2(R2_XORPEN);						// Xor mode 
	pDC->MoveTo((int)Ruutf.X,MY); pDC->LineTo((int)(Ruutf.X+Ruutf.Width),MY);
	pDC->MoveTo(MX,(int)Ruutf.Y); pDC->LineTo(MX,(int)(Ruutf.Y+Ruutf.Height));
	pDC->SetROP2(k);				
	pDC->SelectObject(OP);
	ReleaseDC(pDC);
}

void CMotorsView::RemoveMarker(BOOL rem)
{ 
	if (rem)
	{
		if (MarkerOn) DrawMarker();
		MarkerOn=FALSE;
		MX=MY=-1;
	}
	else
	{
		if (!MarkerOn) DrawMarker();
		MarkerOn=TRUE;
	}
}

void CMotorsView::OnMouseMove(UINT nFlags, CPoint point) 
{
	int i;
	if (!Working) 			// Not at the time of rasterscan
	{
		RemoveMarker();
		Message="";
		Sbar|=1;
		if (Ruutf.Height<10   || Ruutf.Width<10 || 
			Ruutf.Height>10000 || Ruutf.Width>10000 || 
			point.x<Ruutf.X || point.x>Ruutf.X+Ruutf.Width ||
			point.y<Ruutf.Y || point.y>Ruutf.Y+Ruutf.Height || 
			fabs(x_mul)<1e-10)
				goto noneofthat;
		i=(int)((point.x-Ruutf.X)/x_mul+x_min);
		Message.Format(L"Marker is at step: %d",i);
		MX=point.x; MY=point.y;
		RemoveMarker(FALSE);	// DrawMarker
	}
noneofthat:
	CView::OnMouseMove(nFlags, point);
}
