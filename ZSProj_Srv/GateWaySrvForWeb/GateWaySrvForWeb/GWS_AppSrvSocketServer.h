#ifndef GWS_APPSRV_SOCKET_SERVER_H
#define GWS_APPSRV_SOCKET_SERVER_H

#include "GWS_Common.h"

namespace GWS
{
    //���������
    class CAppSrvSocketServer : public CCallback_NetServices, public CSingletonT<CAppSrvSocketServer>
    {
    public:

        //���ܣ�����
        //����ֵ��TRUE �ɹ���FALSE ʧ��
        BOOL Start();

        //���ܣ�ֹͣ
        void Stop();

        //���ܣ��������ݵ�ǰ��
        //������
        //      void* pDataInfo IN ��Ҫ�������ݵĻ�����
        //      long nDataLen IN �������ݴ�С
        //      long nSocketID IN ����Ψһ��ʶ
        void PostSend(void* pDataInfo,long nDataLen,long nSocketID);

        //���ܣ�Ҫ��ر�ĳ�������ϵĿͻ��� 
        void CloseSocket(long nSocketID);

        //���ܣ���ȡ�Ѿ������ϵ��������ĸ���
        long GetConnectedCount();

        virtual ~CAppSrvSocketServer();

        friend class CSingletonT<CAppSrvSocketServer>;

    protected:

        //////////////////////////////////////////////////////////////////////////
        //�¼�

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