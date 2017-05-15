// GPIB.cpp
#include "stdafx.h"
//#include "globals.h"
#include "GPIB.h"
#include "decl-32.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
const char ErrorMnemonic[21][5] = 
{	"EDVR", "ECIC", "ENOL", "EADR", "EARG", "ESAC", "EABO", "ENEB", "EDMA", "",
    "EOIP", "ECAP", "EFSO", "", "EBUS", "ESTB", "ESRQ", "", "", "", "ETAB"
};

extern CString Error;
//*******************************************************************************************

void CGPIB::FreeDll (void)
{  FreeLibrary (Gpib32Lib);
   Gpib32Lib = NULL;
   return;
}

BOOL CGPIB::HaveLib(void)
{   if (!Gpib32Lib) return FALSE; else return TRUE;
}

BOOL CGPIB::HaveBoard(void)
{   if (!Gpib32Lib) return FALSE; 
	int i;
	i=(*Pibfind)("GPIB0");
	if (i!=-1) return TRUE;
	else return FALSE;
}

BOOL CGPIB::LoadDll (void)
{
   if (Gpib32Lib!=NULL) FreeDll();   
   Gpib32Lib = LoadLibrary (LPCTSTR(LibName));
   if (!Gpib32Lib)  
   {   Error="Could not load "; Error+=LibName; 
	   return FALSE; 
   }

   Pibsta  = (int *) GetProcAddress(Gpib32Lib, (LPCSTR)"user_ibsta");
   Piberr  = (int *) GetProcAddress(Gpib32Lib, (LPCSTR)"user_iberr");
   Pibcntl = (long *)GetProcAddress(Gpib32Lib, (LPCSTR)"user_ibcnt");

   Pibclr  = (int (__stdcall *)(int))GetProcAddress(Gpib32Lib, (LPCSTR)"ibclr");
   Pibdev  = (int (__stdcall *)(int, int, int, int, int, int))GetProcAddress(Gpib32Lib, (LPCSTR)"ibdev");
   Pibonl  = (int (__stdcall *)(int, int))GetProcAddress(Gpib32Lib, (LPCSTR)"ibonl");
   Pibsre  = (int (__stdcall *)(int, int))GetProcAddress(Gpib32Lib, (LPCSTR)"ibsre");
   Pibrd   = (int (__stdcall *)(int, PVOID, LONG))GetProcAddress(Gpib32Lib, (LPCSTR)"ibrd");
   Pibwrt  = (int (__stdcall *)(int, PVOID, LONG))GetProcAddress(Gpib32Lib, (LPCSTR)"ibwrt");
   Pibloc  = (int (__stdcall *)(int))GetProcAddress(Gpib32Lib, (LPCSTR)"ibloc");
   Pibsic  = (int (__stdcall *)(int))GetProcAddress(Gpib32Lib, (LPCSTR)"ibsic");
   Pibtrg  = (int (__stdcall *)(int))GetProcAddress(Gpib32Lib, (LPCSTR)"ibtrg");
   Pibfind = (int (__stdcall *)(char *))GetProcAddress(Gpib32Lib, (LPCSTR)"ibtrg");
   Pibln   = (int (__stdcall *)(int, int, int, short int*))GetProcAddress(Gpib32Lib, (LPCSTR)"ibln");

   if ((Pibsta  == NULL) || (Piberr  == NULL) || (Pibcntl == NULL) ||
       (Pibclr  == NULL) || (Pibdev  == NULL) || (Pibonl  == NULL) ||
       (Pibrd   == NULL) || (Pibwrt  == NULL) || (Pibloc  == NULL) || (Pibtrg  == NULL))  
   {	FreeDll();
		return FALSE;
   }
   else return TRUE; 
} 

CGPIB::~CGPIB() 
{	FreeDll();
}

CGPIB::CGPIB()
{	err=0x8000;
	Gpib32Lib=NULL;
	LibName="GPIB-32.DLL";
	LoadDll();
}

void CGPIB::End(int &Dev)
{   if (Gpib32Lib==NULL || Dev !=-1) 
	{	(*Pibloc) (Dev);
		(*Pibonl) (Dev, 0);
	}
	Dev=-1;
}

void CGPIB::GPIBEnd_E(int Dev, char* ErrorMsg)
{	
	char *name,*tx;
	char *EExpl[8]={"DOS err","Function req. GPIB to be CIC","Write handshake err. (No Listener)",
					"GPIB board incorrectly addressed","Invalid argument","GPIB board not System Controller",
					"I/O op aborted (Timeout)","Non-existent GPIB board"};
	
//	if (Dev==The.dev) name="Therm";
//	else if (Dev==Phc.dev) name="SR400";
//	else name="Unknown";
	name="SR400";
	if ((*Piberr)<8)	tx=EExpl[(*Piberr)]; 
	else			tx=(char *)ErrorMnemonic[(*Piberr)];
	Error.Format(L"GPIB(%s) Error : %s\r\nibsta = 0x%x iberr = %d (%s)", 
				 name, ErrorMsg, *Pibsta, *Piberr, tx);
}
					
			// Not so bad coding any more (TR)
BOOL CGPIB::Clear(int Dev)			// SR400 clear was catastrophic to the board - REN line was wrong way, dont know why
{									// It was different for other GPIB board	
	if (Gpib32Lib==NULL || Dev==-1) return TRUE;		// Done nothing
/*
	if (Dev==The.dev)
	{	(*Pibclr) (Dev);
		if ((*Pibsta) & err) 	
		{	GPIBEnd_E(Dev, "Cannot reset device");return FALSE;}
		return TRUE;
	}
*/
//	if (Dev==Phc.dev)
//	{
		(*Pibsic)(0);					// Clear interface
		(*Pibsre)(0,0);					// Flush previous fuckups
		(*Pibsre)(0,1);					// 
		(*Pibwrt) (Dev, "CL;", 3L);	// Clear PhC
		if ((*Pibsta) & err)			
		{	GPIBEnd_E(Dev, "Unable to write cl; to SR400");return  FALSE;}
		Sleep(100);						// Wait 100 ms
		(*Pibsic)(0);					// Flush REN, clear Interface
		(*Pibsre)(0,1);
//	}
	return TRUE;
}

BOOL CGPIB::Detect(GPIBdev *p)
{	short int listen;
	if (!Gpib32Lib) return FALSE;
	(*Pibln)(p->board, p->pad, p->sad, &listen);
	if (listen) {p->started=1; return TRUE;} 
	else		{p->started=0; return FALSE;}
}

//	DefArguments:(,0,FALSE,FALSE)
BOOL CGPIB::Start(GPIBdev *p,char *s,BOOL id, BOOL detectonly)
{	
	BOOL det=FALSE;
	if (s!=NULL)						// Get Channel name
	{
		int j=strlen(s);
		if (j<20)	strcpy(p->name,s);
		else		memmove(p->name,s,19); p->name[19]=0;
	}
	if (Gpib32Lib==NULL) return FALSE;
	if (p->dev!=-1 || p->started==2) End(p->dev);		// If device is started turn it off
    p->dev = (*Pibdev) (p->board, p->pad, p->sad, p->tmo, p->eot, p->eos);
    if ((*Pibsta) & err)
	{	p->started=0;					// Device not present or did not start
		p->dev=-1;
		return FALSE;
	}
	det=Detect(p);						// Commented line did work the opposite of my thinking 
//	if (id && det==TRUE)				// The:det=0; Phc:det=1 -> Result: both appeared offline ???
	if (id)				// ibdev starts always, Detect verifies is it online
	{	strcpy(p->idn,"*IDN?");			// If Detect returns FALSE, device is probably OFF
		if (!(RW(p->dev,p->idn,80)))	// No Detect results in IBSTA errors here
			strcpy(p->idn,"No ID response");	
	}
	if (detectonly)	
	{	End(p->dev);
		p->started=0;
	}
	return TRUE;
}

//	Typical usage: GPIB.Start(Therm,12);  GPIB.Start(SR400, 2);
//	DefArguments:(,,0,0,10,1,0,FALSE)
BOOL CGPIB::Start(int &dev, int pad, int board, int sad, int tmo, int eot, int eos, BOOL detectonly)
{	
//	if (Emulate) return TRUE;
	if (Gpib32Lib==NULL) return FALSE;
	if (dev!=-1) End(dev);				// If device is started turn it off
    dev = (*Pibdev) (board, pad, sad, tmo, eot, eos);
    if ((*Pibsta) & err)
	{	dev=-1; return FALSE;
	}
	if (detectonly)	End(dev);
	return TRUE;
}

BOOL CGPIB::RW(int Dev, char *s, int len)
{
//	if (Emulate) return TRUE;
	if (Gpib32Lib==NULL || Dev==-1) return FALSE;
    (*Pibwrt) (Dev, s, strlen(s));
    if ((*Pibsta) & err)
    {	GPIBEnd_E(Dev, "Unable to write");return FALSE;}
    (*Pibrd) (Dev, s, len);
    if ((*Pibsta) & err)
    {	GPIBEnd_E(Dev, "Unable to read");return FALSE;}
	return TRUE;
}

BOOL CGPIB::W(int Dev, char *s)
{//	if (Emulate) return TRUE;
	if (Gpib32Lib==NULL || Dev==-1) return FALSE;
    (*Pibwrt) (Dev, s, strlen(s));
    if ((*Pibsta) & err)
    {	GPIBEnd_E(Dev, "Unable to write");return FALSE;}
	return TRUE;
}

BOOL CGPIB::R(int Dev, char *s, int len)
{	s[0]=0;
//	if (Emulate) {return TRUE;}
	if (Gpib32Lib==NULL || Dev==-1) return FALSE;
    (*Pibrd) (Dev, s, len);
    if ((*Pibsta) & err)
	{	if ((*Piberr) & EABO)  return TRUE;			 // Timeout on read is not really an error
		GPIBEnd_E(Dev, "Unable to read");return FALSE;}
	return TRUE;
}

BOOL CGPIB::Timeout()								// Timeout on read is not really an error
{   if (!Gpib32Lib) return FALSE;
	if ((*Piberr) & EABO)  return TRUE; else return FALSE;
}

void GPIBdev::Serialize(CArchive& ar)			
{
	int i;
	if (ar.IsStoring())			// We store Globals here, all App class members are temporary
	{	ar << board << dev << pad << sad << tmo << eot << eos << started;
		for (i=0;i<20;i++) ar << name[i];
		for (i=0;i<80;i++) ar << idn[i];
	}
	else
	{	ar >> board >> dev >> pad >> sad >> tmo >> eot >> eos >> started;
		for (i=0;i<20;i++) ar >> name[i];
		for (i=0;i<80;i++) ar >> idn[i];
	}
}

void CGPIB::Serialize(CArchive& ar)			
{
	if (ar.IsStoring())	
	{	ar << LibName;
	}
	else
	{	ar >> LibName;
	}
}




