// MotorsDoc.h : interface of the CMotorsDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOTORSDOC_H__50C72B05_0D46_4590_A180_EDF36B378C6E__INCLUDED_)
#define AFX_MOTORSDOC_H__50C72B05_0D46_4590_A180_EDF36B378C6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Data.h"
#include "Wspec.h"

class CMotorsDoc : public CDocument
{
protected: // create from serialization only
	CMotorsDoc();
	DECLARE_DYNCREATE(CMotorsDoc)

// Attributes
public:

	int		Curves,Scanned;		// Total & Rasterscanned curves
	int		ActSpec;
	CArray<WSpec,WSpec&> Spc;	// Main Data Array
	public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMotorsDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMotorsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMotorsDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOTORSDOC_H__50C72B05_0D46_4590_A180_EDF36B378C6E__INCLUDED_)
