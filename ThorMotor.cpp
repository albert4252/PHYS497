// ThorMotor.cpp: implementation of the ThorMotor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ThorMotor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ThorMotor::ThorMotor()
{
	CurPos=0;
	SetPos=0;
	SweepFrom=500;SweepTo=800;
	SweepStep=0.02;
}

ThorMotor::~ThorMotor()
{

}
