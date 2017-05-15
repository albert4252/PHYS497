// MotorsView.h : interface of the CMotorsView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOTORSVIEW_H__A30E7A77_1E2C_4DE6_B80A_59126879A4B2__INCLUDED_)
#define AFX_MOTORSVIEW_H__A30E7A77_1E2C_4DE6_B80A_59126879A4B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "MCcard.h"
class CMotorsDoc;
class CCommandView;
class WSpec;

class CMotorsView : public CView
{
protected: // create from serialization only
	CMotorsView();
	DECLARE_DYNCREATE(CMotorsView)

// Attributes
public:
	CMotorsDoc* GetDocument();
// Operations
public:
	//////////////////////////////////////////////////////////////////////////
	void	OnShow();
	void	SetDefaultValues();
	void	InitSize();
	void	PrepareData(double &xmin,double &ymin,double &xmax,double &ymax,double &xmul,double &ymul); 
	void	PrepareData(int what); 
	void	GetTicks(double yd, double &d, double &sd);
	BOOL	GetOrder(double xn, double xm, double d, int &i0, int &i1, int &i2);
	void	MakeTicks();
	void	DrawData(WSpec &D,Graphics &G,int points=0); 
	void	DrawTexts(Graphics &g); 
	void	Draw(int what);
	void	DrawPoint(int what,int point);
	void	DrawMarker();
	void	RemoveMarker(BOOL rem=TRUE);
		//////////////////////////////////////////////////////////////////////////
	int		ActSpec;
	BOOL	bXnorm,bYnorm;		// We ask for renormalization
	BOOL	m_bXgrid,m_bYgrid;
	BOOL	MarkerOn;
	int		MX,MY;
	CCommandView *pComm;
	CMotorsDoc *pDoc;
	//////////////////////////////////////////////////////////////////////////
	//	Begin Universal Drawer Datadeclaration block
	HWND	hW; //,hBW;
	GraphicsPath TickPath,GridPath,CurvePath,DensPath;
	Pen		*Tpen,*Gpen,*Cpen,*Dpen;		// Text pen, Grid pen, Curve pen, Density Pen
	SolidBrush *Bbrush,*Wbrush,*Tbrush;		// Black white and transparent brushes
	LinearGradientBrush *LGbrush;
	ARGB	grad1,grad2;					// Gradient Brush colors
	ARGB	white,black,trbkg;				// Colors, transp background
	//	Rectangles
	CStatic	*Win,*BWin;						// Spectrum window
	CRect	Ruut,BRuut;
	RectF	Ruutf,BRuutf,CRuutf;
	int		OffsetX, OffsetY;				// First corner offset beween Ruut & BRuut
	int		StaticEdge;						// Static edge mask ltrb; l=8,t=4,r=2,b=1
	int		TopGutter,BottomGutter,LeftGutter,RightGutter;	// BRuut gutters from edge
	float	width,height;
	//	Text
	StringFormat sformat;
	Font	*tnr12;
	FontFamily *ftnr; 
	float	Font_Size;
	//	Presentation normalization related
	double	x_min,x_max,x_mul,y_min,y_max,y_mul;	// visual coordinates
	PointF *PFP;							// The array
	//	Ticks and labels
	Carry	TexX,TexY,TexV;
	int		Yaxis;							// Y-axis turnaround point
	int		Ytlen,Xtlen,Ybtlen,Xbtlen;		// tick, big tick length
	//	End Universal Drawer Datadeclaration block
	
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMotorsView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMotorsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMotorsView)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnDrawPoint(WPARAM wrd, LPARAM lp);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MotorsView.cpp
inline CMotorsDoc* CMotorsView::GetDocument()
   { return (CMotorsDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOTORSVIEW_H__A30E7A77_1E2C_4DE6_B80A_59126879A4B2__INCLUDED_)
