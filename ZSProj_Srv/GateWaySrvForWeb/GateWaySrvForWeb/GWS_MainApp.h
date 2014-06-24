#ifndef GWS_APP_H
#define GWS_APP_H

#include "GWS_Common.h"

namespace GWS
{
    class CMainApp : public CGVApp
    {
    public:

        CMainApp();

        virtual ~CMainApp();

        virtual bool InitInstance();

        virtual int ExitInstance();

        virtual int Run(int argc, char** argv);

        virtual void Stop();

    protected:

        //����:��ʼ����Ҫ�Ķ���
        //����ֵ��TRUE���ɹ�;FALSE ʧ��
        BOOL Init();

        //����:����������Ϣ
        //����ֵ��TRUE���ɹ�;FALSE ʧ��
        BOOL LoadConfig();

        //����:����������Ϣ
        //����ֵ��TRUE���ɹ�;FALSE ʧ��
        BOOL Load();

        //��������
        void RunService();

        //�¼�:������������������Ӧ��ҵ���߼��̣߳���Ҫ�ڸ��¼���ʹ��ѭ���ȴ���Ϣ��
        //����ֵ: TRUE ������Ϣ�ȴ� FALSE �˳�
        virtual BOOL OnRun();

        //�¼�: ֹͣ����
        virtual void OnStop();

    private:

        CEvent m_StopEvent;
    };

    CMainApp g_CMainApp;
}

#endif