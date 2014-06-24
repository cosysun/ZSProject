#include "GWS_ReLoadThread.h"
#include "GWS_Log.h"
#include "GWS_Config.h"
#include "GWS_DataMgr.h"

#define GWS_DEFAULT_RELOAD_TIME_OUT 15 * 60 * 1000
#define GWS_MIN_RELOAD_TICK_COUNT 1000 * (60 - 1)

namespace GWS
{
	CReloadThread::CReloadThread()
		: CCustomThread(NULL)
	{
		m_bStop = TRUE;
	}

	CReloadThread::~CReloadThread()
	{
		this->Stop();
	}

	BOOL CReloadThread::Start()
	{
		if (!m_bStop)
		{
			return TRUE;
		}

		m_ReloadEvent.Reset();

		if (!CCustomThread::Start())
		{
			this->Stop();

			CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CReloadThread::Start thread start failed !");

			return FALSE;
		}

		return TRUE;
	}

	void CReloadThread::Stop()
	{
		if (m_bStop)
		{
			return;
		}

		m_bStop = TRUE;

		m_ReloadEvent.Set();

		CCustomThread::Stop(STOP_WAIT_MILLISECOND, 0);
	}
	
	void CReloadThread::ReloadDBInfo()
	{
		m_ReloadEvent.Set();
	}

	DWORD CReloadThread::Proc(LPVOID lpParameter)
	{
		m_bStop = FALSE;

		DWORD dwReloadWaitTime = CAppConfig::Instance().GetReLoadWaitTime();

		DWORD dwCurTickCount = GetTickCount();

		while (!m_bStop)
		{
			DWORD dwObjectIndex = 0;

			if (m_ReloadEvent.Wait(dwReloadWaitTime))
			{
				if (m_bStop)
				{
					return 0;
				}
				
				if (GetTickCount() - dwCurTickCount > GWS_MIN_RELOAD_TICK_COUNT)
				{
					//////////////////////////////////////////////////////////////////////////
					//目标数据库表名称
					
                    if (!CAppConfig::Instance().GetIsAuthFromLocal())
                    {
                        if (CDataMgr::Instance().ReLoad())
                        {
                            CSystemLog::Instance().Write(LEVEL_LOG_INFO, "CReloadThread::Proc CDataMgr reload success !");
                        }
                        else
                        {
                            CSystemLog::Instance().Write(LEVEL_LOG_INFO, "CReloadThread::Proc CDataMgr reload failed !");
                        }
                    }

					dwCurTickCount = GetTickCount();
				}
				else
				{
					CSystemLog::Instance().Write(LEVEL_LOG_INFO, "CReloadThread::Proc CDataMgr reload too fast !");
				}

				m_ReloadEvent.Reset();
			}
			else
			{
				m_ReloadEvent.Set();
			}
		}

		return 0;
	}
}