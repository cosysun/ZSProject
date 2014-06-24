#include "GWS_LogDataProcess.h"
#include "GWS_Log.h"
#include "GWS_Config.h"

namespace GWS
{
    CLogDataProcess::CLogDataProcess()
    {
    }

    CLogDataProcess::~CLogDataProcess()
    {
    }

	//数据库连接
	BOOL CLogDataProcess::ConnectDB()
	{
		if (m_DBConn.IsOpen())
		{
			return TRUE;
		}

		char szConn[1024] = {0};

		sprintf(szConn, "Provider=SQLOleDB;Data Source=%s;Initial Catalog=%s;User Id=%s;Password=%s",
			CAppConfig::Instance().GetLogDBAddr(),
			CAppConfig::Instance().GetLogDBName(),
			CAppConfig::Instance().GetLogDBUserName(),
			CAppConfig::Instance().GetLogDBPwd());

		try
		{
			if (CAppConfig::Instance().GetLogIsSafe())
			{
				if (!m_DBConn.Open_Security(szConn, CAppConfig::Instance().GetLogDBUserName(), 
					CAppConfig::Instance().GetLogDBPwd()))
				{
					char szErrorDec[1024] = {0};
					m_DBConn.GetErrorDescription(szErrorDec, 1024);

					CSystemLog::Instance().Write(
						LEVEL_LOG_ERROR, 
						"CLogDataProcess::ConnectDB Open Failed, Error:%s",
						szErrorDec);

					return FALSE;
				}
			}
			else
			{
				if (!m_DBConn.Open(szConn, CAppConfig::Instance().GetLogDBUserName(), 
					CAppConfig::Instance().GetLogDBPwd()))
				{
					char szErrorDec[1024] = {0};
					m_DBConn.GetErrorDescription(szErrorDec, 1024);

					CSystemLog::Instance().Write(
						LEVEL_LOG_ERROR, 
						"CLogDataProcess::ConnectDB Open Failed, Error:%s",
						szErrorDec);

					return FALSE;
				}
			}
		}
		catch (...)
		{
			CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CLogDataProcess::ConnectDB Exception!");

			return FALSE;
		}

		return TRUE;
	}

	BOOL CLogDataProcess::WriteOmasLogInfo()
	{
		if (!ConnectDB())
		{
			CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CLogDataProcess::WriteOmasLogInfo Connect failed!");

			return FALSE;
		}

		try
		{
			CADOCommand pAdoCommand(&m_DBConn,"P_GWS_Write_OmasLog",adCmdStoredProc);

			CADOParameter pParamRetVal(CADORecordset::typeInteger, sizeof(int),CADOParameter::paramReturnValue);
			pAdoCommand.AddParameter(&pParamRetVal);			//注意：返回参数必须以第一个参数加入

			
			CADORecordset aRecordset(&m_DBConn);
			if(!aRecordset.Execute(&pAdoCommand))
			{
				char szErrorBuff[1024] = {0};
				aRecordset.GetLastErrorString(szErrorBuff, 1024);

				CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CLogDataProcess::WriteOmasLogInfo Execute Error: %s",
					szErrorBuff);

				m_DBConn.Close();

				return FALSE;
			}	

			int nValue = 0;
			pParamRetVal.GetValue(nValue);
			if (0 != nValue)
			{
				//数据库的语句执行出错
				CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CLogDataProcess::WriteOmasLogInfo ResCode:%ld", 
					nValue);

				return FALSE;
			}
		}
		catch(...)
		{		
			CSystemLog::Instance().Write(LEVEL_LOG_ERROR, "CLogDataProcess::WriteOmasLogInfo occur exception");

			m_DBConn.Close();

			return FALSE;
		}

		return TRUE;
	}
}
