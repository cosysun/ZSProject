#include "GWS_WorkerThread.h"
#include "GWS_Config.h"
#include "GWS_Log.h"
#include "GWS_OmasThread.h"
#include "GWS_DataMgr.h"
#include "GWS_SocketServer.h"
#include "GWS_AppSrvSocketServer.h"
#include "GWS_AppSrvAuthMgr.h"

namespace GWS
{
	//////////////////////////////////////////////////////////////////////////
	//CClientTask

	CClientTask::CClientTask()
	{
		m_dwRecvSize = 0;

		m_bStop = TRUE;

		m_dwSendSize = 0;	

		m_pSendBuffer = NULL;

		m_dwLocalIP = 0;

	}

	CClientTask::~CClientTask()
	{
		if (m_pSendBuffer)
		{
			delete[] m_pSendBuffer;

			m_pSendBuffer = NULL;
		}
	}

	void CClientTask::Proc(LPVOID lpParam)
	{
		m_bStop = FALSE;

		m_dwLocalIP = GetLocalIP();

		m_dwProjectID = CAppConfig::Instance().GetProjectID();

		m_dwAppTypeID = CAppConfig::Instance().GetAppTypeID();
		
		m_dwRecvSize = CAppConfig::Instance().GetClientRecvBufferSize() + sizeof(TSocketInfo);

		//////////////////////////////////////////////////////////////////////////
        m_dwSendSize = CAppConfig::Instance().GetAppSrvSendBufferSize();	

		m_pSendBuffer = new(std::nothrow) CHAR[m_dwSendSize];

		if (!m_pSendBuffer)
		{
			return;
		}

		//////////////////////////////////////////////////////////////////////////

		CPackage_Queue* pRecvQueue = (CPackage_Queue*)lpParam;

		CSystemLog::Instance().Write(LEVEL_LOG_INFO, "CClientTask::Proc thread start !");
        
        PCHAR pData = NULL;

		while (!m_bStop)
		{
            CPacket_Read recvPacket;

			if (ERROR_PACKAGE_QUEUE_SUCC != pRecvQueue->Pop(recvPacket))
			{
				continue;
			}

            pData = recvPacket.Get_Buffer();

			try
			{
				TSocketInfo* pTSocketInfo = (TSocketInfo*)pData;
				PCHAR pReqData = pData + sizeof(TSocketInfo);

                if (pTSocketInfo->dwOp)
                {
                    return;
                }

				COmasThread::Instance().Increment(COmasThread::OMAS_TYPE_CLIENT_REQ);

				if (this->ProcReq(pReqData, pTSocketInfo->dwDataSize, pTSocketInfo->dwIP, pTSocketInfo->lSocketID))
				{
					COmasThread::Instance().Increment(COmasThread::OMAS_TYPE_CLIENT_REQ_SUCCESS);
				}
				else
				{
					COmasThread::Instance().Increment(COmasThread::OMAS_TYPE_CLIENT_REQ_FAILED);
				}
			}
			catch (...)
			{
				CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CClientTask::Proc ProcReq exception !");
			}
		}

		CSystemLog::Instance().Write(LEVEL_LOG_INFO, "CClientTask::Proc thread stop !");

		return;
	}

	void CClientTask::Stop()
	{
		m_bStop = TRUE;
	}
	
	//////////////////////////////////////////////////////////////////////////
	//GWS协议

	BOOL CClientTask::ProcReq(PCHAR pData, DWORD dwSize, DWORD dwIP, LONG lSocketID)
	{	
		if (!pData || 0 == dwSize)
		{
			return FALSE;
		}

		CHAR szIP[33] = {0};
		ToStringIP(dwIP, szIP);

		// 判断是否是POST
		PCHAR pStart = strstr(pData, "POST /");
		if (!pStart)
		{
			CSocketServer::Instance().PostSend(GWS_HTTP_ERROR_DATA, sizeof(GWS_HTTP_ERROR_DATA) - 1, lSocketID);

			return FALSE;
		}

		// 取POST 内容
		HTTP_REQ_INFO HttpReqInfo = {0};
		if (!ProcReqForPost(pData, dwSize, HttpReqInfo))
		{
			CSocketServer::Instance().PostSend(GWS_HTTP_ERROR_DATA, sizeof(GWS_HTTP_ERROR_DATA) - 1, lSocketID);

			return FALSE;
		}

		//////////////////////////////////////////////////////////////////////////
		//获取AppSocketID
		LONG lAppSrvSocketID = 0;
		BOOL bRet = TRUE;
		bRet =  CAppSrvAuthMgr::Instance().GetAppSrvSocketID(m_dwProjectID, m_dwAppTypeID, lAppSrvSocketID);
		if (!bRet)
		{
			CSystemLog::Instance().Write(
				LEVEL_LOG_ERROR, 
				"CClientTask::ProcClientReq not find AppSrvSocketID by dwAppTypeID:%hu, nProjectID:%ld, SocketID:%ld, IP:%s",
				m_dwAppTypeID, m_dwProjectID, lSocketID, szIP);

			CSocketServer::Instance().PostSend(GWS_HTTP_ERROR_NO_APP, sizeof(GWS_HTTP_ERROR_NO_APP) - 1, lSocketID);

			return FALSE;
		}

		//////////////////////////////////////////////////////////////////////////
		//构造协议

		ZeroMemory(m_pSendBuffer, m_dwSendSize);

		DWORD dwDataSize = 0;
		PGWS_APPSRV_HEAD pAppSrvHead = (PGWS_APPSRV_HEAD)m_pSendBuffer;
		pAppSrvHead->sCmd = GWS_APPSRV_CMD_FUNC_REQ;

		dwDataSize = sizeof(GWS_APPSRV_HEAD);

		PGWS_APPSRV_FUNC_REQ pAppSrvReq = (PGWS_APPSRV_FUNC_REQ)(m_pSendBuffer + dwDataSize);
		pAppSrvReq->sSize = sizeof(GWS_APPSRV_FUNC_REQ);
		pAppSrvReq->nDataSize = HttpReqInfo.dwDataLen;
		pAppSrvReq->nIP = dwIP;

		TGWSSystemData* pTGWSSystemData = (TGWSSystemData*)pAppSrvReq->systemData;
		pTGWSSystemData->lSocketID = lSocketID;
		dwDataSize += sizeof(GWS_APPSRV_FUNC_REQ);

		// 复制业务数据
		if (HttpReqInfo.dwDataLen > 0)
		{
			memcpy(m_pSendBuffer + dwDataSize, HttpReqInfo.szDataBuff, HttpReqInfo.dwDataLen);
			dwDataSize += HttpReqInfo.dwDataLen;
		}

		pAppSrvHead->nSize = dwDataSize;

		CAppSrvSocketServer::Instance().PostSend(m_pSendBuffer, pAppSrvHead->nSize, lAppSrvSocketID);

		return TRUE;
	}

	BOOL CClientTask::ProcReqForPost(PCHAR pData, DWORD dwSize, HTTP_REQ_INFO& aHttpReqInfo)
	{
		// 取字符串长度
		PCHAR pStart = NULL;     
		pStart = strstr(pData, "Content-Length:");
		if (!pStart)
		{
			CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CClientTask::ProcReqForPost not find content-length!");
			return FALSE;
		}
		pStart += strlen("Content-Length:");

		// 找第一个回车换行
		PCHAR pEnd = NULL;
		pEnd = strstr(pStart, "\r\n");
		if (!pEnd)
		{
			CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CClientTask::ProcReqForPost not find \r\n!");
			return FALSE;
		}

		char szDataLen[200] = {0};
		WORD wlen = (WORD)(pEnd - pStart);
		if (wlen > 200)
		{
			return FALSE;
		}
		strncpy(szDataLen, pStart, wlen);
		DWORD dwClientReqSize = atol(szDataLen);
		if (dwClientReqSize == 0)
		{
			CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CClientTask::ProcReqForPost length = 0!");
			return FALSE;  
		}

		// 取POST数据 
		pStart = strstr(pStart, "\r\n\r\n");
		if (!pStart)
		{
			CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CClientTask::ProcReqForPost not find \r\n\r\n!");
			return FALSE;
		}
		pStart += strlen("\r\n\r\n");

		if (pStart - pData >= dwSize)
		{
			CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CClientTask::ProcReqForPost data is lost!");
			return FALSE;  
		}

		string strPostData = pStart;
		aHttpReqInfo.dwDataLen = strPostData.length();
		memcpy(aHttpReqInfo.szDataBuff, strPostData.c_str(), aHttpReqInfo.dwDataLen);
		
		return TRUE;
	}

    //////////////////////////////////////////////////////////////////////////
	//CClientTaskFactory

	CTaskFactory::CTaskFactory()
	{

	}

	CTaskFactory::~CTaskFactory()
	{

	}

	ITask* CTaskFactory::Create()
	{
		CClientTask* pCClientTask = new(std::nothrow) CClientTask();

		return pCClientTask;
	}

	void CTaskFactory::Release(ITask* pITask)
	{
		if (pITask)
		{
			delete pITask;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//CClientWorkerThread

	CWorkerThread::CWorkerThread()
	{
		m_dwThreadRecvBufferSize = CAppConfig::Instance().GetClientRecvBufferSize() + sizeof(TSocketInfo);

		m_CPackage_Queue.Init(
			m_dwThreadRecvBufferSize,
			CAppConfig::Instance().GetClientThreadBufferCount(),
			true);

		m_bStop = TRUE;
	}

	CWorkerThread::~CWorkerThread()
	{
        this->Stop();
	}

	BOOL CWorkerThread::Start()
	{
		if (!m_bStop)
		{
			return TRUE;
		}

		if (CTaskMultiThread::Start(&m_CClientTaskFactory, CAppConfig::Instance().GetClientWorkThreadCount(), &m_CPackage_Queue))
		{
			m_bStop = FALSE;
		}
		else
		{
			return FALSE;
		}

		return TRUE;
	}

	void CWorkerThread::Stop()
	{
		if (m_bStop)
		{
			return;
		}

		m_bStop = TRUE;

        int nIndex = 0;
        int nMaxCount = (int)CAppConfig::Instance().GetClientWorkThreadCount();

        TSocketInfo aTSocketInfo = {0};
        aTSocketInfo.dwOp = 1;

        for (nIndex = 0; nIndex < nMaxCount; ++nIndex)
        {
            m_CPackage_Queue.Push(&aTSocketInfo, sizeof(TSocketInfo));
        }

		CTaskMultiThread::Stop(STOP_WAIT_MILLISECOND);
	}

	void CWorkerThread::AddData(PCHAR pData, DWORD dwSize, DWORD dwIP, LONG lSocketID)
	{
        if (m_bStop)
        {
            return;
        }

		int nResCode  = 0;

		DWORD dwDataSize = sizeof(TSocketInfo) + dwSize;

		if (dwDataSize > m_dwThreadRecvBufferSize)
		{
			CSystemLog::Instance().Write(
				LEVEL_LOG_ERROR, 
				" CWorkerThread::AddData failed, ReqDataSize:%ld + sizeof(TSocketInfo):%lu > ThreadBufferSize:%lu !",
				dwSize, sizeof(TSocketInfo), m_dwThreadRecvBufferSize);

			return;
		}

		CPacket_Write writePacket;

		nResCode = m_CPackage_Queue.GetWritePackage(writePacket, &dwDataSize);

		if(ERROR_PACKAGE_QUEUE_SUCC != nResCode)
		{
			CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CWorkerThread::AddData failed, resCode:%d !",
				nResCode);

			return;
		}

		TSocketInfo aTSocketInfo = {0};

		aTSocketInfo.dwIP = dwIP;
		aTSocketInfo.lSocketID = lSocketID;
		aTSocketInfo.dwDataSize = dwSize;

		writePacket.Write_block<TSocketInfo>(&aTSocketInfo);

		writePacket.Write_block(pData, dwSize);

		nResCode = m_CPackage_Queue.Push(writePacket);

		if (ERROR_PACKAGE_QUEUE_SUCC != nResCode)
		{
			CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CWorkerThread::AddData failed, resCode:%d !",
				nResCode);
		}
	}
}
