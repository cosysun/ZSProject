#ifndef __MEM_POOL_H__
#define __MEM_POOL_H__

#include "../config.h"
#include "packet.h"
#include "AutoLock.h"
#include "linklist_ex.h"

#include <list>
#include <new>
//
using namespace std;

GV_BEGIN

//
#define MAX_COUNT_MEMPOOL_MEMCELL            10 //max count of memory cell
/********************************************
*   error code
********************************************/
#define ERROR_SUCC                          0  //succ 
#define ERROR_MALLOCL_MEMORY                -1 //new memory faild
#define ERROR_MEMPOOL_FULL                  -2 //the memory pool full

/***************************************************
-
-
***************************************************/
class CMem_Pool;
LIST_LINKER(mem_cell_linker);
class BASECLASS_API CMem_Cell : public CPacket_Buffer,public mem_cell_linker
{
	
public:
	CMem_Cell();
	//
	virtual ~CMem_Cell();
public:
	//
	virtual Void* Get_Buffer(Void){return m_pBuffer;}
	//
	virtual UInt Get_Buffer_Size(Void){return m_nBufferSize;}
	//
	virtual Void Add_Reference(Void)
	{
		CAutoLock<CLockObject>        autoLock(&m_Lock);
		//
		m_nReference++;
	}
	//
	virtual Void Del_Reference(Bool destroy = false);
public:
	//
	Void SetSize(CULong nSize){m_nSize = nSize;}
	//
	Void SetSpace(Void* pSpace){m_pSpace = pSpace;}
	//
	Void* GetSpace(){return m_pSpace;};
	//
	ULong GetSize(){return m_nSize;}
	//
	Int CheckBuffer(CULong  nSize);
	//
	Void SetMaster(CMem_Pool*    pMemPool = Null)
	{
		m_pMaster = pMemPool;
	}
	//

private:
	ULong     m_nSize;//
	ULong     m_nBufferSize;//
	Void*             m_pSpace;//
	Void*             m_pBuffer;//
	//
	CLockObject       m_Lock;
	ULong     m_nReference;
	CMem_Pool*        m_pMaster;//
};

/***************************************************
-
-
***************************************************/
class BASECLASS_API CMem_Pool
{
public:
	CMem_Pool(Void);
	~CMem_Pool(Void);
public:
	//
	Void  SetMaxCount_MemCell(ULong  nCount)
	{
		//
		if(nCount > m_nMaxCount_Cell)
		{
			m_nMaxCount_Cell = nCount;
		}
		//
	}
	//init memory pool
	template <class T>
	Int  Create(CUInt  nNum)
	{
		//
		CAutoLock<CLockObject>        autoLock(&m_Lock);
		Int        i = 0;
		T*         pValue;
		//
		for(i=0;i<nNum;++i)
		{
			pValue = Cell_Malloc<T>();
			//
			if(Null != pValue)
			{
				m_List_Idle.push_back(pValue);
			}
		}
		//
		return 0;
	}
	//get memory from memory pool
	template<class T>
	T* Get(CUInt  nSize = 0,Bool construct = false)
	{		
		//
		T*              pReValue = Null;
		Long            nSize_Wish = sizeof(T);
		Long            nSize_Real = 0;
		CMem_Cell*      pMemCell = Null;		
		//
		{
			//
			CAutoLock<CLockObject>        autoLock(&m_Lock);
			//
			if(m_List_Idle.size() > 0)
			{				
				pMemCell = STA_CAST(CMem_Cell*,m_List_Idle.front());				
			}
			//
			if(Null == pMemCell)
			{
				pReValue = Cell_Malloc<T>();
				
			}			
			else
			{
				//
				nSize_Real = pMemCell->GetSize();
				if(nSize_Real != nSize_Wish)
				{
					//
					if(nSize_Real < nSize_Wish)
					{
						pReValue = Cell_Malloc<T>();
					}
					else
					{
						m_List_Idle.pop_front();
						
						pReValue = Cell_Placement<T>(pMemCell,construct);
						
					}
				}
				else
				{
					m_List_Idle.pop_front();
					
					pReValue = Cell_Placement<T>(pMemCell,construct);
					
				}						
			}
			//
			if(nSize > pReValue->Get_Buffer_Size())
			{
				// 
				SubMemory(pReValue->Get_Buffer_Size());
				AddMemory(nSize);
				//
				pReValue->CheckBuffer(nSize);			
			}	
			
			//
			m_List_Busy.push_back(pReValue);
		}

		//	
		//
		return pReValue;
	}
	//get memory from memory pool
	template<class T>
	T* Get_Ex(CUInt  nSize = 0,Bool construct = false)
	{		
		//
		//
		T*              pReValue = Get<T>(nSize,construct);
		//
		if(Null == pReValue)
		{
			return Null;
		}
		//
		pReValue->SetMaster(this);

		//
		return pReValue;
	}
	//release memory
	Void Give(CMem_Cell*  pCell,Bool destruct);
	//release memory
	Void Give_Ex(CMem_Cell*  pCell)
	{
		Give(pCell,false);
	}
	//release free memory
	Void FreeList_Idle();
	//release busy memory
	Void FreeList_Busy();
	//get memory size
	ULong GetMemorySize(){return m_nSize_Memory;}
	//release all memory
	Void Destroy();
	//reset memory
	Void Reset(Bool  destruct = true);
	//get busy cell count
	Long GetCellCount_Busy(){return m_List_Busy.size();}
	//get idle cell count
	Long GetCellCount_Idle(){return m_List_Idle.size();}
	//get cell count waiting for del
	Long GetCellCount_WaitDel(){return STA_CAST(Long,m_List_WaitDel.size());}
	//get cell count
	Long GetCellCount(){return m_List_Busy.size()+m_List_Idle.size()+m_List_WaitDel.size();}
	//
private:
	//
	template <class T>
	T* Cell_Malloc()
	{
		ULong  nSize = sizeof(T);
		Void*    pSpace = GV_NEW Char[nSize];
		T*       pValue = Null;
		//
		if(Null == pSpace)
		{
			return Null;
		}
		//
		pValue = GV_NEW (pSpace)T();
		if(Null == pValue)
		{
			return Null;
		}
		//
		pValue->SetSpace(pSpace);
		pValue->SetSize(nSize);
		//
		AddMemory(nSize);
		//
		return pValue;
	}
	//
	template <class T>
	T* Cell_Placement(CMem_Cell*   pCell,Bool construct = true)
	{
		T*           pReValue = Null;
		Void*        pSpace = Null;
		ULong  nSize = 0;
		//
		if(construct == false)
		{
			//
			pReValue = STA_CAST(T*,pCell);
			
		}
		else
		{
			//
			pSpace = pCell->GetSpace();
			nSize = pCell->GetSize();
			//
			pReValue = GV_NEW (pSpace)T();
			//
			pReValue->SetSpace(pSpace);
			pReValue->SetSize(nSize);
			//
		}
		//
		return pReValue;
	}
	//
	Void AddMemory(Long  nSize)
	{
		CAutoLock<CLockObject>        autoLock(&m_Lock_MemSize);
		m_nSize_Memory += nSize;
	}
	//
	Void SubMemory(Long  nSize)
	{
		CAutoLock<CLockObject>        autoLock(&m_Lock_MemSize);
		m_nSize_Memory -= nSize;
	}
	//	
	Void DestroyMemCell(CMem_Cell*   pCell);
	//
	Void FreeList_WaitDel();
private:
	LIST(List_Mem_Cell,mem_cell_linker);
	//
	List_Mem_Cell                      m_List_Idle;
	List_Mem_Cell                      m_List_Busy;
	List_Mem_Cell                      m_List_WaitDel;
	ULong                              m_nSize_Memory;//the size of all memory
	ULong                              m_nMaxCount_Cell;//max memory cell count
	CLockObject                        m_Lock;
	CLockObject                        m_Lock_List_WaitDel;//
	CLockObject                        m_Lock_MemSize;

};

GV_END

#endif