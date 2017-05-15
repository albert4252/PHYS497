; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CCommandView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "motors.h"
LastPage=0

ClassCount=10
Class1=CCommandView
Class2=CDumpView
Class3=CInitDlg
Class4=CMainFrame
Class5=CMotorsApp
Class6=CAboutDlg
Class7=CMotorsDoc
Class8=CMotorsView
Class9=CPositionDlg
Class10=CSaveDlg

ResourceCount=6
Resource1=IDD_STEPPERS_DIALOG
Resource2=IDR_MAINFRAME
Resource3=IDD_POSITION
Resource4=IDD_ABOUTBOX
Resource5=IDD_INIT
Resource6=IDD_SAVEDLG

[CLS:CCommandView]
Type=0
BaseClass=CFormView
HeaderFile=CommandView.h
ImplementationFile=CommandView.cpp
LastObject=IDC_FASTMOTOR
Filter=D
VirtualFilter=VWC

[CLS:CDumpView]
Type=0
BaseClass=CView
HeaderFile=DumpView.h
ImplementationFile=DumpView.cpp

[CLS:CInitDlg]
Type=0
BaseClass=CDialog
HeaderFile=InitDlg.h
ImplementationFile=InitDlg.cpp

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp

[CLS:CMotorsApp]
Type=0
BaseClass=CWinApp
HeaderFile=Motors.h
ImplementationFile=Motors.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=Motors.cpp
ImplementationFile=Motors.cpp

[CLS:CMotorsDoc]
Type=0
BaseClass=CDocument
HeaderFile=MotorsDoc.h
ImplementationFile=MotorsDoc.cpp

[CLS:CMotorsView]
Type=0
BaseClass=CView
HeaderFile=MotorsView.h
ImplementationFile=MotorsView.cpp

[CLS:CPositionDlg]
Type=0
BaseClass=CDialog
HeaderFile=PositionDlg.h
ImplementationFile=PositionDlg.cpp

[CLS:CSaveDlg]
Type=0
BaseClass=CDialog
HeaderFile=SaveDlg.h
ImplementationFile=SaveDlg.cpp

[DLG:IDD_STEPPERS_DIALOG]
Type=1
Class=CCommandView
ControlCount=51
Control1=IDC_EDIT_GOTO1,edit,1350631552
Control2=IDC_SFROM,static,1342308354
Control3=IDC_STO,static,1342308354
Control4=IDC_SSTEP,static,1342308354
Control5=IDC_EDIT_TO1,edit,1350631552
Control6=IDC_EDIT_FROM1,edit,1350631552
Control7=IDC_EDIT_STEP1,edit,1350631552
Control8=IDC_STATIC,static,1342308354
Control9=IDC_EDIT_POS1,edit,1350633600
Control10=IDC_RASTERSCAN,button,1342251008
Control11=IDC_STATIC,static,1342308352
Control12=IDC_EDIT_CHT,edit,1350566016
Control13=IDC_GO1,button,1342242816
Control14=IDC_MOTOR,button,1342242816
Control15=IDC_RSCAN,button,1342177287
Control16=IDC_FASTMOTOR,button,1342242819
Control17=IDC_P1,button,1342242816
Control18=IDC_P10,button,1342242816
Control19=IDC_P100,button,1342242816
Control20=IDC_P1000,button,1342242816
Control21=IDC_M1,button,1342242816
Control22=IDC_M10,button,1342242816
Control23=IDC_M100,button,1342242816
Control24=IDC_M1000,button,1342242816
Control25=IDC_AUTONORM,button,1342242819
Control26=IDC_EDIT_YMIN,edit,1350631552
Control27=IDC_EDIT_YMAX,edit,1350631552
Control28=IDC_STATIC,static,1342308354
Control29=IDC_SHOW,button,1342242819
Control30=IDC_SAVE,button,1342242816
Control31=IDC_STATIC,static,1342308354
Control32=IDC_EDIT_XMIN,edit,1350631552
Control33=IDC_EDIT_XMAX,edit,1350631552
Control34=IDC_STATIC,static,1342308354
Control35=IDC_STATIC,static,1342308354
Control36=IDC_XGRID,button,1342242819
Control37=IDC_YGRID,button,1342242819
Control38=IDC_XYNORM,button,1342242816
Control39=IDC_YNORM,button,1342242816
Control40=IDC_XNORM,button,1342242816
Control41=IDC_REDRAW,button,1342242816
Control42=IDC_STATIC,button,1342177287
Control43=IDC_ZERONORM,button,1342242819
Control44=IDC_P10000,button,1342242816
Control45=IDC_M10000,button,1342242816
Control46=IDC_GET,button,1342242816
Control47=IDC_SET,button,1342242816
Control48=IDC_SETUP,button,1342242816
Control49=IDC_SAVEALL,button,1342242816
Control50=IDC_RESTORE,button,1342242816
Control51=IDC_MOTORTAB,SysTabControl32,1342177280

[DLG:IDD_INIT]
Type=1
Class=CInitDlg
ControlCount=7
Control1=IDOK,button,1342242817
Control2=IDC_EDIT_SS1,edit,1350631552
Control3=IDC_EDIT_CS1,edit,1350631552
Control4=IDC_EDIT_ACC1,edit,1350631552
Control5=IDC_STATIC,static,1342308354
Control6=IDC_STATIC,static,1342308354
Control7=IDC_STATIC,static,1342308354

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_POSITION]
Type=1
Class=CPositionDlg
ControlCount=15
Control1=IDOK,button,1342242817
Control2=IDC_EDIT_GOTO1,edit,1350631552
Control3=IDC_EDIT_POS1,edit,1350633600
Control4=IDC_STATIC,static,1342308354
Control5=IDC_STATIC,static,1342308354
Control6=IDC_EDIT_HORZL,edit,1350631552
Control7=IDC_EDIT_HORZR,edit,1350631552
Control8=IDC_SET3,button,1342242816
Control9=IDC_READ1,button,1342242816
Control10=IDC_STATIC,static,1342308354
Control11=IDC_GO1,button,1342242816
Control12=IDC_PARK1,button,1342242816
Control13=IDC_INIT1,button,1342242816
Control14=IDC_EDIT_MOVE,edit,1350633600
Control15=IDC_STATIC,static,1342308354

[DLG:IDD_SAVEDLG]
Type=1
Class=CSaveDlg
ControlCount=16
Control1=IDOK,button,1342242817
Control2=IDC_DIR_BROWSE,button,1342242816
Control3=IDC_EDIT_FILE,edit,1350566016
Control4=IDC_EDIT_DIR,edit,1350566016
Control5=IDC_FILE_BROWSE,button,1342242816
Control6=IDC_STATIC,static,1342308352
Control7=IDC_EDIT_SPECTRA,edit,1350633600
Control8=IDC_SAVE,button,1342242816
Control9=IDC_STATIC,static,1342308352
Control10=IDC_EDIT_CURVES,edit,1350633600
Control11=IDC_EDIT_START,edit,1350631552
Control12=IDC_STATIC,static,1342308352
Control13=IDC_EDIT_AMOUNT,edit,1350631552
Control14=IDC_STATIC,static,1342308352
Control15=IDC_TRUNCATE,button,1342242816
Control16=IDC_REFRESH,button,1342242817

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_APP_EXIT
Command2=ID_VIEW_STATUS_BAR
Command3=ID_APP_ABOUT
CommandCount=3

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

