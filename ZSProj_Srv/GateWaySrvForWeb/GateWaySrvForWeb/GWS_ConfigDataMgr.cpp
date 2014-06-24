#include "GWS_ConfigDataMgr.h"
#include "GWS_Log.h"
#include "GWS_Config.h"

namespace GWS
{
	//////////////////////////////////////////////////////////////////////////
    //CConfigDataAppTypeMgr

    CConfigDataAppTypeMgr::CConfigDataAppTypeMgr()
    {
        m_bValid1 = FALSE;

        m_nVersion = -1;

        m_bLoadFromLocal = FALSE;
    }

    CConfigDataAppTypeMgr::~CConfigDataAppTypeMgr()
    {

    }

    BOOL CConfigDataAppTypeMgr::Init(BOOL bLoadFromLocal)
    {
        m_bLoadFromLocal = bLoadFromLocal;

        if (bLoadFromLocal)
        {
            return this->LoadFromLocal();
        }

        if (!this->LoadFromDB())
        {
            return this->LoadFromLocal();
        }

        return TRUE;
    }

    BOOL CConfigDataAppTypeMgr::ReLoad()
    {
        if (m_bLoadFromLocal)
        {
            return FALSE;
        }

        return this->LoadFromDB();
    }

    const TAppType* CConfigDataAppTypeMgr::GetAppType(int nProjectID, unsigned short sAppTypeID)
    {
        ProjAppTypeMap* pPorjAppTypeMap = this->GetValidAppTypeMap();

		ProjAppTypeMap::iterator iter = pPorjAppTypeMap->find(nProjectID);
		if (iter == pPorjAppTypeMap->end())
		{
			return NULL;
		}
		
		AppTypeMap& aTAppTypeMap = iter->second;
        AppTypeMap::iterator entity = aTAppTypeMap.find(sAppTypeID);
        if (entity == aTAppTypeMap.end())
        {
            return NULL;
        }
        return &entity->second;
    }

    BOOL CConfigDataAppTypeMgr::LoadFromDB()
    {
		int nNewVersion = 0;

        //////////////////////////////////////////////////////////////////////////

        ProjAppTypeMap* pProjAppTypeMap = NULL;

        if (m_bValid1)
        {
            pProjAppTypeMap = &m_ProjAppTypeMap2;
        }
        else
        {
            pProjAppTypeMap = &m_ProjAppTypeMap1;
        }

        pProjAppTypeMap->clear();

        //////////////////////////////////////////////////////////////////////////

        try
        {
            CADODatabase dbconn;

            dbconn.SetConnectionTimeout(10);

            CHAR szConn[1024] = {0};

            sprintf(szConn, "Provider=SQLOleDB;Data Source=%s;Initial Catalog=%s;User Id=%s;Password=%s",
                CAppConfig::Instance().GetDBAddr(),
                CAppConfig::Instance().GetDBName(),
                CAppConfig::Instance().GetDBUserName(),
                CAppConfig::Instance().GetDBPwd());

            if (CAppConfig::Instance().GetIsSafe())
            {
                if (!dbconn.Open_Security(szConn, 
                    CAppConfig::Instance().GetDBUserName(), 
                    CAppConfig::Instance().GetDBPwd()))
                {
                    CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CAdoConfigDataMgr::LoadFromDB connect db failed !");

                    return FALSE;
                }
            }
            else
            {
                if (!dbconn.Open(szConn, 
                    CAppConfig::Instance().GetDBUserName(), 
                    CAppConfig::Instance().GetDBPwd()))
                {
                    CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CAdoConfigDataMgr::LoadFromDB connect db failed !");

                    return FALSE;
                }
            }

            //////////////////////////////////////////////////////////////////////////
            //获取AppType
            {
                CADOCommand command(&dbconn, "P_GWS_GetAppType", adCmdStoredProc);

                CADOParameter returnParam(CADORecordset::typeInteger, sizeof(int),CADOParameter::paramReturnValue);
                command.AddParameter(&returnParam);

                CADOParameter versionParam(CADORecordset::typeInteger, sizeof(int),CADOParameter::paramInput);
                versionParam.SetValue(m_nVersion);
                command.AddParameter(&versionParam);

                CADOParameter newVersionParam(CADORecordset::typeInteger, sizeof(int),CADOParameter::paramOutput);
                command.AddParameter(&newVersionParam);

                CADORecordset recordset(&dbconn);

                if (!recordset.Execute(&command))
                {
                    CHAR szInfo[1024] = {0};
                    recordset.GetLastErrorString(szInfo, 1023);

                    CSystemLog::Instance().Write(LEVEL_LOG_ERROR, 
                        "CAdoConfigDataMgr::LoadFromDB db proc(P_GWS_GetAppType) exec failed, Info:%s", 
                        szInfo);

                    return FALSE;
                }

                LONG lCount = 0;
                returnParam.GetValue(lCount);
				
				newVersionParam.GetValue(nNewVersion);

                if (lCount > 0)
                {
                    int nValue = 0;

					short nTemp = 0;
 
                    while(!recordset.IsEOF())
                    {
						if (!recordset.GetFieldValue("CProjectID", nValue))
						{
							return FALSE;
						}
						AppTypeMap& aTAppTypeMap = (*pProjAppTypeMap)[nValue];

                        if(!recordset.GetFieldValue("CID", nValue))
                        {
                            return FALSE;
                        }
                        TAppType& aTAppType = aTAppTypeMap[nValue];

                        if(!recordset.GetFieldValue("CAuthKey", aTAppType.szAuthKey, GWS_APP_TYPE_AUTH_KEY_LEN))
                        {
                            return FALSE;
                        }

                        if(!recordset.GetFieldValue("CIsAuthAction", aTAppType.bIsAuthAciton))
                        {
                            return FALSE;
                        }

                        if(!recordset.GetFieldValue("CIsAuthClientIP", aTAppType.bIsAuthClientIP))
                        {
                            return FALSE;
                        }

                        recordset.MoveNext();
                    }
                }
                else
                {
                    CSystemLog::Instance().Write(LEVEL_LOG_INFO, "CAdoConfigDataMgr::LoadFromDB P_GWS_GetAppType get 0 count !");

                    return TRUE;
                }
            }
        }
        catch (...)
        {
            CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CAdoConfigDataMgr::LoadFromDB db operation failed !");

            return FALSE;
        }

        this->SaveToLocal(pProjAppTypeMap);
        
        m_nVersion = nNewVersion;

        m_bValid1 = !m_bValid1;

        return TRUE;
    }

    BOOL CConfigDataAppTypeMgr::LoadFromLocal()
    {
        ProjAppTypeMap* pProjAppTypeMap = NULL;

        if (m_bValid1)
        {
            pProjAppTypeMap = &m_ProjAppTypeMap2;
        }
        else
        {
            pProjAppTypeMap = &m_ProjAppTypeMap1;
        }

        pProjAppTypeMap->clear();

        if (this->LoadFromLocal(pProjAppTypeMap))
        {
            m_bValid1 = !m_bValid1;
        }
        else
        {
            return FALSE;
        }

        return TRUE;
    }

    BOOL CConfigDataAppTypeMgr::LoadFromLocal(ProjAppTypeMap* pProjAppTypeMap)
    {
        CHAR szFilePath[MAX_PATH * 2] = {0};

        sprintf(szFilePath, "%s\\ConfigData\\AppType.xml", CAppInfo::Instance().GetAppDirectory());

        try
        {
            TiXmlDocument aXmlDoc;

            if (!aXmlDoc.LoadFile(szFilePath))
            {
                return FALSE;
            }

            TiXmlElement *pRoot = aXmlDoc.RootElement(); 

            //第一个子节点  
            TiXmlElement *pCurNode = pRoot->FirstChildElement("AT"); 

            //遍历获取指定节点数据  
            while(pCurNode)  
            {
				int nProjectID = 0;
				pCurNode->Attribute("PID", &nProjectID);
				AppTypeMap& aTAppTypeMap = (*pProjAppTypeMap)[nProjectID];

				TiXmlElement *pChildNode = pCurNode->FirstChildElement("ATI");

				while (pChildNode)
				{
					int nAppTypeID = 0;
					int nValue = 0;

					//子节点第一个属性 id  
					pChildNode->Attribute("ID", &nAppTypeID);

					TAppType& aTAppType = aTAppTypeMap[nAppTypeID];

					const char* pTemp = pChildNode->Attribute("KEY");

					if (pTemp)
					{
						int nLen = strlen(pTemp);

						strncpy(aTAppType.szAuthKey, pTemp, nLen > GWS_APP_TYPE_AUTH_KEY_LEN ? GWS_APP_TYPE_AUTH_KEY_LEN : nLen);
					}
					else
					{
						aTAppType.szAuthKey[0] = NULL;
					}

					pChildNode->Attribute("IAA", &nValue);
					aTAppType.bIsAuthAciton = (BOOL)nValue;

					pChildNode->Attribute("IACIP", &nValue);
					aTAppType.bIsAuthClientIP = (BOOL)nValue;

					pChildNode = pChildNode->NextSiblingElement("ATI");  
				}

                pCurNode = pCurNode->NextSiblingElement("AT");  
            }
        }
        catch (...)
        {
            return FALSE;
        }

        return TRUE;
    }

    void CConfigDataAppTypeMgr::SaveToLocal(ProjAppTypeMap* pProjAppTypeMap)
    {
        if (0 == pProjAppTypeMap->size())
        {
            return;
        }

        CHAR szDirPath[MAX_PATH * 2] = {0};
        CHAR szFilePath[MAX_PATH * 2] = {0};

        sprintf(szDirPath, "%s\\ConfigData", CAppInfo::Instance().GetAppDirectory()); 

        SHCreateDirectoryEx(NULL, szDirPath, NULL);

        sprintf(szFilePath, "%s\\AppType.xml", szDirPath);

        TiXmlDocument aXmlDoc;

        aXmlDoc.LinkEndChild(new TiXmlDeclaration("1.0" , "",""));

        TiXmlElement *pXmlRoot = new TiXmlElement("AppType");

        ProjAppTypeMap::iterator entity = pProjAppTypeMap->begin();
        ProjAppTypeMap::iterator endEntity = pProjAppTypeMap->end();

        for (; entity != endEntity; ++entity)
        {
			TiXmlElement *pXmlProj = new TiXmlElement("AT");
			pXmlProj->SetAttribute("PID", entity->first);

			AppTypeMap& aTAppTypeMap = entity->second;
			for (AppTypeMap::iterator iter = aTAppTypeMap.begin(); iter != aTAppTypeMap.end(); iter++)
			{
				char szIP[50] = {0};

				TiXmlElement *pXmlSub = new TiXmlElement("ATI");
				pXmlSub->SetAttribute("ID" , iter->first);
				pXmlSub->SetAttribute("KEY" , iter->second.szAuthKey);
				pXmlSub->SetAttribute("IAA" , (int)iter->second.bIsAuthAciton);
				pXmlSub->SetAttribute("IACIP", (int)iter->second.bIsAuthClientIP);

				pXmlProj->LinkEndChild(pXmlSub);
			}
			pXmlRoot->LinkEndChild(pXmlProj);
        }

        aXmlDoc.LinkEndChild(pXmlRoot);

        aXmlDoc.SaveFile(szFilePath); 
    }

    CConfigDataAppTypeMgr::ProjAppTypeMap* CConfigDataAppTypeMgr::GetValidAppTypeMap()
    {
        if (m_bValid1)
        {
            return &m_ProjAppTypeMap1;
        }

        return &m_ProjAppTypeMap2;
    }
}