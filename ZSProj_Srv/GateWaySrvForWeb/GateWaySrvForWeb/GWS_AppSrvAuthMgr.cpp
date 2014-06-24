#include "GWS_AppSrvAuthMgr.h"
#include "GWS_Config.h"
#include "GWS_Log.h"
#include "GWS_OmasThread.h"
#include "GWS_DataMgr.h"

namespace GWS
{
    CAppSrvAuthMgr::CAppSrvAuthMgr()
    {
    }

    CAppSrvAuthMgr::~CAppSrvAuthMgr()
    {
		projAppTypeSocketMap::iterator Iter= m_projAppTypeSocketMap.begin();
		projAppTypeSocketMap::iterator endIter = m_projAppTypeSocketMap.end();

        for (; Iter != endIter; ++Iter)
        {
			AppTypeSocketMap& aTAppTypeSocketMap = Iter->second;

			AppTypeSocketMap::iterator entity  = aTAppTypeSocketMap.begin();
			AppTypeSocketMap::iterator endEntity = aTAppTypeSocketMap.end();
            for (; entity != endEntity; ++entity)
			{
				if (entity->second.lSocketIDArray)
				{
					delete[] entity->second.lSocketIDArray;

					entity->second.lSocketIDArray = NULL;
				}
			}
        }

        m_projAppTypeSocketMap.clear();
    }
    
    void CAppSrvAuthMgr::Init()
    {
        m_wMaxSocketCount = CAppConfig::Instance().GetAppSrvMaxConnectCount() * 2;
    }
    
	BOOL CAppSrvAuthMgr::Auth(
		LONG lSocketID, 
		int nProjectID,
		DWORD dwAppTypeID,
		LPCSTR lpAuthKey, 
		DWORD dwIP)
	{
		if (!lpAuthKey)
		{
			return FALSE;
		}

		CHAR szIP[33] = {0};
		ToStringIP(dwIP, szIP);
		
		const TAppType* pTAppTypeInfo = CDataMgr::Instance().GetAppType(nProjectID, dwAppTypeID);

		if (!pTAppTypeInfo)
		{
			CAppSrvLog::Instance().Write(
				LEVEL_LOG_ERROR, 
				"CAppSrvAuthMgr::Auth failed, ProjectID:%lu, dwAppTypeID:%lu not find, IP:%s",
				nProjectID, dwAppTypeID, szIP);

			return FALSE;
		}

		if (0 != strncmp(lpAuthKey, pTAppTypeInfo->szAuthKey, GWS_APP_TYPE_AUTH_KEY_LEN))
		{
			CAppSrvLog::Instance().Write(
				LEVEL_LOG_ERROR, 
				"CAppSrvAuthMgr::Auth failed, ProjectID:%lu, ClientAuthKey;%s, CurAuthKey:%s, dwAppTypeID:%lu, IP:%s",
				nProjectID, lpAuthKey, pTAppTypeInfo->szAuthKey, dwAppTypeID, szIP);

			//��֤ʧ��
			return FALSE;
		}

		CAutoLock<CLockObject> aCAutoLock(&m_Lock);

        if (m_AppSrvAuthMap.find(lSocketID) == m_AppSrvAuthMap.end())
        {
            m_AppSrvAuthMap[lSocketID].dwAppTypeID = dwAppTypeID;
			m_AppSrvAuthMap[lSocketID].dwProjectID = nProjectID;
        }
        else//�Ѿ�����
        {
            m_AppSrvAuthMap[lSocketID].dwAppTypeID = dwAppTypeID;
			m_AppSrvAuthMap[lSocketID].dwProjectID = nProjectID;
        }

		CAppSrvLog::Instance().Write(
			LEVEL_LOG_INFO, 
			"CAppSrvAuthMgr::Auth login, SocketID:%ld, ProjectID:%lu, dwAppTypeID:%lu, IP:%s",
			lSocketID, nProjectID, dwAppTypeID, szIP);

		return TRUE;
	}

    void CAppSrvAuthMgr::SetSocketRoute(LONG lSocketID)
    {
        CAutoLock<CLockObject> aCAutoLock(&m_Lock);

        //////////////////////////////////////////////////////////////////////////
        //��֤�Ƿ�ɹ���¼��δ��¼��APPSRV�򲻿��ṩҵ�����

        AppSrvAuthMap::iterator entity = m_AppSrvAuthMap.find(lSocketID);

        if (entity == m_AppSrvAuthMap.end())
        {
            return;
        }

        //////////////////////////////////////////////////////////////////////////
        //�ж��Ƿ��Ѿ����ڸ�ApppType
		DWORD dwProjectID = entity->second.dwProjectID;
		DWORD dwAppTypeID = entity->second.dwAppTypeID;

		// �����ڸ���Ŀ
		if (m_projAppTypeSocketMap.find(dwProjectID) == m_projAppTypeSocketMap.end())
		{
			AppTypeSocketMap& aTAppTypeSocketMap = m_projAppTypeSocketMap[dwProjectID];
			TAppTypeSocketInfo& aTAppTypeSocketInfo = aTAppTypeSocketMap[dwAppTypeID];
			
			aTAppTypeSocketInfo.lSocketIDArray = new(std::nothrow) LONG[m_wMaxSocketCount];

			if (!aTAppTypeSocketInfo.lSocketIDArray)
			{
				CAppSrvLog::Instance().Write(
					LEVEL_LOG_ERROR, 
					"CAppSrvAuthMgr::SetSocketRoute aTAppTypeSocketInfo.lSocketIDArray new memory failed");

				return;
			}

			aTAppTypeSocketInfo.wCurSocketIDCount = 0;

			aTAppTypeSocketInfo.wIndex = 0;

		}
		
		// ��Ŀ�ﲻ���ڴ� APPSrv		
		AppTypeSocketMap& aTAppTypeSocketMap = m_projAppTypeSocketMap[dwProjectID];
        if (aTAppTypeSocketMap.find(dwAppTypeID) == aTAppTypeSocketMap.end())
        {
            TAppTypeSocketInfo& aTAppTypeSocketInfo = aTAppTypeSocketMap[dwAppTypeID];

            aTAppTypeSocketInfo.lSocketIDArray = new(std::nothrow) LONG[m_wMaxSocketCount];

            if (!aTAppTypeSocketInfo.lSocketIDArray)
            {
                CAppSrvLog::Instance().Write(
                    LEVEL_LOG_ERROR, 
                    "CAppSrvAuthMgr::SetSocketRoute aTAppTypeSocketInfo.lSocketIDArray new memory failed");

                return;
            }

            aTAppTypeSocketInfo.wCurSocketIDCount = 0;

            aTAppTypeSocketInfo.wIndex = 0;
        }

        //////////////////////////////////////////////////////////////////////////
        //Ѱ���Ƿ��Ѿ����ڵ�SocketID

        TAppTypeSocketInfo& aTAppTypeSocketInfo = aTAppTypeSocketMap[dwAppTypeID];

        WORD wIndex = 0;

        for (wIndex = 0; wIndex < aTAppTypeSocketInfo.wCurSocketIDCount; ++wIndex)
        {
            //����Ѿ�ע�ᣬ��ֱ�ӷ��سɹ�
            if (aTAppTypeSocketInfo.lSocketIDArray[wIndex] == lSocketID)
            {
                return;
            }
        }

        //////////////////////////////////////////////////////////////////////////
        //��û�м���

        if (aTAppTypeSocketInfo.wCurSocketIDCount >= m_wMaxSocketCount)
        {
            m_AppSrvAuthMap.erase(lSocketID);

            CAppSrvLog::Instance().Write(
                LEVEL_LOG_ERROR, 
                "CAppSrvAuthMgr::SetSocketRoute over max socket count%hu",
                m_wMaxSocketCount);

            return;
        }

        aTAppTypeSocketInfo.lSocketIDArray[aTAppTypeSocketInfo.wCurSocketIDCount] = lSocketID;
        aTAppTypeSocketInfo.wCurSocketIDCount += 1;
    }

    void CAppSrvAuthMgr::RemoveSocketRoute(LONG lSocketID)
    {
        CAutoLock<CLockObject> aCAutoLock(&m_Lock);

        //////////////////////////////////////////////////////////////////////////
        //��֤�Ƿ�ɹ���¼��δ��¼��APPSRV�򲻿��ṩҵ�����

        AppSrvAuthMap::iterator entity = m_AppSrvAuthMap.find(lSocketID);

        if (entity == m_AppSrvAuthMap.end())
        {
            return;
        }

		//////////////////////////////////////////////////////////////////////////
		//�ж��Ƿ��Ѿ����ڸ���Ŀ

		if (m_projAppTypeSocketMap.find(entity->second.dwProjectID) == m_projAppTypeSocketMap.end())
		{
			return;
		}

        //////////////////////////////////////////////////////////////////////////
        //�ж��Ƿ��Ѿ����ڸ�ApppType
		AppTypeSocketMap& aTAppTypeSocketMap = m_projAppTypeSocketMap[entity->second.dwProjectID];
        if (aTAppTypeSocketMap.find(entity->second.dwAppTypeID) == aTAppTypeSocketMap.end())
        {
            return;
        }

        //////////////////////////////////////////////////////////////////////////
        //Ѱ���Ƿ��Ѿ����ڵ�SocketID

        TAppTypeSocketInfo& aTAppTypeSocketInfo = aTAppTypeSocketMap[entity->second.dwAppTypeID];

        WORD wIndex = 0;

        for (wIndex = 0; wIndex < aTAppTypeSocketInfo.wCurSocketIDCount; ++wIndex)
        {
            if (aTAppTypeSocketInfo.lSocketIDArray[wIndex] == lSocketID)
            {
                aTAppTypeSocketInfo.lSocketIDArray[wIndex] = aTAppTypeSocketInfo.lSocketIDArray[aTAppTypeSocketInfo.wCurSocketIDCount - 1];

                aTAppTypeSocketInfo.wCurSocketIDCount -= 1;
            }
        }
    }

    BOOL CAppSrvAuthMgr::IsLogined(LONG lSocketID, DWORD& dwAppTypeID)
    {
        CAutoLock<CLockObject> aCAutoLock(&m_Lock);

        AppSrvAuthMap::const_iterator entity = m_AppSrvAuthMap.find(lSocketID);

        if (entity == m_AppSrvAuthMap.end())
        {
            return FALSE;
        }

        dwAppTypeID = entity->second.dwAppTypeID;

        return TRUE;
    }

    void CAppSrvAuthMgr::RemoveAppSrv(LONG lSocketID)
    {
        CAutoLock<CLockObject> aCAutoLock(&m_Lock);

		AppSrvAuthMap::iterator entity = m_AppSrvAuthMap.find(lSocketID);

		if (entity == m_AppSrvAuthMap.end())
		{
            return;
		}

		projAppTypeSocketMap::iterator projAppTypeSocketEntity = m_projAppTypeSocketMap.find(entity->second.dwProjectID);

		if (projAppTypeSocketEntity == m_projAppTypeSocketMap.end())
		{
			return;
		}

		AppTypeSocketMap& aTAppTypeSocketMap = m_projAppTypeSocketMap[entity->second.dwProjectID];
        AppTypeSocketMap::iterator appTypeSocketEntity = aTAppTypeSocketMap.find(entity->second.dwAppTypeID);
        
        if (appTypeSocketEntity == aTAppTypeSocketMap.end())
        {
            return;
        }

        WORD wIndex = 0;
        
        for (wIndex = 0; wIndex < appTypeSocketEntity->second.wCurSocketIDCount; ++wIndex)
        {
            if (appTypeSocketEntity->second.lSocketIDArray[wIndex] == lSocketID)
            {
                appTypeSocketEntity->second.lSocketIDArray[wIndex] = appTypeSocketEntity->second.lSocketIDArray[appTypeSocketEntity->second.wCurSocketIDCount - 1];

                appTypeSocketEntity->second.wCurSocketIDCount -= 1;
            }
        }

        m_AppSrvAuthMap.erase(entity);
    }

    USHORT CAppSrvAuthMgr::GetAppSrvCount()
    {
        USHORT uCount = 0;

        CAutoLock<CLockObject> aCAutoLock(&m_Lock);

        uCount = (USHORT)m_AppSrvAuthMap.size();

        return uCount;
    }

	BOOL CAppSrvAuthMgr::GetAppSrvSocketID(DWORD dwProjectID, DWORD dwAppTypeID, LONG& lSocketID)
	{
		if (dwProjectID <= 0)
		{
			return FALSE;
		}

		CAutoLock<CLockObject> aCAutoLock(&m_Lock); 
	
		projAppTypeSocketMap::iterator projEntity = m_projAppTypeSocketMap.find(dwProjectID);
		projAppTypeSocketMap::iterator projEndEntity = m_projAppTypeSocketMap.end();
		if (projEntity == projEndEntity)
		{
			return FALSE;
		}

		AppTypeSocketMap& aTAppTypeSocketMap = m_projAppTypeSocketMap[dwProjectID];
		AppTypeSocketMap::iterator entity = aTAppTypeSocketMap.find(dwAppTypeID);
		AppTypeSocketMap::iterator endEntity = aTAppTypeSocketMap.end();
		if (entity == endEntity)
		{
			return FALSE;
		}

        WORD wIndex = entity->second.wIndex;

        WORD wCount = entity->second.wCurSocketIDCount;

        if (0 == wCount)
        {
            return FALSE;
        }

        WORD wTemp = wIndex % wCount;

        entity->second.wIndex += 1;

        lSocketID = entity->second.lSocketIDArray[wTemp];
	
		return TRUE;
	}
}