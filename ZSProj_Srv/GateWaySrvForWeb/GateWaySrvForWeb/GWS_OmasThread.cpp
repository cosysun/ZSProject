#include "GWS_OmasThread.h"
#include "GWS_Log.h"
#include "GWS_SocketServer.h"
#include "GWS_AppSrvSocketServer.h"
#include "GWS_Config.h"

#define GWS_SNMP_SEND_INTERVAL_TIME 60000   //一分钟发一次统计信息

namespace GWS
{  
	//////////////////////////////////////////////////////////////////////////
	//CSnmpThread

	COmasThread::COmasThread(): CCustomThread(NULL)
	{
		ZeroMemory(&m_Snmp, sizeof(LONG) * COmasThread::OMAS_TYPE_MAX_COUNT);

		m_bStop = TRUE;
	}

	COmasThread::~COmasThread()
	{
		this->Stop();
	}

	BOOL COmasThread::Start()
	{
		if (!m_bStop)
		{
			return TRUE;
		}

		m_StopEvent.Reset();

		if (!CCustomThread::Start())
		{
			CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CAppSnmpThread::Start thread start failed !");

			this->Stop();

			return FALSE;
		}

		return TRUE;
	}

	void COmasThread::Stop()
	{
		if (m_bStop)
		{	
			return;
		}

		m_bStop = TRUE;

		m_StopEvent.Set();

		CCustomThread::Stop(STOP_WAIT_MILLISECOND, 0);
	}

	void COmasThread::Increment(EOMAS_TYPE snmpType)
	{
		if (snmpType >= COmasThread::OMAS_TYPE_MAX_COUNT)
		{
			return;
		}

		InterlockedIncrement(&m_Snmp[snmpType]);
	}

	void COmasThread::Decrement(EOMAS_TYPE snmpType)
	{
		if (snmpType >= COmasThread::OMAS_TYPE_MAX_COUNT)
		{
			return;
		}

		InterlockedDecrement(&m_Snmp[snmpType]);
	}

	DWORD COmasThread::Proc(LPVOID lpParameter)
	{
		m_bStop = FALSE;

		while (!m_bStop)
		{
			if (m_StopEvent.Wait(GWS_SNMP_SEND_INTERVAL_TIME))
			{
				break;
			}
			else
			{
				this->SendSnmpInfo();
			}
		}

		return 0;
	}

	void COmasThread::SendSnmpInfo()
	{
		WORD wIndex = 0;
		LONG aSnmp[COmasThread::OMAS_TYPE_MAX_COUNT] = {0};

		for (; wIndex < COmasThread::OMAS_TYPE_MAX_COUNT; ++wIndex)
		{
			aSnmp[wIndex] = InterlockedExchange(&m_Snmp[wIndex], 0);
		}

		aSnmp[OMAS_TYPE_CLIENT_CONNECT_COUNT] = CSocketServer::Instance().GetConnCount();

		aSnmp[OMAS_TYPE_APPSRV_CONNECT_COUNT] = CAppSrvSocketServer::Instance().GetConnectedCount();

		CSnmpLog::Instance().WriteEx(
			LEVEL_LOG_INFO, 
"ClientRecvCount:%ld, ClientSendCount:%ld, ClientConnectCount:%ld\n\
ClientReq:%ld, ClientReqSuccess:%ld, ClientReqFailed:%ld\n\
ClientLogout:%ld, ClientKeepAlive:%ld, ClientOmas:%ld, ClientUnknown:%ld\n\
AppSrvRecvCount:%ld, AppSrvSendCount:%ld, AppSrvConnectCount:%ld\n\
AppSrvLogin:%ld, AppSrvLoginSuccess:%ld, AppSrvLoginFailed:%ld\n\
AppSrvReq:%ld, AppSrvReqSuccess:%ld, AppSrvReqFailed:%ld \n\
AppSrvLogout:%ld, AppSrvKeepAlive:%ld, AppSrvSystem:%ld, AppSrvUnknown:%ld",
			aSnmp[OMAS_TYPE_CLIENT_RECV_COUNT], aSnmp[OMAS_TYPE_CLIENT_SEND_COUNT], aSnmp[OMAS_TYPE_CLIENT_CONNECT_COUNT],
			aSnmp[OMAS_TYPE_CLIENT_REQ], aSnmp[OMAS_TYPE_CLIENT_REQ_SUCCESS], aSnmp[OMAS_TYPE_CLIENT_REQ_FAILED],
            aSnmp[OMAS_TYPE_CLIENT_LOGOUT], aSnmp[OMAS_TYPE_CLIENT_KEEPALIVE], aSnmp[OMAS_TYPE_CLIENT_OMAS], aSnmp[OMAS_TYPE_CLIENT_UNKNOWN],
			aSnmp[OMAS_TYPE_APPSRV_RECV_COUNT], aSnmp[OMAS_TYPE_APPSRV_SEND_COUNT], aSnmp[OMAS_TYPE_APPSRV_CONNECT_COUNT],
			aSnmp[OMAS_TYPE_APPSRV_LOGIN], aSnmp[OMAS_TYPE_APPSRV_LOGIN_SUCCESS], aSnmp[OMAS_TYPE_APPSRV_LOGIN_FAILED],
			aSnmp[OMAS_TYPE_APPSRV_RES], aSnmp[OMAS_TYPE_APPSRV_RES_SUCCESS], aSnmp[OMAS_TYPE_APPSRV_RES_FAILED],
            aSnmp[OMAS_TYPE_APPSRV_LOGOUT], aSnmp[OMAS_TYPE_APPSRV_KEEPALIVE], aSnmp[OMAS_TYPE_APPSRV_SYSTEM], aSnmp[OMAS_TYPE_APPSRV_UNKNOWN]);
	}
}