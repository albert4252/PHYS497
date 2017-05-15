// SaveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Motors.h"
#include "MotorsDoc.h"
#include "SaveDlg.h"
#include "Wspec.h"
#include "CommandView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSaveDlg dialog


CSaveDlg::CSaveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSaveDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSaveDlg)
	m_FileName = _T("");
	m_FolderName = _T("");
	m_StartCurve = 0;
	m_Curves = 0;
	m_SaveAmount = 0;
	m_Spectra = _T("");
	//}}AFX_DATA_INIT
}


void CSaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSaveDlg)
	DDX_Text(pDX, IDC_EDIT_FILE, m_FileName);
	DDX_Text(pDX, IDC_EDIT_DIR, m_FolderName);
	DDX_Text(pDX, IDC_EDIT_START, m_StartCurve);
	DDX_Text(pDX, IDC_EDIT_CURVES, m_Curves);
	DDX_Text(pDX, IDC_EDIT_AMOUNT, m_SaveAmount);
	DDX_Text(pDX, IDC_EDIT_SPECTRA, m_Spectra);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSaveDlg, CDialog)
	//{{AFX_MSG_MAP(CSaveDlg)
	ON_BN_CLICKED(IDC_DIR_BROWSE, OnDirBrowse)
	ON_BN_CLICKED(IDC_FILE_BROWSE, OnFileBrowse)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_TRUNCATE, OnTruncate)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
//		Browse for directory, copied from SDK example, very useful
//////////////////////////////////////////////////////////////////////////
// ============================================================================
// Function     : DirBrowseCallbackProc
// Purpose      : Callback procedure for DirBrowseDialog
//              : 1) Initialise to cwd on startup
//              : 2) Set status text to the selected directory
// ============================================================================
static int __stdcall BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData)
{
    TCHAR szDir[MAX_PATH] = {NULL};
    if (uMsg == BFFM_INITIALIZED)				//  Browse dialog initialised - set it to the cwd
    {
        if (GetCurrentDirectory(sizeof(szDir)/sizeof(TCHAR), szDir))
            SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)szDir);
    }
    else if (uMsg == BFFM_SELCHANGED)			// Fail if non filesystem  ie "//server" selected : BIF_RETURNONLYFSDIRS
    {
        if (!SHGetPathFromIDList((LPITEMIDLIST) lp ,szDir))
            EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);		// disable the OK button
        SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)szDir);	// Set the status window to the currently selected path (or NULL).
    }
    return 0;
}

void CSaveDlg::OnDirBrowse() 
{
	BOOL	retcode=FALSE;
	BROWSEINFO FB;								// The calling application is responsible for freeing 
	LPMALLOC pMalloc;							// the returned item identifier list using the 
	if (!(SUCCEEDED(SHGetMalloc(&pMalloc)))) return;	// USC : No memory for shell
	SHGetMalloc(&pMalloc);						// shell's task allocator.
	LPITEMIDLIST pidl;
	TCHAR Buf[MAX_PATH];
	
	ZeroMemory(&FB,sizeof(FB));
	FB.hwndOwner=m_hWnd;
    FB.pidlRoot=NULL; 
	FB.lpszTitle=L"Select a new working directory";
    FB.pszDisplayName=Buf; 
	FB.ulFlags=BIF_RETURNONLYFSDIRS|BIF_STATUSTEXT;
    FB.lpfn=BrowseCallbackProc; 
    FB.lParam=0; 
	pidl = SHBrowseForFolder(&FB);
	if (pidl != NULL)							// pidl == NULL -> Canceled
	{
		retcode = SHGetPathFromIDList(pidl, Buf);
		pMalloc->Free(pidl);
		if (retcode)
		{
			SetCurrentDirectory(Buf);
			SetDlgItemText(IDC_EDIT_DIR,Buf);
			m_FolderName=Buf;
		}
	}
}

UINT Browseflags=OFN_EXPLORER|OFN_HIDEREADONLY|OFN_ENABLESIZING;

void CSaveDlg::OnFileBrowse() 
{
	int i;
	CFileDialog *CFD;
	CString Name; 
	CFD=new CFileDialog(FALSE,L"",LPCTSTR(Name),Browseflags);	// TRUE - open, FALSE - save as
	CFD->m_ofn.lpstrFilter=L"SpectraSolve (.SPC)\0*.SPC\0All\0*.*\0\0";
	CFD->m_ofn.lpstrInitialDir=LPCTSTR(Name);
	if (CFD->DoModal()==TRUE)
	{	m_FileName=CFD->GetFileName();					
		Name=CFD->GetPathName();
		i=Name.ReverseFind('\\');
		if (i>0) 
		{	m_FolderName=Name.Left(i);
			SetCurrentDirectory(LPCTSTR(m_FolderName));
		}
		UpdateData(FALSE);
	}
	delete CFD;
}

void CSaveDlg::OnSave() 
{
	UpdateData();
	WSpec Sp;
	CString S;
	long type,sp;
	type = Sp.Check((const wchar_t *)m_FileName, &sp);
	if (type==UNEXIST)
		Sp.Create(m_FileName);
	type=Sp.Check(m_FileName,&sp);
	if (type>=SP90 && type<=SP97)
	{
		int i,leng;
		leng=pDoc->Spc.GetSize();
		if (m_StartCurve+m_SaveAmount>leng) 
		{	m_SaveAmount=leng;
			m_StartCurve=0;
		}
		for (i=m_StartCurve;i<m_SaveAmount;i++)
		{
			pDoc->Spc[i].Append((const wchar_t *)m_FileName);
		}
		Refresh();
	}	
	
}

void CSaveDlg::OnTruncate() 
{
	WSpec Sp;
	long sp=0,type=UNEXIST;
	type=Sp.Check(m_FileName,&sp);
	if (type>=SP90 && type<=SP97)
	{
		DeleteFile(m_FileName);
	}	
	OnSave();
}

void CSaveDlg::OnOK() 
{
	pStep->m_FolderName=m_FolderName;	
	pStep->m_FileName=m_FileName;	
	CDialog::OnOK();
}

BOOL CSaveDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_FolderName=pStep->m_FolderName;	
	m_FileName=pStep->m_FileName;	
	m_SaveAmount=m_Curves=pDoc->Curves;
	m_StartCurve=0;
	Refresh();
	UpdateData(FALSE);	
	return TRUE;  
}

void CSaveDlg::Refresh() 
{
	WSpec Sp;
	long sp=0,type=UNEXIST;
	type=Sp.Check(m_FileName,&sp);
	if (type==UNEXIST) sp=0;
	if (type==UNEXIST || (type>=SP90 && type<=SP97))
	{	m_Spectra.Format(L"%d",sp);
	}
	else m_Spectra="Not a SpectraSolve file";
	UpdateData(FALSE);	
}

void CSaveDlg::OnRefresh() 
{
	Refresh();
}
