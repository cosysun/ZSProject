

#ifndef __THREAD_THREAD_POOL_H__
#define __THREAD_THREAD_POOL_H__

#include "../config.h"
#include "../tool/mem_pool.h"
#include "../tool/AutoLock.h"
#include "thread.h"

GV_BEGIN

class CThread_Pool;
class CWork_Thread;

//=============================================================================
class BASECLASS_API CTask
{
public:
    CTask() {m_pThread = Null; m_id = 0;}
    virtual ~CTask() {m_pThread = Null; m_id = 0;}
	//get task id
    UInt Get_id(Void) const {return m_id;}
	//set task id
    Void Set_id(UInt task_id) {m_id = task_id;}
	//get thread 
    const CWork_Thread* Get_thread(Void) const {return m_pThread;}
	//set thread 
    Void Set_thread(CWork_Thread* thread ) {m_pThread = thread;}
	//task run function
    template<typename ParamType>
    Void Run(ParamType& param) = 0;
	//
protected:
	virtual const CTask& operator= (const CTask& copy_task)
	{
		this->m_id = copy_task.m_id;
		this->m_pThread = copy_task.m_pThread;
		return *this;
	}
	//
private:
    UInt               m_id;//task id
    CWork_Thread*      m_pThread;

};

//=============================================================================
class  CTask_Queue
{
public:
	//
	class CTask_Cell_Base :public CMem_Cell
	{
	public:
		virtual Void Task_exec(Void) =0;
		//
		virtual Void Set_thread(CWork_Thread*) =0;
	};
	//
	template <typename Task, typename TaskParam>
	class CTask_Cell	: public CTask_Cell_Base
	{
	public:
		Void Init(Task& task, TaskParam& task_param)
		{
			m_task = task;
			m_task_param = task_param;
		}

		//
		Void Task_exec(Void) {m_task.Run(m_task_param);}
		//
		Void Set_thread(CWork_Thread* wthread) {m_task.Set_thread(wthread);}

	private:
		Task m_task;
		TaskParam m_task_param;

	};

	//
	template <typename Task, typename TaskParam>
	class CTask_Cell<Task*, TaskParam> : public CTask_Cell_Base
	{
	public:
		Void Init(Task* task, TaskParam& task_param)
		{
		    m_task = task;
			m_task_param = task_param;
		}

		Void Task_exec(Void) {m_task->Run(m_task_param);}
		//
		Void Set_thread(CWork_Thread* wthread) {m_task->Set_thread(wthread);}

	private:
		Task* m_task;
		TaskParam m_task_param;
	};

public:
	~CTask_Queue(Void)
	{
		m_task_cell_list.clear();
	}
	//
    template <typename Task, typename TaskParam>
	Void Alloc_task(UInt alloc_task_num)
	{
		CAutoLock<CLockObject>          autoLock(&m_lock_heap);
		m_task_cell_heap.Create<CTask_Cell<Task, TaskParam> >(alloc_task_num);
	}
	//
    template <typename Task, typename TaskParam>
    Void Add_task(Task& task, TaskParam& task_param)
    {
		CTask_Cell<Task, TaskParam>* tcell;
		{
			CAutoLock<CLockObject>          autoLock(&m_lock_heap);
			tcell = m_task_cell_heap.Get<CTask_Cell<Task, TaskParam> >();
		}

		tcell->Init(task, task_param);

		{
			CAutoLock<CLockObject>          autoLock(&m_lock_list);
			m_task_cell_list.push_back(tcell);
		}
    }
	//
	CTask_Cell_Base* Get_task(Void)
	{
		CAutoLock<CLockObject>          autoLock(&m_lock_list);
		if(m_task_cell_list.size() <=0)
			return Null;
		CTask_Cell_Base*   tcell = STA_CAST(CTask_Cell_Base*, m_task_cell_list.front());
		if (tcell)
			m_task_cell_list.pop_front();
		return tcell;
	}
	//
	Void Del_task(CTask_Cell_Base* task)
	{
		if(task)
		{
			CAutoLock<CLockObject>          autoLock(&m_lock_heap);
			m_task_cell_heap.Give(task,true);
		}
	}

	//

protected:

private:	
	list<CTask_Cell_Base*>     m_task_cell_list;
	CMem_Pool                  m_task_cell_heap;
	//
	CLockObject                m_lock_list;
	CLockObject                m_lock_heap;
};

//=============================================================================

class BASECLASS_API CWork_Thread : public CMem_Cell
{
	typedef CTask_Queue::CTask_Cell_Base Task_Cell_Base;
	//
public:
    CWork_Thread();
    virtual ~CWork_Thread();

	Void Set_Priority(Int priority) {if (m_pThread) m_pThread->Set_Priority(priority);}
	//
	Void Join(Void) {if (m_pThread) m_pThread->Join();}
	//
	Void Exit(Void);
	//
	UInt Get_ThreadID(Void) 
	{
		if (m_pThread) 
			return m_pThread->Get_ThreadID(); 
		else 
			return 0;
	}
	//
	Void Set_task(Task_Cell_Base* tcell, Bool signal = true);
	//
    Void Set_thread_pool(CThread_Pool* thrpool) {m_thread_pool = thrpool;}
	//
    const CThread_Pool* Get_thread_pool(Void) const {return m_thread_pool;}
	//
    Void Process(Void);

protected:

private:
    Semaphore            m_condition;
    CLockObject          m_Lock;
	//
	CThread*             m_pThread;
    CThread_Pool*        m_thread_pool;
    Task_Cell_Base*      m_task;
    volatile Bool        m_exit;
	//
	function_ex*         m_process_func;

};

//=============================================================================
class BASECLASS_API CThread_Pool
{
	typedef CTask_Queue::CTask_Cell_Base Task_Cell_Base;
	friend class CWork_Thread;
	//
public:
    CThread_Pool(UInt thread_num = 0);
    virtual ~CThread_Pool();
	
	//create thread for thread pool
	Void Create_thread(UInt num = 4);
	//set thread's priority
	Void Set_Thread_Priority(Int priority);
	//exit thread pool.all thread exit
	Void Exit(Void);
	//get thread count
	Int Get_thread_num(Void)
	{
		CAutoLock<CLockObject>        autoLock(&m_Lock);
		return m_busy_wthread_list.size() + m_idle_wthread_list.size();
	}
	//
	Int Get_thread_id(UInt* id_buff, Int num)
	{
		Int                              ret_num = 0;
		CAutoLock<CLockObject>        autoLock(&m_Lock);
		CWork_Thread*                    wthread;
		list<CWork_Thread*>::iterator    pos;
		
		for (pos = m_busy_wthread_list.begin();pos != m_busy_wthread_list.end();++pos)
		{
			wthread = STA_CAST(CWork_Thread*,*pos);
			if (ret_num < num)
				id_buff[ret_num++] = wthread->Get_ThreadID();
		}


		for (pos = m_idle_wthread_list.begin();pos != m_idle_wthread_list.end();++pos)
		{
			wthread = STA_CAST(CWork_Thread*,*pos);
			if (ret_num < num)
				id_buff[ret_num++] = wthread->Get_ThreadID();
		}


		return ret_num;
	}
	//create task
    template <typename Task, typename TaskParam>
    Void Create_task(UInt num)
	{
		m_task_queue.Alloc_task<Task, TaskParam>(num);
	}
	//push task
    template <typename Task, typename TaskParam>
    Void Add_task(Task& task, TaskParam& task_param)
	{
		CAutoLock<CLockObject>        autoLock(&m_Lock);
		//
		m_task_queue.Add_task(task, task_param);
		CWork_Thread* wthread;
		{
			wthread = STA_CAST(CWork_Thread*, m_idle_wthread_list.front());
			if (wthread)
			{
				m_idle_wthread_list.pop_front();
				m_busy_wthread_list.push_back(wthread);
			}
		}

		if (wthread)
		{
			Task_Cell_Base* tcell = m_task_queue.Get_task();
			if (tcell)
			{
				wthread->Set_task(tcell,true);
			}
		}
	}
private:
	//
    CWork_Thread* Get_new_wthread(Void);
	//	
	Void Wthread_complete_task(CWork_Thread* wthread, Task_Cell_Base* tcell);
private:	
	CLockObject               m_Lock;
	list<CWork_Thread*>       m_idle_wthread_list;
	list<CWork_Thread*>       m_busy_wthread_list;
	CTask_Queue               m_task_queue;
	CMem_Pool                 m_mem_threadpool;

};

GV_END

#endif 
