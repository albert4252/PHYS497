#if !defined(AFX_SAVEDLG_H__132CB6FB_33FA_46FC_BF25_916A57B325A9__INCLUDED_)
#define AFX_SAVEDLG_H__132CB6FB_33FA_46FC_BF25_916A57B325A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SaveDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
class CCommandView;

class CSaveDlg : public CDialog
{
// Construction
public:
	CSaveDlg(CWnd* pParent = NULL);   // standard constructor
	void	Refresh();
// Dialog Data
	//{{AFX_DATA(CSaveDlg)
	enum { IDD = IDD_SAVEDLG };
	CString	m_FileName;
	CString	m_FolderName;
	int		m_StartCurve;
	int		m_Curves;
	int		m_SaveAmount;
	CString	m_Spectra;
	//}}AFX_DATA
	CMotorsDoc *pDoc;
	CCommandView *pStep;
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaveDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSaveDlg)
	afx_msg void OnDirBrowse();
	afx_msg void OnFileBrowse();
	afx_msg void OnSave();
	afx_msg void OnTruncate();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnRefresh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAVEDLG_H__132CB6FB_33FA_46FC_BF25_916A57B325A9__INCLUDED_)
