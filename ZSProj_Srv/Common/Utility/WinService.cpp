#include "WinService.h"

#define WIN_SERVICE_STOP_PENDING_WAIT_TIME 10000

CWinService::CWinService(CWinServiceContext *pCSysServiceContext, LPCTSTR lpServiceName, LPCTSTR lpServiceDesc) 
    : CNTService(lpServiceName, lpServiceName, lpServiceDesc)
{
    m_pCSysServiceContext = pCSysServiceContext;
    
    m_hStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

CWinService::~CWinService()
{
    CloseHandle(m_hStopEvent);
}

BOOL CWinService::Start(int nArgc, LPTSTR* argv)
{
    //启动服务器
    return CNTService::RegisterService(nArgc, argv);
}

void CWinService::Run(DWORD argc, LPTSTR * argv)
{
    ReportStatus(SERVICE_START_PENDING);

    if (!m_pCSysServiceContext->OnRun())
    {
        return;
    }

    ReportStatus(SERVICE_RUNNING);

    MSG Msg = {0};

    //消息循环
    while (WaitForSingleObject(m_hStopEvent, 10) != WAIT_OBJECT_0) 
    {
        if(PeekMessage(&Msg , NULL, 0, 0,  PM_REMOVE))
        {
            TranslateMessage(&Msg); 
            DispatchMessage(&Msg);
        }
    }
    
    ReportStatus(SERVICE_STOP_PENDING, WIN_SERVICE_STOP_PENDING_WAIT_TIME);
    
    m_pCSysServiceContext->OnStop();
    
    ReportStatus(SERVICE_STOPPED);
}

void CWinService::Stop()
{
    SetEvent(m_hStopEvent);
}