#include "GWS_MainApp.h"
#include "GWS_Log.h"
#include "GWS_Config.h"
#include "GWS_ConfigMonitorThread.h"
#include "GWS_ReLoadThread.h"
#include "GWS_OmasThread.h"
#include "GWS_WorkerThread.h"
#include "GWS_DataMgr.h"
#include "GWS_SocketServer.h"
#include "GWS_AppSrvSocketServer.h"
#include "GWS_AppSrvWorkerThread.h"
#include "GWS_AppSrvAuthMgr.h"

namespace GWS
{
    CMainApp::CMainApp()
    {
    }

    CMainApp::~CMainApp()
    {
        this->Stop();
    }

    bool CMainApp::InitInstance()
    {
        CAppConfig::Instance().LoadBase();

        this->RunService();

        return false;
    }

    int CMainApp::ExitInstance()
    {
        CGVApp::ExitInstance();
        return 0;
    }

    int CMainApp::Run(int argc, char** argv)
    {
        ReportServiceStatus(SERVICE_START_PENDING);

        if (this->OnRun())
        {
            ReportServiceStatus(SERVICE_RUNNING);

            while(!m_StopEvent.Wait(1))
            {
            }
        }

        ReportServiceStatus(SERVICE_STOP_PENDING);

        this->OnStop();

        ReportServiceStatus(SERVICE_STOPPED);

        return 0;
    }

    void CMainApp::Stop()
    {
        m_StopEvent.Set();
    }

    BOOL CMainApp::Init()
    {
        //初始化com环境
        CoInitialize(NULL);

        //////////////////////////////////////////////////////////////////////////
        //初始化日志对象

        CSystemLog::Instance().Init();

        CSnmpLog::Instance().Init();

		CClientLog::Instance().Init();

		CAppSrvLog::Instance().Init();

        //////////////////////////////////////////////////////////////////////////
        //初始化Dump

        new MiniDumper(
            CAppInfo::Instance().GetAppDirectory(), 
            CAppInfo::Instance().GetAppFileName());

        //////////////////////////////////////////////////////////////////////////
        //初始化SOCKET

        if (!CKKSock::InitSock())
        {
            CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CMainApp::Init socket lib init failed !");

            return FALSE;
        }

        return TRUE;
    }

    BOOL CMainApp::LoadConfig()
    {
        //////////////////////////////////////////////////////////////////////////
        //加载配置文件

        if (!CAppConfig::Instance().Load())
        {
            CSystemLog::Instance().Write(
                LEVEL_LOG_ERROR, 
                "CMainApp::LoadConfig load config failed,%s", 
                CAppConfig::Instance().GetConfigFilePath());

            return FALSE;
        }

        //////////////////////////////////////////////////////////////////////////
        //加载数据

        if (!CDataMgr::Instance().Init())
        {
            CSystemLog::Instance().Write(
                LEVEL_LOG_ERROR, 
                "CMainApp::LoadConfig CDataMgr load failed !");
        }

        //////////////////////////////////////////////////////////////////////////
        //初始化对象
        CAppSrvAuthMgr::Instance().Init();

        return TRUE;
    }

    BOOL CMainApp::Load()
    {
        //////////////////////////////////////////////////////////////////////////
        //设置日志等级

        CSystemLog::Instance().SetLogLevel((LEVEL_LOG)CAppConfig::Instance().GetLogLevel());

		CSnmpLog::Instance().SetLogLevel(LEVEL_LOG_INFO);

		CClientLog::Instance().SetLogLevel((LEVEL_LOG)CAppConfig::Instance().GetLogLevel());

		CAppSrvLog::Instance().SetLogLevel((LEVEL_LOG)CAppConfig::Instance().GetLogLevel());

        return TRUE;
    }

    void CMainApp::RunService()
    {
        SetServiceInfo(
            (CHAR*)CAppConfig::Instance().GetServiceName(), 
            (CHAR*)CAppConfig::Instance().GetServiceName());

        RegisterService(g_nArgc, g_pszArgv);
    }

    BOOL CMainApp::OnRun()
    {
        //////////////////////////////////////////////////////////////////////////

        if (!this->Init())
        {
            return false;
        }

        if(!this->LoadConfig())
        {
            return false;
        }

        if (!this->Load())
        {
            return false;
        }

        //////////////////////////////////////////////////////////////////////////
        //加载SNMP线程

        if (!COmasThread::Instance().Start())
        {
            CSystemLog::Instance().Write(
                LEVEL_LOG_ERROR, 
                "CMainApp::OnRun CSnmpThread start failed !");

            return FALSE;
        }

        //////////////////////////////////////////////////////////////////////////
        //加载配置文件监控线程

        if (!CConfigMonitorThread::Instance().Start())
        {
            CSystemLog::Instance().Write(
                LEVEL_LOG_ERROR, 
                "CMainApp::OnRun CConfigMonitorThread start failed !");

            return FALSE;
        }

        //////////////////////////////////////////////////////////////////////////
        //加载重载线程

        if (!CReloadThread::Instance().Start())
        {
            CSystemLog::Instance().Write(
                LEVEL_LOG_ERROR, 
                "CMainApp::OnRun CReloadThread start failed !");

            return FALSE;
        }

		//////////////////////////////////////////////////////////////////////////
		//加载AppSrv工作线程

		if (!CAppSrvWorkerThread::Instance().Start())
		{
			CSystemLog::Instance().Write(
				LEVEL_LOG_ERROR, 
				"CMainApp::OnRun CAppSrvWorkerThread start failed !");

			return FALSE;
		}

		//////////////////////////////////////////////////////////////////////////
		//加载AppSrv IOCP

		if (!CAppSrvSocketServer::Instance().Start())
		{
			CSystemLog::Instance().Write(
				LEVEL_LOG_ERROR, 
				"CMainApp::OnRun CAppSrvSocketServer start failed !");

			return FALSE;
		}

        //////////////////////////////////////////////////////////////////////////
        //加载工作线程

        if (!CWorkerThread::Instance().Start())
        {
            CSystemLog::Instance().Write(
                LEVEL_LOG_ERROR, 
                "CMainApp::OnRun CWorkerThread start failed !");

            return FALSE;
        }

        //////////////////////////////////////////////////////////////////////////
        //加载与前端数据传输
        
        if (!CSocketServer::Instance().Start())
        {
            CSystemLog::Instance().Write(
                LEVEL_LOG_ERROR, 
                "CMainApp::OnRun CSocketServer start failed !");

            return FALSE;
        }

        return TRUE;
    }

    void CMainApp::OnStop()
    {
        CSocketServer::Instance().Stop();

		CWorkerThread::Instance().Stop();

		CAppSrvWorkerThread::Instance().Stop();

        CAppSrvSocketServer::Instance().Stop();

        CReloadThread::Instance().Stop();

        CConfigMonitorThread::Instance().Stop();

        COmasThread::Instance().Stop();
    }
}