#ifndef GWS_SOCKET_SERVER_H
#define GWS_SOCKET_SERVER_H

#include "GWS_Common.h"

namespace GWS
{
    //网络服务类
    class CSocketServer : public CCallback_NetServices, public CSingletonT<CSocketServer>
    {
    public:

        virtual ~CSocketServer();

        friend class CSingletonT<CSocketServer>;

        BOOL Start();

        void Stop();

        void PostSend(PCHAR pData, DWORD dwSize, LONG lSocketID);
        
        void CloseSocket(LONG lSocketID);

        WORD GetConnCount();

    protected:

        //////////////////////////////////////////////////////////////////////////
        //事件

        /*------------------------------
	    *   notify recv data
	    *  return value,0(succ),else :errorcode
	    ------------------------------*/
	    virtual Int NotifyRecvData(Void* pData,Long nDataLen,ULong ulSourceIP,Int nSourcePort,Long  nSocketID);
	    /*------------------------------
	    *   notify new client connected
	    *  return value,0(succ),else :errorcode
	    ------------------------------*/
	    virtual Int NotifyNewClientConnected(ULong   ulRemoteIP,Int  nRemotePort,Long nSocketID);
	    /*------------------------------
	    *   notify client closed
	    *  return value,0(succ),else :errorcode
	    ------------------------------*/
	    virtual Int NotifyClientClosed(ULong   ulRemoteIP,Int  nRemotePort,Long nSocketID,ERROR_NETSERVICES errorCode);
	    /*------------------------------
	    *   notify the status of connected remote srv
	    * return value,0(succ),else :errorcode
	    ------------------------------*/
	    virtual Int NotifyConnectRemoteSrv(ULong   ulRemoteIP,Int  nRemotePort,Long nSocketID,ERROR_NETSERVICES errorCode);

    private:

        CSocketServer();

        CNetServices m_Server;

        BOOL m_bStop;
    };
}

#endif