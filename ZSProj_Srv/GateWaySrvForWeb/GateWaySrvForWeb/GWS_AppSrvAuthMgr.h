#ifndef GWS_APPSRV_AUTH_MGR_H
#define GWS_APPSRV_AUTH_MGR_H

#include "GWS_Common.h"

namespace GWS
{
    //验证管理类
    class CAppSrvAuthMgr : public CSingletonT<CAppSrvAuthMgr>
    {
    public:

		virtual ~CAppSrvAuthMgr();
		
		friend class CSingletonT<CAppSrvAuthMgr>;
		
		void Init();

		BOOL Auth(
			LONG lSocketID, 
			int nProjectID,
			DWORD dwAppTypeID,
			LPCSTR lpAuthKey, 
			DWORD dwIP);

        void SetSocketRoute(LONG lSocketID);

        void RemoveSocketRoute(LONG lSocketID);

        //功能：是否已经登录
        //参数：
        //      LONG lSocketID IN 连接唯一编号
        //      DWORD&  dwAppTypeID OUT 但前连接对应的服务类型编号
        //返回值：TRUE 已经登录；FALSE 未登录
        BOOL IsLogined(LONG lSocketID, DWORD& dwAppTypeID);

        //功能：移除客户端（客户端断开时调用）
        //参数：
        //      LONG lSocketID IN 连接唯一编号
        void RemoveAppSrv(LONG lSocketID);

        //功能：获取已经连接并且验证成功的客户端数
        //返回值：已经连接并且验证成功的客户端数
        USHORT GetAppSrvCount();

		BOOL GetAppSrvSocketID(DWORD dwProjectID, DWORD dwAppTypeID, LONG& lSocketID);

    private:

        CAppSrvAuthMgr();

        WORD m_wMaxSocketCount;

        struct TAppSocketInfo 
        {
			DWORD dwProjectID;
            DWORD dwAppTypeID;
            BOOL bRoute;
        };

        typedef map<LONG, TAppSocketInfo> AppSrvAuthMap;

        AppSrvAuthMap m_AppSrvAuthMap;

		struct TAppTypeSocketInfo
		{
			LONG* lSocketIDArray;

            WORD wCurSocketIDCount;

            WORD wIndex;
		};

		typedef map<DWORD, TAppTypeSocketInfo> AppTypeSocketMap;

		typedef map<DWORD, AppTypeSocketMap> projAppTypeSocketMap;

		projAppTypeSocketMap m_projAppTypeSocketMap;

        CLockObject m_Lock;
    };
}

#endif