// GPIB.h
#ifndef _GPIB_H_
#define _GPIB_H_
#include "decl-32.h"
/////////////////////////////////////////////////////////////////////////////
struct	GPIBdev
{
public:
	char	name[20];
	char	idn[80];
	int		board,dev,pad,sad,tmo,eot,eos,started;
	GPIBdev() {dev=-1;board=0;pad=2;sad=0;tmo=10;eot=1;eos=0;started=0;name[0]=idn[0]=0;}
	virtual void Serialize(CArchive& ar);			// All doubles, just in case
};

class CGPIB 
{
public:
	virtual ~CGPIB();
	CGPIB();
	BOOL	RW(int Dev, char *s, int len);
	BOOL	R(int Dev, char *s, int len);
	BOOL	W(int Dev, char *s);
	BOOL	Timeout();						// did we encounter timeout?
	BOOL	Clear(int Dev);					// SR400 clear was catastrophic to the board 
	BOOL	Start(GPIBdev *pdev,char *s=0,BOOL id=FALSE, BOOL detonly=FALSE);
	BOOL	Start(int &Dev, int pad, int board=0, int sad=0, int tmo=10, int eot=1, int eos=0, 
				  BOOL detectonly=FALSE);
	void	End(int &Dev);
	BOOL	HaveLib(void);					// Did GPIB lib load
	BOOL	HaveBoard(void);				// Do we have GPIB board
	int		GetStatus() {if (Pibsta==NULL) return NULL; else return *Pibsta;}
	BOOL	Detect(GPIBdev *p);				// Is the device online, sets p->started
	CString	LibName;						// Library name
	BOOL	LoadDll();
	virtual void Serialize(CArchive& ar);
protected:
	int		err;
	HINSTANCE Gpib32Lib;
	void	FreeDll();
	void	GPIBEnd_E(int ud, char* ErrorMsg);
private:
	int  *Pibsta;
	int  *Piberr;
	long *Pibcntl;
	int (__stdcall *Pibclr)(int ud);
	int (__stdcall *Pibdev)(int ud, int pad, int sad, int tmo, int eot, int eos);
	int (__stdcall *Pibonl)(int ud, int v);
	int (__stdcall *Pibsre)(int ud, int v);
	int (__stdcall *Pibloc)(int ud);
	int (__stdcall *Pibsic)(int ud);
	int (__stdcall *Pibtrg)(int ud);
	int (__stdcall *Pibrd)(int ud, PVOID buf, LONG cnt);
	int (__stdcall *Pibwrt)(int ud, PVOID buf, LONG cnt);
	int (__stdcall *Pibfind)(char *name);
	int (__stdcall *Pibln)(int ud, int pad, int sad, short int *listen);
};

#endif _GPIB_H_
