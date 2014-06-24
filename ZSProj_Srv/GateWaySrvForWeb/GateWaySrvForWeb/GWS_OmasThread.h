#ifndef GWS_SNMP_MGR_H
#define GWS_SNMP_MGR_H

#include "GWS_Common.h"

namespace GWS
{
	//SNMP��ʱ�����߳�
	class COmasThread : CCustomThread, public CSingletonT<COmasThread>
	{
	public:

		//ͳ����Ϣ����
		enum EOMAS_TYPE
		{
			OMAS_TYPE_CLIENT_RECV_COUNT = 0,					//�ͻ��˽��հ���
			OMAS_TYPE_CLIENT_SEND_COUNT,						//�ͻ��˷��Ͱ���
			OMAS_TYPE_CLIENT_CONNECT_COUNT,						//�ͻ�����

			OMAS_TYPE_CLIENT_REQ,								//�ͻ���������
			OMAS_TYPE_CLIENT_REQ_SUCCESS,						//�ͻ�������ɹ���
			OMAS_TYPE_CLIENT_REQ_FAILED,						//�ͻ�������ʧ����

            OMAS_TYPE_CLIENT_LOGOUT,							//�ͻ��˵ǳ���
            OMAS_TYPE_CLIENT_KEEPALIVE,                         //�ͻ���������
            OMAS_TYPE_CLIENT_OMAS,								//ϵͳ����������
            OMAS_TYPE_CLIENT_UNKNOWN,                           //�ͻ���δ֪������

			OMAS_TYPE_APPSRV_RECV_COUNT,						//�ͻ��˽��հ���
			OMAS_TYPE_APPSRV_SEND_COUNT,						//�ͻ��˷��Ͱ���
			OMAS_TYPE_APPSRV_CONNECT_COUNT,						//�ͻ�����
			
			OMAS_TYPE_APPSRV_LOGIN,								//Ӧ�÷����¼��
			OMAS_TYPE_APPSRV_LOGIN_SUCCESS,						//Ӧ�÷����¼�ɹ���
			OMAS_TYPE_APPSRV_LOGIN_FAILED,						//Ӧ�÷����¼ʧ����

			OMAS_TYPE_APPSRV_RES,								//Ӧ�÷�������
			OMAS_TYPE_APPSRV_RES_SUCCESS,						//Ӧ�÷��������سɹ���
			OMAS_TYPE_APPSRV_RES_FAILED,						//Ӧ�÷���������ʧ����

            OMAS_TYPE_APPSRV_LOGOUT,                            //Ӧ�÷���ǳ���
            OMAS_TYPE_APPSRV_KEEPALIVE,                         //Ӧ�÷���������
            OMAS_TYPE_APPSRV_SYSTEM,                            //ϵͳ��������
            OMAS_TYPE_APPSRV_UNKNOWN,                           //Ӧ�÷���δ֪������

			OMAS_TYPE_MAX_COUNT,								//ͳ����Ϣ���ֵ����������Ч��
		};

		//���ܣ�����
		//����ֵ��TRUE �ɹ���FALSE ʧ��
		BOOL Start();

		//���ܣ�ֹͣ
		void Stop();

		//���ܣ�ĳһ��ͳ����Ϣ����ֵ��1
		//������
		//      EOMAS_TYPE snmpType IN ͳ����Ϣ����
		void Increment(EOMAS_TYPE snmpType);

		//���ܣ�ĳһ��ͳ����Ϣ����ֵ��1
		//������
		//      EOMAS_TYPE snmpType IN ͳ����Ϣ����
		void Decrement(EOMAS_TYPE snmpType);


		virtual ~COmasThread();

		friend class CSingletonT<COmasThread>;

	protected:

		virtual DWORD Proc(LPVOID lpParameter);

		void SendSnmpInfo();

	private:

		COmasThread();

		//////////////////////////////////////////////////////////////////////////

		BOOL m_bStop;

		CEvent m_StopEvent;

		LONG m_Snmp[COmasThread::OMAS_TYPE_MAX_COUNT];
	};
}

#endif