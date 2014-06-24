#include "GWS_Config.h"

#define GWS_CONFIG_APP_SERVER "GWS_SERVER"
#define GWS_CONFIG_KEY_SERVICE_NAME "ServiceName"
#define GWS_CONFIG_KEY_LOCAL_IP "LocalIP"
#define GWS_CONFIG_KEY_LOG_LEVEL "LogLevel"
#define GWS_CONFIG_KEY_IS_RE_LOAD_WAIT_TIME "ReLoadWaitTime"
#define GWS_CONFIG_KEY_IS_RE_LOAD_DB_INFO_NOW "IsReLoadDBInfoNow"
#define GWS_CONFIG_KEY_IS_AUTH_FROM_LOCAL "IsAuthFromLocal"
#define GWS_CONFIG_KEY_PROJECTID "ProjectID"
#define GWS_CONFIG_KEY_APPTypeID "AppTypeID"


#define GWS_CONFIG_APP_CLIENT "GWS_CLIENT"
#define GWS_CONFIG_KEY_CLIENT_PORT "Port"
#define GWS_CONFIG_KEY_CLIENT_LISTEN_IP "ListenIP"
#define GWS_CONFIG_KEY_CLIENT_MAX_CONNECT_COUNT "MaxConnectCount"
#define GWS_CONFIG_KEY_CLIENT_RECV_BUFFER_SIZE "RecvBufferSize"
#define GWS_CONFIG_KEY_CLIENT_SEND_BUFFER_SIZE "SendBufferSize"
#define GWS_CONFIG_KEY_CLIENT_RECV_BUFFER_COUNT "RecvBufferCount"
#define GWS_CONFIG_KEY_CLIENT_SEND_BUFFER_COUNT "SendBufferCount"
#define GWS_CONFIG_KEY_CLIENT_THREAD_BUFFER_COUNT "ThreadBufferCount"
#define GWS_CONFIG_KEY_CLIENT_WORK_THREAD_COUNT "WorkThreadCount"
#define GWS_CONFIG_KEY_CLIENT_SOCKET_TIME_OUT "SocketTimeout"

#define GWS_CONFIG_APP_APPSRV "GWS_APPSRV"
#define GWS_CONFIG_KEY_APPSRV_PORT "Port"
#define GWS_CONFIG_KEY_APPSRV_LISTEN_IP "ListenIP"
#define GWS_CONFIG_KEY_APPSRV_MAX_CONNECT_COUNT "MaxConnectCount"
#define GWS_CONFIG_KEY_APPSRV_RECV_BUFFER_SIZE "RecvBufferSize"
#define GWS_CONFIG_KEY_APPSRV_SEND_BUFFER_SIZE "SendBufferSize"
#define GWS_CONFIG_KEY_APPSRV_RECV_BUFFER_COUNT "RecvBufferCount"
#define GWS_CONFIG_KEY_APPSRV_SEND_BUFFER_COUNT "SendBufferCount"
#define GWS_CONFIG_KEY_APPSRV_THREAD_BUFFER_COUNT "ThreadBufferCount"
#define GWS_CONFIG_KEY_APPSRV_WORK_THREAD_COUNT "WorkThreadCount"

#define GWS_CONFIG_APP_DB "GWS_DB"
#define GWS_CONFIG_KEY_DB_ADDR "DBAddr"
#define GWS_CONFIG_KEY_DB_NAME "DBName"
#define GWS_CONFIG_KEY_DB_USERNAME "DBUserName"
#define GWS_CONFIG_KEY_DB_PWD "DBPwd"

#define GWS_CONFIG_KEY_DATA "GWS_DATA"
#define GWS_CONFIG_KEY_DATA_IS_SAFE "IsSafe"
#define GWS_CONFIG_KEY_DATA_DB_ADDR "DBAddr"
#define GWS_CONFIG_KEY_DATA_DB_NAME "DBName"
#define GWS_CONFIG_KEY_DATA_DB_USERNAME "DBUserName"
#define GWS_CONFIG_KEY_DATA_DB_PWD "DBPwd"

#define GWS_CONFIG_KEY_LOG "GWS_LOG"
#define GWS_CONFIG_KEY_LOG_IS_ON "IsOn"
#define GWS_CONFIG_KEY_LOG_WORK_THREAD_COUNT "WorkThreadCount"
#define GWS_CONFIG_KEY_LOG_IS_SAFE "IsSafe"
#define GWS_CONFIG_KEY_LOG_DB_ADDR "DBAddr"
#define GWS_CONFIG_KEY_LOG_DB_NAME "DBName"
#define GWS_CONFIG_KEY_LOG_DB_USERNAME "DBUserName"
#define GWS_CONFIG_KEY_LOG_DB_PWD "DBPwd"

namespace GWS
{
	CAppConfig::CAppConfig()
	{
		sprintf(m_szConfigFilePath, "%s\\config\\system.ini", CAppInfo::Instance().GetAppDirectory());
	}

	CAppConfig::~CAppConfig()
	{

	}

	PCSTR CAppConfig::GetConfigFilePath()
	{
		//////////////////////////////////////////////////////////////////////////
		//获取配置文件路径

		return m_szConfigFilePath;
	}

    void CAppConfig::LoadBase()
    {
        CHAR szValue[GWS_TEMP_VALUE_SIZE] = {0};

        //ServiceName
        CIniFile::ReadString(GWS_CONFIG_APP_SERVER, GWS_CONFIG_KEY_SERVICE_NAME, "Qi Fan Web Func Srv", szValue, GWS_TEMP_VALUE_SIZE, m_szConfigFilePath);
        strncpy(m_szServiceName, szValue, MAX_PATH - 1);
    }

	BOOL CAppConfig::Load()
	{
		CHAR szValue[GWS_TEMP_VALUE_SIZE] = {0};
		long lValue = 0;
		long lIndex = 0;

		//////////////////////////////////////////////////////////////////////////
		//GWS_SERVER

		//LocalIP
		CIniFile::ReadString(GWS_CONFIG_APP_SERVER, GWS_CONFIG_KEY_LOCAL_IP, "", szValue, GWS_TEMP_VALUE_SIZE, m_szConfigFilePath);
		strncpy(m_szLocalIP, szValue, SOCKET_IP_MAX_LENGTH);

		if (0 == m_szLocalIP[0])
		{
			strcpy(m_szLocalIP, "0.0.0.0");
		}

		//LogLevel
		lValue = (LONG)CIniFile::ReadInt(GWS_CONFIG_APP_SERVER, GWS_CONFIG_KEY_LOG_LEVEL, 0, m_szConfigFilePath);

		if (lValue <= 0)
		{
			m_logLevel = LEVEL_LOG_FAILD;
		}
		else if (lValue > LEVEL_LOG_DEBUG)
		{
			m_logLevel = LEVEL_LOG_DEBUG;
		}
		else
		{
			m_logLevel = (LEVEL_LOG)lValue;
		}

        //IsAuthFromLocal
        m_bIsAuthFromLocal = (BOOL)CIniFile::ReadInt(GWS_CONFIG_APP_SERVER, GWS_CONFIG_KEY_IS_AUTH_FROM_LOCAL, 0, m_szConfigFilePath);

		//ReLoadWaitTime
		m_dwReLoadWaitTime = (DWORD)CIniFile::ReadInt(GWS_CONFIG_APP_SERVER, GWS_CONFIG_KEY_IS_RE_LOAD_WAIT_TIME, 0, m_szConfigFilePath);

		m_dwReLoadWaitTime = m_dwReLoadWaitTime * 1000 * 60;

		if (0 == m_dwReLoadWaitTime)
		{
			m_dwReLoadWaitTime = INFINITE;
		}

		//m_dwProjectID
		m_dwProjectID = (DWORD)CIniFile::ReadInt(GWS_CONFIG_APP_SERVER, GWS_CONFIG_KEY_PROJECTID, 0, m_szConfigFilePath);

		//m_dwAppTypeID
		m_dwAppTypeID = (DWORD)CIniFile::ReadInt(GWS_CONFIG_APP_SERVER, GWS_CONFIG_KEY_APPTypeID, 0, m_szConfigFilePath);

		//////////////////////////////////////////////////////////////////////////
		//GWS_CLIENT

		//ListenIP
		CIniFile::ReadString(GWS_CONFIG_APP_CLIENT, GWS_CONFIG_KEY_CLIENT_LISTEN_IP, "", szValue, GWS_TEMP_VALUE_SIZE, m_szConfigFilePath);
		strncpy(m_szClientListenIP, szValue, SOCKET_IP_MAX_LENGTH);

        if (NULL == m_szClientListenIP[0])
        {
            strcpy(m_szClientListenIP, "0.0.0.0");
        }

		//Port
		lValue = (LONG)CIniFile::ReadInt(GWS_CONFIG_APP_CLIENT, GWS_CONFIG_KEY_CLIENT_PORT, 0, m_szConfigFilePath);

		if (lValue > 65535 || lValue <= 0)
		{
			return FALSE;
		}

		m_wClientPort = (WORD)lValue;

		//MaxConnectCount
		lValue = (LONG)CIniFile::ReadInt(GWS_CONFIG_APP_CLIENT, GWS_CONFIG_KEY_CLIENT_MAX_CONNECT_COUNT, 0, m_szConfigFilePath);

		if (lValue > 65535 || lValue <= 0)
		{
			return FALSE;
		}

		m_wClientMaxConnectCount = (WORD)lValue;
		
		//RecvBufferSize
		m_dwClientRecvBufferSize = (DWORD)CIniFile::ReadInt(GWS_CONFIG_APP_CLIENT, GWS_CONFIG_KEY_CLIENT_RECV_BUFFER_SIZE, 0, m_szConfigFilePath);

		if (m_dwClientRecvBufferSize < CLIENT_MIN_RECV_BUFFER_SIZE)
		{
			m_dwClientRecvBufferSize = CLIENT_MIN_RECV_BUFFER_SIZE;
		}

		//SendBufferSize
		m_dwClientSendBufferSize = (DWORD)CIniFile::ReadInt(GWS_CONFIG_APP_CLIENT, GWS_CONFIG_KEY_CLIENT_SEND_BUFFER_SIZE, 0, m_szConfigFilePath);

		if (m_dwClientSendBufferSize < CLIENT_MIN_SEND_BUFFER_SIZE)
		{
			m_dwClientSendBufferSize = CLIENT_MIN_SEND_BUFFER_SIZE;
		}

        //RecvBufferCount
        m_dwClientRecvBufferCount = (DWORD)GetPrivateProfileIntA(GWS_CONFIG_APP_CLIENT, GWS_CONFIG_KEY_CLIENT_RECV_BUFFER_COUNT, 0, m_szConfigFilePath);

        if (0 == m_dwClientRecvBufferCount)
        {
            m_dwClientRecvBufferCount = 1;
        }

        //SendBufferCount
        m_dwClientSendBufferCount = (DWORD)GetPrivateProfileIntA(GWS_CONFIG_APP_CLIENT, GWS_CONFIG_KEY_CLIENT_SEND_BUFFER_COUNT, 0, m_szConfigFilePath);

        if (0 == m_dwClientSendBufferCount)
        {
            m_dwClientSendBufferCount = 500;
        }

		//ThreadBufferCount
		lValue = (LONG)CIniFile::ReadInt(GWS_CONFIG_APP_CLIENT, GWS_CONFIG_KEY_CLIENT_THREAD_BUFFER_COUNT, 0, m_szConfigFilePath);

		if (lValue > 65535 || lValue <= 0)
		{
			m_wClientThreadBufferCount = 10;
		}

		m_wClientThreadBufferCount = (WORD)lValue;

		//WorkThreadCount
		lValue = (LONG)CIniFile::ReadInt(GWS_CONFIG_APP_CLIENT, GWS_CONFIG_KEY_CLIENT_WORK_THREAD_COUNT, 0, m_szConfigFilePath);

		if (lValue > 65535 || lValue <= 0)
		{
			m_wClientWorkThreadCount = 10;
		}

		m_wClientWorkThreadCount = (WORD)lValue;

		//////////////////////////////////////////////////////////////////////////
		//GWS_APPSRV
		
		//ListenIP
		CIniFile::ReadString(GWS_CONFIG_APP_APPSRV, GWS_CONFIG_KEY_APPSRV_LISTEN_IP, "", szValue, GWS_TEMP_VALUE_SIZE, m_szConfigFilePath);
		strncpy(m_szAppSrvListenIP, szValue, SOCKET_IP_MAX_LENGTH);

        if (NULL == m_szAppSrvListenIP[0])
        {
            strcpy(m_szAppSrvListenIP, "0.0.0.0");
        }

		//Port
		lValue = (LONG)CIniFile::ReadInt(GWS_CONFIG_APP_APPSRV, GWS_CONFIG_KEY_APPSRV_PORT, 0, m_szConfigFilePath);

		if (lValue > 65535 || lValue <= 0)
		{
			return FALSE;
		}

		m_wAppSrvPort = (WORD)lValue;

		//MaxAppSrvConnectCount
		lValue = (LONG)CIniFile::ReadInt(GWS_CONFIG_APP_APPSRV, GWS_CONFIG_KEY_APPSRV_MAX_CONNECT_COUNT, 0, m_szConfigFilePath);

		if (lValue > 65535 || lValue <= 0)
		{
			return FALSE;
		}

		m_wAppSrvMaxConnectCount = (WORD)lValue;

		//RecvBufferSize
		m_dwAppSrvRecvBufferSize = (DWORD)CIniFile::ReadInt(GWS_CONFIG_APP_APPSRV, GWS_CONFIG_KEY_APPSRV_RECV_BUFFER_SIZE, 0, m_szConfigFilePath);

		if (m_dwAppSrvRecvBufferSize < APPSRV_MIN_RECV_BUFFER_SIZE)
		{
			m_dwAppSrvRecvBufferSize = APPSRV_MIN_RECV_BUFFER_SIZE;
		}

		//SendBufferSize
		m_dwAppSrvSendBufferSize = (DWORD)CIniFile::ReadInt(GWS_CONFIG_APP_APPSRV, GWS_CONFIG_KEY_APPSRV_SEND_BUFFER_SIZE, 0, m_szConfigFilePath);

		if (m_dwAppSrvSendBufferSize < APPSRV_MIN_SEND_BUFFER_SIZE)
		{
			m_dwAppSrvSendBufferSize = APPSRV_MIN_SEND_BUFFER_SIZE;
		}

		//RecvBufferCount
		m_dwAppSrvRecvBufferCount = (DWORD)GetPrivateProfileIntA(GWS_CONFIG_APP_APPSRV, GWS_CONFIG_KEY_APPSRV_RECV_BUFFER_COUNT, 0, m_szConfigFilePath);

		if (0 == m_dwAppSrvRecvBufferCount)
		{
			m_dwAppSrvRecvBufferCount = 1;
		}

		//SendBufferCount
		m_dwAppSrvSendBufferCount = (DWORD)GetPrivateProfileIntA(GWS_CONFIG_APP_APPSRV, GWS_CONFIG_KEY_APPSRV_SEND_BUFFER_COUNT, 0, m_szConfigFilePath);

		if (0 == m_dwAppSrvSendBufferCount)
		{
			m_dwAppSrvSendBufferCount = 500;
		}

		//ThreadBufferCount
		lValue = (LONG)CIniFile::ReadInt(GWS_CONFIG_APP_APPSRV, GWS_CONFIG_KEY_APPSRV_THREAD_BUFFER_COUNT, 0, m_szConfigFilePath);

		if (lValue > 65535 || lValue <= 0)
		{
			m_wAppSrvThreadBufferCount = 1000;
		}

		m_wAppSrvThreadBufferCount = (WORD)lValue;

		//WorkThreadCount
		lValue = (LONG)CIniFile::ReadInt(GWS_CONFIG_APP_APPSRV, GWS_CONFIG_KEY_APPSRV_WORK_THREAD_COUNT, 0, m_szConfigFilePath);

		if (lValue > 65535 || lValue <= 0)
		{
			m_wAppSrvThreadBufferCount = 10;
		}

		m_wAppSrvWorkThreadCount = (WORD)lValue;
        
		//////////////////////////////////////////////////////////////////////////
		//GWS_DATA

        m_bIsSafe = (BOOL)GetPrivateProfileIntA(GWS_CONFIG_KEY_DATA, GWS_CONFIG_KEY_DATA_IS_SAFE, 0, m_szConfigFilePath);

		//DBAddr
		GetPrivateProfileStringA(GWS_CONFIG_KEY_DATA, GWS_CONFIG_KEY_DATA_DB_ADDR, "", szValue, GWS_TEMP_VALUE_SIZE, m_szConfigFilePath);
		strncpy(m_szDBAddr, szValue, GWS_DB_STR_MAX_LENGTH);

		//DBName
		GetPrivateProfileStringA(GWS_CONFIG_KEY_DATA, GWS_CONFIG_KEY_DATA_DB_NAME, "", szValue, GWS_TEMP_VALUE_SIZE, m_szConfigFilePath);
		strncpy(m_szDBName, szValue, GWS_DB_STR_MAX_LENGTH);

		//DBUserName
		GetPrivateProfileStringA(GWS_CONFIG_KEY_DATA, GWS_CONFIG_KEY_DATA_DB_USERNAME, "", szValue, GWS_TEMP_VALUE_SIZE, m_szConfigFilePath);
		strncpy(m_szDBUserName, szValue, GWS_DB_STR_MAX_LENGTH);

		//DBPwd
		GetPrivateProfileStringA(GWS_CONFIG_KEY_DATA, GWS_CONFIG_KEY_DATA_DB_PWD, "", szValue, GWS_TEMP_VALUE_SIZE, m_szConfigFilePath);
		strncpy(m_szDBPwd, szValue, GWS_DB_STR_MAX_LENGTH);

		//////////////////////////////////////////////////////////////////////////
		//GWS_LOGDB

		m_bGWSLogIsOn = (BOOL)GetPrivateProfileIntA(GWS_CONFIG_KEY_LOG, GWS_CONFIG_KEY_LOG_IS_ON, 0, m_szConfigFilePath);

		m_bLogIsSafe = (BOOL)GetPrivateProfileIntA(GWS_CONFIG_KEY_LOG, GWS_CONFIG_KEY_LOG_IS_SAFE, 0, m_szConfigFilePath);

		//DBAddr
		GetPrivateProfileStringA(GWS_CONFIG_KEY_LOG, GWS_CONFIG_KEY_LOG_DB_ADDR, "", szValue, GWS_TEMP_VALUE_SIZE, m_szConfigFilePath);
		strncpy(m_szLogDBAddr, szValue, GWS_DB_STR_MAX_LENGTH);

		//DBName
		GetPrivateProfileStringA(GWS_CONFIG_KEY_LOG, GWS_CONFIG_KEY_LOG_DB_NAME, "", szValue, GWS_TEMP_VALUE_SIZE, m_szConfigFilePath);
		strncpy(m_szLogDBName, szValue, GWS_DB_STR_MAX_LENGTH);

		//DBUserName
		GetPrivateProfileStringA(GWS_CONFIG_KEY_LOG, GWS_CONFIG_KEY_LOG_DB_USERNAME, "", szValue, GWS_TEMP_VALUE_SIZE, m_szConfigFilePath);
		strncpy(m_szLogDBUserName, szValue, GWS_DB_STR_MAX_LENGTH);

		//DBPwd
		GetPrivateProfileStringA(GWS_CONFIG_KEY_LOG, GWS_CONFIG_KEY_LOG_DB_PWD, "", szValue, GWS_TEMP_VALUE_SIZE, m_szConfigFilePath);
		strncpy(m_szLogDBPwd, szValue, GWS_DB_STR_MAX_LENGTH);

		lValue = (LONG)CIniFile::ReadInt(GWS_CONFIG_KEY_LOG, GWS_CONFIG_KEY_LOG_WORK_THREAD_COUNT, 0, m_szConfigFilePath);
		if (lValue > 65535 || lValue <= 0)
		{
			m_wOmasWorkThreadCount = 10;
		}
		m_wOmasWorkThreadCount = (WORD)lValue;

		return TRUE;
	}

	void CAppConfig::ReLoad()
	{
		TCHAR szValue[GWS_TEMP_VALUE_SIZE] = {0};
		long lValue = 0;

		//LogLevel
		lValue = (LONG)CIniFile::ReadInt(GWS_CONFIG_APP_SERVER, GWS_CONFIG_KEY_LOG_LEVEL, 0, m_szConfigFilePath);

		if (lValue <= 0)
		{
			m_logLevel = LEVEL_LOG_FAILD;
		}
		else if (lValue > LEVEL_LOG_DEBUG)
		{
			m_logLevel = LEVEL_LOG_DEBUG;
		}
		else
		{
			m_logLevel = (LEVEL_LOG)lValue;
		}

        //IsAuthFromLocal
        m_bIsAuthFromLocal = (BOOL)CIniFile::ReadInt(GWS_CONFIG_APP_SERVER, GWS_CONFIG_KEY_IS_AUTH_FROM_LOCAL, 0, m_szConfigFilePath);

		//IsReLoadDBInfoNow
		m_bIsReLoadDBInfoNow = (BOOL)GetPrivateProfileIntA(GWS_CONFIG_APP_SERVER, GWS_CONFIG_KEY_IS_RE_LOAD_DB_INFO_NOW, 0, m_szConfigFilePath);
	}

	PCSTR CAppConfig::GetServiceName()
	{
		return m_szServiceName;
	}

	PCSTR CAppConfig::GetLocalIP()
	{
		return m_szLocalIP;
	}

	LEVEL_LOG CAppConfig::GetLogLevel()
	{
		return m_logLevel;
	}

	DWORD CAppConfig::GetReLoadWaitTime()
	{
		return m_dwReLoadWaitTime;
	}

	BOOL CAppConfig::GetIsReLoadDBInfoNow()
	{    
		if (m_bIsReLoadDBInfoNow)
		{       
			WritePrivateProfileStringA(GWS_CONFIG_APP_SERVER, GWS_CONFIG_KEY_IS_RE_LOAD_DB_INFO_NOW, "0", m_szConfigFilePath);

			m_bIsReLoadDBInfoNow = FALSE;

			return TRUE;
		}

		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	//CLIENT

	PCSTR CAppConfig::GetClientListenIP()
	{
		if (m_szClientListenIP[0] == 0)
		{
			return m_szLocalIP;
		}

		return m_szClientListenIP;
	}

	WORD CAppConfig::GetClientPort()
	{
		return m_wClientPort;
	}

	WORD CAppConfig::GetClientMaxConnectCount()
	{
		return m_wClientMaxConnectCount;
	}

	DWORD CAppConfig::GetClientRecvBufferSize()
	{
		return m_dwClientRecvBufferSize;
	}

	DWORD CAppConfig::GetClientSendBufferSize()
	{
		return m_dwClientSendBufferSize;
	}

    DWORD CAppConfig::GetClientRecvBufferCount()
    {
        return m_dwClientRecvBufferCount;
    }

    DWORD CAppConfig::GetClientSendBufferCount()
    {
        return m_dwClientSendBufferCount;
    }

	WORD CAppConfig::GetClientThreadBufferCount()
	{
		return m_wClientThreadBufferCount;
	}

	WORD CAppConfig::GetClientWorkThreadCount()
	{
		return m_wClientWorkThreadCount;
	}

    BOOL CAppConfig::GetIsAuthFromLocal()
    {
        return m_bIsAuthFromLocal;
    }

	//////////////////////////////////////////////////////////////////////////
	//APPSRV

	PCSTR CAppConfig::GetAppSrvListenIP()
	{
		if (m_szAppSrvListenIP[0] == 0)
		{
			return m_szLocalIP;
		}

		return m_szAppSrvListenIP;
	}

	WORD CAppConfig::GetAppSrvPort()
	{
		return m_wAppSrvPort;
	}

	WORD CAppConfig::GetAppSrvMaxConnectCount()
	{
		return m_wAppSrvMaxConnectCount;
	}

	DWORD CAppConfig::GetAppSrvRecvBufferSize()
	{
		return m_dwAppSrvRecvBufferSize;
	}

	DWORD CAppConfig::GetAppSrvSendBufferSize()
	{
		return m_dwAppSrvSendBufferSize;
	}

	DWORD CAppConfig::GetAppSrvRecvBufferCount()
	{
		return m_dwAppSrvRecvBufferCount;
	}

	DWORD CAppConfig::GetAppSrvSendBufferCount()
	{
		return m_dwAppSrvSendBufferCount;
	}

	WORD CAppConfig::GetAppSrvThreadBufferCount()
	{
		return m_wAppSrvThreadBufferCount;
	}

	WORD CAppConfig::GetAppSrvWorkThreadCount()
	{
		return m_wAppSrvWorkThreadCount;
	}

	//////////////////////////////////////////////////////////////////////////
    BOOL CAppConfig::GetIsSafe()
    {
        return m_bIsSafe;
    }

	PCSTR CAppConfig::GetDBAddr()
	{
		return m_szDBAddr;
	}

	PCSTR CAppConfig::GetDBName()
	{
		return m_szDBName;
	}

	PCSTR CAppConfig::GetDBUserName()
	{
		return m_szDBUserName;
	}

	PCSTR CAppConfig::GetDBPwd()
	{
		return m_szDBPwd;
	}

	DWORD CAppConfig::GetProjectID()
	{
		return m_dwProjectID;
	}

	DWORD CAppConfig::GetAppTypeID()
	{
		return m_dwAppTypeID;
	}

	//////////////////////////////////////////////////////////////////////////

	BOOL CAppConfig::GetLogIsOn()
	{
		return m_bGWSLogIsOn;
	}

	BOOL CAppConfig::GetLogIsSafe()
	{
		return m_bLogIsSafe;
	}

	PCSTR CAppConfig::GetLogDBAddr()
	{
		return m_szLogDBAddr;
	}

	PCSTR CAppConfig::GetLogDBName()
	{
		return m_szLogDBName;
	}

	PCSTR CAppConfig::GetLogDBUserName()
	{
		return m_szLogDBUserName;
	}

	PCSTR CAppConfig::GetLogDBPwd()
	{
		return m_szLogDBPwd;
	}

	WORD CAppConfig::GetOmasWorkThreadCount()
	{
		return m_wOmasWorkThreadCount;
	}
}