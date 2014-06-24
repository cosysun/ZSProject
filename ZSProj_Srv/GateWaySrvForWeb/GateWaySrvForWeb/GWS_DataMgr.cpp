#include "GWS_DataMgr.h"
#include "GWS_Log.h"
#include "GWS_Config.h"

namespace GWS
{
	//////////////////////////////////////////////////////////////////////////

	CDataMgr::CDataMgr()
	{
	}

	CDataMgr::~CDataMgr()
	{
	}

	BOOL CDataMgr::Init()
	{
        if (!m_CConfigDataAppTypeMgr.Init(CAppConfig::Instance().GetIsAuthFromLocal()))
        {
            CSystemLog::Instance().Write(LEVEL_LOG_ERROR, 
                "CDataMgr::Init CConfigDataAppTypeMgr Init failed");

            return FALSE;
        }

		return TRUE;
	}

	BOOL CDataMgr::ReLoad()
	{
        m_CConfigDataAppTypeMgr.ReLoad();

		return TRUE;
	}

    const TAppType* CDataMgr::GetAppType(int nProjectID, unsigned short sAppTypeID)
    {
        return m_CConfigDataAppTypeMgr.GetAppType(nProjectID, sAppTypeID);
    }
}