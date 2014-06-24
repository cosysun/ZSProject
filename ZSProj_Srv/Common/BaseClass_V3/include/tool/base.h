
#ifndef __BASE_H__
#define __BASE_H__

//
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "../config.h"

using namespace std;

GV_BEGIN

/*--------------------------------------------
_       class CBase
--------------------------------------------*/
class BASECLASS_API CBase  
{
public:
	CBase();
	virtual ~CBase();
public:	
	//last param : max length of the buffer
	static Void TrimString(CChar*  pString,Char*   pReString,Int nMaxLen);
	//
	static Int StringToInt(CChar*  pString);
	//
	static Void IntToString(Int intData,Char*   pReString,Int nMaxLen);
	//
	static Void LongToString(Long  nData,Char*   pReString,Int nMaxLen);
	//
	static Void ULongToString(ULong  ulData,Char*   pReString,Int nMaxLen);
	//
	static Void DoubleToString(Double dblVal, Char*   pReString,Int nMaxLen,Int ndigits=20);
	//
	static Void FloatToString(Float fValue, Char*   pReString,Int nMaxLen);
	//
	static Void CharToString(Char charData , Char*   pReString,Int nMaxLen);
	//
	static Void ToUpper(Char*  pString);
	//
	static Void ToLower(Char*  pString);
	//
	/***********************************************
	get the local time by format
    eg£º2005-08-30 21:09:09
    eg: "%Y-%m-%d %H:%M:%S"
    ***********************************************/
	static Void GetFormatDate( CChar*  pszFormat,Char*   pReString,Int nMaxLen);	
	//
	static Void Sleep(CInt intSecond,CInt intMillisecond );
	//make dir (support recursive)
	static Void MakeDir(CChar*  pszDir );
	//change ip addr 
	static Void IPChange_IntToString(ULong  ulIP , Char*   pReString,Int nMaxLen);
	//change ip addr
	static ULong IPChange_StringToInt(CChar* pszIP);
	//get format date .the return value is ULong
	static ULong TimeString2Time(Char *szDate,Char *szTime);
	//get the path of the current module runing
	static Void  GetLocalModuleFilePath(Char*   pszPath,Int  nMaxLen);
	//get file size
	static Int64  GetFileLen(Char*   pszFile);
	//copy file
	static Int  Copy_File(Char*   pFile_Source,Char*   pFile_Dest);
	//get tick count
	static ULong Get_Tickcount();
	//set block socket
	static Void Set_Noblocking(Int   nSocket);
	//close socket 
	static Void CloseSocket(Int  nSocket);
	//conert time_t to string
	static Void Time_L2String(ULong   ulTime,Char*  pszTime,Int  nMaxLen);
	//get time milliseconds
	static UInt64 Get_Time();
};


GV_END

#endif
