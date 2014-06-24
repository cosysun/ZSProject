#include "GWS_AppSrvSocketServer.h"
#include "GWS_Config.h"
#include "GWS_Log.h"
#include "GWS_OmasThread.h"
#include "GWS_AppSrvWorkerThread.h"
#include "GWS_AppSrvAuthMgr.h"

namespace GWS
{
	//////////////////////////////////////////////////////////////////////////
	//CAppSrvSocketServer

	CAppSrvSocketServer::CAppSrvSocketServer()
	{
		m_bStop = TRUE;
	}

	CAppSrvSocketServer::~CAppSrvSocketServer()
	{
		this->Stop();
	}

	BOOL CAppSrvSocketServer::Start()
	{
		if (!m_bStop)
		{
			return TRUE;
		}

		m_bStop = FALSE;
		
		SYSTEM_INFO info;
		GetSystemInfo(&info);

		m_CNetIOCP.SetLogLevel(LEVEL_LOG_FAILD);

		if (ERROR_NETSRV_SUCC != m_CNetIOCP.NetServices_Start(
			inet_addr(CAppConfig::Instance().GetAppSrvListenIP()),
			CAppConfig::Instance().GetAppSrvPort(),
			CAppConfig::Instance().GetAppSrvMaxConnectCount(),
			CAppConfig::Instance().GetAppSrvRecvBufferSize(),
			CAppConfig::Instance().GetAppSrvSendBufferSize(),
			(USHORT)CAppConfig::Instance().GetAppSrvRecvBufferCount(),
			(USHORT)CAppConfig::Instance().GetAppSrvSendBufferCount(),
			info.dwNumberOfProcessors,
			30,
			this))
		{
			CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CAppSrvSocketServer::Start CNetIOCP.CreateNetIOCPService is failed !");

			m_bStop = TRUE;

			return FALSE;
		}

		CSystemLog::Instance().Write(LEVEL_LOG_DEBUG, "CAppSrvSocketServer::Start CNetIOCP.CreateNetIOCPService is success !");

		m_CNetIOCP.SetPackageHeadInfo(0, sizeof(DWORD), sizeof(GWS_APPSRV_HEAD));
		
		m_CNetIOCP.SetLogLevel(LEVEL_LOG_FAILD);

		return TRUE;
	}

	void CAppSrvSocketServer::Stop()
	{
		if (m_bStop)
		{
			return;
		}

		m_bStop = TRUE;

		try
		{
			m_CNetIOCP.NetServices_Stop();
		}
		catch (...)
		{
		}
	}

	void CAppSrvSocketServer::PostSend(void* pDataInfo,long nDataLen,long nSocketID)
	{
		if (m_bStop)
		{
			return;
		}

		COmasThread::Instance().Increment(COmasThread::OMAS_TYPE_APPSRV_SEND_COUNT);

		m_CNetIOCP.SendData(nSocketID, pDataInfo, nDataLen);
	}

	void CAppSrvSocketServer::CloseSocket(long nSocketID)
	{
		if (m_bStop)
		{
			return;
		}

        CAppSrvAuthMgr::Instance().RemoveAppSrv(nSocketID);

		m_CNetIOCP.CloseSocket(nSocketID);
	}

	long CAppSrvSocketServer::GetConnectedCount()
	{
		return m_CNetIOCP.GetSocketCount_Connected();
	}

	Int CAppSrvSocketServer::NotifyRecvData(Void* pData,Long nDataLen,ULong ulSourceIP,Int nSourcePort,Long  nSocketID)
	{
		if (!pData || 0 >= nDataLen || m_bStop)
		{
			return 0;
		}

		COmasThread::Instance().Increment(COmasThread::OMAS_TYPE_APPSRV_RECV_COUNT);

		CAppSrvWorkerThread::Instance().AddData((PCHAR)pData, nDataLen, (DWORD)ulSourceIP, nSocketID);

		return 0;
	}

	Int CAppSrvSocketServer::NotifyNewClientConnected(ULong ulRemoteIP,Int  nRemotePort,Long nSocketID)
	{
		char szIP[33] = {0};
		ToStringIP(ulRemoteIP, szIP);

		CAppSrvLog::Instance().Write(
			LEVEL_LOG_INFO, 
			"CAppSrvSocketServer::NotifyNewClientConnected Client Connected, SocketID:%ld, IP:%s, port:%d", 
			nSocketID, szIP, nRemotePort);

		return 0;
	}

	Int CAppSrvSocketServer::NotifyClientClosed(ULong ulRemoteIP,Int nRemotePort,Long nSocketID,ERROR_NETSERVICES errorCode)
	{
		char szIP[33] = {0};
		ToStringIP(ulRemoteIP, szIP);

		CAppSrvLog::Instance().Write(
			LEVEL_LOG_INFO, 
			"CAppSrvSocketServer::NotifyClientClosed Client Closeed, SocketID:%ld, IP:%s, port:%d", 
			nSocketID, szIP, nRemotePort);

        CAppSrvAuthMgr::Instance().RemoveAppSrv(nSocketID);

		return 0;
	}

	Int CAppSrvSocketServer::NotifyConnectRemoteSrv(ULong ulRemoteIP,Int nRemotePort,Long nSocketID,ERROR_NETSERVICES errorCode)
	{
		return 0;
	}
}