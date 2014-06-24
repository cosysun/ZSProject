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

		//功能：获取配置文件路径（全）
		//返回值：配置文件路径
		PCSTR GetConfigFilePath();

        void LoadBase();

		//功能：加载配置信息
		//返回值：TRUE 成功；FALSE 失败
		BOOL Load();

		//功能：重新加载配置信息（只加载部分可重载的信息）
		void ReLoad();

		//功能：获取服务名称
		//返回值：服务名称
		PCSTR GetServiceName();

		//功能：获取服务器本地绑定IP
		//返回值：服务器本地绑定IP
		PCSTR GetLocalIP();

		//功能：获取日志等级
		//返回值：日志等级
		LEVEL_LOG GetLogLevel();

		//功能：获取重载线程的等待时间
		//返回值：重载线程的等待时间
		DWORD GetReLoadWaitTime();

		//功能：获取是否重载数据库配置信息
		//返回值：是否重载数据库配置信息
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

		CHAR m_szConfigFilePath[MAX_PATH];                      //配置文件路径

		//////////////////////////////////////////////////////////////////////////
		//GWS_SERVER

		CHAR m_szServiceName[MAX_PATH];                         //服务名称

		CHAR m_szLocalIP[SOCKET_IP_MAX_SIZE];                   //绑定的本地IP地址

		LEVEL_LOG m_logLevel;                                   //日志等级

		DWORD m_dwReLoadWaitTime;                               //重新读取等待超时时间（为0时则无超时）

		BOOL m_bIsReLoadDBInfoNow;                              //即时重读数据库配置信息

        BOOL m_bIsAuthFromLocal;                                //是否只从本地读取

		DWORD m_dwProjectID;

		DWORD m_dwAppTypeID;

		//////////////////////////////////////////////////////////////////////////
		//GWS_CLIENT

		CHAR m_szClientListenIP[SOCKET_IP_MAX_SIZE];

		WORD m_wClientPort;

		WORD m_wClientMaxConnectCount;							//客户端最大连接数

		DWORD m_dwClientRecvBufferSize;                         //接收缓冲区大小
		DWORD m_dwClientSendBufferSize;                         //发送缓冲区大小

        DWORD m_dwClientRecvBufferCount;						//接收缓冲区个数
        DWORD m_dwClientSendBufferCount;                        //发送缓冲区个数

		WORD m_wClientThreadBufferCount;						//线程消息交互缓冲区个数

		WORD m_wClientWorkThreadCount;                          //工作线程

		//////////////////////////////////////////////////////////////////////////
		//GWS_APPSRV

		CHAR m_szAppSrvListenIP[SOCKET_IP_MAX_SIZE];
	
		WORD m_wAppSrvPort;

		WORD m_wAppSrvMaxConnectCount;							//应用服务连接数

		DWORD m_dwAppSrvRecvBufferSize;                         //接收缓冲区大小
		DWORD m_dwAppSrvSendBufferSize;                         //发送缓冲区大小

		DWORD m_dwAppSrvRecvBufferCount;						//接收缓冲区个数
		DWORD m_dwAppSrvSendBufferCount;                        //发送缓冲区个数

		WORD m_wAppSrvThreadBufferCount;						//线程消息交互缓冲区个数

		WORD m_wAppSrvWorkThreadCount;                          //工作线程

		//////////////////////////////////////////////////////////////////////////
		//GWS_DB

        BOOL m_bIsSafe;

		CHAR m_szDBAddr[GWS_DB_STR_MAX_SIZE];					//数据库地址

		CHAR m_szDBName[GWS_DB_STR_MAX_SIZE];					//数据库名

		CHAR m_szDBUserName[GWS_DB_STR_MAX_SIZE];				//数据库用户名

		CHAR m_szDBPwd[GWS_DB_STR_MAX_SIZE];					//数据库密码


		//////////////////////////////////////////////////////////////////////////
		// GWS_LOGDB

		BOOL m_bGWSLogIsOn;

		WORD m_wOmasWorkThreadCount;

		BOOL m_bLogIsSafe;

		CHAR m_szLogDBAddr[GWS_DB_STR_MAX_SIZE];					//数据库地址

		CHAR m_szLogDBName[GWS_DB_STR_MAX_SIZE];					//数据库名

		CHAR m_szLogDBUserName[GWS_DB_STR_MAX_SIZE];				//数据库用户名

		CHAR m_szLogDBPwd[GWS_DB_STR_MAX_SIZE];						//数据库密码
	};
}

#endif