#ifndef GWS_CLIENT_WORKER_THREAD_H
#define GWS_CLIENT_WORKER_THREAD_H

#include "GWS_Common.h"

#define  MAX_AUTH_SIZE 33
#define  MAX_HTTP_DATA_SIZE 10 * 1024


// http������Ϣ�ṹ��
typedef struct tagHttpReqInfo
{
	DWORD dwDataLen;
	char szDataBuff[MAX_HTTP_DATA_SIZE];
}HTTP_REQ_INFO, *PHTTP_REQ_INFO;

namespace GWS
{
	//////////////////////////////////////////////////////////////////////////
	//�ͻ����������

	class CClientTask : public ITask
	{
	public:

		CClientTask();

		virtual ~CClientTask();

		virtual void Proc(LPVOID lpParam);

		virtual void Stop();

	protected:

		//////////////////////////////////////////////////////////////////////////
		//GWSЭ��

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
	//�ͻ���������󹤳�

	class CTaskFactory : public ITaskFactory
	{
	public:

		CTaskFactory();

		virtual ~CTaskFactory();

		virtual ITask* Create();

		virtual void Release(ITask* pITask);
	};

	//////////////////////////////////////////////////////////////////////////
	//�ͻ����߳������

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