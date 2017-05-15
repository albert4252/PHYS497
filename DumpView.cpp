// DumpView.cpp : implementation file
//

#include "stdafx.h"
#include "math.h"
#include "Motors.h"
#include "MotorsDoc.h"
#include "DumpView.h"
#include "CommandView.h"
#include "MotorsView.h"
#include "WSpec.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDumpView

IMPLEMENT_DYNCREATE(CDumpView, CView)

CDumpView::CDumpView()
{
	black=0xff000000; white=0xffffffff;				// ARGB colors
	trbkg=0xffe0f0f0; curve=0xff101060;				// aqua, Curvecolor
	arbkg=0xffd0ffd0;								// green
	Font_Size=12;
	PFP=NULL;
	x_min=x_max=x_mul=y_mul=y_min=y_max=0;			// To ask for autonorm, (registrysaved)
	xsize=ysize=-1;
	CursorCurve=-1;
}

CDumpView::~CDumpView()
{
}


BEGIN_MESSAGE_MAP(CDumpView, CView)
	//{{AFX_MSG_MAP(CDumpView)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER_LOAD,OnLoad)
	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDumpView drawing

void CDumpView::OnDraw(CDC* pDC)
{
	BOOL retc;
	retc=InitSize();
	if (!amount)
	{	CString S="No data";
		pDC->TextOut(10,10,S);
		return;
	}
	if (!retc) return;
	DrawAll();
}

/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CDumpView::AssertValid() const{CView::AssertValid();}
void CDumpView::Dump(CDumpContext& dc) const{CView::Dump(dc);}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDumpView message handlers

BOOL CDumpView::InitSize()
{
	pDoc=(CMotorsDoc *)GetDocument();	
	if (pDoc==NULL) return FALSE;
	amount=pDoc->Spc.GetSize();
	ActSpec=pDoc->ActSpec;
	if (!amount) return FALSE;
	GetClientRect(&BRuut);
	ratio=sqrt((double)BRuut.Height()/(double)BRuut.Width());	
	xnum=(int)floor(sqrt((double)amount)/ratio+0.5);	// Columns
	if (xnum==0) return FALSE;
	ynum=amount/xnum;
	if (amount%xnum) ynum++;					// Lines
	xsize=BRuut.Width()/xnum;
	ysize=BRuut.Height()/ynum;
	FontFamily  fontFamily(L"Times New Roman");
	ftnr=fontFamily.Clone();
	Font fo(&fontFamily, Font_Size, FontStyleRegular, UnitPixel);
	tnr12=fo.Clone();
	Pen pen(black);	Tpen=pen.Clone(); 
	Pen cen(curve); Cpen=cen.Clone();		// Clone pens for curves
	SolidBrush br(black),wr(white),tr(trbkg),ar(arbkg);
	Bbrush=(SolidBrush *)br.Clone();
	Wbrush=(SolidBrush *)wr.Clone();
	Tbrush=(SolidBrush *)tr.Clone();
	Abrush=(SolidBrush *)ar.Clone();
	sformat.SetAlignment(StringAlignmentNear);
	return TRUE;
}

void CDumpView::DrawAll()
{
	int i;
	amount=pDoc->Spc.GetSize();
	for (i=0;i<amount;i++)
	{
		DrawOne(i%xnum,i/xnum,i);
	}
}

void CDumpView::Clear()
{
	Graphics G(this->m_hWnd);
	G.Clear(white);
}

LRESULT CDumpView::OnLoad(WPARAM wrd, LPARAM lp)	// For a draw-message from workerthread
{
	if (wrd>=0) Draw(wrd);
	return 0;
}

void CDumpView::Draw(int i)
{
	if (i==0 || xsize==-1) 
		if (!InitSize()) return; 
	DrawOne(i%xnum,i/xnum,i);
}

void CDumpView::DrawOne(int nx,int ny,int i)
{
	wchar_t buffer[128];
	Graphics G(this->m_hWnd);
	Ruutf.X=(float)(nx*xsize);
	Ruutf.Y=(float)(ny*ysize);
	Ruutf.Width=(float)xsize;
	Ruutf.Height=(float)ysize;
	if (i==ActSpec)
		G.FillRectangle(Abrush,Ruutf);
	else
		G.FillRectangle(Tbrush,Ruutf);
	G.DrawRectangle(Tpen,Ruutf);
	PointF pointF(Ruutf.X+2,Ruutf.Y+2);
	swprintf(buffer,L"%d",i+1);
	G.DrawString(buffer,-1, tnr12, pointF, &sformat, Bbrush);
	if (pDoc->Spc[i].GetLength()>0)
	{
		PrepareData(i);
		DrawData(pDoc->Spc[i],G,i);
	}
}

extern CString sActCurve;

void CDumpView::PrepareData(double &xmin,double &ymin,double &xmax,double &ymax,
							  double &xmul,double &ymul) 
{
	double	dx;
	dx=xmax-xmin;									// If dx==0 it is sign that these values are invalid
	if (fabs(dx)<1e-10 || fabs(dx)>1e10)			// Check validity
	{	dx=(x_max-x_min); xmin=x_min; xmax=x_max;}	
	if (fabs(dx)<1e-10 || fabs(dx)>1e10) 
	{	dx=1; xmin=0; xmax=1;}						// Use defaults if invalid
	xmul=(double)xsize/dx;
	dx=ymax-ymin;
	if (fabs(dx)<1e-10 || fabs(dx)>1e10) 
	{	dx=(y_max-y_min); ymin=y_min; ymax=y_max;}
	if (fabs(dx)<1e-10 || fabs(dx)>1e10) 
	{	dx=1; ymin=0; ymax=1;}
	ymul=(double)ysize/dx;
}

void CDumpView::PrepareData(int what)
{
	double	xn, xm, yn, ym, xx, yx;
	yn = _wtof(pComm->m_sYmin); 
	ym = _wtof(pComm->m_sYmax);	// Defaults are edit winlet values
	xn = _wtof(pComm->m_sXmin); 
	xm = _wtof(pComm->m_sXmax);
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

void CDumpView::DrawData(WSpec &D,Graphics &G,int what) 
{
	PointF	*PFH;
	double	xmin,ymin,xmax,ymax,xmul,ymul,x,y;
	int		i,leng;
	PrepareData(xmin,ymin,xmax,ymax,xmul,ymul);		
	leng=D.GetLength();
	PFH=(PointF *)malloc(sizeof(PointF)*leng);
	for (i=0;i<leng;i++)							
	{
		D.GetPoint(x,y,i);
		x=(x-xmin)*xmul;
		y=(y-ymin)*ymul;
		if (x<0) x=0; if (y<0) y=0;
		if (x>(double)xsize) x=xsize;
		if (y>(double)ysize) y=ysize;
		PFH[i].X=(float)(x+(what%xnum)*xsize);
		PFH[i].Y=(float)(ysize-y+(what/xnum)*ysize);
	}
	
	G.DrawLines(Cpen,PFH,leng);
	free(PFH);
}

void CDumpView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int i,j,k,l;
	CString Tm,Sm;
	i=xnum*(point.y/ysize)+point.x/xsize;
	pZoom->RemoveMarker();
	if (CursorCurve>=0 && CursorCurve<amount)
	{
		pDoc->ActSpec=i;
		pDoc->UpdateAllViews(NULL);
		sActCurve.Format(L"Curve:%d",i+1);
		Message=pDoc->Spc[i].GetComment();
		l=Message.GetLength();
		k=Message.ReverseFind('[');
		Tm=Message.Left(k);
		l=Tm.GetLength();
		j=Tm.Find(')');
		Sm=Tm.Right(l-j-1);
		Sm.TrimLeft();
		Sm.TrimRight();
		sSlowPos=_T("Motor:");
		sSlowPos+=Sm;
		AfxGetApp()->m_pMainWnd->PostMessage(WM_USER_ANNOUNCE,0xd,0);
	}
	CView::OnLButtonUp(nFlags, point);
}

void CDumpView::OnMouseMove(UINT nFlags, CPoint point) 
{
	int i;
	i=xnum*(point.y/ysize)+point.x/xsize;
	pZoom->RemoveMarker();
	if (i>=0 && i<amount)
	{
		if (i!=CursorCurve)
		{
			CursorCurve=i;
			Message=pDoc->Spc[i].GetComment();
			AfxGetApp()->m_pMainWnd->PostMessage(WM_USER_ANNOUNCE,0x1,0);
		}
	}
	else CursorCurve=-1;
	CView::OnMouseMove(nFlags, point);
}

