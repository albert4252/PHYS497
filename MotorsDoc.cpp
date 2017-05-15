// MotorsDoc.cpp : implementation of the CMotorsDoc class
//

#include "stdafx.h"
#include "Motors.h"

#include "MotorsDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMotorsDoc

IMPLEMENT_DYNCREATE(CMotorsDoc, CDocument)

BEGIN_MESSAGE_MAP(CMotorsDoc, CDocument)
	//{{AFX_MSG_MAP(CMotorsDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMotorsDoc construction/destruction

CMotorsDoc::CMotorsDoc()
{
	ActSpec=-1;
}

CMotorsDoc::~CMotorsDoc()
{
}

BOOL CMotorsDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMotorsDoc serialization

void CMotorsDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMotorsDoc diagnostics

#ifdef _DEBUG
void CMotorsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMotorsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMotorsDoc commands
