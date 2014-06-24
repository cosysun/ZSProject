
#ifndef __AUTOLOCK_H__
#define __AUTOLOCK_H__

#include "../config.h"

GV_BEGIN

class BASECLASS_API CLockObject
{
public:
	CLockObject();
	virtual ~CLockObject();
public:
	Void Lock()
	{
#ifdef WIN32
		::EnterCriticalSection(&m_hMutex);
#else
		pthread_mutex_lock(&m_hMutex);
#endif
		m_lCounter++;
	}
	Void Unlock()
	{
		m_lCounter--;
#ifdef WIN32
		::LeaveCriticalSection(&m_hMutex);
#else
		pthread_mutex_unlock(&m_hMutex);
#endif
	}
	//
	Long GetCounter(){return m_lCounter;}
private:

#ifdef WIN32
	CRITICAL_SECTION   m_hMutex;
#else
	pthread_mutex_t    m_hMutex;
#endif
	Long              m_lCounter;
};

/*----------------------------------------
*
----------------------------------------*/
class BASECLASS_API Semaphore
{
public:
	Semaphore(Int nStartCount = 0);
	~Semaphore();
	//modfied by gavin on 2013.7.17
	inline Void Lock(ULong  ulMillTime = 0xFFFFFFFF) { Wait(ulMillTime); }
	//
	inline Void Unlock() { Notify(); }
	//
	inline Int Get_Wait_number() { return m_nWait_Number; }
	//
	Long GetCounter(){return m_lCounter;}
	//set semaphore
	Void Set(){Notify();};
	//wait for time(millsecond),return value[=0: signed, !=0: timeout]
	Int Wait(ULong  ulMillTime);//add by gavin on 2012.8.1
protected:
	//
	Void      Wait();
	//
	Void      Notify();
protected:
	//
#ifdef WIN32
	HANDLE       m_hSemaphone;
#else
	sem_t*      m_hSemaphone;
#endif
	Int              m_nWait_Number;
	Long             m_lCounter;
};


/*-----------------------------------------
*
-----------------------------------------*/
template <class T>
class BASECLASS_API CAutoLock
{
public:
	CAutoLock(T*  t)
	{
		if(Null != t)
		{
			m_pLock = t;
			//
			m_pLock->Lock();
		}

	}
	//
	~CAutoLock()
	{
		if(Null != m_pLock)
		{
			m_pLock->Unlock();
		}
	}
	
private:	
	T*                        m_pLock;
};



GV_END

#endif 
