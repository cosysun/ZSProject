#include "Thread.h"

#ifndef _LINUX
#include <windows.h>
#else

#endif

//////////////////////////////////////////////////////////////////////////
//CCustomThreadImp

#ifndef _LINUX

class CCustomThreadImp
{
public:

	CCustomThreadImp(IThreadProcHandler* pIThreadProcHandler, LPVOID lpParameter, SIZE_T dwStackSize = 0)
	{
		m_pIThreadProcHandler = pIThreadProcHandler;

		m_lpParameter = lpParameter;

		m_dwStackSize = dwStackSize;

		m_hThread = NULL;
	}

	virtual ~CCustomThreadImp()
	{
		this->Stop(3000);
	}

	virtual BOOL Start()
	{
		if (m_hThread)
		{
			return TRUE;
		}

		m_hThread = CreateThread(NULL, m_dwStackSize, CCustomThreadImp::ThreadProc, this, 0, &m_dwThreadId);

		if (!m_hThread)
		{
			return FALSE;
		}

		return TRUE;
	}

	virtual void Stop(DWORD dwWaitTime, DWORD dwExitCode = 0)
	{
		if (!m_hThread)
		{
			return;
		}

		switch (WaitForSingleObject(m_hThread, dwWaitTime))
		{
		case WAIT_TIMEOUT:
			{
				TerminateThread(m_hThread, dwExitCode);
			}
			break;
		}

		CloseHandle(m_hThread);

		m_hThread = NULL;
	}

private:

	static DWORD WINAPI ThreadProc(LPVOID lpParameter)
	{
		CCustomThreadImp *pCCustomThreadImp = (CCustomThreadImp *)lpParameter;

		return pCCustomThreadImp->m_pIThreadProcHandler->Proc(pCCustomThreadImp->m_lpParameter);
	}

	LPVOID m_lpParameter;//线程的参数

	SIZE_T m_dwStackSize;

	DWORD m_dwThreadId;

	HANDLE m_hThread;

	IThreadProcHandler* m_pIThreadProcHandler;
};

#else

#endif

//////////////////////////////////////////////////////////////////////////
//CCustomThread

CCustomThread::CCustomThread(LPVOID lpParameter, SIZE_T dwStackSize) 
{
	m_pCCustomThreadImp = new CCustomThreadImp(this, lpParameter, dwStackSize);
}

CCustomThread::~CCustomThread()
{
	if (m_pCCustomThreadImp)
	{
		delete m_pCCustomThreadImp;
	}
}

BOOL CCustomThread::Start()
{	
	if (!m_pCCustomThreadImp)
	{
		return FALSE;
	}
	
	return m_pCCustomThreadImp->Start();
}

void CCustomThread::Stop(DWORD dwWaitTime, DWORD dwExitCode)
{
	if (!m_pCCustomThreadImp)
	{
		return;
	}

	m_pCCustomThreadImp->Stop(dwWaitTime, dwExitCode);
}

//////////////////////////////////////////////////////////////////////////
//CCustomMultiThreadImp

#ifndef _LINUX

class CCustomMultiThreadImp
{
public:

	//功能:初始化
	CCustomMultiThreadImp()
	{
		m_bStop = TRUE;
	}

	virtual ~CCustomMultiThreadImp()
	{
		this->Stop(3000);
	}

	virtual BOOL Start(IThreadProcHandler* pIThreadProcHandler, UINT uThreadCount, LPVOID lpParameter, SIZE_T dwStackSize = 0)
	{
		if (!m_bStop)
		{
			return TRUE;
		}

		if (0 == uThreadCount)
		{
			return FALSE;
		}

		m_lpParameter = lpParameter;
		
		m_pIThreadProcHandler = pIThreadProcHandler;

		UINT uIndex = 0;

		DWORD dwThreadID = 0;

		for (; uIndex < uThreadCount; ++uIndex)
		{
			HANDLE hThread = CreateThread(NULL, 0, CCustomMultiThreadImp::ThreadProc, this, CREATE_SUSPENDED, &dwThreadID);

			if (!hThread)
			{
				break;
			}

			m_ThreadHandleAarray.push_back(hThread);

			m_ThreadIDAarray.push_back(dwThreadID);
		}

		vector<HANDLE>::iterator entity = m_ThreadHandleAarray.begin();
		vector<HANDLE>::iterator endEntity = m_ThreadHandleAarray.end();       

		if (m_ThreadHandleAarray.size() == uThreadCount)
		{
			for(; entity != endEntity; ++entity)
			{
				ResumeThread(*entity);
			}
		}
		else    //当创建的线程数不符合uThreadCount时为异常情况
		{
			this->Stop(3000);

			return FALSE;
		}

		m_bStop = FALSE;

		return TRUE;
	}

	virtual void Stop(DWORD dwMilliseconds)
	{
		if (m_bStop)
		{
			return;
		}

		m_bStop = TRUE;

		if (m_ThreadHandleAarray.size() > 0)
		{
			WaitForMultipleObjects((DWORD)m_ThreadHandleAarray.size(), &m_ThreadHandleAarray.front(), TRUE, dwMilliseconds);

			vector<HANDLE>::iterator entity = m_ThreadHandleAarray.begin();
			vector<HANDLE>::iterator endEntity = m_ThreadHandleAarray.end();       

			for(; entity != endEntity; ++entity)
			{
				TerminateThread(*entity, 0);

				CloseHandle(*entity);
			}
		}

		m_ThreadHandleAarray.clear();

		m_ThreadIDAarray.clear();
	}

	void GetThreadIDArray(vector<DWORD>* pThreadIDArray)
	{
		if (pThreadIDArray)
		{
			pThreadIDArray->resize(m_ThreadIDAarray.size());

			vector<DWORD>::iterator entity = m_ThreadIDAarray.begin();
			vector<DWORD>::iterator endEntity = m_ThreadIDAarray.end();       

			for(; entity != endEntity; ++entity)
			{
				pThreadIDArray->push_back(*entity);
			}
		}
	}

private:

	static DWORD WINAPI ThreadProc(LPVOID lpParameter)
	{
		CCustomMultiThreadImp *pCCustomMultiThreadImp = (CCustomMultiThreadImp *)lpParameter;

		return pCCustomMultiThreadImp->m_pIThreadProcHandler->Proc(pCCustomMultiThreadImp->m_lpParameter);
	}

	vector<HANDLE> m_ThreadHandleAarray;

	vector<DWORD> m_ThreadIDAarray;

	LPVOID m_lpParameter;//线程的参数

	BOOL m_bStop;   //停止标志
	
	IThreadProcHandler* m_pIThreadProcHandler;
};

#else

#endif

//////////////////////////////////////////////////////////////////////////
//CCustomMultiThread

CCustomMultiThread::CCustomMultiThread()
{
	m_pCCustomMultiThreadImp = new CCustomMultiThreadImp();
}

CCustomMultiThread::~CCustomMultiThread()
{
	if (m_pCCustomMultiThreadImp)
	{
		delete m_pCCustomMultiThreadImp;
	}
}

BOOL CCustomMultiThread::Start(UINT uThreadCount, LPVOID lpParameter, SIZE_T dwStackSize)
{
	if (!m_pCCustomMultiThreadImp)
	{
		return FALSE;
	}
	
	return m_pCCustomMultiThreadImp->Start(this, uThreadCount, lpParameter, dwStackSize);
}

void CCustomMultiThread::Stop(DWORD dwMilliseconds)
{
	if (!m_pCCustomMultiThreadImp)
	{
		return;
	}
	
	m_pCCustomMultiThreadImp->Stop(dwMilliseconds);
}

void CCustomMultiThread::GetThreadIDArray(vector<DWORD>* pThreadIDArray)
{
	if (!m_pCCustomMultiThreadImp)
	{
		return;
	}

	m_pCCustomMultiThreadImp->GetThreadIDArray(pThreadIDArray);
}

//////////////////////////////////////////////////////////////////////////
//ITask

ITask::ITask()
{

}

ITask::~ITask()
{

}

//////////////////////////////////////////////////////////////////////////
//ITaskFactory

ITaskFactory::ITaskFactory()
{

}

ITaskFactory::~ITaskFactory()
{

}

//////////////////////////////////////////////////////////////////////////
//CTaskMultiThreadImp

#ifndef _LINUX

class CTaskMultiThreadImp
{
public:

	CTaskMultiThreadImp()
	{
		m_bStop = TRUE;

		m_pITaskFactory = NULL;

		m_lpParameter = NULL;
	}

	virtual ~CTaskMultiThreadImp()
	{
		this->Stop(3000);
	}

	BOOL Start(ITaskFactory* pITaskFactory, UINT uThreadCount, LPVOID lpParameter = NULL)
	{
		if (!m_bStop)
		{
			return TRUE;
		}

		if (!pITaskFactory)
		{
			return FALSE;
		}

		if (0 == uThreadCount)
		{
			return FALSE;
		}

		m_bStop = FALSE;

		m_lpParameter = lpParameter;

		m_pITaskFactory = pITaskFactory;

		UINT uIndex = 0;

		DWORD dwThreadID = 0;

		for (; uIndex < uThreadCount; ++uIndex)
		{
			HANDLE hThread = CreateThread(NULL, 0, CTaskMultiThreadImp::ThreadProc, 
				this, CREATE_SUSPENDED, &dwThreadID);

			if (!hThread)
			{
				break;
			}

			ITask* pITask = m_pITaskFactory->Create();

			if (pITask)
			{
				m_ThreadTaskMap.insert(ThreadTaskMap::value_type(dwThreadID, pITask));
			}
			else
			{
				break;
			}

			m_ThreadHandleAarray.push_back(hThread);
		}

		vector<HANDLE>::iterator entity = m_ThreadHandleAarray.begin();
		vector<HANDLE>::iterator endEntity = m_ThreadHandleAarray.end();       

		if (m_ThreadHandleAarray.size() == uThreadCount)
		{
			for(; entity != endEntity; ++entity)
			{
				ResumeThread(*entity);
			}
		}
		else    //当创建的线程数不符合wThreadCount时为异常情况
		{
			this->Stop(3000);

			return FALSE;
		}

		return TRUE;
	}

	void Stop(DWORD dwMilliseconds)
	{
		if (m_bStop)
		{
			return;
		}

		m_bStop = TRUE;

		//////////////////////////////////////////////////////////////////////////
		//先停止
		{
			ThreadTaskMap::iterator entity = m_ThreadTaskMap.begin();
			ThreadTaskMap::iterator endEntity = m_ThreadTaskMap.end();       

			for(; entity != endEntity; ++entity)
			{
				entity->second->Stop();
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//等待线程结束
		if (m_ThreadHandleAarray.size() > 0)
		{
			WaitForMultipleObjects((DWORD)m_ThreadHandleAarray.size(), &m_ThreadHandleAarray.front(), TRUE, dwMilliseconds);

			vector<HANDLE>::iterator entity = m_ThreadHandleAarray.begin();
			vector<HANDLE>::iterator endEntity = m_ThreadHandleAarray.end();       

			for(; entity != endEntity; ++entity)
			{
				TerminateThread(*entity, 0);

				CloseHandle(*entity);
			}
		}

		m_ThreadHandleAarray.clear();

		//////////////////////////////////////////////////////////////////////////
		//释放内存
		{
			ThreadTaskMap::iterator entity = m_ThreadTaskMap.begin();
			ThreadTaskMap::iterator endEntity = m_ThreadTaskMap.end();       

			for(; entity != endEntity; ++entity)
			{
				if (entity->second)
				{
					m_pITaskFactory->Release(entity->second);
				}
			}

			m_ThreadTaskMap.clear();
		}
	}

	void GetThreadIDArray(vector<DWORD>* pThreadIDArray)
	{
		if (pThreadIDArray)
		{
			pThreadIDArray->reserve(m_ThreadTaskMap.size());

			ThreadTaskMap::iterator entity = m_ThreadTaskMap.begin();
			ThreadTaskMap::iterator endEntity = m_ThreadTaskMap.end();       

			for(; entity != endEntity; ++entity)
			{
				pThreadIDArray->push_back(entity->first);
			}
		}
	}
	
private:

	static DWORD WINAPI ThreadProc(LPVOID lpParameter)
	{
		CTaskMultiThreadImp* pCTaskMultiThreadImp = (CTaskMultiThreadImp*)lpParameter;

		ThreadTaskMap::iterator entity = pCTaskMultiThreadImp->m_ThreadTaskMap.find(GetCurrentThreadId());
		ThreadTaskMap::iterator endEntity = pCTaskMultiThreadImp->m_ThreadTaskMap.end();

		if (entity != endEntity)
		{
			entity->second->Proc(pCTaskMultiThreadImp->m_lpParameter);
		}

		return 0;
	}

	vector<HANDLE> m_ThreadHandleAarray;

	typedef map<DWORD, ITask*> ThreadTaskMap;

	ThreadTaskMap m_ThreadTaskMap;

	ITaskFactory* m_pITaskFactory;

	LPVOID m_lpParameter;//线程的参数

	BOOL m_bStop;   //停止标志
};

#else

#endif

//////////////////////////////////////////////////////////////////////////
//CTaskMultiThread

CTaskMultiThread::CTaskMultiThread()
{
	m_pCTaskMultiThreadImp = new CTaskMultiThreadImp();
}

CTaskMultiThread::~CTaskMultiThread()
{
	if (m_pCTaskMultiThreadImp)
	{
		delete m_pCTaskMultiThreadImp;
	}
}

BOOL CTaskMultiThread::Start(ITaskFactory* pITaskFactory, UINT uThreadCount, LPVOID lpParameter)
{
	if (!m_pCTaskMultiThreadImp)
	{
		return FALSE;
	}
	
	return m_pCTaskMultiThreadImp->Start(pITaskFactory, uThreadCount, lpParameter);
}

void CTaskMultiThread::Stop(DWORD dwMilliseconds)
{
	if (!m_pCTaskMultiThreadImp)
	{
		return;
	}
	
	m_pCTaskMultiThreadImp->Stop(dwMilliseconds);
}

void CTaskMultiThread::GetThreadIDArray(vector<DWORD>* pThreadIDArray)
{
	if (!m_pCTaskMultiThreadImp)
	{
		return;
	}
	
	m_pCTaskMultiThreadImp->GetThreadIDArray(pThreadIDArray);
}

//////////////////////////////////////////////////////////////////////////
// CThread

CThread::CThread(void)
{
    m_hThread = NULL;
}

CThread::~CThread(void)
{
    if( m_hThread != NULL )
    {
        CloseHandle(m_hThread);
        m_hThread = NULL;
    }
}

bool CThread::CreateThread(DWORD (WINAPI* ThreadFunc)(LPVOID), LPVOID pThreadParam)
{
    if( m_hThread != NULL )
        return false;
    m_hThread = ::CreateThread(NULL, 0, ThreadFunc, pThreadParam, 0, NULL);
    return m_hThread != NULL;
}

bool CThread::CreateThread( DWORD (WINAPI* ThreadFunc)(LPVOID), LPVOID pThreadParam, DWORD& dwThreadID )
{
    if( m_hThread != NULL )
        return false;
    m_hThread = ::CreateThread(NULL, 0, ThreadFunc, pThreadParam, 0, &dwThreadID);
    return m_hThread != NULL;
}

bool CThread::WaitForExit(DWORD dwWaitTime)
{
    if( WAIT_TIMEOUT == WaitForSingleObject(m_hThread, dwWaitTime) )
    {
        //强杀
        TerminateThread(m_hThread, 1);
    }
    CloseHandle(m_hThread);
    m_hThread = NULL;
    return true;
}