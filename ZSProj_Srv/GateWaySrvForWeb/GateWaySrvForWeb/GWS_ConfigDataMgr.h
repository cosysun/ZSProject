#ifndef GWS_CONFIG_DATA_MGR_H
#define GWS_CONFIG_DATA_MGR_H

#include "GWS_Common.h"

//Client 验证
#define GWS_CLIENT_TYPE_AUTH_KEY_SIZE 51
#define GWS_CLIENT_TYPE_AUTH_KEY_LEN 50

//APPTYPE 验证
#define GWS_APP_TYPE_AUTH_KEY_SIZE 51
#define GWS_APP_TYPE_AUTH_KEY_LEN 50


namespace GWS
{
	struct TAppType
	{
        CHAR szAuthKey[GWS_APP_TYPE_AUTH_KEY_SIZE];
        BOOL bIsAuthAciton;
        BOOL bIsAuthClientIP;
	};
    //////////////////////////////////////////////////////////////////////////
    //CConfigDataAppTypeMgr

    class CConfigDataAppTypeMgr
    {
    public:

        CConfigDataAppTypeMgr();

        virtual ~CConfigDataAppTypeMgr();

        BOOL Init(BOOL bLoadFromLocal);

        BOOL ReLoad(); 

        const TAppType* GetAppType(int nProjectID, unsigned short sAppTypeID);

    private:

        //////////////////////////////////////////////////////////////////////////

        typedef map<unsigned short, TAppType> AppTypeMap;
		
		typedef map<int, AppTypeMap> ProjAppTypeMap;

        //////////////////////////////////////////////////////////////////////////

        BOOL LoadFromDB();

        BOOL LoadFromLocal();

        BOOL LoadFromLocal(ProjAppTypeMap* pAppTypeMap);

        void SaveToLocal(ProjAppTypeMap* pAppTypeMap);

        //////////////////////////////////////////////////////////////////////////

        ProjAppTypeMap* GetValidAppTypeMap();

        //////////////////////////////////////////////////////////////////////////

        ProjAppTypeMap m_ProjAppTypeMap1;

        ProjAppTypeMap m_ProjAppTypeMap2;

        BOOL m_bValid1;

        int m_nVersion;

        BOOL m_bLoadFromLocal;
    };
}

#endif