#ifndef __LOGFILE_MGR_H__
#define __LOGFILE_MGR_H__

#include "../config.h"
#include "LogFile.h"
#include "AutoLock.h"
#include "base.h"
//
#include <map>
using namespace std;


GV_BEGIN

class CLogFileMgr
{
public:
	CLogFileMgr();
	~CLogFileMgr();
public:
	//set dir
	Void SetLogDir(CChar*  pDir);
	//close one file log
	Void Disable(CChar*  pFileName);
	//open one file log
	Void Enable(CChar*  pFileName);
	//close all file log
	Void DisableAll();
	//open all file log
	Void EnableAll();
	//write log
	Void Log(CChar*  pFileName,LEVEL_LOG  logLevel,Char*   pInfo);
	//write log
	Void Log2(CChar* pFileName,LEVEL_LOG  logLevel,Char* pszFormat,...);
	//close all file
	Void CloseAll();
	//set file log level
	Void SetLogLevel(CChar* pFileName,LEVEL_LOG  logLevel);
private:
	
private:
	map<string,CLogFile*>                 m_map_logfile;
	CLockObject                           m_Lock;
	string                                m_strDir;
};

GV_END

#endif