#ifndef GWS_CONFIG_MONITOR_THREAD_H
#define GWS_CONFIG_MONITOR_THREAD_H

#include "GWS_Common.h"

namespace GWS
{   
	//系统配置文件监控类
	class CConfigMonitorThread : public CCustomThread, public CSingletonT<CConfigMonitorThread>
	{
	public:

		//功能：启动
		//返回值：TRUE 成功；FALSE 失败
		BOOL Start();

		//功能：停止
		void Stop();

		virtual ~CConfigMonitorThread();

		friend class CSingletonT<CConfigMonitorThread>;

	protected:

		virtual DWORD Proc(LPVOID lpParameter);

	private:

		CConfigMonitorThread();

		BOOL m_bStop;

		CEvent m_hStopEvent;
	};
}

#endif
