#include "ThreadSync.h"

#ifndef _LINUX
#include <windows.h>
#else

#endif

#include <vector>

using namespace std;

//////////////////////////////////////////////////////////////////////////
//CThreadMutexImp

#ifndef _LINUX

class CThreadMutexImp
{
public:

	CThreadMutexImp()
	{
		InitializeCriticalSection(&m_CriticalSection);
	}

	virtual ~CThreadMutexImp()
	{
		DeleteCriticalSection(&m_CriticalSection);
	}

	virtual void Lock()
	{
		EnterCriticalSection(&m_CriticalSection);
	}

	virtual void UnLock()
	{
		LeaveCriticalSection(&m_CriticalSection);
	}

private:
	CRITICAL_SECTION m_CriticalSection;
};

#else

class CThreadMutexImp
{
public:
	CThreadMutexImp()
	{

	}

	virtual ~CThreadMutexImp()
	{

	}
private:

	virtual void Lock()
	{

	}

	virtual void UnLock()
	{

	}
};

#endif

//////////////////////////////////////////////////////////////////////////
//CThreadMutex

CThreadMutex::CThreadMutex()
{
	m_pCThreadMutexImp = new CThreadMutexImp();
}

CThreadMutex::~CThreadMutex()
{
	if (m_pCThreadMutexImp)
	{
		delete m_pCThreadMutexImp;
	}
}

void CThreadMutex::Lock()
{
	m_pCThreadMutexImp->Lock();
}

void CThreadMutex::UnLock()
{
	m_pCThreadMutexImp->UnLock();
}

//////////////////////////////////////////////////////////////////////////
//CSmartMutex

CSmartMutex::CSmartMutex(CThreadMutex &aCThreadMutex)
{
	m_pCThreadMutex = &aCThreadMutex;

	m_pCThreadMutex->Lock();
}

CSmartMutex::~CSmartMutex()
{
	m_pCThreadMutex->UnLock();
}

//////////////////////////////////////////////////////////////////////////
//ISyncObject

#ifndef _LINUX

BOOL ISyncObject::WaitSyncObjects(
	ISyncObject** pISyncObjectArray, 
	DWORD dwCount, 
	BOOL bWaitAll, 
	DWORD dwMilliseconds,
	DWORD& dwObjectIndex)
{
	if (!pISyncObjectArray
		|| 0 == dwCount
		|| MAXIMUM_WAIT_OBJECTS < dwCount)
	{
		throw;
	}
	
	DWORD dwIndex = 0;
	
	vector<HANDLE> handleArray;	
	
	handleArray.resize(dwCount);
	
	for (dwIndex = 0; dwIndex < dwCount; ++dwIndex)
	{
		handleArray[dwIndex] = (HANDLE)pISyncObjectArray[dwIndex]->GetHandle();
	}
	
	dwIndex = WaitForMultipleObjects(dwCount, &handleArray.front(), bWaitAll, dwMilliseconds) - WAIT_OBJECT_0;
	
	switch (dwIndex)
	{
	case WAIT_TIMEOUT:
		{
			return FALSE;
		}
		break;
	case WAIT_FAILED:
		{
			return FALSE;
		}
	}
	
	dwObjectIndex = dwIndex;
	
	return TRUE;
}

#else

#endif

//////////////////////////////////////////////////////////////////////////
//CEventImp

#ifndef _LINUX

class CEventImp
{
public: 

	CEventImp()
	{
		m_hEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
		
		if (!m_hEvent)
		{
			throw;
		}
	}

	virtual ~CEventImp()
	{
		if (m_hEvent)
		{
			CloseHandle(m_hEvent);
		}
	}

	void Set()
	{
		if (m_hEvent)
		{
			SetEvent(m_hEvent);
		}
	}

	void Reset()
	{
		if (m_hEvent)
		{
			ResetEvent(m_hEvent);
		}
	}

	BOOL Wait(DWORD dwMilliseconds)
	{
		if (!m_hEvent)
		{
			throw;
		}
		
		if (WAIT_OBJECT_0 == WaitForSingleObject(m_hEvent, dwMilliseconds))
		{
			return TRUE;
		}
		
		return FALSE;
	}

	void* GetHandle()
	{
		return m_hEvent;
	}

private:

	HANDLE m_hEvent;
};

#else

#endif

//////////////////////////////////////////////////////////////////////////
//CEvent

CEvent::CEvent()
{
	m_pCEventImp = new CEventImp();
}

CEvent::~CEvent()
{
	if (m_pCEventImp)
	{
		delete m_pCEventImp;
	}
}

void CEvent::Set()
{
	if (!m_pCEventImp)
	{
		throw;
	}
	
	m_pCEventImp->Set();
}

void CEvent::Reset()
{
	if (!m_pCEventImp)
	{
		throw;
	}
	
	m_pCEventImp->Reset();
}

BOOL CEvent::Wait(DWORD dwMilliseconds)
{
	if (!m_pCEventImp)
	{
		throw;
	}
	
	return m_pCEventImp->Wait(dwMilliseconds);
}

void* CEvent::GetHandle()
{
	if (!m_pCEventImp)
	{
		throw;
	}
	
	return m_pCEventImp->GetHandle();
}

//////////////////////////////////////////////////////////////////////////
//CSemaphoreImp

#ifndef _LINUX

class CSemaphoreImp
{
public:

	CSemaphoreImp()
	{
		m_hSemaphore = CreateSemaphoreA(NULL, 0, LONG_MAX, NULL);

		if (!m_hSemaphore)
		{
			throw;
		}
	}

	virtual ~CSemaphoreImp()
	{
		if (m_hSemaphore)
		{
			CloseHandle(m_hSemaphore);
		}
	}

	void Release()
	{	
		if (m_hSemaphore)
		{
			ReleaseSemaphore(m_hSemaphore, 1,NULL);
		}

	}

	BOOL Wait(DWORD dwMilliseconds)
	{
		if (!m_hSemaphore)
		{
			throw;
		}

		if (WAIT_OBJECT_0 == WaitForSingleObject(m_hSemaphore, dwMilliseconds))
		{
			return TRUE;
		}

		return FALSE;
	}

	void* GetHandle()
	{
		return m_hSemaphore;
	}

private:

	HANDLE m_hSemaphore;
};

#else

#endif

//////////////////////////////////////////////////////////////////////////
//CSemaphore

CSemaphore::CSemaphore()
{
	m_pCSemaphoreImp = new CSemaphoreImp();
}

CSemaphore::~CSemaphore()
{
	if (m_pCSemaphoreImp)
	{
		delete m_pCSemaphoreImp;
	}
}

void CSemaphore::Release()
{
	if (!m_pCSemaphoreImp)
	{
		throw;
	}
	
	m_pCSemaphoreImp->Release();
}

BOOL CSemaphore::Wait(DWORD dwMilliseconds)
{
	if (!m_pCSemaphoreImp)
	{
		throw;
	}
	
	return m_pCSemaphoreImp->Wait(dwMilliseconds);
}

void* CSemaphore::GetHandle()
{
	if (!m_pCSemaphoreImp)
	{
		throw;
	}
	
	return m_pCSemaphoreImp->GetHandle();
}

//////////////////////////////////////////////////////////////////////////
//CProcessMutexImp

#ifndef _LINUX

class CProcessMutexImp
{
public:

	CProcessMutexImp()
	{
		m_hMutex = NULL;
	}

	virtual ~CProcessMutexImp()
	{
		this->Release();
	}
	
	//是否初始化成功
	BOOL Create(LPCSTR lpName, DWORD dwMilliseconds)
	{
		m_hMutex = CreateMutex(NULL, FALSE, lpName);

		if (!m_hMutex)
		{
			return FALSE;
		}

		if (ERROR_ALREADY_EXISTS == GetLastError())
		{	
			if (dwMilliseconds > 0)
			{
				if (WAIT_OBJECT_0 == WaitForSingleObject(m_hMutex, dwMilliseconds))
				{
					return TRUE;
				}
			}

			this->Release();

			return FALSE;
		}

		return TRUE;
	}

	void Release()
	{
		if (m_hMutex)
		{
			ReleaseMutex(m_hMutex);
			CloseHandle(m_hMutex);
			
			m_hMutex = NULL;
		}
	}

	void* GetHandle()
	{
		return m_hMutex;
	}

protected:
private:

	HANDLE m_hMutex;
};

#else

#endif

//////////////////////////////////////////////////////////////////////////
//CProcessMutex

CProcessMutex::CProcessMutex()
{
	m_pCProcessMutexImp = new CProcessMutexImp();
}

CProcessMutex::~CProcessMutex()
{
	if (m_pCProcessMutexImp)
	{
		delete m_pCProcessMutexImp;
	}
}

BOOL CProcessMutex::Create(LPCSTR lpName, DWORD dwMilliseconds)
{
	return m_pCProcessMutexImp->Create(lpName, dwMilliseconds);
}

void CProcessMutex::Release()
{
	m_pCProcessMutexImp->Release();
}

void* CProcessMutex::GetHandle()
{
	if (!m_pCProcessMutexImp)
	{
		return NULL;
	}

	return m_pCProcessMutexImp->GetHandle();
}