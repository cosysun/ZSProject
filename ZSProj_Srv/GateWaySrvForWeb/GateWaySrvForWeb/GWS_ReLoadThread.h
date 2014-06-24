#ifndef GWS_RELOAD_THREAD_H
#define GWS_RELOAD_THREAD_H

#include "GWS_Common.h"

namespace GWS
{
	//������Ϣ�߳�
	class CReloadThread : public CCustomThread, public CSingletonT<CReloadThread>
	{
	public:

		virtual ~CReloadThread();

		//���ܣ�����
		//����ֵ��TRUE �ɹ���FALSE ʧ��
		BOOL Start();

		//���ܣ�ֹͣ
		void Stop();

		//���ܣ��������ݿ�������Ϣ
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