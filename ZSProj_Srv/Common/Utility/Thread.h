#ifndef THREAD_H
#define THREAD_H

#include "BaseTypeDef.h"
#include <list>
#include <vector>
#include <map>
#include <stdio.h>

using namespace std;

//////////////////////////////////////////////////////////////////////////
//IThreadProcHandler �̴߳���

class IThreadProcHandler
{
public:
	IThreadProcHandler(){};
	virtual ~IThreadProcHandler(){};

	virtual DWORD Proc(LPVOID lpParameter) = 0;
};

//////////////////////////////////////////////////////////////////////////
//CCustomThread �Զ����߳��࣬����ֻ��ͨ���̳���ʹ�ã�����ʵ��Proc�ӿ�

class CCustomThreadImp;

class CCustomThread : public IThreadProcHandler
{
public:

    //����:���캯��
    //����:
    //      LPVOID lpParameter IN �̺߳����Ĳ���
    //      SIZE_T dwStackSize IN �߳̿��С��0ֵʱĬ��ΪϵͳĬ�ϴ�С
    CCustomThread(LPVOID lpParameter, SIZE_T dwStackSize = 0);

	virtual ~CCustomThread();

    //����:�����߳�
    virtual BOOL Start();

    //����:ֹͣ�߳�
    //����:
    //      DWORD dwWaitTime IN �ȴ���ʱ�����ֹ�߳�
    //      DWORD dwExitCode IN �߳��˳�����
    virtual void Stop(DWORD dwWaitTime, DWORD dwExitCode = 0);

protected:

    //����:�̴߳������
    //����:
    //      LPVOID lpParameter IN �̺߳����Ĳ���
    //����ֵ: �̷߳���ֵ
    virtual DWORD Proc(LPVOID lpParameter) = 0;

private:

    CCustomThreadImp* m_pCCustomThreadImp;
};

//////////////////////////////////////////////////////////////////////////
//CCustomMultiThread ���̣߳�����ֻ��ͨ���̳���ʹ�ã�����ʵ��Proc�ӿ�

class CCustomMultiThreadImp;

class CCustomMultiThread : public IThreadProcHandler
{
public:

    //����:��ʼ��
    CCustomMultiThread();

    virtual ~CCustomMultiThread();

    //����:���������̳߳�
    //����:
    //      UINT  uThreadCount IN �̸߳���
    //      LPVOID lpParameter IN �̺߳����Ĳ���
    //      SIZE_T dwStackSize IN �߳̿��С��0ֵʱĬ��ΪϵͳĬ�ϴ�С
    //�쳣
    virtual BOOL Start(UINT uThreadCount, LPVOID lpParameter, SIZE_T dwStackSize = 0);

    //����:�ȴ����ٺ�������ֹͣ���ҹر������߳�
    //����:
    //      DWORD dwMilliseconds IN ���ٺ���,���Ϊ 0 ʱ���Ͻ��������߳�
    virtual void Stop(DWORD dwMilliseconds);
    
    //����:��ȡ�߳�ID����
    //����:
	//      vector<DWORD>* pThreadIDArray OUT �߳�ID����
    void GetThreadIDArray(vector<DWORD>* pThreadIDArray);

protected:

    //����:�ȴ����ٺ��������߳�
    //����:
    //      LPVOID lpParameter IN �̺߳����Ĳ���
    //����ֵ: �̷߳���ֵ
    virtual DWORD Proc(LPVOID lpParameter) = 0;

private:

    vector<HANDLE> m_ThreadHandleAarray;

    vector<DWORD> m_ThreadIDAarray;

    LPVOID m_lpParameter;//�̵߳Ĳ���

    BOOL m_bStop;   //ֹͣ��־
    
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

	//����:��ȡ�߳�ID����
	//����:
	//      vector<DWORD>* pThreadIDArray OUT �߳�ID����
	void GetThreadIDArray(vector<DWORD>* pThreadIDArray);

private:

	class CTaskMultiThreadImp* m_pCTaskMultiThreadImp;
};

//////////////////////////////////////////////////////////////////////////
// �߳���

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