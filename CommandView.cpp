// CommandView.cpp : implementation file
//

#include "stdafx.h"
#include "math.h"
#include "Motors.h"
#include "MotorsDoc.h"
#include "ThorMotor.h"
#include "MCcard.h"
#include "SaveDlg.h"
#include "MainFrm.h"
#include "CommandView.h"
#include "DumpView.h"
#include "MotorsView.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "master.h"
#include "pvcam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//#define EMULATE
#define MOTOR_MOVES 0x9876
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CCommandView, CFormView)

CCommandView::CCommandView()
	: CFormView(CCommandView::IDD)
	, m_temp(_T(""))
	, m_backrg(_T(""))
	, m_s1(_T(""))
	, m_s2(_T(""))
	, m_p1(_T(""))
	, m_p2(_T(""))
	, m_detect(FALSE)
	, m_points(FALSE)
	, m_585(0)
	, m_coeff(0)
{
	//{{AFX_DATA_INIT(CCommandView)
	m_bAutonorm = FALSE;
	m_bZeronorm = FALSE;
	m_bFastM=TRUE;
	m_bShowLast=FALSE;
	m_bXgrid=FALSE;
	m_bYgrid=FALSE;
	m_Pos=0;
	m_Goto=570;
	m_From=570;
	m_To=800;
	m_Step=0.02;
	m_points = false;
	//}}AFX_DATA_INIT
	pComm=0;
	m_detect = FALSE;
	m_s1 = "676"; //region of interest
	m_s2 = "696";
	m_p1 = "46";
	m_p2 = "66";
	m_585 = 13112;
	background = 0;
	m_coeff = 142.213;
}

CCommandView::~CCommandView()
{
	MCEnableAxis(B.mcb, MC_ALL_AXES, FALSE);	// Off all motors
	B.CloseThor();
	//add deinitializing CCD here?
	if (det == 1) {
		pl_cam_close(hCam);
		pl_pvcam_uninit(); //if we are turning it off from the state where the camera is ON
	}
}

void CCommandView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCommandView)
	DDX_Check(pDX, IDC_AUTONORM, m_bAutonorm);
	DDX_Check(pDX, IDC_FASTMOTOR, m_bFastM);
	DDX_Check(pDX, IDC_SHOW, m_bShowLast);
	DDX_Check(pDX, IDC_XGRID, m_bXgrid);
	DDX_Check(pDX, IDC_YGRID, m_bYgrid);
	DDX_Control(pDX, IDC_MOTORTAB, c_ModeTab);
	DDX_Text(pDX, IDC_EDIT_POS1, m_Pos);
	DDX_Text(pDX, IDC_EDIT_GOTO1, m_Goto);
	DDX_Text(pDX, IDC_EDIT_FROM1, m_From);
	DDX_Text(pDX, IDC_EDIT_TO1, m_To);
	DDX_Text(pDX, IDC_EDIT_STEP1, m_Step);
	DDX_Text(pDX, IDC_EDIT_CHT, m_sCHT);
	DDX_Text(pDX, IDC_EDIT_YMAX, m_sYmax);
	DDX_Text(pDX, IDC_EDIT_YMIN, m_sYmin);
	DDX_Text(pDX, IDC_EDIT_XMAX, m_sXmax);
	DDX_Text(pDX, IDC_EDIT_XMIN, m_sXmin);
	DDX_Check(pDX, IDC_ZERONORM, m_bZeronorm);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_TEMPERATURE, m_temp);
	DDX_Text(pDX, IDC_BCKGR, m_backrg);
	DDX_Text(pDX, IDC_HBEG, m_s1);
	DDX_Text(pDX, IDC_HEND, m_s2);
	DDX_Text(pDX, IDC_VBEG, m_p1);
	DDX_Text(pDX, IDC_VEND, m_p2);
	DDX_Check(pDX, IDC_Detector, m_detect);
	DDX_Check(pDX, IDC_REGIME, m_points);
	DDX_Text(pDX, IDC_585, m_585);
	DDX_Text(pDX, IDC_coeff, m_coeff);
}


BEGIN_MESSAGE_MAP(CCommandView, CFormView)
	//{{AFX_MSG_MAP(CCommandView)
	ON_BN_CLICKED(IDC_GO1, OnGo1)
	ON_NOTIFY(TCN_SELCHANGE, IDC_MOTORTAB, OnSelchangeMotortab)
	ON_BN_CLICKED(IDC_MOTOR, OnMotor)
	ON_BN_CLICKED(IDC_RASTERSCAN, OnRasterscan)
	ON_BN_CLICKED(IDC_AUTONORM, OnRedraw)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_XYNORM, OnXynorm)
	ON_BN_CLICKED(IDC_XNORM, OnXnorm)
	ON_BN_CLICKED(IDC_YNORM, OnYnorm)
	ON_BN_CLICKED(IDC_ZERONORM, OnRedraw)
	ON_BN_CLICKED(IDC_REDRAW, OnRedraw)
	ON_BN_CLICKED(IDC_SETUP, OnSetup)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_CONTROL_RANGE(BN_CLICKED,IDC_P1,  IDC_M10000, OnMove)
	ON_BN_CLICKED(IDC_BACKGROUND, &CCommandView::OnBnClickedBackground)
	ON_BN_CLICKED(IDC_Detector, &CCommandView::OnBnClickedDetector)
	ON_BN_CLICKED(IDC_REGIME, &CCommandView::OnBnClickedRegime)
	ON_BN_CLICKED(IDC_Detector, &CCommandView::OnBnClickedDetector)
	ON_BN_CLICKED(IDC_REGIME, &CCommandView::OnBnClickedRegime)
	ON_BN_CLICKED(IDC_BUTTON1, &CCommandView::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CCommandView::OnBnClickedButton2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CCommandView::AssertValid() const{CFormView::AssertValid();}
void CCommandView::Dump(CDumpContext& dc) const{CFormView::Dump(dc);}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CCommandView message handlers

void CCommandView::OnGo1() 
{
	UpdateData(TRUE);	
	Goto((int)(m_Goto*c2_inv+c1_inv),B.ActMotor); //now m_XXX are in nm
	UpdateData(FALSE);
}

void CCommandView::OnMove(UINT nIDC) 
{
	UpdateData();
	m_Pos = (int)(m_Pos*c2_inv + c1_inv); //temporary converting nanometers to positions
	switch (nIDC)
		{
	case IDC_P1:	Goto(m_Pos+10,B.ActMotor);
	default:		break;
	case IDC_P10:	Goto(m_Pos+100,B.ActMotor); break;
	case IDC_P100:	Goto(m_Pos+1000,B.ActMotor); break;
	case IDC_P1000:	Goto(m_Pos+10000,B.ActMotor); break;
	case IDC_P10000:Goto(m_Pos+100000,B.ActMotor); break;
	case IDC_M1:	Goto(m_Pos-10,B.ActMotor); break;
	case IDC_M10:	Goto(m_Pos-100,B.ActMotor); break;
	case IDC_M100:	Goto(m_Pos-1000,B.ActMotor); break;
	case IDC_M1000:	Goto(m_Pos-10000,B.ActMotor); break;
	case IDC_M10000:Goto(m_Pos-100000,B.ActMotor); break;
	}
	m_Pos = (int)(m_Pos*c2 + c1); //converting positions back to nanometers, in case some other subroutine wants it
	UpdateData(FALSE);
}

void CCommandView::OnMotor() 
{
	UpdateData();
	B.ParkAll();
	UpdateData(FALSE);	
}

void CCommandView::OnSave() 
{
	CSaveDlg D;
	if (Working)  { Break=1; return; }	// We collect data
	D.pDoc=(CMotorsDoc *)GetDocument();	// For link
	D.pStep=this;						// Some parameters reside in the view
	if (D.pDoc!=NULL)
		D.DoModal();
}

void CCommandView::OnRedraw() 
{
	UpdateData();
	pDoc->UpdateAllViews(NULL);	
}

void CCommandView::OnXynorm() 
{
	if (pZoom)
	{
		pZoom->bYnorm=pZoom->bXnorm=TRUE;
		pZoom->Invalidate();
	}
}

void CCommandView::OnXnorm() 
{
	if (pZoom)
	{
		pZoom->bXnorm=TRUE;
		pZoom->Invalidate();
	}
}

void CCommandView::OnYnorm() 
{
	if (pZoom)
	{
		pZoom->bYnorm=TRUE;
		pZoom->Invalidate();
	}
}

void CCommandView::OnInitialUpdate() 
{
	BOOL res=0;
	CFormView::OnInitialUpdate();
	ReadRegistry();					// Card and motor data is read in
	pDoc=(CMotorsDoc *)GetDocument();
	CMainFrame *M=(CMainFrame *)pApp->m_pMainWnd;
	OnFirst();
	B.pComm=this;
	B.ActMotor=2;
	//////////////////////////////////////////////////////////////////////////
	//	Add tabs
	//////////////////////////////////////////////////////////////////////////
	//c_ModeTab.InsertItem(0,L"Spectrometer");
	//c_ModeTab.InsertItem(1,L"Vertical");
	c_ModeTab.InsertItem(2,L"Spectometer"); //this is just a tab, it is not decided here which motor is active
	c_ModeTab.SetCurSel(B.ActMotor);
	B.CreateThor();
	MCEnableAxis(B.mcb,MC_ALL_AXES,FALSE);	// Off all motors
	//NoRasterHide(B.ActMotor>1);
	//////////////////////////////////////////////////////////////////////////
	char Buf[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,(wchar_t *)&Buf);
	SetDlgItemText(IDC_EDIT_DIR, (wchar_t *)Buf);
	m_FolderName=Buf;
	Message="Idle";
	int i,j;
	i=(FastMotor&1)^1;
	j=B.Th[i].SweepFrom;					// Convenience values
	sSlowPos.Format(L"Motor:%d",j);
	sActCurve.Format(L"Curve:%d",0);
	sCurves.Format(L"of:%d",0);
	M->PostMessage(WM_USER_ANNOUNCE,0x1d);
	UpdateData(FALSE);
}

extern BOOL IsBroken();

void CCommandView::OnTimer(UINT nIDEvent) 
{	
	double d;
	TCHAR st[40];
	switch(nIDEvent)
	{
	default:	
		CFormView::OnTimer(nIDEvent); 
		return;
	case MOTOR_MOVES:
		KillTimer(nIDEvent); uTimer=0;
		B.GetCurPos(d,B.ActMotor);	
		m_Pos=(int)fabs(d);
		_swprintf(st,L"%i",m_Pos);
		SetDlgItemText(IDC_EDIT_POS1,st);
		if (B.IsMoving(B.ActMotor))
			uTimer=SetTimer(MOTOR_MOVES,200,NULL);		// Allow to return, use sparingly
		break;
	}
}

BOOL CCommandView::PreTranslateMessage(MSG* pMsg)	// Rasterscan break function
{
	if (pMsg->wParam==VK_ESCAPE || pMsg->wParam==VK_SPACE)
	{	
		BOOL ism=B.IsMoving();						// Is any motor moving, set Break on ESC do nothing on SPACE								
		if (ism && pMsg->wParam==VK_ESCAPE) Break=TRUE;		
		return TRUE;								// Do not dispatch message
	}
	else return CFormView::PreTranslateMessage(pMsg);
}

void CCommandView::OnDestroy() 
{
	UpdateData();
	UpdateRegistry();
	CFormView::OnDestroy();
}


//////////////////////////////////////////////////////////////////////////

int CCommandView::Goto(int pos,int motor)			// A macro, sets timer which updates dialog
{			
#ifdef EMULATE
	B.SetCurPos(pos);
#else
	uTimer=SetTimer(MOTOR_MOVES,50,NULL);			// Set timer
	B.GotoNewPos(pos,motor);						// Go somewhere, if motor==-1 use ActMotor		
#endif
	return pos;
}

void CCommandView::SetData(BOOL way)		// set Tabbed data only
{
	double d;
	if (way)	//	Data moves from dialog to the active motor structure. From nanometers to positions
	{
		c2_inv = m_coeff; //steps per nm
		c1_inv = m_585 - c2_inv*585.249; //nanometers to positions pos=nm*c2_inv+c1_inv
		c2 = 1 / c2_inv; // positions to nanometers nm=pos*c2+c1
		c1 = -c1_inv / c2_inv;
		B.Th[B.ActMotor].SetPos=(int)(m_Goto*c2_inv+c1_inv);
		B.Th[B.ActMotor].SweepFrom = (int)(m_From*c2_inv + c1_inv);
		B.Th[B.ActMotor].SweepTo = (int)(m_To*c2_inv + c1_inv);
		B.Th[B.ActMotor].SweepStep = (int)(m_Step*c2_inv);
		if (m_bFastM)  FastMotor=B.ActMotor;
	}
	else //data moves from motor structure to dialogue
	{
		B.GetCurPos(d,B.ActMotor); 
		m_Pos=B.Th[B.ActMotor].CurPos*c2+c1;
		//m_Goto = B.Th[B.ActMotor].SetPos*c2 + c1; --- NB one does not really need to update them as they are not supposed to be calculated, they are given by user.
		//m_From = B.Th[B.ActMotor].SweepFrom*c2 + c1;
		//m_To = B.Th[B.ActMotor].SweepTo*c2 + c1;
		//m_Step = B.Th[B.ActMotor].SweepStep*c2;
		if (FastMotor==B.ActMotor) m_bFastM=TRUE; else m_bFastM=FALSE;
	}
	
}

extern void CheckTimers(double &cht);	

BOOL CCommandView::UpdateData(BOOL way)
{
	if (way)
	{
		CFormView::UpdateData(way);
		if (!Working)				// Data stays the same
		{
			SetData(way);
			ChT=_wtof(m_sCHT);
			s1 = _wtoi(m_s1); //added Nov 5, 2015
			s2 = _wtoi(m_s2);
			p1 = _wtoi(m_p1);
			p2 = _wtoi(m_p2);
			if (m_detect) det = 1; //one if CCD
			else det = 0; //zero if APD/PMT
			CheckTimers(ChT);
		}
	}
	else
	{
		if (!Working)
		{
			CheckTimers(ChT);
		}
		m_sCHT.Format(L"%-7.3f",ChT);
		SetData(way);
		CFormView::UpdateData(way);
	}
	return TRUE;
}

//	Tab dialog functions
void CCommandView::BShow(int way,int nID)
{	CWnd *E=NULL; 
	E=(CWnd *)GetDlgItem(nID);
	if (E!=NULL) E->ShowWindow(way);
}

void CCommandView::NoRasterHide(BOOL hide)
{
	int what;
	if (hide) what=SW_HIDE; else what=SW_RESTORE;
	BShow(what,IDC_FASTMOTOR);
	BShow(what,IDC_EDIT_TO1);
	BShow(what,IDC_EDIT_FROM1);
	BShow(what,IDC_EDIT_STEP1);
	BShow(what,IDC_RSCAN);
	BShow(what,IDC_STO);
	BShow(what,IDC_SFROM);
	BShow(what,IDC_SSTEP);
}

void CCommandView::ActionHide(BOOL hide)
{
	CEdit *E;
	int what;
	if (hide) what=SW_HIDE; else what=SW_RESTORE;
	E=(CEdit *)GetDlgItem(IDC_EDIT_CHT);
	if (E!=NULL) E->SetReadOnly(hide);
	E=(CEdit *)GetDlgItem(IDC_EDIT_FROM1);
	if (E!=NULL) E->SetReadOnly(hide);
	E=(CEdit *)GetDlgItem(IDC_EDIT_TO1);
	if (E!=NULL) E->SetReadOnly(hide);
	E=(CEdit *)GetDlgItem(IDC_EDIT_STEP1);
	if (E!=NULL) E->SetReadOnly(hide);
	E=(CEdit *)GetDlgItem(IDC_EDIT_GOTO1);
	if (E!=NULL) E->SetReadOnly(hide);
	BShow(what,IDC_EDIT_GOTO1);
	BShow(what,IDC_FASTMOTOR);
	BShow(what,IDC_SAVEALL);
	BShow(what,IDC_RESTORE);
	BShow(what,IDC_GO1);
	BShow(what,IDC_GET);
	BShow(what,IDC_SET);
	BShow(what,IDC_MOTOR);
	BShow(what,IDC_SETUP);
	BShow(what,IDC_P1);
	BShow(what,IDC_P10);
	BShow(what,IDC_P100);
	BShow(what,IDC_P1000);
	BShow(what,IDC_P10000);
	BShow(what,IDC_M1);
	BShow(what,IDC_M10);
	BShow(what,IDC_M100);
	BShow(what,IDC_M1000);
	BShow(what,IDC_M10000);
}
/////////////////////////////////////////////////////////////////////////////////////
//		Registry part
/////////////////////////////////////////////////////////////////////////////////////
void CCommandView::UpdateRegistry()
{	CString S="Board";
	pApp=(CMotorsApp *)AfxGetApp();
	pApp->RegSave(L"Settings",L"ChT",ChT);
	pApp->RegSave(L"Settings",L"Xgrid",m_bXgrid);
	pApp->RegSave(L"Settings",L"Ygrid",m_bYgrid);
	pApp->RegSave(L"Settings",L"Autonorm",m_bAutonorm);
	pApp->RegSave(L"Settings",L"Zeronorm",m_bZeronorm);
	pApp->RegSave(L"Settings",L"FastMotor",FastMotor);
	pApp->RegSave(L"Settings",L"Goto",m_Goto);
	pApp->RegSave(L"Settings",L"Pos",m_Pos);
	pApp->WriteProfileString(_T("Settings"),_T("File"),LPCTSTR(m_FileName));
	pApp->WriteProfileString(_T("Settings"),_T("Folder"),LPCTSTR(m_FolderName));
	pApp->WriteProfileString(_T("Settings"),_T("Ymax"),LPCTSTR(m_sYmax));
	pApp->WriteProfileString(_T("Settings"),_T("Ymin"),LPCTSTR(m_sYmin));
	pApp->WriteProfileString(_T("Settings"),_T("Xmax"),LPCTSTR(m_sXmax));
	pApp->WriteProfileString(_T("Settings"),_T("Xmin"),LPCTSTR(m_sXmin));
//	B.UpdateRegistry(S,pApp);
	CRect Rct;
	GetWindowRect(&Rct);
	pApp->RegSaveRect(L"Settings\\DialogRect",Rct);
}

void CCommandView::ReadRegistry()
{	CString S="Board";
	pApp=(CMotorsApp *)AfxGetApp();
//	B.ReadRegistry(S,pApp);
	m_FileName=pApp->GetProfileString(_T("Settings"),_T("File"),L"Savefile.spc");
	m_FolderName=pApp->GetProfileString(_T("Settings"),_T("Folder"),L".");
	m_sYmax=pApp->GetProfileString(_T("Settings"),_T("Ymax"),L"1000");
	m_sYmin=pApp->GetProfileString(_T("Settings"),_T("Ymin"),L"0");
	m_sXmax=pApp->GetProfileString(_T("Settings"),_T("Xmax"),L"1000");
	m_sXmin=pApp->GetProfileString(_T("Settings"),_T("Xmin"),L"0");
	ChT=pApp->dRegRead(L"Settings",L"ChT",L"0.1");
	m_bAutonorm=pApp->iRegRead(L"Settings",L"Autonorm",L"0");
	m_bZeronorm=pApp->iRegRead(L"Settings",L"Zeronorm",L"0");
	m_bXgrid=pApp->iRegRead(L"Settings",L"Xgrid",L"0");
	m_bYgrid=pApp->iRegRead(L"Settings",L"Ygrid",L"0");
	FastMotor=pApp->iRegRead(L"Settings",L"FastMotor",L"0");
	FastMotor = 2; // brute force switch to the spectrometer'motor
	m_Goto=pApp->iRegRead(L"Settings",L"Goto",L"1000");
	m_Pos=pApp->iRegRead(L"Settings",L"Pos",L"1000");
	CRect Rct;
	pApp->RegReadRect(L"Settings\\DialogRect",Rct);
	if (Rct.bottom>Rct.top)
	{	int x,y,cx,cy;
		x=Rct.left; y=Rct.top;
		CRect R; GetWindowRect(&R);
		cx=R.Width(); cy=R.Height();
		SetWindowPos(&wndTop,x,y,cx,cy,SWP_SHOWWINDOW);   
	}
	Sleep(20);
}

//////////////////////////////////////////////////////////////////////////
void CCommandView::OnSelchangeMotortab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData(TRUE);
	B.ActMotor=c_ModeTab.GetCurSel();		// Get current selection
	ActionHide(Working);					// Hide if rasterscanning
	NoRasterHide(B.ActMotor>1);
	UpdateData(FALSE);
	*pResult = 0;
}

void CCommandView::OnFirst()
{
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = pDoc->GetNextView(pos);
		if (pView->IsKindOf( RUNTIME_CLASS( CDumpView )))	pDump=(CDumpView *)pView;
		if (pView->IsKindOf( RUNTIME_CLASS( CMotorsView ))) pZoom=(CMotorsView *)pView;
	}
	pComm=this;
	pCard=&B;
	if (pDump) pDump->pComm=this;
	if (pZoom) pZoom->pComm=this;
} 

void CCommandView::OnRasterscan() 
{
	Break=FALSE;
	scan = TRUE;
	baseline = FALSE;
	calibration = FALSE;
	UpdateData();
	SetDlgItemText(IDC_SAVE,L"ESC to Break");
	Message = "Spectrum Measurement Scan; Reported as OD if Baseline is Available";
	Sbar=1;										// Start Timer show Message
	pDump->Clear();
	ActionHide(TRUE);
	pApp->StartInThread(RASTERSCAN);
	
}

void CCommandView::OnSetup() 
{
	int res=MCDLG_ConfigureAxis(this->m_hWnd,B.mcb,B.ActMotor+1,MCDLG_CHECKACTIVE|MCDLG_PROMPT,NULL);
	if (res==MCERR_NOERROR)						// IDOK
	{
		B.ObtainInfo(B.ActMotor);
	}
	double pos;
	B.GetCurPos(pos);
	UpdateData(FALSE);
}


//////////////////////////////////////////////////////////////////////////////

void CCommandView::OnBnClickedBackground()
{
	//measuring CCD background. This background is then subtracted from the measured signal. Analog of background correction in WinSpec
	{
		Break = FALSE;
		UpdateData(); //need to have valid exposure time ChT and ROI parameters
		if (m_detect == false) {
			return; //CCD camera is not on, get out, you must have pushed it by mistake
		}
		rs_bool stat;
		uns32 count, index;
		int16 enumValue;
		stat = pl_get_param(hCam, PARAM_TEMP, ATTR_CURRENT, (void *)&enumValue);
		unsigned short s1, s2, p1, p2; // s- horizontal limits, p-vertical limits (inclusive)
		s1 = _wtoi(m_s1);
		s2 = _wtoi(m_s2);
		p1 = _wtoi(m_p1);
		p2 = _wtoi(m_p2);
		rgn_type region = { s1, s2, 1, p1, p2, 1 };
		//make it based on user-defined parameters, binning constants are one - software binning, 
		//each pixel can deliver up to 2^16 counts.
		uns32 size; // will be figured out by the CCD camera based on the parameters of the region
		uns16 *frame;
		int16 status;
		uns32 not_needed;
		uns32 i; //cycle variable

		//camera is initialized the moment someone checks the checkbox; here we only initialize the sequence
		pl_exp_init_seq();
		pl_exp_setup_seq(hCam, 1, 1, &region, TIMED_MODE, int(ChT*1000.0), &size);
		//camera, number of images to take, numbers of regions to take, region description, mode, exposure time (units?), size is filled
		frame = (uns16*)malloc(size); //meaning it makes a one-dimensional array of uns16

		/* Start the acquisition */
		pl_exp_start_seq(hCam, frame); // take a frame

		/* wait for data or error */
		while (pl_exp_check_status(hCam, &status, &not_needed) &&
			(status != READOUT_COMPLETE && status != READOUT_FAILED));

		/* Check Error Codes */
		if (status == READOUT_FAILED) {
			printf("Data collection error: %i\n", pl_error_code());
			//return (0);
		}
		background = 0;
		//uns16 a[1000];
		for (i = 0; i < size / 2 - 1; i++)
		{
			//a[i]=frame[i];
			background = background + frame[i]; //software binning
		}
		/* Finish the sequence */
		pl_exp_finish_seq(hCam, frame, 0);
		/*Uninit the sequence */
		pl_exp_uninit_seq();
		free(frame);
		m_backrg.Format(L"%d", background);
		m_temp.Format(L"%d", enumValue / 100);
		UpdateData(false); //putting the temperature and background values into respective fields
	}
}


///switching between detector types. Even if CCD camera is powered on, someone needs to tell it to go to -75 C for optimal performance.
//This part also checks if everything is OK with the camera.

void CCommandView::OnBnClickedDetector()
{
	Break = FALSE;
	UpdateData(); //need to have valid det value
	rs_bool status, status_check;     /* status of pvcam functions                   */
	rs_bool avail_flag; /* ATTR_AVAIL, param is available              */
	uns16 access;       /* ATTR_ACCESS, param is read, write or exists */
	uns16 type, error_code;         /* ATTR_TYPE, param data type                  */
	if (m_detect) det = 1; //one if CCD
	else det = 0; //zero if APD/PMT
	if (det == 1) //i.e. if we are supposed to be detecting with the camera
	{
		//initialize the camera
		int16 temperature = -7500; //required format: temperature in C times 100
		int16 enumValue; //to be used to READ temperature
		pl_pvcam_init();
		//error_code = pl_error_code();
		pl_cam_get_name(0, cam_name); //cam num runs from zero to number of cameras -1
		//error_code = pl_error_code();
		status_check=pl_cam_open(cam_name, &hCam, OPEN_EXCLUSIVE);
		//error_code = pl_error_code();
		//and then we also need to set the camera temperature to -75 C for optimal noise:
		status = pl_get_param(hCam, PARAM_TEMP_SETPOINT, ATTR_AVAIL, (void *)&avail_flag);
		if (status) {
			/* check to see if parameter id is supported by hardware or software */
			if (avail_flag) {
				/* we got a valid parameter, now get access rights and data type */
				status = pl_get_param(hCam, PARAM_TEMP_SETPOINT, ATTR_ACCESS, (void *)&access);
				if (status && access == ACC_READ_WRITE) {
					/* we can set it, let's be safe and check to make sure
					it is the right data type */
					status = pl_get_param(hCam, PARAM_TEMP_SETPOINT, ATTR_TYPE,
						(void *)&type);
					if (status) {
						if (type == TYPE_INT16) {
							pl_get_param(hCam, PARAM_TEMP, ATTR_CURRENT, (void *)&enumValue);
							/* OK lets write to it */
							pl_set_param(hCam, PARAM_TEMP_SETPOINT, (void *)&temperature);
						} //end of if correct data type
						else {
							printf("data type mismatch for param_id "
								"%x\n", PARAM_TEMP_SETPOINT);
							status = FALSE;
						} //end of incorrect data type
					} //end of if status 
				} // end of if status is OK and one can write into this parameter
			} //end of if avail_flag
		}//end of if status
	} //end of if uning camera  
	else{ // if camera was one and we switch from the camera to the photon couning detector
		pl_cam_close(hCam);
		pl_pvcam_uninit();
	} //end of else - turning to singe detector
}


void CCommandView::OnBnClickedRegime() //this is a kinetics mode. In spectrometer mode the actual number of points is calculated based on the parameters of the scan.
{
	UpdateData();
	if (m_points) points = 30000;
	else points = 0;
	points = points + 1;
}


void CCommandView::OnBnClickedButton1() //baseline button
{
	Break = FALSE;
	scan = FALSE;
	baseline = TRUE;
	calibration = FALSE;
	UpdateData();
	SetDlgItemText(IDC_SAVE, L"ESC to Break");
	Message = "Baseline Scan, Reporting Transmission";
	Sbar = 1;										// Start Timer show Message
	pDump->Clear();
	ActionHide(TRUE);
	pApp->StartInThread(RASTERSCAN);
}


void CCommandView::OnBnClickedButton2() //calibration button
{
	Break = FALSE;
	scan = FALSE;
	baseline = FALSE;
	calibration = TRUE;
	UpdateData();
	SetDlgItemText(IDC_SAVE, L"ESC to Break");
	Message = "Calibration Scan, Signal vs. Motor Position";
	Sbar = 1;										// Start Timer show Message
	pDump->Clear();
	ActionHide(TRUE);
		pApp->StartInThread(RASTERSCAN);
}
