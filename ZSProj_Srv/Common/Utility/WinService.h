#ifndef SYSBASE_SERVICE_WIN_SERVICE_H
#define SYSBASE_SERVICE_WIN_SERVICE_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "NTService.h"

//////////////////////////////////////////////////////////////////////////
//系统服务执行内容

class CWinServiceContext
{
public:

    CWinServiceContext(){};
    virtual ~CWinServiceContext(){};

    //事件:启动服务（这里启动相应的业务逻辑线程，不要在该事件中使用循环等待消息）
    //返回值: TRUE 进入消息等待 FALSE 退出
    virtual BOOL OnRun() = 0;
    
    //事件: 停止服务
    virtual void OnStop() = 0;
};

//////////////////////////////////////////////////////////////////////////
//系统服务类

class CWinService : private CNTService
{
public:

    //异常
    CWinService(CWinServiceContext *pCSysServiceContext, LPCTSTR lpServiceName, LPCSTR lpServiceDesc);

    virtual ~CWinService();

    BOOL Start(int nArgc = __argc, LPTSTR* argv = __argv);

protected:
    
    //////////////////////////////////////////////////////////////////////////

    void Run(DWORD argc, LPTSTR * argv);

    void Stop();

private:

    HANDLE m_hStopEvent;

    CWinServiceContext *m_pCSysServiceContext;
};


#endif