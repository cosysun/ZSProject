#ifndef GWS_CLIENT_WORKER_THREAD_H
#define GWS_CLIENT_WORKER_THREAD_H

#include "GWS_Common.h"

#define  MAX_AUTH_SIZE 33
#define  MAX_HTTP_DATA_SIZE 10 * 1024


// http请求信息结构体
typedef struct tagHttpReqInfo
{
	DWORD dwDataLen;
	char szDataBuff[MAX_HTTP_DATA_SIZE];
}HTTP_REQ_INFO, *PHTTP_REQ_INFO;

namespace GWS
{
	//////////////////////////////////////////////////////////////////////////
	//客户端任务对象

	class CClientTask : public ITask
	{
	public:

		CClientTask();

		virtual ~CClientTask();

		virtual void Proc(LPVOID lpParam);

		virtual void Stop();

	protected:

		//////////////////////////////////////////////////////////////////////////
		//GWS协议

		BOOL ProcReq(PCHAR pData, DWORD dwSize, DWORD dwIP, LONG lSocketID);

        BOOL ProcReqForPost(PCHAR pData, DWORD dwSize, HTTP_REQ_INFO& aHttpReqInfo);

	private:

		BOOL m_bStop;

		DWORD m_dwRecvSize;

		CHAR* m_pSendBuffer;

		DWORD m_dwSendSize;

		DWORD m_dwLocalIP; 

		DWORD m_dwProjectID;

		DWORD m_dwAppTypeID;
	};

	//////////////////////////////////////////////////////////////////////////
	//客户端任务对象工厂

	class CTaskFactory : public ITaskFactory
	{
	public:

		CTaskFactory();

		virtual ~CTaskFactory();

		virtual ITask* Create();

		virtual void Release(ITask* pITask);
	};

	//////////////////////////////////////////////////////////////////////////
	//客户端线程组对象

	class CWorkerThread : public CTaskMultiThread, public CSingletonT<CWorkerThread>
	{
	public:

		virtual ~CWorkerThread();

		friend class CSingletonT<CWorkerThread>;

		BOOL Start();

		void Stop();

		void AddData(PCHAR pData, DWORD dwSize, DWORD dwIP, LONG dwSocketID);

	private:

		CWorkerThread();

		BOOL m_bStop;

		CPackage_Queue m_CPackage_Queue;

		CTaskFactory m_CClientTaskFactory;

		DWORD m_dwThreadRecvBufferSize;
	};
}

#endif