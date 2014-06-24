#ifndef GWS_CONFIG_MONITOR_THREAD_H
#define GWS_CONFIG_MONITOR_THREAD_H

#include "GWS_Common.h"

namespace GWS
{   
	//ϵͳ�����ļ������
	class CConfigMonitorThread : public CCustomThread, public CSingletonT<CConfigMonitorThread>
	{
	public:

		//���ܣ�����
		//����ֵ��TRUE �ɹ���FALSE ʧ��
		BOOL Start();

		//���ܣ�ֹͣ
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
