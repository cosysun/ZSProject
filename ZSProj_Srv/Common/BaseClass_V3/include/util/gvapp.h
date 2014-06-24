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
	//获取服务的名字
	char*  GetServiceName(){return m_szServiceName;}
	//获取服务的显示名字
	char*  GetDisplayName(){return m_szDisplayName;}
	//获取主线程指针
	static CGVApp*  GetApp(){return m_pInstance;}
	//初始化
	virtual bool InitInstance();
	//主线程函数
	virtual int Run(int argc,char**  agrv)=0;
	//停止
	virtual void Stop()=0;
	//释放资源
	virtual int ExitInstance();
	//注册服务
	bool RegisterService(int argc,char** argv);
	//更新服务的状态
	void ReportServiceStatus(unsigned long ulCurrentStatus);
	//设置服务的信息
	void SetServiceInfo(char*   pszServiceName,char*  pszDisplayName);
	//获取当前程序运行的目录，最后不带"\"
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
