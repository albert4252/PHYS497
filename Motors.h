// Motors.h : main header file for the MOTORS application
//

#if !defined(AFX_MOTORS_H__C58640D1_8AB1_435C_B003_5C2BBC193EDC__INCLUDED_)
#define AFX_MOTORS_H__C58640D1_8AB1_435C_B003_5C2BBC193EDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "WorkerThreadInfo.h"
#include "master.h"
#include "pvcam.h"
/////////////////////////////////////////////////////////////////////////////
// CMotorsApp:
// See Motors.cpp for the implementation of this class
//

#define INIT_HDWR		11		// Initialize hardware at the beginning
#define RASTERSCAN		12
#define MANUAL			13
#define	CURSE			14
//////////////////////////////////////////////////////////////////////////
//	Global variables block
//////////////////////////////////////////////////////////////////////////
class CMotorsApp;
class CMotorsDoc;
class CDumpView;
class CMotorsView;
class CCommandView;
class MCcard;

#ifdef MOTORS_MAIN_APPLICATION
	BOOL	Working=0;
	Thread  BOOL	Worker=FALSE;	// Worker is TRUE when we are in the workerthread
	int		Break=FALSE;
	int		Sbar=0;
	int		FastMotor;
	int		SlowPos,FastPos;		// Slow and Fast motor Pos
	double	LoadValue;				// 
	double	ChT;					// Channel time in s
	int     points, det, s1, s2, p1, p2, background, start_pos, end_pos, step_pos;
	double c1,c2,c1_inv, c2_inv; //conversion between positions and wavelength
	char cam_name[CAM_NAME_LEN];    /* camera name                    */
	int16 hCam;                     /* camera handle                  */
	CString Error;
	CString Message;
	CString sCurves;
	CString sActCurve;
	CString sSlowPos;
	CMotorsApp	 *pApp=NULL;
	CMotorsDoc	 *pDoc=NULL;
	CDumpView	 *pDump=NULL;
	CMotorsView  *pZoom=NULL;
	CCommandView *pComm=NULL;
	MCcard		 *pCard=NULL;
	BOOL scan, baseline, baseline_measured, calibration;
	double base[100000];
#else
	extern	BOOL	Working;
	extern	Thread  BOOL	Worker;	
	extern	int		Break;
	extern	int		Sbar;
	extern	int		FastMotor;
	extern	int		SlowPos,FastPos;
	extern	double	LoadValue;		 
	extern	double	ChT;		
	extern  int     points, det, s1, s2, p1, p2, background, start_pos, end_pos, step_pos;
	extern double c1, c2, c1_inv, c2_inv; //conversion between positions and wavelength
	extern	CString Error;
	extern	CString Message;
	extern	CString sCurves;
	extern	CString sActCurve;
	extern	CString sSlowPos;
	extern char cam_name[CAM_NAME_LEN];    /* camera name                    */
	extern int16 hCam;                     /* camera handle                  */
	extern	CMotorsApp	 *pApp;
	extern	CMotorsDoc	 *pDoc;
	extern	CDumpView	 *pDump;
	extern	CMotorsView  *pZoom;
	extern	CCommandView *pComm;
	extern	MCcard		 *pCard;
	extern BOOL scan, baseline, baseline_measured, calibration;
	extern double base[100000];
#endif MOTORS_MAIN_APPLICATION

//////////////////////////////////////////////////////////////////////////

class CMotorsApp : public CWinApp
{
public:
	CMotorsApp();
	virtual ~CMotorsApp();
	int		iRegRead(TCHAR *title,TCHAR *subtitle,TCHAR *defaut); 
	void	RegSave(TCHAR *title, TCHAR *subtitle, int i);
	double	dRegRead(TCHAR *title, TCHAR *subtitle, TCHAR *defaut);
	void	RegSave(TCHAR *title, TCHAR *subtitle, double d);
	void	RegSaveRect(TCHAR *s, CRect &r);
	void	RegReadRect(TCHAR *s, CRect &r);
// Overrides
	//		Worker Thread related
	BOOL	m_bWorkerActive;
	CWinThread*			m_pWorkerThread;
	CWorkerThreadInfo	m_ThreadInfo;
	HANDLE	m_hEventStart;
	HANDLE	m_hEventDone;
	HANDLE	m_hEventKillThread;
	HANDLE	m_hEventThreadKilled;
	HANDLE	m_hEventStopThread;
	HANDLE	m_hEventThreadStopped;
	void	KillThread();					// Kill thread		
	void	StopThread();					// Signal to process in the other thread to stop
	void	StartInThread(int op);			// Start thread
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMotorsApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CMotorsApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOTORS_H__C58640D1_8AB1_435C_B003_5C2BBC193EDC__INCLUDED_)
