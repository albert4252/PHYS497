#include "stdafx.h"
#include "math.h"
#include "MCcard.h"
#include "resource.h"
#include "CommandView.h"

MCcard::MCcard()
{	mcb=0; Motors=0;
}

MCcard::~MCcard()
{
	CloseThor();
}

extern int Sbar;

BOOL MCcard::GetError(int err,char *s)
{	
	char st[256];
	MCTranslateErrorEx(err,st,128);
	Error=s; 
	if (!Error.IsEmpty()) Error+=_T(":");
	Error+=st;
	Sbar=2;
	return TRUE;
}


BOOL MCcard::ObtainInfo(int i)
{	
	int res;
	if (!mcb || Motors<1 || i<0 || i>=Motors) return FALSE;			// No cards or no motors
	Th[i].FLT.cbSize=sizeof(MCFILTEREX);
	res=MCGetFilterConfigEx(mcb,i+1,&Th[i].FLT);		// NB! The use of axis=0 means AllAxes	
	if (res)										//	   But it cannot be done anywhere here	
	{	GetError(res,"GetFilterConfig");
		return FALSE;								// If we fail we cannot park etc.
	}
	Th[i].MTE.cbSize=sizeof(MCMOTIONEX);
	res=MCGetMotionConfigEx(mcb,i+1,&Th[i].MTE);
	if (res)
	{	GetError(res,"GetMotionConfigEx");		
		return FALSE;
	}
	Th[i].ACF.cbSize=sizeof(MCAXISCONFIG);
	res=MCGetAxisConfiguration(mcb,i+1,&Th[i].ACF);	
	if (res)
	{	GetError(mcb,"GetAxisConfig");
		return FALSE;
	}
	return TRUE;
}

BOOL MCcard::CreateThor()
{	
	int res,i;
	mcb=MCOpen(0,MC_OPEN_BINARY | MC_OPEN_EXCLUSIVE,0);	// Open card driver	
	if (mcb<0) 
	{	GetError(-mcb,"MCOpen");
		return FALSE;
	}
	MCP.cbSize=sizeof(MCP);
	res=MCGetConfigurationEx(mcb,&MCP);					// Get Card configuration
	if (res)
	{	GetError(mcb,"GetConfigurationEx");
		return FALSE;
	}
	Motors=MCP.NumberAxes;								// Motors is valid for all runtime
	//	Read configuration in from specified file if found & applicability verified
	res=MCDLG_RestoreAxis(mcb,MC_ALL_AXES,0,"Thor_save.ini");
	if (res)
	{	GetError(res,"RestoreAxis:Thor_save.ini load failed");
	}
	for (i=0;i<Motors;i++)								// Read in filter data, acc vel, pid
		if (!ObtainInfo(i)) return FALSE;
	return TRUE;
}

BOOL MCcard::CloseThor()
{	
	int res;
	if (mcb>0)
	{
		res=MCDLG_SaveAxis(mcb,MC_ALL_AXES,0,"Thor_save.ini");
		if (res)
		{	GetError(res,"SaveAxis");
		}
		if (MCClose(mcb)) 	
		{	GetError(mcb,"MCClose");
			return FALSE;
		}
		mcb=0;
	}
	return TRUE;
}

//	Get current motor position; if motor argument missing get all positions
BOOL MCcard::GetCurPos(double &pos,int motor)
{
	int res,i;
	double d;
	if (!mcb || Motors<1) return FALSE;			// No cards or no motors
	if (motor==-1) 
	{	
		for (i=0;i<Motors;i++)					// Do for all motors
			if (GetCurPos(Th[i].CurPos,i))	return FALSE;
		return TRUE;
	}
	res=MCGetPositionEx(mcb,motor+1,&d);		// Reports error number or 0 for success
	if (res)
	{	GetError(res);
		return FALSE;
	}
	else Th[motor].CurPos=pos=d;				// Stores found position in ThorMotor and in pos
	return TRUE;
}
		
BOOL MCcard::SetCurPos(double pos,int motor)	// Fix a value to the current position of motor  
{
	if (!mcb || Motors<1) return FALSE;			// No cards or no motors
	if (motor<-1 || motor>=Motors) motor=-1;	// Motor -1 -> Set position for all motors
	MCSetPosition(mcb,motor+1,pos);				
	return TRUE;
}

BOOL MCcard::GotoNewPos(double pos,int motor,BOOL usetmo)
{
	double d;
	if (!mcb || Motors<1) return FALSE;
	if (motor==-1) motor=ActMotor;
	if (!GetCurPos(d,motor)) return FALSE;
	return MoveRelative(pos-d,motor,usetmo);
}

BOOL MCcard::MoveRelative(double amount,int motor,BOOL usetmo)
{
	int res;
	double v,a,timo;
	if (!mcb || Motors<1) return FALSE;
	if (motor==-1) motor=ActMotor;
	a=Th[motor].MTE.Acceleration;  if (a<10 || a>1e9) a=5e6;
	v=Th[motor].MTE.Velocity;		if (v<10 || v>1e9) v=1e4;
	if (usetmo) timo=fabs(amount/v)+2*sqrt(fabs(amount/a))+.5;
	else timo=0;
	MCEnableAxis(mcb,motor+1,TRUE);
	MCMoveRelative(mcb,motor+1,amount);
	res=MCIsStopped(mcb,motor+1,timo);
	if (usetmo) MCEnableAxis(mcb,motor+1,FALSE);
	return TRUE;
}

BOOL MCcard::Stop()								// Unconditional off to all motors
{
	if (!mcb || Motors<1) return FALSE;
	MCEnableAxis(mcb,MC_ALL_AXES,FALSE);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//	We turn off soft limits & enable hard limits permanently &
//	Set Acceleration & Velocity to their max values
BOOL MCcard::Reconf()					
{												// Returns True if GetMotionConfigs succeeded
	int i,res;							
	double v,a;								
	if (!mcb || Motors<1) return FALSE;			
	for (i=0;i<Motors;i++)
	{
		res=0;
		Th[i].MTE.cbSize=Th[i].tMTE.cbSize=sizeof(MCMOTIONEX);
		res=MCGetMotionConfigEx(mcb,i+1,&Th[i].tMTE);	// If res>0 we cannot get old values back
		if (res)
		{	GetError(res,"Abort in Reconf:GetMotionConfigEx");
			return  FALSE;					// Just fail
		}
		memmove(&Th[i].MTE,&Th[i].tMTE,sizeof(MCMOTIONEX));
		Th[i].tMTE.SoftLimitMode=0;		
		Th[i].tMTE.HardLimitMode=MC_LIMIT_MINUS|MC_LIMIT_PLUS|MC_LIMIT_ABRUPT;	
		a=Th[i].tMTE.Acceleration;
		v=Th[i].tMTE.Velocity;
		if (a<10 || a>1e7) a=5e7;	
		if (v<10 || v>1e5) v=1e4;
		res=MCSetMotionConfigEx(mcb,i+1,&Th[i].tMTE);	// If res>0 we cannot get old values back
		if (res)
		{	GetError(res,"Reconf:SetMotionConfigEx");	// We do not fail though
		}
	}
	return TRUE;									 
}			
										
BOOL MCcard::Backconf()					
{												// Returns True if GetMotionConfigs succeeded
	int i,j=0,res;							
	if (!mcb || Motors<1) return FALSE;			
	for (i=0;i<Motors;i++)
	{
		res=MCSetMotionConfigEx(mcb,i+1,&Th[i].MTE);	// If res>0 we cannot get old values back
		if (res) j++;
	}
	if (j)
	{
		Error.Format(L"Backconf:SetMotionConfigEx had %d errors",j);	// We do not fail though
		return FALSE;
	}
	return TRUE;									// If we exit from here reconf tells did we 
}													// adjust speeds

//	Since parking of all motors takes same amount of time as parking of one motor
//	and besides is less stressful for the mirror, we only park them all together
BOOL MCcard::ParkAll()							// Goto to positive hard limit, i.e. screw full in 
{	
	int i;										// Set position values to 300,000
	double timo;								// Come back to position: 150,000
	BOOL reconf=FALSE;
	if (!mcb || Motors<1) return FALSE;			
	reconf=Reconf();							// TRUE if Backconf is possible
	timo=35;									// Max move is ~330,000 steps, timeout 35 sec
	MCEnableAxis(mcb,MC_ALL_AXES,TRUE);
	for (i=0;i<Motors;i++)
		MCMoveRelative(mcb,i+1,400000);			// All axes go in
	MCIsStopped(mcb,MC_ALL_AXES,timo);		// Give it another 35 sec
	SetCurPos(300000);							// Set position of all motors to value 300,000
	MCEnableAxis(mcb,MC_ALL_AXES,TRUE);
	for (i=0;i<Motors;i++)
		MCMoveRelative(mcb,i+1,-170000);			// All axes go in
	MCIsStopped(mcb,MC_ALL_AXES,timo);
	MCEnableAxis(mcb,MC_ALL_AXES,FALSE);
	if (reconf)
		return Backconf();
	return FALSE;								// Reconf failed
}

BOOL MCcard::IsMoving(int motor)				// Check with no wait is this motor moving
{												
	if (!mcb || Motors<1) return FALSE;	
	if (motor<0 || motor>=Motors) motor=-1;		// All motors
	if (MCIsStopped(mcb,motor+1,0)) 
		 return FALSE;							// Motor is not moving
	else return TRUE;							// Motor is moving
}

extern int Break;

BOOL MCcard::WaitonMotor(int motor)				// Check with no wait is this motor moving
{												
	if (!mcb || Motors<1) return FALSE;	
	if (motor<0 || motor>=Motors) motor=-1;		// All motors
	if (MCIsStopped(mcb,motor+1,0)) return TRUE;// Motor is not moving
	if (pComm)
	{	pComm->SetTimer(pComm->uTimer,50,NULL);
	}
	while(!Break)
	{
		if (Break) return FALSE;
		if (MCIsStopped(mcb,motor+1,0.1)) return TRUE;// Motor is not moving
	}
	return TRUE;								// Report thet motor is not moving
}