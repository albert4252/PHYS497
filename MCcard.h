#if !defined(AFX_MCCARD_H__5CD7C9C4_507D_46EA_AF49_F1AF9BF655C8__INCLUDED_)
#define AFX_MCCARD_H__5CD7C9C4_507D_46EA_AF49_F1AF9BF655C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mcapi.h"
#include "mcdlg.h"
#include "ThorMotor.h"
class CMotorsApp;
class CCommandView;

class MCcard  
{
public:
	HCTRLR			mcb;			// card handle
	MCPARAMEX		MCP;			
	ThorMotor		Th[8];			// Max Axes is eight
	int				Motors;			// The number of axes
	int				ActMotor;		// Dialog page indicator only
	CString			Error;
	CCommandView	*pComm;
public:
	MCcard();
	virtual ~MCcard();
//	void	UpdateRegistry(CString &S,CMotorsApp *p);
//	void	ReadRegistry(CString &S,CMotorsApp *p);
	BOOL	ObtainInfo(int i);		// Obtain info about an axis
	BOOL	CreateThor();			// True if created
	BOOL	CloseThor();			// True if closed
	BOOL	GetError(int err, char *st="");		// True if error created
//	1 X-axis;  2 Y-axis;  3 Diag. motor
	BOOL	GetCurPos(double &pos,int motor=-1);		
	BOOL	SetCurPos(double pos,int motor=-1);
	BOOL	GotoNewPos(double pos,int motor=-1,BOOL usetmo=TRUE);
	BOOL	MoveRelative(double amount,int motor=-1,BOOL usetmo=TRUE);
	BOOL	IsMoving(int motor=-1);				// Is this/some motor moving; does not wait
	BOOL	WaitonMotor(int motor=-1);			// Wait until (all) motor(s) stops 
	BOOL	Stop();				// Turn all motors off, default anyway
	BOOL	ParkAll();				
	BOOL	Reconf();			// Set Accel. & Veloc. to their max values
	BOOL	Backconf();			// Set Accel. & Veloc. to their previous values
};

#endif // !defined(AFX_MCCARD_H__5CD7C9C4_507D_46EA_AF49_F1AF9BF655C8__INCLUDED_)
