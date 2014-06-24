#ifndef GWS_DATA_MGR_H
#define GWS_DATA_MGR_H

#include "GWS_Common.h"
#include "GWS_ConfigDataMgr.h"

namespace GWS
{
	//���ݿ����ù�����
	class CDataMgr : public CSingletonT<CDataMgr>
	{
	public:
	
		virtual ~CDataMgr();

		friend class CSingletonT<CDataMgr>;

		//���ܣ���ʼ�������ض�̬�����ȡ���ݣ�
		//����ֵ��TRUE �ɹ���FALSE ʧ��
		BOOL Init();

		//���ܣ���������
		//����ֵ��TRUE �ɹ���FALSE ʧ��
		BOOL ReLoad();


        const TAppType* GetAppType(int nProjectID, unsigned short sAppTypeID);

	private:

		CDataMgr();
        CConfigDataAppTypeMgr m_CConfigDataAppTypeMgr;
	};
}

#endif