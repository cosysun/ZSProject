#ifndef __MSG_QUEUE_H__
#define __MSG_QUEUE_H__


#include "AutoLock.h"
#include "../config.h"
/*************************************
*    define the constant
*************************************/
#define RESULT_SUCC                0     //succ 
#define RESULT_BUFFERWARN          1     //the buffer will full

#define RESULT_DATA_TIMEOUT        4     //timeout
#define RESULT_ERROR               5     //error
#define RESULT_BUFFERFULL          6     //buffer full
#define RESULT_BUFFERSMALL         7     //the buffer is small
#define RESULT_NODATA              8     //no data
#define RESULT_SYSTEMERROR         9     //system error
#define RESULT_ALLDATA_USING       10    //data using
#define RESULT_DATA_NOTFIND        11    //data not found
#define RESULT_DATA_PARAMERROR     12    //param error
#define RESULT_SYSTEM_NOINIT       13    //no init

#define DATAFLAG_HAS               1     //has data
#define DATAFLAG_NONE              0     //no data

//

GV_BEGIN

class BASECLASS_API CMsg_Queue
{
public:
	CMsg_Queue(Void);
	~CMsg_Queue(Void);
public:
	//push data
	Int Push(Void* pData,Long nDataLen);
	//pop data,no thread safe,ulMillTime:millisecond 
    Int Pop(Void* &pData,Long &nDataLen,ULong  ulMillTime = 3600000);
	//pop data,thhread safe,ulMillTime:millisecond 
	Int Pop_M(Void*   pData,Long  nMaxSize,Long &nDataLen_R,ULong  ulMillTime = 3600000);
	//inti
	Int Init(Long nMaxSpace, Long nMaxSize,Long  nMaxSize_Warning = 500, Bool bUseSemaphoreFlag = false);
	//get the buffer size
	Long Size(){return m_nSize;}
	//get the max size of memory
	ULong MemorySize_Max();
	//get the size of memory using
	ULong GetSize_Memory();
	//clear
	Void  Clear();
	//get the data len of the next package
	ULong GetDataLen_NextPackage();	
private:
	//
	Void   NotifySemaphore();
	//
	Void   WaitSemaphore(ULong  ulMillTime = 0);//modied by gavin on 2013.7.17
protected:
	CLockObject            m_SingleOjbect;//
	Semaphore*             m_pSemaphore;//
	Bool                   m_bUseSemaphore;//
	Void*                  m_pMsgQueue;//
	Long                   m_nWritePos;//write position
	Long                   m_nReadPos;//read position
	Long                   m_nDataSpaceLen;//space len
	Long                   m_nMaxSize;//max item
	Long                   m_nSize;//item count
	Long                   m_nReallocMemCount;//
	Long                   m_nPackageCount_timeout;//
	Long                   m_nMaxSize_Warning;//
	ULong                  m_ulMemorySize_Used;//
	//
protected:
	//
	virtual Void Destroy();
};


GV_END

#endif