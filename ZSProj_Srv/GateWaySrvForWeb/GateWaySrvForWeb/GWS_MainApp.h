#ifndef GWS_APP_H
#define GWS_APP_H

#include "GWS_Common.h"

namespace GWS
{
    class CMainApp : public CGVApp
    {
    public:

        CMainApp();

        virtual ~CMainApp();

        virtual bool InitInstance();

        virtual int ExitInstance();

        virtual int Run(int argc, char** argv);

        virtual void Stop();

    protected:

        //功能:初始化必要的对象
        //返回值：TRUE：成功;FALSE 失败
        BOOL Init();

        //功能:加载配置信息
        //返回值：TRUE：成功;FALSE 失败
        BOOL LoadConfig();

        //功能:加载运行信息
        //返回值：TRUE：成功;FALSE 失败
        BOOL Load();

        //启动服务
        void RunService();

        //事件:启动服务（这里启动相应的业务逻辑线程，不要在该事件中使用循环等待消息）
        //返回值: TRUE 进入消息等待 FALSE 退出
        virtual BOOL OnRun();

        //事件: 停止服务
        virtual void OnStop();

    private:

        CEvent m_StopEvent;
    };

    CMainApp g_CMainApp;
}

#endif