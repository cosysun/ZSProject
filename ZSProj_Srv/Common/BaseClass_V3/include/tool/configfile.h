#ifndef __CONFIGFILE_H__
#define __CONFIGFILE_H__

#include "../config.h"
#include "AutoLock.h"
#include <fstream>
#include <iostream>

/*--------------------------------------
*        ini file format
*    [base]
*       ip = 127.0.0.1
*       port = 1234
*    [db]
*       srvip = 192.168.8.239
*       port = 1433
----------------------------------------*/


GV_BEGIN

class BASECLASS_API CConfigFile
{
public:
	CConfigFile(Void);
	~CConfigFile(Void);
public:
	//open file (pszFileName:   d:/a/b/c.ini)
	Bool  OpenFile(Char* pszFileName);
	//read string(return value :data len of return value
	ULong ReadString(Char* pszSection,Char* pszKey,Char*   pszReBuffer,ULong   ulMaxBufferLen);
	//read integer(return: true(succ) false(faild))
	Bool  ReadInteger(Char* pszSection,Char* pszKey,Int&    nValue);
private:
	//destroy all data
	Void  Destroy();
private:
	Char                     m_szFileName[512];//file name
	Bool                     m_bFlag_Open;//
	map<string,Void*>        m_map_keyvalue;//
	CLockObject              m_lock;
};


GV_END

#endif


