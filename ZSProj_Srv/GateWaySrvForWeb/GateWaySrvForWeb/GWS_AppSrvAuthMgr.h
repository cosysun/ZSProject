#ifndef GWS_APPSRV_AUTH_MGR_H
#define GWS_APPSRV_AUTH_MGR_H

#include "GWS_Common.h"

namespace GWS
{
    //��֤������
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

        //���ܣ��Ƿ��Ѿ���¼
        //������
        //      LONG lSocketID IN ����Ψһ���
        //      DWORD&  dwAppTypeID OUT ��ǰ���Ӷ�Ӧ�ķ������ͱ��
        //����ֵ��TRUE �Ѿ���¼��FALSE δ��¼
        BOOL IsLogined(LONG lSocketID, DWORD& dwAppTypeID);

        //���ܣ��Ƴ��ͻ��ˣ��ͻ��˶Ͽ�ʱ���ã�
        //������
        //      LONG lSocketID IN ����Ψһ���
        void RemoveAppSrv(LONG lSocketID);

        //���ܣ���ȡ�Ѿ����Ӳ�����֤�ɹ��Ŀͻ�����
        //����ֵ���Ѿ����Ӳ�����֤�ɹ��Ŀͻ�����
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