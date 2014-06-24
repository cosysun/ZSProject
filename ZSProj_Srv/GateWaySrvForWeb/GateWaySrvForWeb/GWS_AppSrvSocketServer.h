#ifndef GWS_APPSRV_SOCKET_SERVER_H
#define GWS_APPSRV_SOCKET_SERVER_H

#include "GWS_Common.h"

namespace GWS
{
    //网络服务类
    class CAppSrvSocketServer : public CCallback_NetServices, public CSingletonT<CAppSrvSocketServer>
    {
    public:

        //功能：启动
        //返回值：TRUE 成功；FALSE 失败
        BOOL Start();

        //功能：停止
        void Stop();

        //功能：发送数据到前端
        //参数：
        //      void* pDataInfo IN 需要发送数据的缓冲区
        //      long nDataLen IN 发送数据大小
        //      long nSocketID IN 连接唯一标识
        void PostSend(void* pDataInfo,long nDataLen,long nSocketID);

        //功能：要求关闭某个连接上的客户端 
        void CloseSocket(long nSocketID);

        //功能：获取已经连接上的连接数的个数
        long GetConnectedCount();

        virtual ~CAppSrvSocketServer();

        friend class CSingletonT<CAppSrvSocketServer>;

    protected:

        //////////////////////////////////////////////////////////////////////////
        //事件

		virtual Int NotifyRecvData(Void* pData,Long nDataLen,ULong ulSourceIP,Int nSourcePort,Long  nSocketID);

		virtual Int NotifyNewClientConnected(ULong ulRemoteIP,Int  nRemotePort,Long nSocketID);

		virtual Int NotifyClientClosed(ULong ulRemoteIP,Int nRemotePort,Long nSocketID,ERROR_NETSERVICES errorCode);

		virtual Int NotifyConnectRemoteSrv(ULong ulRemoteIP,Int nRemotePort,Long nSocketID,ERROR_NETSERVICES errorCode);

    private:

        CAppSrvSocketServer();

        CNetServices m_CNetIOCP;

        BOOL m_bStop;
    };
}

#endif