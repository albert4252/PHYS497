#if !defined(AFX_THORMOTOR_H__A5543D45_54BE_4288_B5D2_87C66E4D2248__INCLUDED_)
#define AFX_THORMOTOR_H__A5543D45_54BE_4288_B5D2_87C66E4D2248__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MCapi.h"

class ThorMotor  
{
public:
	ThorMotor();
	virtual ~ThorMotor();
	double			CurPos;
	MCAXISCONFIG	ACF;	// Max amount
	MCFILTEREX		FLT;
	MCMOTIONEX		MTE,tMTE;
	int				SweepFrom,SweepTo,SweepStep,SetPos;
};

#endif // !defined(AFX_THORMOTOR_H__A5543D45_54BE_4288_B5D2_87C66E4D2248__INCLUDED_)
