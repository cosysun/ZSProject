#include "GWS_Log.h"

#define GWS_LOG_BUFFER_SIZE 2048

namespace GWS
{
	//////////////////////////////////////////////////////////////////////////

	CLog::CLog()
	{

	}

	CLog::~CLog()
	{

	}

	void CLog::Write(LEVEL_LOG logLevel, LPCSTR lpFormat, ...)
	{
		char szLogBuf[GWS_LOG_BUFFER_SIZE]  = {0};
		int nLen = 0;
		va_list args;

		va_start(args, lpFormat);
		nLen = vsprintf(szLogBuf, lpFormat, args);
		va_end(args);

		m_CLogFile.WriteLogInfo(szLogBuf, logLevel);
	}

	void CLog::WriteEx(LEVEL_LOG logLevel, LPCSTR lpFormat, ...)
	{
		char szLogBuf[GWS_LOG_BUFFER_SIZE]  = {0};
		szLogBuf[0] = '\n';
		int nLen = 0;
		va_list args;

		va_start(args, lpFormat);
		nLen = vsprintf(szLogBuf + 1, lpFormat, args);
		va_end(args);

		szLogBuf[nLen + 1] = '\n';
		szLogBuf[nLen + 2] = 0;

		m_CLogFile.WriteLogInfo(szLogBuf, logLevel);
	}

	void CLog::SetLogLevel(LEVEL_LOG logLevel)
	{
		m_CLogFile.SetDebugLevel(logLevel);
	}

	//////////////////////////////////////////////////////////////////////////

	CSystemLog::CSystemLog()
	{

	}

	CSystemLog::~CSystemLog()
	{

	}

	void CSystemLog::Init()
	{
		CHAR szDirPath[MAX_PATH * 2] = {0};

		sprintf(szDirPath, "%s\\Log\\System", CAppInfo::Instance().GetAppDirectory()); 

		SHCreateDirectoryEx(NULL, szDirPath, NULL);

		m_CLogFile.InitLogFile(szDirPath, "SystemLog.txt", LEVEL_LOG_INFO);

		CLog::Write(LEVEL_LOG_INFO, "CSystemLog::CSystemLog Version:%lu", APP_VERSION_NUM);
	}

	//////////////////////////////////////////////////////////////////////////

	CClientLog::CClientLog()
	{

	}

	CClientLog::~CClientLog()
	{

	}

	void CClientLog::Init()
	{
		CHAR szDirPath[MAX_PATH * 2] = {0};

		sprintf(szDirPath, "%s\\Log\\Client", CAppInfo::Instance().GetAppDirectory()); 

		SHCreateDirectoryEx(NULL, szDirPath, NULL);

		m_CLogFile.InitLogFile(szDirPath, "ClientLog.txt", LEVEL_LOG_INFO);

		CLog::Write(LEVEL_LOG_INFO, "CClientLog::CClientLog Version:%lu", APP_VERSION_NUM);
	}


	//////////////////////////////////////////////////////////////////////////
	//应用服务相关日志

	CAppSrvLog::CAppSrvLog()
	{

	}

	CAppSrvLog::~CAppSrvLog()
	{

	}

	void CAppSrvLog::Init()
	{
		CHAR szDirPath[MAX_PATH * 2] = {0};

		sprintf(szDirPath, "%s\\Log\\AppSrv", CAppInfo::Instance().GetAppDirectory()); 

		SHCreateDirectoryEx(NULL, szDirPath, NULL);

		m_CLogFile.InitLogFile(szDirPath, "AppSrvLog.txt", LEVEL_LOG_INFO);

		CLog::Write(LEVEL_LOG_INFO, "CAppSrvLog::CAppSrvLog Version:%lu", APP_VERSION_NUM);
	}

	//////////////////////////////////////////////////////////////////////////

	CSnmpLog::CSnmpLog()
	{

	}

	CSnmpLog::~CSnmpLog()
	{

	}

	void CSnmpLog::Init()
	{
		CHAR szDirPath[MAX_PATH * 2] = {0};

		sprintf(szDirPath, "%s\\Log\\Snmp", CAppInfo::Instance().GetAppDirectory()); 

		SHCreateDirectoryEx(NULL, szDirPath, NULL);

		m_CLogFile.InitLogFile(szDirPath, "SnmpLog.txt", LEVEL_LOG_INFO);

		CLog::WriteEx(LEVEL_LOG_INFO, "CSnmpLog::CSnmpLog Version:%lu", APP_VERSION_NUM);
	}
	
    BOOL CSnmpLog::Init(LPCSTR lpDirName)
    {
        if (!lpDirName)
        {
            return FALSE;
        }

        CHAR szDirPath[MAX_PATH * 2] = {0};

        sprintf(szDirPath, "%s\\Log\\Snmp_%s", CAppInfo::Instance().GetAppDirectory(), lpDirName); 

        SHCreateDirectoryEx(NULL, szDirPath, NULL);

        m_CLogFile.InitLogFile(szDirPath, "SnmpLog.txt", LEVEL_LOG_INFO);

        CLog::WriteEx(LEVEL_LOG_INFO, "CSnmpLog::CSnmpLog Version:%lu", APP_VERSION_NUM);

        return TRUE;
    }
}