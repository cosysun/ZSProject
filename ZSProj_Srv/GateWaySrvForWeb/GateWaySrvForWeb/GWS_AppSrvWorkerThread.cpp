#include "GWS_AppSrvWorkerThread.h"
#include "GWS_Config.h"
#include "GWS_Log.h"
#include "GWS_OmasThread.h"
#include "GWS_AppSrvAuthMgr.h"
#include "GWS_SocketServer.h"
#include "GWS_AppSrvSocketServer.h"
#include "GWS_DataMgr.h"

namespace GWS
{
    //////////////////////////////////////////////////////////////////////////
    //CAppSrvTask

    CAppSrvTask::CAppSrvTask()
    {
        m_dwRecvSize = CAppConfig::Instance().GetAppSrvRecvBufferSize() + sizeof(TSocketInfo);

        m_bStop = TRUE;

        m_dwSendSize = CAppConfig::Instance().GetClientSendBufferSize();

		m_pSendBuffer = new(std::nothrow) CHAR[m_dwSendSize];
    }

    CAppSrvTask::~CAppSrvTask()
    {
		if (m_pSendBuffer)
		{
			delete[] m_pSendBuffer;
		}
    }

    void CAppSrvTask::Proc(LPVOID lpParam)
    {
		CPackage_Queue* pRecvQueue = (CPackage_Queue*)lpParam;
    
        m_bStop = FALSE;
        
        if (!m_pSendBuffer)
        {
			CSystemLog::Instance().Write(LEVEL_LOG_INFO, "CAppSrvTask::Proc new send buffer failed !");
			
			return;
        }

        CSystemLog::Instance().Write(LEVEL_LOG_INFO, "CAppSrvTask::Proc thread start !");
        
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

                this->ProcReq(pReqData, 
					pTSocketInfo->dwDataSize, 
					pTSocketInfo->dwIP, 
					pTSocketInfo->lSocketID);
            }
            catch (...)
            {
                CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CAppSrvTask::Proc ProcReq exception !");
            }
        }

        CSystemLog::Instance().Write(LEVEL_LOG_INFO, "CAppSrvTask::Proc thread stop !");

        return;
    }
    
    void CAppSrvTask::Stop()
    {
		m_bStop = TRUE;
    }

    void CAppSrvTask::ProcReq(PCHAR pData, DWORD dwSize, DWORD dwIP, LONG lSocketID)
    {
		PGWS_APPSRV_HEAD pHead = (PGWS_APPSRV_HEAD)pData;

        DWORD dwAppTypeID = 0;

		if (CAppSrvAuthMgr::Instance().IsLogined(lSocketID, dwAppTypeID))
		{
			//进入业务流程
			switch (pHead->sCmd)
			{
			case GWS_APPSRV_CMD_FUNC_REQ:
				{
					COmasThread::Instance().Increment(COmasThread::OMAS_TYPE_APPSRV_RES);

					if (this->ProcAppSrvReq(pData,dwSize, dwIP, lSocketID, (unsigned short)dwAppTypeID))
					{
						COmasThread::Instance().Increment(COmasThread::OMAS_TYPE_APPSRV_RES_SUCCESS);
					}
					else
					{
						COmasThread::Instance().Increment(COmasThread::OMAS_TYPE_APPSRV_RES_FAILED);
					}
				}
				break;
			case GWS_APPSRV_CMD_KEEPALIVE:
				{
                    COmasThread::Instance().Increment(COmasThread::OMAS_TYPE_APPSRV_KEEPALIVE);
                    
					this->ProcKeepAliveReq(pData,dwSize, dwIP, lSocketID);
				}
				break;
			case GWS_APPSRV_CMD_LOGIN:
				{
					COmasThread::Instance().Increment(COmasThread::OMAS_TYPE_APPSRV_LOGIN);

					this->ProcHasLoginReq(pData, dwSize, dwIP, lSocketID);
						
                    COmasThread::Instance().Increment(COmasThread::OMAS_TYPE_APPSRV_LOGIN_SUCCESS);
				}
				break;
            case GWS_APPSRV_CMD_SYSTEM:
                {
                    COmasThread::Instance().Increment(COmasThread::OMAS_TYPE_APPSRV_SYSTEM);

                    this->ProcSystemReq(pData, dwSize, dwIP, lSocketID);
                }
                break;
			case GWS_APPSRV_CMD_LOGOUT:
				{
                    COmasThread::Instance().Increment(COmasThread::OMAS_TYPE_APPSRV_LOGOUT);

					this->ProcLogoutReq(pData,dwSize, dwIP, lSocketID);
				}
				break;
			default:
				{
                    COmasThread::Instance().Increment(COmasThread::OMAS_TYPE_APPSRV_UNKNOWN);

					this->ProcUnknownReq(pData,dwSize, dwIP, lSocketID);
				}
			}
		}
		else
		{
			//进入
			switch (pHead->sCmd)
			{
			case GWS_APPSRV_CMD_LOGIN:
				{
					COmasThread::Instance().Increment(COmasThread::OMAS_TYPE_APPSRV_LOGIN);

					if (this->ProcLoginReq(pData, dwSize, dwIP, lSocketID))
					{
						COmasThread::Instance().Increment(COmasThread::OMAS_TYPE_APPSRV_LOGIN_SUCCESS);
					}
					else
					{
						COmasThread::Instance().Increment(COmasThread::OMAS_TYPE_APPSRV_LOGIN_FAILED);

						CAppSrvSocketServer::Instance().CloseSocket(lSocketID);
					}
				}
				break;
			default:
				{
                    COmasThread::Instance().Increment(COmasThread::OMAS_TYPE_APPSRV_UNKNOWN);

					this->ProcUnknownReq(pData,dwSize, dwIP, lSocketID);
				}
			}
		}
    }
    
	BOOL CAppSrvTask::ProcLoginReq(PCHAR pData, DWORD dwSize, DWORD dwIP, LONG lSocketID)
	{
		BOOL bSuccess = FALSE;

		char szIP[33] = {0};
		ToStringIP(dwIP, szIP);

		if (dwSize >= sizeof(GWS_APPSRV_HEAD) + sizeof(GWS_APPSRV_LOGIN_REQ))
		{
			PGWS_APPSRV_HEAD pHead = (PGWS_APPSRV_HEAD)pData;
			PGWS_APPSRV_LOGIN_REQ pReq = (PGWS_APPSRV_LOGIN_REQ)(pData + sizeof(GWS_APPSRV_HEAD));

			DWORD dwAppSrvResDataSize = sizeof(GWS_APPSRV_HEAD) + sizeof(GWS_APPSRV_LOGIN_RES);

			if (dwAppSrvResDataSize <= m_dwSendSize)
			{
				ZeroMemory(m_pSendBuffer, m_dwSendSize);
				
				PGWS_APPSRV_HEAD pResHead = (PGWS_APPSRV_HEAD)m_pSendBuffer;
				pResHead->sCmd = pHead->sCmd;
				pResHead->nSize = sizeof(GWS_APPSRV_HEAD) + sizeof(GWS_APPSRV_LOGIN_RES);

				PGWS_APPSRV_LOGIN_RES pRes = (PGWS_APPSRV_LOGIN_RES)(m_pSendBuffer + sizeof(GWS_APPSRV_HEAD));
				pRes->sSize = sizeof(GWS_APPSRV_LOGIN_RES);

				if (dwSize == sizeof(GWS_APPSRV_HEAD) + sizeof(GWS_APPSRV_LOGIN_REQ))
				{
					CHAR szAuthKey[51] = {0};

					memcpy(szAuthKey, pReq->szAuthKey, 50);
					
					if (CAppSrvAuthMgr::Instance().Auth(
						lSocketID, 
						pReq->sProjectID,
						pReq->sAppTypeID,
						szAuthKey, 
						dwIP))
					{   
						CAppSrvLog::Instance().Write(LEVEL_LOG_INFO, "AppSrv login success,dwAppTypeID:%hu, IP:%s", 
							pReq->sAppTypeID, szIP);

						bSuccess = TRUE;
					}
					else
					{
						CAppSrvLog::Instance().Write(LEVEL_LOG_INFO, "AppSrv login failed, dwAppTypeID:%hu, IP:%s, AuthInfo:%s", 
							pReq->sAppTypeID, szIP, szAuthKey);

						pRes->sResCode = GWS_APPSRV_RES_CODE_LOGIN_FAILED;
					}

                    //先发送登录信息
					CAppSrvSocketServer::Instance().PostSend(m_pSendBuffer, pResHead->nSize, lSocketID);

                    if (!pReq->btNoServices)
                    {
                        //再使APPSRV可以处理业务请求
                        CAppSrvAuthMgr::Instance().SetSocketRoute(lSocketID);
                    }
				}
				else
				{
					CSystemLog::Instance().Write(
						LEVEL_LOG_WARNING, 
						"CAppSrvTask::ProcLogin protocol error, DataLen:%ld SocketID:%ld, IP:%s",
						dwSize, lSocketID, szIP);
				}
			}
			else
			{
				CSystemLog::Instance().Write(
					LEVEL_LOG_ERROR, 
					"CAppSrvTask::ProcLogin error, ThreadSendBufferSize:%ld < AppSrvResDataSize:%ld, SocketID:%ld, IP:%s",
					m_dwSendSize, dwAppSrvResDataSize, lSocketID, szIP);
			}
		}
		else
		{
			CSystemLog::Instance().Write(
				LEVEL_LOG_WARNING, 
				"CAppSrvTask::ProcLogin protocol error, DataLen:%ld SocketID:%ld, IP:%s",
				dwSize, lSocketID, szIP);
		}

		return bSuccess;
	}

    void CAppSrvTask::ProcHasLoginReq(PCHAR pData, DWORD dwSize, DWORD dwIP, LONG lSocketID)
    {
        ZeroMemory(m_pSendBuffer, m_dwSendSize);

        PGWS_APPSRV_HEAD pHead = (PGWS_APPSRV_HEAD)pData;
        PGWS_APPSRV_LOGIN_REQ pReq = (PGWS_APPSRV_LOGIN_REQ)(pData + sizeof(GWS_APPSRV_HEAD));

        PGWS_APPSRV_HEAD pResHead = (PGWS_APPSRV_HEAD)m_pSendBuffer;
        pResHead->sCmd = pHead->sCmd;
        pResHead->nSize = sizeof(GWS_APPSRV_HEAD) + sizeof(GWS_APPSRV_LOGIN_RES);

        PGWS_APPSRV_LOGIN_RES pRes = (PGWS_APPSRV_LOGIN_RES)(m_pSendBuffer + sizeof(GWS_APPSRV_HEAD));
        pRes->sSize = sizeof(GWS_APPSRV_LOGIN_RES);

        CAppSrvSocketServer::Instance().PostSend(m_pSendBuffer, pResHead->nSize, lSocketID);
    }

	void CAppSrvTask::ProcLogoutReq(PCHAR pData, DWORD dwSize, DWORD dwIP, LONG lSocketID)
	{
		char szIP[33] = {0};
		ToStringIP(dwIP, szIP);

		CSystemLog::Instance().Write(
			LEVEL_LOG_DEBUG, 
			"CAppSrvTask::ProcLogout SocketID:%ld, IP:%s",
			lSocketID, szIP);

		CAppSrvSocketServer::Instance().CloseSocket(lSocketID);
	}

    void CAppSrvTask::ProcKeepAliveReq(PCHAR pData, DWORD dwSize, DWORD dwIP, LONG lSocketID)
    {
        CSystemLog::Instance().Write(
            LEVEL_LOG_DEBUG, 
            "CAppSrvTask::ProcKeepAlive SocketID:%ld, IP:%ld",
            lSocketID, dwIP);
    }

    void CAppSrvTask::ProcUnknownReq(PCHAR pData, DWORD dwSize, DWORD dwIP, LONG lSocketID)
    {
        PGWS_APPSRV_HEAD pHead = (PGWS_APPSRV_HEAD)pData;

        char szIP[33] = {0};
        ToStringIP(dwIP, szIP);

        CSystemLog::Instance().Write(
            LEVEL_LOG_DEBUG, 
            "CAppSrvTask::ProcUnknown cmd:%hu, SocketID:%ld, IP:%s",
            pHead->sCmd, lSocketID, szIP);
    }

    void CAppSrvTask::ProcSystemReq(PCHAR pData, DWORD dwSize, DWORD dwIP, LONG lSocketID)
    {
        PGWS_APPSRV_HEAD pHead = (PGWS_APPSRV_HEAD)pData;

        char szIP[33] = {0};
        ToStringIP(dwIP, szIP);

        CSystemLog::Instance().Write(
            LEVEL_LOG_DEBUG, 
            "CAppSrvTask::ProcSystem SocketID:%ld, IP:%s", lSocketID, szIP);

        PGWS_APPSRV_SYSTEM_REQ pReq = (PGWS_APPSRV_SYSTEM_REQ)(pData + sizeof(GWS_APPSRV_HEAD));

        if (pReq->nParamFlag & GWS_APPSRV_REQ_PARAM_FLAG_SERVICE)
        {
            //再使APPSRV可以处理业务请求
            CAppSrvAuthMgr::Instance().SetSocketRoute(lSocketID);
        }
        else if (pReq->nParamFlag & GWS_APPSRV_REQ_PARAM_FLAG_NO_SERVICE)
        {
            CAppSrvAuthMgr::Instance().RemoveSocketRoute(lSocketID);
        }

        CHAR sendBuffer[1024] = {0};

        PGWS_APPSRV_HEAD pResHead = (PGWS_APPSRV_HEAD)sendBuffer;
        pResHead->nSize = sizeof(GWS_APPSRV_HEAD) + sizeof(GWS_APPSRV_SYSTEM_RES);
        pResHead->sCmd = GWS_APPSRV_CMD_SYSTEM;
        
        PGWS_APPSRV_SYSTEM_RES pRes = (PGWS_APPSRV_SYSTEM_RES)(pData + sizeof(GWS_APPSRV_HEAD));
        pRes->sSize = sizeof(GWS_APPSRV_SYSTEM_RES);
        
        pRes->nParamFlag = pReq->nParamFlag;

        CAppSrvSocketServer::Instance().PostSend(sendBuffer, pResHead->nSize, lSocketID);
    }
    
	BOOL CAppSrvTask::ProcAppSrvReq(PCHAR pData, DWORD dwSize, DWORD dwIP,LONG lSocketID, unsigned short sAppTypeID)
	{
		BOOL bSuccess = FALSE;
		WORD wSize = 0;
		BOOL bCallRes = FALSE;

		char szIP[33] = {0};
		ToStringIP(dwIP, szIP);

		if (dwSize >= sizeof(GWS_APPSRV_HEAD) + sizeof(GWS_APPSRV_FUNC_RES))
		{
			PGWS_APPSRV_HEAD pAppResHead = (PGWS_APPSRV_HEAD)pData;
			PGWS_APPSRV_FUNC_RES pAppRes = (PGWS_APPSRV_FUNC_RES)(pData + sizeof(GWS_APPSRV_HEAD));

			if (dwSize == sizeof(GWS_APPSRV_HEAD) + sizeof(GWS_APPSRV_FUNC_RES) + pAppRes->sTakeSize + pAppRes->nDataSize)
			{
				if (m_dwSendSize >= pAppRes->nDataSize )
				{
                    //////////////////////////////////////////////////////////////////////////
                    //发送数据

                    ZeroMemory(m_pSendBuffer, m_dwSendSize);

					sprintf(m_pSendBuffer, GWS_HTTP_RETURN_FORMAT, pAppRes->nDataSize);
					int nLen = strlen(m_pSendBuffer);
                    if (pAppRes->nDataSize > 0)
                    {
                        memcpy(m_pSendBuffer + nLen, pData + sizeof(GWS_APPSRV_HEAD) + sizeof(GWS_APPSRV_FUNC_RES) + pAppRes->sTakeSize, 
                            pAppRes->nDataSize);
                    }

					TGWSSystemData* pTGWSSystemData = (TGWSSystemData* )pAppRes->systemData;
					
                    CSocketServer::Instance().PostSend(m_pSendBuffer, pAppRes->nDataSize + nLen, pTGWSSystemData->lSocketID);

                    //////////////////////////////////////////////////////////////////////////

					bSuccess = TRUE;
				}
				else
				{
					CSystemLog::Instance().Write(
						LEVEL_LOG_ERROR, 
						"CAppSrvTask::ProcAppSrvReq error, SendBufferSize:%lu < SendSize:%lu, SocketID:%ld, IP:%s",
						m_dwSendSize, 
						pAppRes->nDataSize, 
						lSocketID, 
						szIP);
				}
			}
			else
			{
				CSystemLog::Instance().Write(
					LEVEL_LOG_WARNING, 
					"CAppSrvTask::ProcAppSrvReq error, dwSize:%ld,AppSrvDataSize:%lu, SocketID:%ld, IP:%s",
					dwSize, pAppRes->nDataSize, lSocketID, szIP);
			}
		}
		else
		{
			CSystemLog::Instance().Write(
				LEVEL_LOG_WARNING, 
				"CAppSrvTask::ProcAppSrvReq error, dwSize:%ld, SocketID:%ld, IP:%s",
				dwSize, lSocketID, szIP);
		}

		return bSuccess;
	}
	
	//////////////////////////////////////////////////////////////////////////
	//CAppSrvTaskFactory
	
	CAppSrvTaskFactory::CAppSrvTaskFactory()
	{
	
	}

	CAppSrvTaskFactory::~CAppSrvTaskFactory()
	{
	
	}

	ITask* CAppSrvTaskFactory::Create()
	{
		CAppSrvTask* pCAppSrvTask = new(std::nothrow) CAppSrvTask();
		
		return pCAppSrvTask;
	}

	void CAppSrvTaskFactory::Release(ITask* pITask)
	{
		if (pITask)
		{
			delete pITask;
		}
	}
	
	//////////////////////////////////////////////////////////////////////////
	//CAppSrvWorkerThread

	CAppSrvWorkerThread::CAppSrvWorkerThread()
	{
		m_dwThreadRecvBufferSize = CAppConfig::Instance().GetAppSrvRecvBufferSize() + sizeof(TSocketInfo);

		m_CPackage_Queue.Init(
			m_dwThreadRecvBufferSize,
			CAppConfig::Instance().GetAppSrvThreadBufferCount(),
			true);

		m_bStop = TRUE;
	}

	CAppSrvWorkerThread::~CAppSrvWorkerThread()
	{
        this->Stop();
	}

	BOOL CAppSrvWorkerThread::Start()
	{
		if (!m_bStop)
		{
			return TRUE;
		}

		if (CTaskMultiThread::Start(&m_CAppSrvTaskFactory, CAppConfig::Instance().GetAppSrvWorkThreadCount(), &m_CPackage_Queue))
		{
			m_bStop = FALSE;
		}
		else
		{
			return FALSE;
		}

		return TRUE;
	}

	void CAppSrvWorkerThread::Stop()
	{
		if (m_bStop)
		{
			return;
		}

        m_bStop = TRUE;

        int nIndex = 0;
        int nMaxCount = (int)CAppConfig::Instance().GetAppSrvWorkThreadCount();

        TSocketInfo aTSocketInfo = {0};
        aTSocketInfo.dwOp = 1;

        for (nIndex = 0; nIndex < nMaxCount; ++nIndex)
        {
            m_CPackage_Queue.Push(&aTSocketInfo, sizeof(TSocketInfo));
        }


		CTaskMultiThread::Stop(STOP_WAIT_MILLISECOND);
	}

	void CAppSrvWorkerThread::AddData(PCHAR pData, DWORD dwSize, DWORD dwIP, LONG lSocketID)
	{
		if (m_bStop)
		{
			return;
		}

		int nResCode  = 0;

		DWORD dwDataSize = sizeof(TSocketInfo) + dwSize;

		if (dwDataSize > m_dwThreadRecvBufferSize)
		{
			CSystemLog::Instance().Write(LEVEL_LOG_ERROR, 
				" CWorkerThread::AddData failed, ReqDataSize:%ld + sizeof(TSocketInfo):%lu > ThreadBufferSize:%lu !",
				dwSize, sizeof(TSocketInfo), m_dwThreadRecvBufferSize);

			return;
		}

		CPacket_Write writePacket;

		nResCode = m_CPackage_Queue.GetWritePackage(writePacket, &dwDataSize);

		if(ERROR_PACKAGE_QUEUE_SUCC != nResCode)
		{
			CSystemLog::Instance().Write(LEVEL_LOG_ERROR, " CWorkerThread::AddData failed, resCode:%d !",
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
			CSystemLog::Instance().Write(LEVEL_LOG_ERROR, " CWorkerThread::AddData failed, resCode:%d !",
				nResCode);
		}
	}
}