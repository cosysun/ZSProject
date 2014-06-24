
#ifndef __THREAD_THREAD_H__
#define __THREAD_THREAD_H__

#include "../config.h"
#include "../tool/function_ex.h"


GV_BEGIN
//
#ifdef LINUX
   //typedef void*      HANDLE;    
#endif


class BASECLASS_API CThread
{
public:
    //CThread();
	//
    explicit CThread(function_ex* pfunc);
	//
    ~CThread();
	//
    Bool operator==(const CThread& other) const;
	//
    Bool operator!=(const CThread& other) const;
	//set thread priority
	Void Set_Priority(Int priority);
	//wait thread exit
	//(timeout value :millisecond)
    Void Join(ULong  ulTimeout = 5000);
	//exit thread
	Void Exit_Thread();
	//create thread,start thread
	Int Create_Thread();
	//get thread id
	UInt  Get_ThreadID(Void) {return m_uiThreadID;}
	//
    static Void Sleep(ULong ulTime);
	//
    static Void yield(Void);
	//get error code
	Int   GetLastError();
private:
	HANDLE              m_hThread;//
	UInt                m_uiThreadID;//
	function_ex*        m_pthreadfunc;
    bool                m_joinable;

};


GV_END


#endif 
