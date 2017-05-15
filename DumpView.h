#if !defined(AFX_DUMPVIEW_H__1F3CE92C_E8BF_49E1_ADA3_FCBB6F69323E__INCLUDED_)
#define AFX_DUMPVIEW_H__1F3CE92C_E8BF_49E1_ADA3_FCBB6F69323E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DumpView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
class CMotorsDoc;
class CCommandView;
class WSpec;

class CDumpView : public CView
{
protected:
	CDumpView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDumpView)
	BOOL	InitSize();
	void	DrawOne(int nx,int ny,int what);
	void	DrawAll();
	void	Clear();
	void	Draw(int what);
	void	PrepareData(double &xmin,double &ymin,double &xmax,double &ymax,
						double &xmul,double &ymul); 
	void	PrepareData(int what); 
	void	DrawData(WSpec &D,Graphics &G,int what); 
		
// Attributes
public:
	CCommandView *pComm;
	CMotorsDoc *pDoc;
	double	ratio;
	int		amount,ActSpec;
	int		xnum,ynum;
	int		xsize,ysize;
	BOOL	bYnorm,bXnorm;
	int		CursorCurve;
	//////////////////////////////////////////////////////////////////////////
	Pen		*Tpen,*Cpen;						// Text pen, Curve pen
	SolidBrush *Bbrush,*Wbrush,*Tbrush,*Abrush;	// Black white and transparent brushes
	ARGB	white,black,trbkg,arbkg,curve;		// Colors, transp background
	//	Rectangles
	CRect	BRuut;
	RectF	Ruutf;
	float	width,height;
	//	Text
	StringFormat sformat;
	Font	*tnr12;
	FontFamily *ftnr; 
	float	Font_Size;
	//	Presentation normalization related
	double	x_min,x_max,x_mul,y_min,y_max,y_mul;	// visual coordinates
	PointF *PFP;							// The array
	//	End Universal Drawer Datadeclaration block
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDumpView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDumpView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDumpView)
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnLoad(WPARAM wrd, LPARAM lp);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DUMPVIEW_H__1F3CE92C_E8BF_49E1_ADA3_FCBB6F69323E__INCLUDED_)
