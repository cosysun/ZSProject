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

    //����:��ȡӦ�ó�����(��������·��)
    //����ֵ: Ӧ�ó�����(��������·��)
    PCSTR GetAppFilePath();

    //����:��ȡӦ�ó�����(����������·��)
    //����ֵ: Ӧ�ó�����
    PCSTR GetAppFileName();
  
    //����:��ȡӦ�ó�������Ŀ¼
    //����ֵ: Ӧ�ó�������Ŀ¼
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