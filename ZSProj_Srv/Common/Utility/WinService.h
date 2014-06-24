#ifndef SYSBASE_SERVICE_WIN_SERVICE_H
#define SYSBASE_SERVICE_WIN_SERVICE_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "NTService.h"

//////////////////////////////////////////////////////////////////////////
//ϵͳ����ִ������

class CWinServiceContext
{
public:

    CWinServiceContext(){};
    virtual ~CWinServiceContext(){};

    //�¼�:������������������Ӧ��ҵ���߼��̣߳���Ҫ�ڸ��¼���ʹ��ѭ���ȴ���Ϣ��
    //����ֵ: TRUE ������Ϣ�ȴ� FALSE �˳�
    virtual BOOL OnRun() = 0;
    
    //�¼�: ֹͣ����
    virtual void OnStop() = 0;
};

//////////////////////////////////////////////////////////////////////////
//ϵͳ������

class CWinService : private CNTService
{
public:

    //�쳣
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