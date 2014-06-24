#ifndef SYSBASE_UTILITY_H
#define SYSBASE_UTILITY_H

#include "BaseTypeDef.h"

#include <vector>
using namespace std;

class CAppInfoImp;

class CAppInfo
{
public:

    virtual ~CAppInfo();

    static CAppInfo& Instance();

    //功能:获取应用程序名(包含完整路径)
    //返回值: 应用程序名(包含完整路径)
    PCSTR GetAppFilePath();

    //功能:获取应用程序名(不包含完整路径)
    //返回值: 应用程序名
    PCSTR GetAppFileName();
  
    //功能:获取应用程序所在目录
    //返回值: 应用程序所在目录
    PCSTR GetAppDirectory();
    
private:

    CAppInfo();

    static CAppInfo s_CAppInfo;
    
    class CAppInfoImp* m_pCAppInfoImp;
};

void ToStringIP(DWORD dwIP, LPSTR lpDest, DWORD dwSize = 33);

int SplitStr(const string &str,const char delimiters, vector<string> &v);

DWORD GetLocalIP();

#endif