#include"Utility.h"
#include <stdio.h>

static const CHAR s_StringEmpty[1] = "";

//////////////////////////////////////////////////////////////////////////
//CAppInfoImp

class CAppInfoImp
{
public:

	CAppInfoImp()
	{
		ZeroMemory(m_szFilePath, MAX_PATH);

		ZeroMemory(m_szAppDir, MAX_PATH);

		m_pFileName = NULL;

		GetModuleFileName(NULL, m_szFilePath, MAX_PATH);

		m_pFileName = strrchr(m_szFilePath, '\\') + 1;

		strcpy(m_szAppDir, m_szFilePath);

		PSTR lastPtr =strrchr(m_szAppDir, '\\');

		*lastPtr = NULL;
	}

	virtual ~CAppInfoImp()
	{
	
	}

	PCSTR GetAppFilePath()
	{
		return m_szFilePath;
	}

	PCSTR GetAppFileName()
	{
		return m_pFileName;
	}

	PCSTR GetAppDirectory()
	{
		return m_szAppDir;
	}

private:

	CHAR m_szFilePath[MAX_PATH];

	PSTR m_pFileName;

	CHAR m_szAppDir[MAX_PATH];
};

//////////////////////////////////////////////////////////////////////////
//CAppInfo Ӧ�ó�����Ϣ

CAppInfo CAppInfo::s_CAppInfo;

CAppInfo& CAppInfo::Instance()
{
    return s_CAppInfo;
}

CAppInfo::CAppInfo()
{
	m_pCAppInfoImp = new CAppInfoImp();
}

CAppInfo::~CAppInfo()
{
    if (m_pCAppInfoImp)
    {
		delete m_pCAppInfoImp;
    }
}

PCSTR CAppInfo::GetAppFilePath()
{
	if (!m_pCAppInfoImp)
	{
		return s_StringEmpty;
	}
	
	return m_pCAppInfoImp->GetAppFilePath();
}

PCSTR CAppInfo::GetAppFileName()
{
	if (!m_pCAppInfoImp)
	{
		return s_StringEmpty;
	}
	
	return m_pCAppInfoImp->GetAppFileName();
}

PCSTR CAppInfo::GetAppDirectory()
{
	if (!m_pCAppInfoImp)
	{
		return s_StringEmpty;
	}
	
	return m_pCAppInfoImp->GetAppDirectory();
}

//////////////////////////////////////////////////////////////////////////

void ToStringIP(DWORD dwIP, LPSTR lpDest, DWORD dwSize)
{
    if (!lpDest || dwSize <= 32)
    {
        return;
    }

    BYTE* pByte = (BYTE*)&dwIP;
    
    sprintf(lpDest, "%d.%d.%d.%d", pByte[0], pByte[1], pByte[2], pByte[3]);
}

//////////////////////////////////////////////////////////////////////////

int SplitStr(const string &str,const char delimiters, vector<string> &v)
{
    v.clear();
    string::size_type spos=0;
    string::size_type epos=str.size();
    while((epos=str.find(delimiters,spos))!=string::npos)
    {
        v.push_back(str.substr(spos,epos-spos));
        spos=epos+1;
    }
    v.push_back(str.substr(spos,epos-spos));
    int intRet=(int)v.size();
    return intRet;
}

DWORD GetLocalIP()
{
	char Name[255];//�������ڴ�Ż�õ��������ı��� 
	DWORD dwIP = 0;

	PHOSTENT hostinfo;
	if(gethostname (Name, sizeof(Name)) == 0)
	{ 
		//����ɹ��ؽ������������������name����ָ���Ļ������� 
		if((hostinfo = gethostbyname(Name)) != NULL) 
		{ 
			//���ǻ�ȡ���������������������ɹ��Ļ���������һ��ָ�룬ָ��hostinfo��hostinfo 
			//ΪPHOSTENT�͵ı��������漴���õ�����ṹ�� 
			char *pIP = inet_ntoa(*(struct in_addr *)*hostinfo->h_addr_list);

			return inet_addr(pIP);
		}
	}

	return dwIP;
}