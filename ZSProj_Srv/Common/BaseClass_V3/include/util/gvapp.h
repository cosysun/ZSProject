#ifndef __CGVAPP_H__
#define __CGVAPP_H__

#include <string>
using namespace std;

//
#define Null  0

class CGVApp
{
public:
	CGVApp();
	~CGVApp(void);
	//��ȡ���������
	char*  GetServiceName(){return m_szServiceName;}
	//��ȡ�������ʾ����
	char*  GetDisplayName(){return m_szDisplayName;}
	//��ȡ���߳�ָ��
	static CGVApp*  GetApp(){return m_pInstance;}
	//��ʼ��
	virtual bool InitInstance();
	//���̺߳���
	virtual int Run(int argc,char**  agrv)=0;
	//ֹͣ
	virtual void Stop()=0;
	//�ͷ���Դ
	virtual int ExitInstance();
	//ע�����
	bool RegisterService(int argc,char** argv);
	//���·����״̬
	void ReportServiceStatus(unsigned long ulCurrentStatus);
	//���÷������Ϣ
	void SetServiceInfo(char*   pszServiceName,char*  pszDisplayName);
	//��ȡ��ǰ�������е�Ŀ¼����󲻴�"\"
	string GetLocalModuleFilePath();
private:
	char         m_szServiceName[256];
	char         m_szDisplayName[256];
protected:
	static CGVApp*      m_pInstance;
};

//
extern int        g_nArgc;
extern char**     g_pszArgv;

#endif
