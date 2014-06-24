#ifndef __PROTOCOL_TRACKER_H__
#define __PROTOCOL_TRACKER_H__

#include "../config.h"
#include "AutoLock.h"

#include <list>
using namespace std;
using namespace GV;

GV_BEGIN


//
typedef struct tagHeader_Tracker
{
	ULong        ulTime_Trakcer;//
	ULong        ulThreadID;//
	Char         szData[24];//
}Header_Tracker,*PHeader_Tracker,**PPHeader_Tracker;

//
class CPackTracker
{
public:
	CPackTracker(Void);
	~CPackTracker(Void);
public:
	//
	Void  Clear();
	//
	Int  Create(Long  lMaxCountItem,Short sLenAllPackage,Short sOffset_PackageSizeData,Short sLen_PackageSizeData,Short sOffset_PackageCmdData,Short sLen_PackageCmdData);
	//
	Void PushData(Char*  pData,Int  nDataLen);
	//print
	Void Print();
	//
	Short GetSize_Tracker(){return m_sLen_AllTrackerData;}
private:
	//
	Void Destroy();
private:
	Long                m_lMaxCount;//
	Long                m_lCounter;//
	Long                m_lPos_Write;//
	Long                m_lPos_Oldest;//
	Short               m_sLen_AllTrackerData;//
	Short               m_sOffset_PackageSizeData;//
	Short               m_sLen_PackageSizeData;//
	Short               m_sOffset_PackageCmdData;//
	Short               m_sLen_PackageCmdData;//
	//
	CLockObject         m_lock_obj;
	PHeader_Tracker     m_pList_Tracker;//
};

GV_END

#endif