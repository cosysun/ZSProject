#ifndef THREAD_SYNC_H
#define THREAD_SYNC_H

#include "BaseTypeDef.h"

//////////////////////////////////////////////////////////////////////////
//线程互斥

class CThreadMutexImp;

class CThreadMutex
{
public:

	CThreadMutex();

	virtual ~CThreadMutex();

	//功能:锁定
	virtual void Lock();

	//功能:解锁
	virtual void UnLock();

private:

	class CThreadMutexImp* m_pCThreadMutexImp;
};

//////////////////////////////////////////////////////////////////////////
//CSmartMutex 智能互斥

class CSmartMutex
{
public:

	CSmartMutex(CThreadMutex &aCThreadMutex);

	~CSmartMutex();

private:

	CThreadMutex *m_pCThreadMutex;

};

#define CREATE_UNIQUE_VAR_NAME_2(x,y) x##y
#define CREATE_UNIQUE_VAR_NAME_1(x,y) CREATE_UNIQUE_VAR_NAME_2(x,y)
#define CREATE_UNIQUE_VAR_NAME(x) CREATE_UNIQUE_VAR_NAME_1(x,__COUNTER__)

//快速创建一个智能互斥对象
#define CREATE_SMART_MUTEX(aCThreadMutex) CSmartMutex CREATE_UNIQUE_VAR_NAME(_aSmartMutex)(aCThreadMutex);

#define CREATE_SMART_MUTEX_PTR(pCThreadMutex) CSmartMutex CREATE_UNIQUE_VAR_NAME(_aSmartMutex)(*pCThreadMutex);

//////////////////////////////////////////////////////////////////////////
//CSyncObject

class ISyncObject
{
public:

	ISyncObject(){};
	
	virtual ~ISyncObject(){};

    virtual	BOOL Wait(DWORD dwMilliseconds) = 0;
	
	virtual void* GetHandle() = 0;
	
	static BOOL WaitSyncObjects(ISyncObject** pISyncObjectArray, DWORD dwCount, BOOL bWaitAll, DWORD dwMilliseconds, DWORD& dwObjectIndex);
};

//////////////////////////////////////////////////////////////////////////
//CEvent

class CEventImp;

class CEvent : public ISyncObject
{
public: 

	CEvent();

	virtual ~CEvent();

	void Set();

	void Reset();

	virtual BOOL Wait(DWORD dwMilliseconds);
	
	virtual void* GetHandle();

private:

	class CEventImp* m_pCEventImp;
};

//////////////////////////////////////////////////////////////////////////
//CSemaphore

class CSemaphoreImp;

class CSemaphore : public ISyncObject
{
public:
	
	CSemaphore();
	
	virtual ~CSemaphore();
	
	void Release();
	
	virtual BOOL Wait(DWORD dwMilliseconds);

	virtual void* GetHandle();

private:

	class CSemaphoreImp* m_pCSemaphoreImp;
};

//////////////////////////////////////////////////////////////////////////
//CProcessMutex

class CProcessMutexImp;

class CProcessMutex
{
public:
	
	CProcessMutex();
	
	//是否初始化成功
	BOOL Create(LPCSTR lpName, DWORD dwMilliseconds = 0);
	
	void Release();
	
	virtual ~CProcessMutex();

	virtual void* GetHandle();

protected:
private:

	class CProcessMutexImp* m_pCProcessMutexImp;
};

#endif