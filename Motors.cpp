#include "stdafx.h"
#define MOTORS_MAIN_APPLICATION
#include "Motors.h"
#undef  MOTORS_MAIN_APPLICATION
#include "MainFrm.h"
#include "MotorsDoc.h"
#include "MotorsView.h"
#include "CommandView.h"
#include "DumpView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMotorsApp

BEGIN_MESSAGE_MAP(CMotorsApp, CWinApp)
	//{{AFX_MSG_MAP(CMotorsApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMotorsApp construction

CMotorsApp::CMotorsApp()
{
	m_pWorkerThread = NULL;					// If not NULL we start to kill it on exit	
	m_hEventStart =			CreateEvent(NULL, FALSE, FALSE, NULL);		// auto reset, initially reset
	m_hEventDone =			CreateEvent(NULL, TRUE,  TRUE,  NULL);		// manual reset, initially set
	m_hEventKillThread =	CreateEvent(NULL, FALSE, FALSE, NULL);		// auto reset, initially reset
	m_hEventThreadKilled =	CreateEvent(NULL, FALSE, FALSE, NULL);		// auto reset, initially reset
	m_hEventStopThread =	CreateEvent(NULL, TRUE,  FALSE, NULL);		// manual reset, initially reset
	m_hEventThreadStopped =	CreateEvent(NULL, FALSE, FALSE, NULL);		// auto reset, initially reset
	
	m_ThreadInfo.m_hEventStart =		m_hEventStart;
	m_ThreadInfo.m_hEventDone =			m_hEventDone;
	m_ThreadInfo.m_hEventKillThread =	m_hEventKillThread;
	m_ThreadInfo.m_hEventThreadKilled = m_hEventThreadKilled;
	m_ThreadInfo.m_hEventStopThread =	m_hEventStopThread;
	m_ThreadInfo.m_hEventThreadStopped =m_hEventThreadStopped;
	m_bWorkerActive=FALSE;
}

CMotorsApp::~CMotorsApp()
{
	KillThread();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMotorsApp object

CMotorsApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMotorsApp initialization

BOOL CMotorsApp::InitInstance()
{
	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	SetRegistryKey(_T("TR Software"));
	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)
	// Register document templates

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMotorsDoc),
		RUNTIME_CLASS(CMainFrame),       // From view 
		RUNTIME_CLASS(CCommandView));
	AddDocTemplate(pDocTemplate);
	AddDocTemplate(new CSingleDocTemplate(IDR_ZOOMTYPE,	// Zoom
		RUNTIME_CLASS(CMotorsDoc),
		RUNTIME_CLASS(CMainFrame),
		RUNTIME_CLASS(CMotorsView)));
	AddDocTemplate(new CSingleDocTemplate(IDR_DUMPTYPE,	// dump
		RUNTIME_CLASS(CMotorsDoc),
		RUNTIME_CLASS(CMainFrame),
		RUNTIME_CLASS(CDumpView)));
	
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	
	pApp=this;
	StartInThread(INIT_HDWR);	// Start Worker Detect & Initialize HDWR 
	
	return TRUE;
}

int CMotorsApp::iRegRead(TCHAR *title, TCHAR *subtitle, TCHAR *defaut)
{	CString S; int i;
	S=GetProfileStringW(title,subtitle,defaut);
	i = _wtoi((const wchar_t *)S);
	return i;
}

void CMotorsApp::RegSave(TCHAR *title, TCHAR *subtitle, int i)
{
	TCHAR s[_MAX_PATH];
	_swprintf(s, L"%i", i);
	WriteProfileStringW(title, subtitle, s);
}

double CMotorsApp::dRegRead(TCHAR *title, TCHAR *subtitle, TCHAR *defaut)
{
	CString S; double d;
	S = GetProfileString(title, subtitle, defaut);
	d = _wtof((const wchar_t *)S);
	return d;
}

void CMotorsApp::RegSave(TCHAR *title, TCHAR *subtitle, double d)
{
	TCHAR s[_MAX_PATH];
	_swprintf(s, L"%g", d);
	WriteProfileString(title, subtitle, s);
}


void CMotorsApp::RegSaveRect(TCHAR *s, CRect &r)
{
	if (r.top<r.bottom)
	{
		RegSave(s,L"top",r.top);
		RegSave(s,L"bottom",r.bottom);
		RegSave(s,L"left",r.left);
		RegSave(s,L"right",r.right);
	}
}

void CMotorsApp::RegReadRect(TCHAR *s,CRect &r)
{
	CRect R;
	R.top=iRegRead(s,L"top",L"0");
	R.bottom=iRegRead(s,L"bottom",L"0");
	R.left=iRegRead(s,L"left",L"0");
	R.right=iRegRead(s,L"right",L"100");
	if (R.top<R.bottom) r=R;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CMotorsApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CMotorsApp message handlers
//***********************************  Worker thread Exit/Entrance *************************************************//
extern UINT WorkerThreadProc(LPVOID pParam);	
void CMotorsApp::KillThread()		// Kill this thread altogether
{
	if (m_pWorkerThread == NULL) return;
	SetEvent(m_hEventKillThread);
	SetEvent(m_hEventStart);		// In case we are in an INFINITE loop
	WaitForSingleObject(m_hEventThreadKilled, INFINITE);
	m_bWorkerActive = FALSE;
	ResetEvent(m_hEventStart);		
	m_pWorkerThread = NULL;
}

void CMotorsApp::StopThread()		// Stop process in worker thread
{
	if (m_pWorkerThread == NULL) return;
	SetEvent(m_hEventStopThread);
	SetEvent(m_hEventStart);		// In case we are in an INFINITE loop
	WaitForSingleObject(m_hEventThreadStopped, INFINITE);
	m_bWorkerActive = FALSE;
	ResetEvent(m_hEventStart);		
	ResetEvent(m_hEventStopThread);
}

void CMotorsApp::StartInThread(int op)
{
	if (m_pWorkerThread == NULL)		// Begin the worker-thread loop.   Done once or when killed
	{	m_pWorkerThread = AfxBeginThread(WorkerThreadProc, &m_ThreadInfo, THREAD_PRIORITY_ABOVE_NORMAL);}
	m_ThreadInfo.Operation=op;			// WFC Cannot withstand two normal priority or no normal priority threads???
	m_ThreadInfo.m_hwndNotifyProgress = AfxGetMainWnd()->m_hWnd;
	m_ThreadInfo.ThreadID=GetCurrentThreadId();
	// We need to Set/Reset all events, because they may be get mis-set
	ResetEvent(m_hEventDone);		// Not in progress, actually
	ResetEvent(m_hEventKillThread);	// ...
	ResetEvent(m_hEventStopThread);	// ...
	ResetEvent(m_hEventThreadKilled);
	ResetEvent(m_hEventThreadStopped);
	m_bWorkerActive = TRUE;
	SetEvent(m_hEventStart);		// This starts the thread, so its the last one
}



