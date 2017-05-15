///////////////////////////////////////////////////////////////////////////
//
//  NAME
//    MCDlg.h - Motion Control API setup dialogs header file
//
//  DESCRIPTION
//    Include this header file in your application if you will be using
//    the MCAPI setup dialogs.
//
//    #include "mcdlg.h"
//
//  RELEASE HISTORY
//    Copyright (c) 1999-2003 by Precision Micro Control Corp. All rights
//    reserved.
//
//    Version 2.1a      22-Feb-99       Programmer: Brian Gaynor
//      - First release as a stand-alone product (version number set equal
//        to the API release).
//
//    Version 2.1b       7-May-99       Programmer: R. Brian Gaynor
//      - Added SS_USERITEM style definition for Windows 3.1.
//
//    Version 2.1c      25-Jun-99       Programmer: R. Brian Gaynor
//      - No changes to this module
//
//    Version 2.20      17-Jan-00       Programmer: R. Brian Gaynor
//      - Added configuration flags for MCDLG_ConfigureAxis()
//      - new bitmap for MCDLG_LEDCLASS
//      - CreateWindow() font selection for the MCDLG_READOUTCLASS
//
//    Version 2.21       6-Mar-00       Programmer: R. Brian Gaynor
//      - No changes to this module
//
//    Version 2.22      31-Mar-00       Programmer: R. Brian Gaynor
//      - MC_RestoreDigitalIO will no longer restore channels dedicated to
//        stepper control
//
//    Version 2.23      16-May-00       Programmer: Brian Gaynor
//      - Added extra precision to the accel / decel / velocity settings
//        in the servo configuration dialog
//      - New flag "MCDLG_CHECKACTIVE" added for MCDLG_ConfigureAxis()
//        and MCDLG_RestoreAxis() (checks and warns user if motor is 
//        moving before sending settings to an axis
//      - New style RDTS_DIALOGBOX replaces SS_USERITEM (wasn't unique)
//
//    Version 3.0.0      1-Mar-01       Programmer: Brian Gaynor
//      - Removed 16-bit Windows code.
//      - Added extended version of MCDLG_ControllerDesc() and 
//        MCDLG_ModuleDesc() giving program more options (name only,
//        description only, etc.)
//      - Added  MCDLG_CommandFileExt() to return API supported command
//        file (MCCL) extensions registered by the API.
//
//    Version 3.0.1      1-May-01       Programmer: Brian Gaynor
//      - No changes to this module
//
//    Version 3.0.2      6-Jun-01       Programmer: Brian Gaynor
//      - No changes to this module
//
//    Version 3.1.0     19-Sep-01       Programmer: Brian Gaynor
//      - No changes to this module
//
//    Version 3.2.0     11-Jan-02       Programmer: Brian Gaynor
//      - No changes to this module
//
//    Version 3.2.1     26-Apr-02       Programmer: Brian Gaynor
//      - No changes to this module
//
//    Version 3.3.0		15-Jan-03		Programmer: Brian Gaynor
//      - No changes to this module
//
//    Version 3.4.0		 2-Jun-03		Programmer: Brian Gaynor
//      - No changes to this module
//
//    Version 3.4.1		31-Oct-03		Programmer: Brian Gaynor
//      - No changes to this module
//
///////////////////////////////////////////////////////////////////////////

#ifndef _INC_MCDLG      // include only once
#define _INC_MCDLG

#ifdef __cplusplus      // avoid c++ name mangling
extern "C" {
#endif 


//
//  LED Window Class
//
#define LEDM_GETUNCHECKCOLOR    (WM_USER + 0x3000)
#define LEDM_GETCHECKCOLOR      (WM_USER + 0x3001)
#define LEDM_GETINDETRMCOLOR    (WM_USER + 0x3002)
#define LEDM_SETUNCHECKCOLOR    (WM_USER + 0x3003)
#define LEDM_SETCHECKCOLOR      (WM_USER + 0x3004)
#define LEDM_SETINDETRMCOLOR    (WM_USER + 0x3005)

#define MCDLG_LEDCLASS          "MCDLG_LEDWindowClass"

//
//  READOUT Window Class
//
#define RDTM_GETTEXTCOLOR       (WM_USER + 0x3010)
#define RDTM_GETBKCOLOR         (WM_USER + 0x3011)
#define RDTM_SETTEXTCOLOR       (WM_USER + 0x3012)
#define RDTM_SETBKCOLOR         (WM_USER + 0x3013)

#define RDTS_LEFT               0x0000      // same as SS_LEFT
#define RDTS_CENTER             0x0001      // same as SS_CENTER
#define RDTS_RIGHT              0x0002      // same as SS_RIGHT
#define RDTS_DIALOGBOX          0x0008      // replaces SS_USERITEM

#define MCDLG_READOUTCLASS      "MCDLG_ReadoutWindowClass"

//
//  TERMINAL Window Class (used by winctl)
//
#define TLM_RCVCHAR             (WM_USER + 0x3020)
#define TLM_PUTS                (WM_USER + 0x3021)
#define TLM_ECHOS               (WM_USER + 0x3022)
#define TLM_GETECHO             (WM_USER + 0x3023)
#define TLM_SETECHO             (WM_USER + 0x3024)
#define TLM_GETCONNECT          (WM_USER + 0x3025)
#define TLM_SETCONNECT          (WM_USER + 0x3026)
#define TLM_MAKEROOM            (WM_USER + 0x3027)
#define TLM_SETHANDLE           (WM_USER + 0x3028)

#define MCDLG_TERMINALCLASS "MCDLG_TerminalWindowClass"


//
//  MCDLG_RestoreAxis / MCDLG_SaveAxis / MCDLG_ConfigureAxis flags
//
#define MCDLG_PROMPT            0x0001      // save / restore
#define MCDLG_NOMOTION          0x0002      // save / restore / configure
#define MCDLG_NOFILTER          0x0004      // save / restore / configure
#define MCDLG_NOPHASE           0x0008      // save / restore
#define MCDLG_NOPOSITION        0x0010      // save / restore / configure
#define MCDLG_NOSCALE           0x0020      // save / restore
#define MCDLG_NOHARDLIMITS      0x0040      // configure
#define MCDLG_NOSOFTLIMITS      0x0080      // configure
#define MCDLG_NORATES           0x0100      // configure
#define MCDLG_NOPROFILES        0x0200      // save / restore / configure
#define MCDLG_NOMISC            0x0400      // configure
#define MCDLG_CHECKACTIVE       0x0800      // configure / restore


//
//  MCDLG_ControllerDesc / MCDLG_ModuleDesc flags
//
#define MCDLG_NAMEONLY          0x0001
#define MCDLG_DESCONLY          0x0002


//
//  Function prototypes
//
extern long int WINAPI  MCDLG_AboutBox(HWND hWnd, LPCSTR szTitle, long int BitmapID);
extern LPCSTR WINAPI    MCDLG_CommandFileExt(long int nType, long int nFlags, LPSTR szBuffer, long int nLength);
extern long int WINAPI  MCDLG_ConfigureAxis(HWND hWnd, HCTRLR hCtlr, WORD wAxis, long int iFlags, LPCSTR szTitle);
extern LPCSTR WINAPI    MCDLG_ControllerDesc(short int nType, LPSTR szBuffer, long int nLength);
extern LPCSTR WINAPI    MCDLG_ControllerDescEx(long int nType, long int nFlags, LPSTR szBuffer, long int nLength);
extern long int WINAPI  MCDLG_ControllerInfo(HWND hWnd, HCTRLR hCtlr, long int iFlags, LPCSTR szTitle);
extern long int WINAPI  MCDLG_DownloadFile(HWND hWnd, HCTRLR hCtlr, long int iFlags, LPCSTR szFileName);
extern long int WINAPI  MCDLG_Initialize(void);
extern long int WINAPI  MCDLG_ListControllers(short int IDArray[], short int Size);
extern LPCSTR WINAPI    MCDLG_ModuleDesc(short int nType, LPSTR szBuffer, long int nLength);
extern LPCSTR WINAPI    MCDLG_ModuleDescEx(long int nType, long int nFlags, LPSTR szBuffer, long int nLength);
extern long int WINAPI  MCDLG_RestoreAxis(HCTRLR hCtlr, WORD wAxis, long int iFlags, LPCSTR PrivateIniFile);
extern long int WINAPI  MCDLG_RestoreDigitalIO(HCTRLR hCtlr, WORD wStartChannel, WORD wEndChannel, LPCSTR PrivateIniFile);
extern long int WINAPI  MCDLG_SaveAxis(HCTRLR hCtlr, WORD wAxis, long int iFlags, LPCSTR PrivateIniFile);
extern long int WINAPI  MCDLG_SaveDigitalIO(HCTRLR hCtlr, WORD wStartChannel, WORD wEndChannel, LPCSTR PrivateIniFile);
extern long int WINAPI  MCDLG_Scaling(HWND hWnd, HCTRLR hCtlr, WORD wAxis, long int iFlags, LPCSTR szTitle);
extern short int WINAPI MCDLG_SelectController(HWND hWnd, short int CurrentID, long int iFlags, LPCSTR szTitle);

#ifdef __cplusplus
}
#endif 

#endif
