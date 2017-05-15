// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Motors.h"
#include "DumpView.h"
#include "CommandView.h"
#include "MotorsView.h"
#include "MotorsDoc.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SBAR 0xab00


/////////////////////////////////////////////////////////////////////////////
//	Splitter window
IMPLEMENT_DYNAMIC(CViewExSplitWnd, CSplitterWnd)
CViewExSplitWnd::CViewExSplitWnd(){}
CViewExSplitWnd::~CViewExSplitWnd(){}

CWnd* CViewExSplitWnd::GetActivePane(int* pRow, int* pCol)
{	ASSERT_VALID(this);						// attempt to use active view of frame window
	CWnd* pView = NULL;
	CFrameWnd* pFrameWnd = GetParentFrame();
	ASSERT_VALID(pFrameWnd);
	pView = pFrameWnd->GetActiveView();		// failing that, use the current focus
	if (pView == NULL)	pView = GetFocus();
	return pView;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER_ANNOUNCE,OnAnnounce)
	END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_POS,		// Slow motor position
	ID_INDICATOR_ITER,		// ActCurve
	ID_INDICATOR_AMOUNT		// Amount of curves to load
	
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

CMainFrame::~CMainFrame()
{
	GdiplusShutdown(gdiplusToken);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
/*
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
*/
	int i;
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	if ((i=m_wndStatusBar.CommandToIndex(ID_INDICATOR_POS)) != -1)
		m_wndStatusBar.SetPaneInfo(i,ID_INDICATOR_POS,SBPS_NORMAL,150);   
	if ((i=m_wndStatusBar.CommandToIndex(ID_INDICATOR_ITER)) != -1)
		m_wndStatusBar.SetPaneInfo(i,ID_INDICATOR_ITER,SBPS_NORMAL,100);   
	if ((i=m_wndStatusBar.CommandToIndex(ID_INDICATOR_AMOUNT)) != -1)
		m_wndStatusBar.SetPaneInfo(i,ID_INDICATOR_AMOUNT,SBPS_NORMAL,100);   
	if ((i=m_wndStatusBar.CommandToIndex(ID_SEPARATOR)) != -1)
		m_wndStatusBar.SetPaneInfo(i,ID_SEPARATOR,SBPS_STRETCH,0);   
	
//	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
//	EnableDocking(CBRS_ALIGN_ANY);
//	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	int i=(-1)^(FWS_ADDTOTITLE|FWS_PREFIXTITLE);    
	cs.style&=i;
	if( !CFrameWnd::PreCreateWindow(cs) ) return FALSE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const{CFrameWnd::AssertValid();}
void CMainFrame::Dump(CDumpContext& dc) const{CFrameWnd::Dump(dc);}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	CDC* pDC=GetDC();									// Done before OnCreate()
	CMotorsApp* pApp=(CMotorsApp*)AfxGetApp();
	int ic=pDC->GetDeviceCaps(HORZRES);					// How big is the screen, actually
	ReleaseDC(pDC);
	// create a splitter with 1 row, 3 columns
	if (!m_wndSplitter.CreateStatic(this, 1, 3)) return FALSE;
	if (ic>1600)
	{
		m_wndSplitter.SetColumnInfo( 0, 215, 200 );		// Set Col 0 to be 600 pix wide ideally or at least 100
		m_wndSplitter.SetColumnInfo( 1, 1000, 200 );	// Set Col 1 to be 1000 pix wide ideally or at least 100
		m_wndSplitter.SetColumnInfo( 2, 600, 100 );		// Set Col 1 to be 1000 pix wide ideally or at least 100
		if (!m_wndSplitter.CreateView(0, 0,					
			RUNTIME_CLASS(CCommandView), CSize(216, 1200), pContext))	return FALSE;
		if (!m_wndSplitter.CreateView(0, 1,					
			RUNTIME_CLASS(CMotorsView), CSize(1600, 1200), pContext))	return FALSE;
		if (!m_wndSplitter.CreateView(0, 2,					
			RUNTIME_CLASS(CDumpView), CSize(ic*1/5, 1200), pContext))	return FALSE;
	}
	else 
	{
		m_wndSplitter.SetColumnInfo( 0, 200, 100 );			
		m_wndSplitter.SetColumnInfo( 1, 500, 50 );			
		m_wndSplitter.SetColumnInfo( 2, 200, 50 );			
		if (!m_wndSplitter.CreateView(0, 0,					
			RUNTIME_CLASS(CCommandView), CSize(255, 1200), pContext))	return FALSE;
		if (!m_wndSplitter.CreateView(0, 1,					
			RUNTIME_CLASS(CMotorsView), CSize(501, 1200), pContext))	return FALSE;
		if (!m_wndSplitter.CreateView(0, 2,					
			RUNTIME_CLASS(CDumpView), CSize(160, 1200), pContext))	return FALSE;
	}
	return TRUE;
}

LRESULT CMainFrame::OnAnnounce(WPARAM wrd, LPARAM lp)	// For turnaround between threads
{	UINT u;
	int i;
	if (wrd&1)
	{
		u=ID_SEPARATOR;			
		if ((i=m_wndStatusBar.CommandToIndex(u)) != -1) 
			m_wndStatusBar.SetPaneText(i,LPCTSTR(Message));
		Sbar&=(-1)^1;
	}
	if (wrd&2)
	{
		u=ID_SEPARATOR;			
		if ((i=m_wndStatusBar.CommandToIndex(u)) != -1) 
			m_wndStatusBar.SetPaneText(i,LPCTSTR(Error));
		Sbar&=(-1)^2;
	}
	if (wrd&4)
	{
		u=ID_INDICATOR_POS;			
		if ((i=m_wndStatusBar.CommandToIndex(u)) != -1) 
			m_wndStatusBar.SetPaneText(i,LPCTSTR(sSlowPos));
		Sbar&=(-1)^4;
	}
	if (wrd&8)
	{
		u=ID_INDICATOR_ITER;			
		if ((i=m_wndStatusBar.CommandToIndex(u)) != -1) 
			m_wndStatusBar.SetPaneText(i,LPCTSTR(sActCurve));
		Sbar&=(-1)^8;
	}
	if (wrd&0x10)
	{
		u=ID_INDICATOR_AMOUNT;			
		if ((i=m_wndStatusBar.CommandToIndex(u)) != -1) 
			m_wndStatusBar.SetPaneText(i,LPCTSTR(sCurves));
		Sbar&=(-1)^0x10;
	}
	if (wrd&0x100)
	{
		SsTimer(1);	// Start Timer
		Sbar&=(-1)^0x100;
	}
	if (wrd&0x200)
	{
		SsTimer(2);	// Stop Timer
		Sbar&=(-1)^0x200;
	}
	if (wrd&0x400)	// Says that thread is active
	{
		Sbar&=(-1)^0x400;
	}
	if (wrd&0x800)	// Says that rasterscan got done 
	{
		Message="Idle";
		Sbar|=1;
		pComm->c_ModeTab.SetCurSel(FastMotor);
		pComm->ActionHide(Working);
		pComm->SetDlgItemText(IDC_SAVE,L"Save");
		pDoc->UpdateAllViews(NULL);
		Sbar&=(-1)^0x800;
	}
	return 0;
}


void CMainFrame::OnTimer(UINT nIDEvent) 
{
	switch(nIDEvent)
	{
	default:
		CFrameWnd::OnTimer(nIDEvent);
		break;
	case SBAR:
		if (Sbar)
		{
			OnAnnounce(Sbar,0);
			Sbar=0;
		}
		uTimer=SetTimer(SBAR,200,NULL);
		break;
	}
}

void CMainFrame::SsTimer(int what) 
{
	if (what==1)	// Start Timer
	{
		if (uTimer) KillTimer(uTimer); 
		uTimer=SetTimer(SBAR,50,NULL);
	}
	if (what==2)	// Stop Timer
	{
		if (uTimer) KillTimer(uTimer); uTimer=0;
	}
}