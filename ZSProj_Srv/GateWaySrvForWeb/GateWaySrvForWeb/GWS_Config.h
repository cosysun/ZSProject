#ifndef GWS_CONFIG_H
#define GWS_CONFIG_H

#include "GWS_Common.h"

namespace GWS
{
	class CAppConfig : public CSingletonT<CAppConfig> 
	{
	public:

		virtual ~CAppConfig();

		friend class CSingletonT<CAppConfig>;

		//���ܣ���ȡ�����ļ�·����ȫ��
		//����ֵ�������ļ�·��
		PCSTR GetConfigFilePath();

        void LoadBase();

		//���ܣ�����������Ϣ
		//����ֵ��TRUE �ɹ���FALSE ʧ��
		BOOL Load();

		//���ܣ����¼���������Ϣ��ֻ���ز��ֿ����ص���Ϣ��
		void ReLoad();

		//���ܣ���ȡ��������
		//����ֵ����������
		PCSTR GetServiceName();

		//���ܣ���ȡ���������ذ�IP
		//����ֵ�����������ذ�IP
		PCSTR GetLocalIP();

		//���ܣ���ȡ��־�ȼ�
		//����ֵ����־�ȼ�
		LEVEL_LOG GetLogLevel();

		//���ܣ���ȡ�����̵߳ĵȴ�ʱ��
		//����ֵ�������̵߳ĵȴ�ʱ��
		DWORD GetReLoadWaitTime();

		//���ܣ���ȡ�Ƿ��������ݿ�������Ϣ
		//����ֵ���Ƿ��������ݿ�������Ϣ
		BOOL GetIsReLoadDBInfoNow();

        BOOL GetIsAuthFromLocal();

		DWORD GetProjectID();

		DWORD GetAppTypeID();

		//////////////////////////////////////////////////////////////////////////
		//CLIENT

		PCSTR GetClientListenIP();

		WORD GetClientPort();

		WORD GetClientMaxConnectCount();

		DWORD GetClientRecvBufferSize();

		DWORD GetClientSendBufferSize();

        DWORD GetClientRecvBufferCount();

        DWORD GetClientSendBufferCount();

		WORD GetClientThreadBufferCount();

		WORD GetClientWorkThreadCount();

		//////////////////////////////////////////////////////////////////////////
		//APPSRV

		PCSTR GetAppSrvListenIP();

		WORD GetAppSrvPort();

		WORD GetAppSrvMaxConnectCount();

		DWORD GetAppSrvRecvBufferSize();

		DWORD GetAppSrvSendBufferSize();

		DWORD GetAppSrvRecvBufferCount();

		DWORD GetAppSrvSendBufferCount();

		WORD GetAppSrvThreadBufferCount();

		WORD GetAppSrvWorkThreadCount();

		//////////////////////////////////////////////////////////////////////////
		//DB

        BOOL GetIsSafe();

		PCSTR GetDBAddr();

		PCSTR GetDBName();

		PCSTR GetDBUserName();

		PCSTR GetDBPwd();

		//////////////////////////////////////////////////////////////////////////
		//LogDB

		BOOL GetLogIsOn();

		WORD GetOmasWorkThreadCount();

		BOOL GetLogIsSafe();

		PCSTR GetLogDBAddr();

		PCSTR GetLogDBName();

		PCSTR GetLogDBUserName();

		PCSTR GetLogDBPwd();

	private:

		CAppConfig();

		CHAR m_szConfigFilePath[MAX_PATH];                      //�����ļ�·��

		//////////////////////////////////////////////////////////////////////////
		//GWS_SERVER

		CHAR m_szServiceName[MAX_PATH];                         //��������

		CHAR m_szLocalIP[SOCKET_IP_MAX_SIZE];                   //�󶨵ı���IP��ַ

		LEVEL_LOG m_logLevel;                                   //��־�ȼ�

		DWORD m_dwReLoadWaitTime;                               //���¶�ȡ�ȴ���ʱʱ�䣨Ϊ0ʱ���޳�ʱ��

		BOOL m_bIsReLoadDBInfoNow;                              //��ʱ�ض����ݿ�������Ϣ

        BOOL m_bIsAuthFromLocal;                                //�Ƿ�ֻ�ӱ��ض�ȡ

		DWORD m_dwProjectID;

		DWORD m_dwAppTypeID;

		//////////////////////////////////////////////////////////////////////////
		//GWS_CLIENT

		CHAR m_szClientListenIP[SOCKET_IP_MAX_SIZE];

		WORD m_wClientPort;

		WORD m_wClientMaxConnectCount;							//�ͻ������������

		DWORD m_dwClientRecvBufferSize;                         //���ջ�������С
		DWORD m_dwClientSendBufferSize;                         //���ͻ�������С

        DWORD m_dwClientRecvBufferCount;						//���ջ���������
        DWORD m_dwClientSendBufferCount;                        //���ͻ���������

		WORD m_wClientThreadBufferCount;						//�߳���Ϣ��������������

		WORD m_wClientWorkThreadCount;                          //�����߳�

		//////////////////////////////////////////////////////////////////////////
		//GWS_APPSRV

		CHAR m_szAppSrvListenIP[SOCKET_IP_MAX_SIZE];
	
		WORD m_wAppSrvPort;

		WORD m_wAppSrvMaxConnectCount;							//Ӧ�÷���������

		DWORD m_dwAppSrvRecvBufferSize;                         //���ջ�������С
		DWORD m_dwAppSrvSendBufferSize;                         //���ͻ�������С

		DWORD m_dwAppSrvRecvBufferCount;						//���ջ���������
		DWORD m_dwAppSrvSendBufferCount;                        //���ͻ���������

		WORD m_wAppSrvThreadBufferCount;						//�߳���Ϣ��������������

		WORD m_wAppSrvWorkThreadCount;                          //�����߳�

		//////////////////////////////////////////////////////////////////////////
		//GWS_DB

        BOOL m_bIsSafe;

		CHAR m_szDBAddr[GWS_DB_STR_MAX_SIZE];					//���ݿ��ַ

		CHAR m_szDBName[GWS_DB_STR_MAX_SIZE];					//���ݿ���

		CHAR m_szDBUserName[GWS_DB_STR_MAX_SIZE];				//���ݿ��û���

		CHAR m_szDBPwd[GWS_DB_STR_MAX_SIZE];					//���ݿ�����


		//////////////////////////////////////////////////////////////////////////
		// GWS_LOGDB

		BOOL m_bGWSLogIsOn;

		WORD m_wOmasWorkThreadCount;

		BOOL m_bLogIsSafe;

		CHAR m_szLogDBAddr[GWS_DB_STR_MAX_SIZE];					//���ݿ��ַ

		CHAR m_szLogDBName[GWS_DB_STR_MAX_SIZE];					//���ݿ���

		CHAR m_szLogDBUserName[GWS_DB_STR_MAX_SIZE];				//���ݿ��û���

		CHAR m_szLogDBPwd[GWS_DB_STR_MAX_SIZE];						//���ݿ�����
	};
}

#endif