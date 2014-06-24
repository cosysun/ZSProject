#ifndef __PERFROMANCETIMER_H__
#define __PERFROMANCETIMER_H__

#include "../config.h"

GV_BEGIN

typedef Void (*PLOGFUNC_Performance)(CChar*   szLogFileName,Char*  pszFormat,...);

class BASECLASS_API CPerformanceTimer
{
public:
	CPerformanceTimer(CChar*   szLogFileName,CChar* szLogName,PLOGFUNC_Performance  pLogFunc,Int iAlarmTimer = 0);
	CPerformanceTimer();
	~CPerformanceTimer();
	//
	Void		t_Start();
	//
	Void     	t_End();
	//
	ULong       t_Value();//millisecond
	//
private:
	//
	Void		Init();
#ifdef WIN32
	LARGE_INTEGER              m_Start;
	LARGE_INTEGER              m_End;
	LARGE_INTEGER              m_Freq;
#else
	struct timeval           m_Start;
	struct timeval           m_End;
	struct timezone            m_Zone;
#endif	 
	//
	Char		m_szLogName[260];
	Char        m_szLogFileName[512];
	Int         m_AlarmTimer;
	PLOGFUNC_Performance        m_pLogFunc;
};


/*----------------------------------------
*
-----------------------------------------*/
class BASECLASS_API CGVDebuger
{
public:
	CGVDebuger(CChar* szLogName,CChar* szInfo,PLOGFUNC_Performance  pLogFunc);
	~CGVDebuger();
private:
	PLOGFUNC_Performance        m_pLogFunc;
	Char		                m_szLogName[260];
	Char                        m_szInfo[260];
};


GV_END

#endif