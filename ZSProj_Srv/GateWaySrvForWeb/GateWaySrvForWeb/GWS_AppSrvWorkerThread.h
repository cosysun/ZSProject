#ifndef GWS_APPSRV_WORKER_THREAD_H
#define GWS_APPSRV_WORKER_THREAD_H

#include "GWS_Common.h"

namespace GWS
{
    //////////////////////////////////////////////////////////////////////////
    //应用服务端任务对象

    class CAppSrvTask : public ITask
    {
    public:
    
		CAppSrvTask();
		
		virtual ~CAppSrvTask();

		virtual void Proc(LPVOID lpParam);

		virtual void Stop();

    protected:
        
        void ProcReq(PCHAR pData, DWORD dwSize, DWORD dwIP, LONG lSocketID);
        
		BOOL ProcAppSrvReq(PCHAR pData, DWORD dwSize, DWORD dwIP, LONG lSocketID, unsigned short sAppTypeID);

        BOOL ProcLoginReq(PCHAR pData, DWORD dwSize, DWORD dwIP, LONG lSocketID);

        void ProcHasLoginReq(PCHAR pData, DWORD dwSize, DWORD dwIP, LONG lSocketID);

        void ProcLogoutReq(PCHAR pData, DWORD dwSize, DWORD dwIP, LONG lSocketID);

        void ProcKeepAliveReq(PCHAR pData, DWORD dwSize, DWORD dwIP, LONG lSocketID);

        void ProcUnknownReq(PCHAR pData, DWORD dwSize, DWORD dwIP, LONG lSocketID);

        void ProcSystemReq(PCHAR pData, DWORD dwSize, DWORD dwIP, LONG lSocketID);

    private:

        BOOL m_bStop;

		CHAR* m_pSendBuffer;
		
        DWORD m_dwSendSize;
        
        DWORD m_dwRecvSize;
    };
    
    //////////////////////////////////////////////////////////////////////////
    //应用服务端任务对象工厂
    
	class CAppSrvTaskFactory : public ITaskFactory
	{
	public:
	
		CAppSrvTaskFactory();

		virtual ~CAppSrvTaskFactory();

		virtual ITask* Create();

		virtual void Release(ITask* pITask);
	};
	
	//////////////////////////////////////////////////////////////////////////
	//应用服务端线程组对象

	class CAppSrvWorkerThread : public CTaskMultiThread, public CSingletonT<CAppSrvWorkerThread>
	{
	public:

		virtual ~CAppSrvWorkerThread();

		friend class CSingletonT<CAppSrvWorkerThread>;

		BOOL Start();

		void Stop();

		void AddData(PCHAR pData, DWORD dwSize, DWORD dwIP, LONG lSocketID);

	private:
	
		CAppSrvWorkerThread();

		BOOL m_bStop;

		CPackage_Queue m_CPackage_Queue;

		CAppSrvTaskFactory m_CAppSrvTaskFactory;

		DWORD m_dwThreadRecvBufferSize;
	};
}

#endif