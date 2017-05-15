#if !defined(AFX_COMMANDVIEW_H__51DD8FA2_7996_4663_BE13_C963EC754283__INCLUDED_)
#define AFX_COMMANDVIEW_H__51DD8FA2_7996_4663_BE13_C963EC754283__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CommandView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCommandView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "MCcard.h"
#include "Data.h"
#include "afxwin.h"

class CCommandView : public CFormView
{
protected:
	CCommandView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCommandView)

// Form Data
public:
	//{{AFX_DATA(CCommandView)
	enum { IDD = IDD_STEPPERS_DIALOG };
	BOOL	m_bAutonorm;
	BOOL	m_bFastM;
	BOOL	m_bShowLast;
	BOOL	m_bXgrid;
	BOOL	m_bYgrid;
	CTabCtrl	c_ModeTab;
	double		m_Pos; //changed in April 2017. Interface is now in nm, and these are converted to positions internally
	double		m_Goto;
	double		m_From;
	double		m_To;
	double		m_Step;
	CString	m_sCHT;
	CString	m_sYmax;
	CString	m_sYmin;
	CString	m_sXmax; 
	CString	m_sXmin;
	BOOL	m_bZeronorm;
	//}}AFX_DATA

// Attributes
public:
	MCcard	B;									// PCX-PCI100 motor driver card
	CString	m_FolderName,m_FileName;
	UINT	uTimer;
	int		WorkingMotor;						// Acts as ActMotor at rasterscan time
		
// Operations
public:
	int		Goto(int pos,int motor=-1);			// A macro, sets timer which updates dialog
	void	MotorSleep();						// OnTimer message dispatcher
	void	SetData(BOOL way);					// set Tabbed data only
	BOOL	UpdateData(BOOL way=TRUE);
	void	ActionHide(BOOL hide);
//	void	CheckTimers(double &cht);			// Get canonical values for timer
	void	ReadRegistry();
	void	UpdateRegistry();
	void	BShow(int way,int nID);
	void	NoRasterHide(BOOL hide);
//	BOOL	WaitforReady();						// 
//	BOOL	Detect(double &data);				// Detector
	void	OnFirst();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommandView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCommandView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCommandView)
	afx_msg void OnGo1();
	afx_msg void OnSelchangeMotortab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMotor();
	afx_msg void OnRasterscan();
	afx_msg void OnRedraw();
	afx_msg void OnSave();
	afx_msg void OnXynorm();
	afx_msg void OnXnorm();
	afx_msg void OnYnorm();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnSetup();
	//}}AFX_MSG
	afx_msg void OnMove(UINT nIDC);
	afx_msg void OnBnClickedBackground();
	DECLARE_MESSAGE_MAP()
public:
	CString m_temp; //temperature
	CString m_backrg; //background value
	CString m_s1; //ROI horizontal lower limit
	CString m_s2; //ROI horizontal higher limit
	CString m_p1; //ROI vertical ower limit
	CString m_p2; // ROI vertical higher limit
	BOOL m_detect; // detector choice. For false - counter, for true - CCD 
	BOOL m_points; // regime choice, For false - rasterscan, for true - kinetics
	
	afx_msg void OnBnClickedDetector();
	afx_msg void OnBnClickedRegime();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	int m_585;
	double m_coeff;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMANDVIEW_H__51DD8FA2_7996_4663_BE13_C963EC754283__INCLUDED_)
