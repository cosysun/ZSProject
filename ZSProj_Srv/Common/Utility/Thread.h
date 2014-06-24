#ifndef THREAD_H
#define THREAD_H

#include "BaseTypeDef.h"
#include <list>
#include <vector>
#include <map>
#include <stdio.h>

using namespace std;

//////////////////////////////////////////////////////////////////////////
//IThreadProcHandler 线程处理

class IThreadProcHandler
{
public:
	IThreadProcHandler(){};
	virtual ~IThreadProcHandler(){};

	virtual DWORD Proc(LPVOID lpParameter) = 0;
};

//////////////////////////////////////////////////////////////////////////
//CCustomThread 自定义线程类，该类只能通过继承来使用，必须实现Proc接口

class CCustomThreadImp;

class CCustomThread : public IThreadProcHandler
{
public:

    //功能:构造函数
    //参数:
    //      LPVOID lpParameter IN 线程函数的参数
    //      SIZE_T dwStackSize IN 线程块大小，0值时默认为系统默认大小
    CCustomThread(LPVOID lpParameter, SIZE_T dwStackSize = 0);

	virtual ~CCustomThread();

    //功能:启动线程
    virtual BOOL Start();

    //功能:停止线程
    //参数:
    //      DWORD dwWaitTime IN 等待多时秒后中止线程
    //      DWORD dwExitCode IN 线程退出代码
    virtual void Stop(DWORD dwWaitTime, DWORD dwExitCode = 0);

protected:

    //功能:线程处理过程
    //参数:
    //      LPVOID lpParameter IN 线程函数的参数
    //返回值: 线程返回值
    virtual DWORD Proc(LPVOID lpParameter) = 0;

private:

    CCustomThreadImp* m_pCCustomThreadImp;
};

//////////////////////////////////////////////////////////////////////////
//CCustomMultiThread 多线程，该类只能通过继承来使用，必须实现Proc接口

class CCustomMultiThreadImp;

class CCustomMultiThread : public IThreadProcHandler
{
public:

    //功能:初始化
    CCustomMultiThread();

    virtual ~CCustomMultiThread();

    //功能:启动创建线程池
    //参数:
    //      UINT  uThreadCount IN 线程个数
    //      LPVOID lpParameter IN 线程函数的参数
    //      SIZE_T dwStackSize IN 线程块大小，0值时默认为系统默认大小
    //异常
    virtual BOOL Start(UINT uThreadCount, LPVOID lpParameter, SIZE_T dwStackSize = 0);

    //功能:等待多少毫秒后结束停止并且关闭所有线程
    //参数:
    //      DWORD dwMilliseconds IN 多少毫秒,如果为 0 时马上结束所有线程
    virtual void Stop(DWORD dwMilliseconds);
    
    //功能:获取线程ID数组
    //参数:
	//      vector<DWORD>* pThreadIDArray OUT 线程ID数组
    void GetThreadIDArray(vector<DWORD>* pThreadIDArray);

protected:

    //功能:等待多少毫秒后结束线程
    //参数:
    //      LPVOID lpParameter IN 线程函数的参数
    //返回值: 线程返回值
    virtual DWORD Proc(LPVOID lpParameter) = 0;

private:

    vector<HANDLE> m_ThreadHandleAarray;

    vector<DWORD> m_ThreadIDAarray;

    LPVOID m_lpParameter;//线程的参数

    BOOL m_bStop;   //停止标志
    
    class CCustomMultiThreadImp* m_pCCustomMultiThreadImp;
};

//////////////////////////////////////////////////////////////////////////
//ITask

class ITask
{
public:

	ITask();

	virtual ~ITask();

	virtual void Proc(LPVOID lpParam) = 0;

	virtual void Stop() = 0;

};

//////////////////////////////////////////////////////////////////////////
//ITaskFactory

class ITaskFactory
{
public:
	ITaskFactory();

	virtual ~ITaskFactory();

	virtual ITask* Create() = 0;

	virtual void Release(ITask* pITask) = 0;
};

//////////////////////////////////////////////////////////////////////////
//CTaskMultiThread

class CTaskMultiThreadImp;

class CTaskMultiThread
{
public:
	
	CTaskMultiThread();

	virtual ~CTaskMultiThread();

	BOOL Start(ITaskFactory* pITaskFactory, UINT uThreadCount, LPVOID lpParameter = NULL);

	void Stop(DWORD dwMilliseconds);

	//功能:获取线程ID数组
	//参数:
	//      vector<DWORD>* pThreadIDArray OUT 线程ID数组
	void GetThreadIDArray(vector<DWORD>* pThreadIDArray);

private:

	class CTaskMultiThreadImp* m_pCTaskMultiThreadImp;
};

//////////////////////////////////////////////////////////////////////////
// 线程类

class CThread
{
public:
    CThread(void);
    virtual ~CThread(void);

    bool CreateThread(DWORD (WINAPI* ThreadFunc)(LPVOID), LPVOID pThreadParam);

    bool CreateThread(DWORD (WINAPI* ThreadFunc)(LPVOID), LPVOID pThreadParam, DWORD& dwThreadID);

    bool WaitForExit(DWORD dwWaitTime = 5*1000);

protected:
    HANDLE m_hThread;
};

#endif