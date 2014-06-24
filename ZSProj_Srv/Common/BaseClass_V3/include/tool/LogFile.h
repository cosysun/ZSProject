
/*----------------------------------
*
*/
#ifndef __LOG_H__
#define __LOG_H__

#include "../config.h"
#include "AutoLock.h"

GV_BEGIN

#define  MAX_SIZE_LOGFILE                (10*1024*1024)    //max length of the file

//the level of the log
typedef enum
{
	LEVEL_LOG_FAILD=1, //faild
	LEVEL_LOG_ERROR,   //error
	LEVEL_LOG_WARNING, //warning
	LEVEL_LOG_INFO,    //info
	LEVEL_LOG_DEBUG    //debug
}LEVEL_LOG;
//-----------------------
class BASECLASS_API CLogFile  
{
public:
	//
	CLogFile();
	virtual ~CLogFile();
	
	//write log
	Bool WriteLogInfo(Char *pszLogInfo,LEVEL_LOG nDebugLevel = LEVEL_LOG_ERROR);
	//write log
	Bool WriteLogInfo(LEVEL_LOG nDebugLevel,Char*  pszFormat,...);
	//
	ULong GetFileSize();
	//
	Bool InitLogFile(Char* pFilePath,Char* pFileName,LEVEL_LOG nDebugLevel = LEVEL_LOG_ERROR,CULong ulFileSize_Max = MAX_SIZE_LOGFILE);
	//
	Void SetDebugLevel(const  LEVEL_LOG   nDebugLevel);
	//
	Void Enable(){CAutoLock<CLockObject>  autoLock(&m_LockObject);m_bEnable = true;}
	//
	Void Disable(){CAutoLock<CLockObject>  autoLock(&m_LockObject);m_bEnable = false;}
	//
	Bool IsEnable(){CAutoLock<CLockObject>  autoLock(&m_LockObject);return m_bEnable;}
private:
	//
	Void CheckFileSize();
	//
	Void CloseFile();
	//
	Bool OpenFile(Char*  pFilePath,Char*  pszMode);
	//
	Bool WriteInfo(Char*   pszInfo,LEVEL_LOG nDebugLevel);
	//
	Void AddFileSize(ULong nSize){m_ulFileSize += nSize;}
private:
	Char                   m_szLogFilePath[260];//
	ULong          m_ulFileSize;//
	LEVEL_LOG              m_nDebugLevel;//
	FILE*                  m_hFile;//
	CLockObject            m_LockObject;//
	ULong          m_ulMax_FileSize;//
	Bool                   m_bEnable;//
};


//
Void BASECLASS_API LOG_SetDir(CChar*   pDir);
//
Void BASECLASS_API LOG_SetLogLevel(CChar*  pszFile,LEVEL_LOG   logLevel);
//
Void BASECLASS_API LOG_LogInfo(CChar*  pFileName,LEVEL_LOG  logLevel,Char*   pInfo);
//
Void BASECLASS_API LOG_LogInfo2(CChar* pFileName,LEVEL_LOG  logLevel,Char* pszFormat,...);
//
Void BASECLASS_API LOG_Disable(CChar*  pFileName);
//
Void BASECLASS_API LOG_Enable(CChar*  pFileName);
//
Void BASECLASS_API LOG_DisableAll();
//
Void BASECLASS_API LOG_EnableAll();
//
Void BASECLASS_API LOG_CloseAll();




GV_END

#endif 
