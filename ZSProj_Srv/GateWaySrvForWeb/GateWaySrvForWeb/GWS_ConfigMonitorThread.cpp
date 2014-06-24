#include "GWS_ConfigMonitorThread.h"
#include "GWS_Config.h"
#include "GWS_Log.h"
#include "GWS_ReloadThread.h"

namespace GWS
{
	CConfigMonitorThread::CConfigMonitorThread() : CCustomThread(NULL)
	{
		m_bStop = TRUE;
	}

	CConfigMonitorThread::~CConfigMonitorThread()
	{
		this->Stop();
	}

	BOOL CConfigMonitorThread::Start()
	{
		if (!m_bStop)
		{
			return TRUE;
		}

		if (!CCustomThread::Start())
		{
			CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CAppConfigMonitorThread::Start start thread failed !");

			return FALSE;
		}

		return TRUE;
	}

	void CConfigMonitorThread::Stop()
	{
		if (m_bStop)
		{
			return;
		}

		m_bStop = TRUE;

		m_hStopEvent.Set();

		CCustomThread::Stop(STOP_WAIT_MILLISECOND);
	}

#ifndef _LINUX 

	DWORD CConfigMonitorThread::Proc(LPVOID lpParameter)
	{
		m_bStop = FALSE;

		m_hStopEvent.Reset();

		char szDir[MAX_PATH] = {0};
		WIN32_FILE_ATTRIBUTE_DATA fileData = {0};

		sprintf(szDir, TEXT("%s\\config"), CAppInfo::Instance().GetAppDirectory());

		HANDLE hChangeHandle = FindFirstChangeNotification(szDir, FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE);

		if (INVALID_HANDLE_VALUE == hChangeHandle)
		{
			return 0;
		}

		if (!GetFileAttributesEx(CAppConfig::Instance().GetConfigFilePath(), GetFileExInfoStandard, (LPVOID)&fileData))
		{
			return 0;
		}

		const int nCount = 2;
		const int nStopEventIndex = 0;
		const int nChangeHandleIndex = 1;

		HANDLE hHandles[nCount] = {0};
		hHandles[nStopEventIndex] = (HANDLE)m_hStopEvent.GetHandle();
		hHandles[nChangeHandleIndex] = hChangeHandle;

		DWORD dwCurrentTickCount = GetTickCount();
		DWORD dwLastReloadDBInfoTickCount = dwCurrentTickCount;

		while (!m_bStop)
		{
			switch (WaitForMultipleObjects(nCount, hHandles, FALSE, INFINITE))
			{
			case WAIT_FAILED :
			case nStopEventIndex :
				{
					FindCloseChangeNotification(hChangeHandle);

					return 0;
				}    
				break;
			case nChangeHandleIndex :
				{
					WIN32_FILE_ATTRIBUTE_DATA tempfileData = {0};

					if (!GetFileAttributesEx(CAppConfig::Instance().GetConfigFilePath(), GetFileExInfoStandard, (LPVOID)&tempfileData))
					{
						continue;
					}

					if (fileData.ftLastWriteTime.dwLowDateTime != tempfileData.ftLastWriteTime.dwLowDateTime
						|| fileData.ftLastWriteTime.dwHighDateTime != tempfileData.ftLastWriteTime.dwHighDateTime)
					{
						//配置文件被修改,重读
						CAppConfig::Instance().ReLoad();

						//日志等级
						CSystemLog::Instance().SetLogLevel(CAppConfig::Instance().GetLogLevel());
						CClientLog::Instance().SetLogLevel(CAppConfig::Instance().GetLogLevel());
						CAppSrvLog::Instance().SetLogLevel(CAppConfig::Instance().GetLogLevel());

						//重加载数据库信息
						if (CAppConfig::Instance().GetIsReLoadDBInfoNow())
						{
							CReloadThread::Instance().ReloadDBInfo();
						}

						//////////////////////////////////////////////////////////////////////////

						fileData = tempfileData;

						CSystemLog::Instance().Write(LEVEL_LOG_DEBUG, "CAppConfigMonitorThread::Proc reload config info !");
					}

					FindNextChangeNotification(hChangeHandle);
				}
				break;
			default:
				return 0;
			}
		}

		return 0;   
	}

#else

#endif
}