// WorkerThreadInfo.h: interface for the CWorkerThreadInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORKERTHREADINFO_H__3499A9E9_6B1A_4177_8A7A_3323F9218362__INCLUDED_)
#define AFX_WORKERTHREADINFO_H__3499A9E9_6B1A_4177_8A7A_3323F9218362__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CScanDAQApp;

class CWorkerThreadInfo				// A mere Structure
{
public:
	CWorkerThreadInfo();			// Not used
	virtual ~CWorkerThreadInfo();
	HWND	m_hwndNotifyDone;		// Set to point to CView      in document
	HWND	m_hwndNotifyProgress;		//				to CMainframe
	HANDLE	m_hEventStart;			// Starts at least Pure Calculation
	HANDLE	m_hEventDone;			// Either Simplex or Calculation is done
	HANDLE	m_hEventKillThread;		// We want to get rid with thread
	HANDLE	m_hEventStopThread;		// We want just to stop calculations
	HANDLE	m_hEventThreadKilled;	// Response from simplex
	HANDLE	m_hEventThreadStopped;	// Response from simplex

	// Parameter Exchange
	int				Operation;		// What to DO
	LPARAM	LP;						// Usually pointer of known type		
	LPARAM	WP;						// Usually something else		
	DWORD	ThreadID;				// Control thread ID for messaging
};

#endif // !defined(AFX_WORKERTHREADINFO_H__3499A9E9_6B1A_4177_8A7A_3323F9218362__INCLUDED_)
