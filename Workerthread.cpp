// WorkerThread.cpp: 
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "math.h"
#include "motors.h"
#include "WorkerThreadInfo.h"
#include "MainFrm.h"
#include "MCcard.h"
#include "motorsdoc.h"
#include "motorsview.h"
#include "dumpview.h"
#include "NIDAQmx.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "master.h"
#include "pvcam.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif 
extern CMotorsApp theApp;
extern int Working;
CWorkerThreadInfo* pInfo;
CMainFrame *M;

//////////////////////////////////////////////////////////////////////////

void	Curse() 
{
	Beep(1000,1000);
		
}

int KbHit()
{
	MSG msg;
	msg.wParam=0;
	::PeekMessage(&msg,0,WM_KEYFIRST,WM_KEYLAST,PM_NOREMOVE);
	return (int)msg.wParam;
}

int Getch()
{
	MSG msg;
	msg.wParam=0;
	::GetMessage(&msg,0,WM_KEYFIRST,WM_KEYLAST);
	return (int)msg.wParam;
}

BOOL IsBroken()
{
	BOOL got=FALSE;
	WPARAM wParam;
	while (KbHit())							// Check is there a key available
	{										// First, getch checks and sets Break
		wParam=Getch();						// Second, empties KB from spurious messages 
		if (wParam==VK_ESCAPE) got=TRUE;
	}
	return got;
}

//////////////////////////////////////////////////////////////////////////
//	Set PhCtimers
void CheckTimers(double &cht)					// Get canonical values for timer
{
	double e;
	char s[40];
	sprintf(s,"%1.0E",cht*1e7);
	e=atof(s);
	cht=e*1e-7;
	if (cht<0.030) cht=0.030;
	if (cht>60.0)  cht=60.0;
}

BOOL WaitforReady()					// 
{	
	int i,j;
	i=(int)(ChT*1e3);								// Soft channel time
	j=i/10;
	if (i%10) j++;
	for (i=0;i<j;i++)
	{
		Sleep(10);	
		if (Break || IsBroken()) 
		{
			Message="Soft Broken";
			Sbar|=1;
			Break=1;
			return FALSE;
		}
	}
	return TRUE;
}

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
////////DETECT//////////////////////DETECT
//
BOOL Detect1(double &data)				// Detector; this subroutine talks to the counter board
//returns true or false depending on if counting successful or not, and counter reading is stored in "data"
{
#ifdef EMULATE //does that when program is told to pretend collecting data
	retc = WaitforReady();
	data = rand(); //random number
#else
	int         error = 0;
	uInt32      data1;
	TaskHandle  taskHandle = 0;
	char        errBuff[2048] = { '\0' };
	DAQmxErrChk(DAQmxCreateTask("", &taskHandle)); // DAQmx Configure Code
	DAQmxErrChk(DAQmxCreateCICountEdgesChan(taskHandle, "Dev1/ctr0", "", DAQmx_Val_Rising, 0, DAQmx_Val_CountUp));
	// DAQmx Start Code
	//counting starting from zero up, rising edges, using device Dev1/ctr0
	DAQmxErrChk(DAQmxStartTask(taskHandle)); //starts DAQmx task
	Sleep(int(ChT*1000.0));	//argument of Sleep is integer number of milliseconds
	DAQmxErrChk(DAQmxReadCounterScalarU32(taskHandle, 1.0, &data1, NULL)); // DAQmx Read Code
	//DAQmxReadCounterScalarU32 (TaskHandle taskHandle, float64 timeout, uInt32 *value, bool32 *reserved);
	data = double(data1);
	DAQmxStopTask(taskHandle);
	DAQmxClearTask(taskHandle);
	return(1);

Error:
	puts("");
	if (DAQmxFailed(error))DAQmxGetExtendedErrorInfo(errBuff, 2048);
	if (taskHandle != 0) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
	}//end if
	return(0); //as original program did...
#endif EMULATE
}//end detect
//////////////////////////////////////
//Kinetics example from Princeton Instruments site
//static void AcquireStandard( int16 hCam );

BOOL Detect2(double &data, uns16 Cam, uns32 sze) //detection using ROI of the CCD
{
	//Start the acquisition. Camera is initialized upstream
	uns32 not_needed;
	int16 status;
	uns32 i; //cycle variable
	uns16 *frame;
	frame = (uns16*)malloc(sze); //meaning it makes a one-dimensional array of uns16
	pl_exp_start_seq(Cam, frame); // take a frame
	/* wait for data or error */
	while (pl_exp_check_status(Cam, &status, &not_needed) &&
		(status != READOUT_COMPLETE && status != READOUT_FAILED));
	/* Check Error Codes */
	if (status == READOUT_FAILED) {
		printf("CCD Data collection error: %i\n", pl_error_code());
		return (0);
	}
	data = 0;
	//uns16 a[1000]; was here for debugging purposes only
	for (i = 0; i<sze / 2 - 1; i++) //for some odd reason, size is 2N+2 where N is total number of points in the ROI
	{  //only the first N elements contain meaningful data
		//a[i]=frame[i];
		data = data + frame[i]; //software binning
	}
	/* Finish the sequence */
	data = data - background; //taking background correction, implies there is some background
	pl_exp_finish_seq(Cam, frame, 0);
	free(frame);
	return (1); //good - return one
}



//////////////////////////////////////////////////////////////////////////
//	PCMCard itself is thread safe
//	It sets the CString Error in case there is an error
//	We want to read down & make local all variables from the Card and both Motors
//////////////////////////////////////////////////////////////////////////
void RasterScan()
{
	CString Ms;
	int what, steps;
	int j, jj, ff, ft, fs, fastmotor, pos_chan;
	double d, u, v;
	CMainFrame *M = (CMainFrame *)pApp->m_pMainWnd;
	Break = FALSE;
	rgn_type region = { s1, s2, 1, p1, p2, 1 }; // s- horizontal limits, p-vertical limits, ones refer to no hardware binning
	//software binning allows collecting up to 2^16 from each pixel of ROI.
	uns32 size; // will be figured out based on parameters of the region

	if (det == 1) //if we are using CCD camera as a detector - initializing the camera
	{
		pl_pvcam_init(); // Initialize the PVCam Library and Open the First Camera */
		pl_cam_get_name(0, cam_name);
		pl_cam_open(cam_name, &hCam, OPEN_EXCLUSIVE);
		pl_exp_init_seq();
		pl_exp_setup_seq(hCam, 1, 1, &region, TIMED_MODE, int(ChT*1000.0), &size);
		//camera, number of images to take, numbers of regions to take, region description, mode, exposure time (ms), size is filled
	} //end of CCD initialization, we have camera handle and size determined
	//
	if (points==0) { //gutted Rasterscan, only one axis left
	//////////////////////////////////////////////////////////////////////////
	//	Localize all motor parameters 
	fastmotor = FastMotor;
	ff = pCard->Th[fastmotor].SweepFrom;	//these are integer motor steps
	ft = pCard->Th[fastmotor].SweepTo;
	fs = pCard->Th[fastmotor].SweepStep;
	//
	if (ff < start_pos) baseline_measured = FALSE; //turning off baseline correction if baseline is wrong
	if (ft>end_pos) baseline_measured = FALSE;
	if (fs != step_pos) baseline_measured = FALSE;
	if (baseline_measured == FALSE) Message = "No Proper Baseline Available";
	if ((ft - ff)<fs || fs<1)
	{
		Error = "Scan:To/From/Step conflict"; return;
	}
		if (ft<ff && fs>0) fs = -fs;
		jj = (int)ceil(fabs((ft - ff) / fs + .1)); //number of fast axis points
	//////////////////////////////////////////////////////////////////////////
	CheckTimers(ChT);
	pCard->GetCurPos(u, fastmotor);
	pCard->GotoNewPos(ff - fs, fastmotor);
	pCard->WaitonMotor();								// Wait until both motors are stopped			
	pDoc->Curves = 1; //this is not real rasterscan, there is one curve at a time,  with scanning one motor
	sCurves.Format(L"of:%d", pDoc->Curves);
	M->SendMessage(WM_USER_ANNOUNCE, 0x210);				// Stop FrameWnd timer ??
	steps = ((ft - ff) / fs + 1); // another incarnation of fast axis steps?
	pDoc->Spc.SetSize(pDoc->Curves);
	what = 0; //just one spectrum at a time
		pDoc->ActSpec = what;
		pDoc->Spc[what].CleanCreate(steps, 8, 8, (double)ff, (double)ft);
		sActCurve.Format(L"Curve:%d", what + 1);
		M->SendMessage(WM_USER_ANNOUNCE, 0xc);
		CheckTimers(ChT);
		//Ms.Format(L"Motors:(RasterScan) %d [%d of %d]", sf + i*ss, what + 1, pDoc->Curves);
		pDoc->Spc[what].SetComment((char *)LPCTSTR(Ms));
		for (j = 0; j < jj; j++)			// Fast Motor is the only one used
		{
			pos_chan = ff + j*fs; //position in channels
			if (Break) break;
			pCard->WaitonMotor(fastmotor);
			pCard->GotoNewPos(pos_chan, fastmotor);
			if (det == 1) Detect2(d, hCam, size);
			else 	Detect1(d);	//NB: here it asks counter; after this point "data" and "d" contain counter reading.
			if (baseline)
			{
				pDoc->Spc[what].SetPoint(pos_chan*c2 + c1, d, j); //converted to nanometers, transmission signal
				base[pos_chan] = d;
			}
			if (calibration) pDoc->Spc[what].SetPoint(pos_chan, d, j); //not converted to nanometers
			if (scan) 
			{
				if (baseline_measured)	d = log10(base[pos_chan]/d); //converted to absorbance if baseline is available
				pDoc->Spc[what].SetPoint(pos_chan*c2 + c1, d, j); //converted to nanometers, 
			}
			PostMessage(pZoom->m_hWnd, WM_USER_LOAD, what, j);		// Draw a point, update statusbar positions
			}
		pCard->WaitonMotor(fastmotor);
		pCard->GotoNewPos(ff - fs, fastmotor); //returning the fast motor to where it started approximately?
		PostMessage(pDump->m_hWnd, WM_USER_LOAD, what, -1);			// Draw a spectrum
	pCard->GotoNewPos(u, fastmotor);
	if (Break)
	{
		pDoc->Curves = what;
		pDoc->Spc.SetSize(pDoc->Curves);
	}
	Working = FALSE;
	pApp->m_pMainWnd->PostMessage(WM_USER_ANNOUNCE, 0x900);			// Start FrameWnd timer, announce that load is done
	Break = FALSE;
	if (baseline)
	{
		baseline_measured = TRUE; //baseline becomes available AFTER the baseline scan is successfully performed.
		start_pos = ff; // saving beginning and end and step of the region for which baseline is available
		end_pos = ft;
		step_pos = fs;
	}
	} //end of if points=0; Classic rasterscan
	//////////////////////////////////////////////////////////////////////////
	else if (points>0) { //Kinetics or adjustment
	pDoc->Curves = 1;
		sCurves.Format(L"of:%d", pDoc->Curves);
		M->SendMessage(WM_USER_ANNOUNCE, 0x210);				// Stop FrameWnd timer ??
		//number of points in one scan
		pDoc->Spc.SetSize(pDoc->Curves);
		CheckTimers(ChT);
		what = 0;
		pDoc->ActSpec = what;
		pDoc->Spc[what].CleanCreate(points, 8, 8, 0.0, (double)points);
		for (j = 0; j<points; j++)			// Fast Forward
		{
			if (Break) break;
			if (det == 1) Detect2(d, hCam, size);
			else 	Detect1(d);   //NB: here it asks counter; after this point "data" and "d" contain counter reading.
			pDoc->Spc[what].SetPoint(j*ChT, d, j);
			PostMessage(pZoom->m_hWnd, WM_USER_LOAD, what, j);
			// Draw a point in zoom window, update statusbar positions
		}         //NB: here it asks counter; after this point "data" and "d" contain counter reading.
		PostMessage(pDump->m_hWnd, WM_USER_LOAD, what, -1);
		// Draw a spectrum to dump
		if (Break)
		{
			pDoc->Curves = what;
			pDoc->Spc.SetSize(pDoc->Curves);
		}
		Working = FALSE;
		pApp->m_pMainWnd->PostMessage(WM_USER_ANNOUNCE, 0x900);			// Start FrameWnd timer, announce that load is done
		Break = FALSE;
		what = what + 1;
	} //end of if points>0
	if (det == 1) //using CCD camera as detector, deinitializing sequence. Camera is deinitialized upon exit or upon switching 
		//to another detector
	{
		/*Uninit the sequence */
		pl_exp_uninit_seq();
	} //end of deinitializing the CCD sequence
}

//////////////////////////////////////////////////////////////////////////

BOOL Work()								// Thread has been started
{
	Working=TRUE;					
	switch(pInfo->Operation)			// pInfo is Global for simplicity
	{
		default: break;		// only HGLOAD is time-critical
		case CURSE:			Curse();					break;
		case RASTERSCAN:	RasterScan();				break;
	}
	Working=FALSE;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////****************    Worker Thread Maintenance    *****************///////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

UINT WorkerThreadProc(LPVOID pParam)
{	
	pInfo = (CWorkerThreadInfo*)pParam;
	BOOL bCompleted;
	Worker=TRUE;								// This thread is the Worker thread
	while (TRUE)								// Infinite loop, Out only with kill Signal 
	{	bCompleted = FALSE;
		if (WaitForSingleObject(pInfo->m_hEventStart, INFINITE)	!= WAIT_OBJECT_0) break;
		if (WaitForSingleObject(pInfo->m_hEventKillThread, 0)	== WAIT_OBJECT_0) 
		{Killed:	
			SetEvent(pInfo->m_hEventThreadKilled); return 0;	 
		}										// Check once for the AXE
		if (WaitForSingleObject(pInfo->m_hEventStopThread, 0)	== WAIT_OBJECT_0) 
		{Stopped:	
			SetEvent(pInfo->m_hEventDone);
			SetEvent(pInfo->m_hEventThreadStopped); // Designate us as stopped
			ResetEvent(pInfo->m_hEventStopThread);	// Manual Reset otherwise never gets here
			ResetEvent(pInfo->m_hEventStart);
			continue;							// Fall asleep, please.
		}
		ResetEvent(pInfo->m_hEventDone);		// set this event to "not done"
		M=(CMainFrame *)(theApp.m_pMainWnd);
		Sbar|=0x400;							// Let them know we started
		bCompleted = Work();
//		Break=Pause=Tweak=0;					// All break globals die here	
		SetEvent(pInfo->m_hEventDone);			// set event done
		if (WaitForSingleObject(pInfo->m_hEventStopThread, 0) == WAIT_OBJECT_0) goto Stopped;	
		if (WaitForSingleObject(pInfo->m_hEventKillThread, 0) == WAIT_OBJECT_0) goto Killed;	
		if (!bCompleted) goto Stopped;			// In case of errors designate thread to be stopped
	}
	return 0;									// As soon as we return thread is killed
}



