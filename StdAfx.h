// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__61A51EAE_0F78_45BD_9E56_572123C6C037__INCLUDED_)
#define AFX_STDAFX_H__61A51EAE_0F78_45BD_9E56_572123C6C037__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _WIN32_WINNT 0x0600
#define WINVER 0x0600
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define Thread  __declspec( thread )		// This way a variable is told to be thread specific
#include <afxmt.h>			// Mutexes

#include <afxtempl.h>		// MFC support for Internet Explorer 4 Common Controls
typedef  CArray<double,double> Carry;

#include <gdiplus.h>		// GDIplus.dll !
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

#define WM_USER_ANNOUNCE (WM_USER+1)	// Announce the error message or log it
#define WM_USER_LOAD (WM_USER+2)		// Load point

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__61A51EAE_0F78_45BD_9E56_572123C6C037__INCLUDED_)
