#ifndef GWS_RELOAD_THREAD_H
#define GWS_RELOAD_THREAD_H

#include "GWS_Common.h"

namespace GWS
{
	//重载信息线程
	class CReloadThread : public CCustomThread, public CSingletonT<CReloadThread>
	{
	public:

		virtual ~CReloadThread();

		//功能：启动
		//返回值：TRUE 成功；FALSE 失败
		BOOL Start();

		//功能：停止
		void Stop();

		//功能：重载数据库数据信息
		void ReloadDBInfo();

		friend class CSingletonT<CReloadThread>;

	protected:

		virtual DWORD Proc(LPVOID lpParameter);

	private:

		CReloadThread();

		BOOL m_bStop;

		CEvent m_ReloadEvent;
	};

}

#endif