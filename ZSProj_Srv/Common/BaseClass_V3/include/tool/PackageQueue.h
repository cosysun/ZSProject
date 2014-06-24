#ifndef __PACKAGEQUEUE_H__
#define __PACKAGEQUEUE_H__

#include "mem_pool.h"
#include "AutoLock.h"
#include "packet.h"
//#include "linklist.h"


GV_BEGIN

//´íÎó´úÂë
#define ERROR_PACKAGE_QUEUE_SUCC                0 //succ
#define ERROR_PACKAGE_QUEUE_NODATA              -1  //no data
#define ERROR_PACKAGE_QUEUE_NOMEMORY            -2 //new memory faild
#define ERROR_PACKAGE_QUEUE_PARAMS              -3  //param faild
#define ERROR_PACKAGE_QUEUE_SYSTEM              -4  //system faild
#define ERROR_PACKAGE_QUEUE_NOENOUGHMEM         -5  //not enough memory
#define ERROR_PACKAGE_QUEUE_FULL                -6  //buffer full


/***************************************************
-
-
***************************************************/
LIST_LINKER(package_cell_linker);
class BASECLASS_API CPackage_Cell:public CMem_Cell,public package_cell_linker
{
public:
	CPackage_Cell();
	virtual ~CPackage_Cell();
public:
	//
	Void SetPackage_Buffer(CPacket_Buffer*  pBuffer)
	{
		if(Null == pBuffer)
		{
			return;
		}
		//
		m_pPacketbuffer = pBuffer;
		m_pPacketbuffer->Add_Reference();
	}
	//
	CPacket_Buffer*  GetPackage_Buffer(){return m_pPacketbuffer;}
	//
public:
	ULong             m_nSize_Data;//
	ULong             m_nTime_Begin;//
	//
	CPacket_Buffer*           m_pPacketbuffer;
};

/***************************************************
-
-
***************************************************/
class BASECLASS_API CPackage_Queue
{
public:
	CPackage_Queue(Void);
	~CPackage_Queue(Void);
public:
	//init
	Int  Init(CULong nMaxSize_Package,CULong nMaxCount_Package,Bool bUseSemaphore = false);
	//push data
	Int  Push(CVoid*  pData,CLong  nDataLen);
	//push data
	Int  Push(CPacket_Write&     writePacket);
	//get write buffer
	Int  GetWritePackage(CPacket_Write&   writePacket,ULong*   pSize = Null);
	//pop data,ulMillTime:millisecond 
	Int  Pop(Void*  pData,CLong nBufferLen,Long &nDataLen,Bool bDestruct = true,ULong  ulMillTime = 3600000);//modified by gavin on 2013.7.17
	//pop data (get many pack) ,ulMillTime:millisecond 
	Int  Pop_Mp(Void*  pData,CLong nBufferLen,Long &nDataLen,ULong  ulMillTime = 3600000);//add by gavin on 2012.9.21
	//pop data ,ulMillTime:millisecond 
	Int  Pop(CPacket_Read&     readPacket,ULong  ulMillTime = 3600000);//add by gavin on 2013.7.17
	//cell count
	Int  Size(){return m_List_Package.size();}
	//all memory size
	ULong GetSize_Memory();
	//cell count,memory pool
	ULong GetItemCount_MemoryPool();
	//destroy 
	Void  Destroy();
	//get next data len of package list
	ULong GetDataLen_NextPackage();
	
protected:
	//
	Bool  CheckFull()
	{
		CAutoLock<CLockObject>        autoLock(&m_LockObj_PackList);
		if(m_nCount_Package >= m_nMaxCount_Package)
		{
			return true;
		}
		//
		return false;
	}
	//
	Void   NotifySemaphore();
	//
	Void   WaitSemaphore(ULong  ulMillTime = 0);//add by gavin on 2013.7.17
private:
	LIST(CPackage_Cell_List,package_cell_linker);
	CPackage_Cell_List                  m_List_Package;//
	CMem_Pool                           m_MemPool_Mem;//
	CMem_Pool                           m_MemPool_Package;//
	ULong                               m_nMaxSize_Package;//
	ULong                               m_nMaxCount_Package;//
	CLockObject                         m_LockObj_PackList;
	Semaphore*                          m_pSemaphore;//
	Bool                                m_bUseSemaphore;//
	ULong                               m_nCount_Package;//
};


GV_END

#endif
