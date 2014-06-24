#include "GWS_SocketServer.h"
#include "GWS_Config.h"
#include "GWS_Log.h"
#include "GWS_OmasThread.h"
#include "GWS_WorkerThread.h"
#include "GWS_DataMgr.h"

namespace GWS
{
	//////////////////////////////////////////////////////////////////////////
	//CSocketServer

	CSocketServer::CSocketServer()
	{
		m_bStop = TRUE;
	}

	CSocketServer::~CSocketServer()
	{
		this->Stop();
	}

	BOOL CSocketServer::Start()
	{
		if (!m_bStop)
		{
			return TRUE;
		}

		m_bStop = FALSE;

        SYSTEM_INFO info;
        GetSystemInfo(&info);

		if (m_Server.NetServices_Start(
            inet_addr(CAppConfig::Instance().GetClientListenIP()),
			CAppConfig::Instance().GetClientPort(),
			CAppConfig::Instance().GetClientMaxConnectCount(),
			CAppConfig::Instance().GetClientRecvBufferSize(),
            CAppConfig::Instance().GetClientSendBufferSize(),
            CAppConfig::Instance().GetClientRecvBufferCount(),  //MaxCount_RecvPack
            CAppConfig::Instance().GetClientSendBufferCount(),  //MaxCount_SendPack
            (SHORT)info.dwNumberOfProcessors,  //ThreadCount_HandleMsg
            30,
            this))
		{
			CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CSocketServer::Start web server is failed !");

			m_bStop = TRUE;

			return FALSE;
		}

        m_Server.SetProtocolType(PROTOCOL_TYPE_HTTP);
		
		m_Server.SetLogLevel(CAppConfig::Instance().GetLogLevel());

		CSystemLog::Instance().Write(LEVEL_LOG_DEBUG, "CSocketServer::Start web server is success !");

		return TRUE;
	}

	void CSocketServer::Stop()
	{
		if (m_bStop)
		{
			return;
		}

		m_bStop = TRUE;

		m_Server.NetServices_Stop();
	}

	void CSocketServer::PostSend(PCHAR pData, DWORD dwSize, LONG lSocketID)
	{
        if (m_bStop)
        {
            return;
        }

		COmasThread::Instance().Increment(COmasThread::OMAS_TYPE_CLIENT_SEND_COUNT);

		m_Server.SendData((LONG)lSocketID, pData, (LONG)dwSize);
	}

	void CSocketServer::CloseSocket(LONG lSocketID)
	{
        if (m_bStop)
        {
            return;
        }

		m_Server.CloseSocket(lSocketID);

        //////////////////////////////////////////////////////////////////////////

        CClientLog::Instance().Write(
            LEVEL_LOG_DEBUG, 
            "CSocketServer::CloseSocket Client Closed, SocketID:%ld,", 
            lSocketID);

        //////////////////////////////////////////////////////////////////////////
	}

	WORD CSocketServer::GetConnCount()
	{
        if (m_bStop)
        {
            return 0;
        }

		return (WORD)m_Server.GetSocketCount_Connected();
	}

    /*------------------------------
    *   notify recv data
    *  return value,0(succ),else :errorcode
    ------------------------------*/
    Int CSocketServer::NotifyRecvData(Void* pData, Long nDataLen,ULong ulSourceIP,Int nSourcePort,Long  nSocketID)
    {
        if (!pData || 0 >= nDataLen)
        {
            return 0;
        }

        COmasThread::Instance().Increment(COmasThread::OMAS_TYPE_CLIENT_RECV_COUNT);

        CWorkerThread::Instance().AddData((PCHAR)pData, (DWORD)nDataLen, ulSourceIP, (DWORD)nSocketID);

        return 0;
    }

    /*------------------------------
    *   notify new client connected
    *  return value,0(succ),else :errorcode
    ------------------------------*/
    Int CSocketServer::NotifyNewClientConnected(ULong ulRemoteIP,Int  nRemotePort,Long nSocketID)
    {
        char szIP[33] = {0};
        ToStringIP(ulRemoteIP, szIP);

        CClientLog::Instance().Write(
            LEVEL_LOG_INFO, 
            "CSocketServer::NotifyNewClientConnected Client Connected, SocketID:%ld, IP:%s", 
            nSocketID, szIP);

        return 0;
    }

    /*------------------------------
    *   notify client closed
    *  return value,0(succ),else :errorcode
    ------------------------------*/
    Int CSocketServer::NotifyClientClosed(ULong ulRemoteIP,Int  nRemotePort,Long nSocketID,ERROR_NETSERVICES errorCode)
    {
        char szIP[33] = {0};
        ToStringIP(ulRemoteIP, szIP);

        CClientLog::Instance().Write(
            LEVEL_LOG_INFO, 
            "CSocketServer::NotifyClientClosed Client Closed, SocketID:%ld, IP:%s, ErrorCode:%d", 
            nSocketID, szIP, (int)errorCode);

        return 0;
    }

    /*------------------------------
    *   notify the status of connected remote srv
    * return value,0(succ),else :errorcode
    ------------------------------*/
    Int CSocketServer::NotifyConnectRemoteSrv(ULong ulRemoteIP,Int  nRemotePort,Long nSocketID,ERROR_NETSERVICES errorCode)
    {
        return 0;
    }
}