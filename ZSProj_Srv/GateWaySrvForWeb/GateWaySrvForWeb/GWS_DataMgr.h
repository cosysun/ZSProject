#ifndef GWS_DATA_MGR_H
#define GWS_DATA_MGR_H

#include "GWS_Common.h"
#include "GWS_ConfigDataMgr.h"

namespace GWS
{
	//数据库配置管理类
	class CDataMgr : public CSingletonT<CDataMgr>
	{
	public:
	
		virtual ~CDataMgr();

		friend class CSingletonT<CDataMgr>;

		//功能：初始化（加载动态库与读取数据）
		//返回值：TRUE 成功；FALSE 失败
		BOOL Init();

		//功能：重载数据
		//返回值：TRUE 成功；FALSE 失败
		BOOL ReLoad();


        const TAppType* GetAppType(int nProjectID, unsigned short sAppTypeID);

	private:

		CDataMgr();
        CConfigDataAppTypeMgr m_CConfigDataAppTypeMgr;
	};
}

#endif