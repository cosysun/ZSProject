#ifndef GWS_SNMP_MGR_H
#define GWS_SNMP_MGR_H

#include "GWS_Common.h"

namespace GWS
{
	//SNMP定时发送线程
	class COmasThread : CCustomThread, public CSingletonT<COmasThread>
	{
	public:

		//统计信息类型
		enum EOMAS_TYPE
		{
			OMAS_TYPE_CLIENT_RECV_COUNT = 0,					//客户端接收包数
			OMAS_TYPE_CLIENT_SEND_COUNT,						//客户端发送包数
			OMAS_TYPE_CLIENT_CONNECT_COUNT,						//客户端数

			OMAS_TYPE_CLIENT_REQ,								//客户端请求数
			OMAS_TYPE_CLIENT_REQ_SUCCESS,						//客户端请求成功数
			OMAS_TYPE_CLIENT_REQ_FAILED,						//客户端请求失败数

            OMAS_TYPE_CLIENT_LOGOUT,							//客户端登出数
            OMAS_TYPE_CLIENT_KEEPALIVE,                         //客户端心跳数
            OMAS_TYPE_CLIENT_OMAS,								//系统功能请求数
            OMAS_TYPE_CLIENT_UNKNOWN,                           //客户端未知命令数

			OMAS_TYPE_APPSRV_RECV_COUNT,						//客户端接收包数
			OMAS_TYPE_APPSRV_SEND_COUNT,						//客户端发送包数
			OMAS_TYPE_APPSRV_CONNECT_COUNT,						//客户端数
			
			OMAS_TYPE_APPSRV_LOGIN,								//应用服务登录数
			OMAS_TYPE_APPSRV_LOGIN_SUCCESS,						//应用服务登录成功数
			OMAS_TYPE_APPSRV_LOGIN_FAILED,						//应用服务登录失败数

			OMAS_TYPE_APPSRV_RES,								//应用服务结果数
			OMAS_TYPE_APPSRV_RES_SUCCESS,						//应用服务结果返回成功数
			OMAS_TYPE_APPSRV_RES_FAILED,						//应用服务结果返回失败数

            OMAS_TYPE_APPSRV_LOGOUT,                            //应用服务登出数
            OMAS_TYPE_APPSRV_KEEPALIVE,                         //应用服务心跳数
            OMAS_TYPE_APPSRV_SYSTEM,                            //系统功能请求
            OMAS_TYPE_APPSRV_UNKNOWN,                           //应用服务未知命令数

			OMAS_TYPE_MAX_COUNT,								//统计信息最大值（该类型无效）
		};

		//功能：启动
		//返回值：TRUE 成功；FALSE 失败
		BOOL Start();

		//功能：停止
		void Stop();

		//功能：某一个统计信息数据值加1
		//参数：
		//      EOMAS_TYPE snmpType IN 统计信息类型
		void Increment(EOMAS_TYPE snmpType);

		//功能：某一个统计信息数据值减1
		//参数：
		//      EOMAS_TYPE snmpType IN 统计信息类型
		void Decrement(EOMAS_TYPE snmpType);


		virtual ~COmasThread();

		friend class CSingletonT<COmasThread>;

	protected:

		virtual DWORD Proc(LPVOID lpParameter);

		void SendSnmpInfo();

	private:

		COmasThread();

		//////////////////////////////////////////////////////////////////////////

		BOOL m_bStop;

		CEvent m_StopEvent;

		LONG m_Snmp[COmasThread::OMAS_TYPE_MAX_COUNT];
	};
}

#endif