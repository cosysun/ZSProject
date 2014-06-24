#ifndef SYSBASE_WEB_SERVER_H
#define SYSBASE_WEB_SERVER_H
 
#include <WinSock2.h>
#include <vector>
#include <queue>
#include <map>

#include "ThreadSync.h"

using namespace std;

class CWebServer;

class CWebServerHandler
{
public:

    CWebServerHandler(){};

    virtual ~CWebServerHandler(){};

    virtual void OnAccept(CWebServer* pCWebServer, DWORD dwSocketID, DWORD dwIP) = 0;

    virtual void OnRecv(CWebServer* pCWebServer, PCHAR pData, DWORD dwSize, DWORD dwIP, DWORD dwSocketID) = 0;

    virtual void OnClose(DWORD dwSocketID, DWORD dwIP) = 0;
};

class CWebServer
{
public:

    CWebServer();

    virtual ~CWebServer();

    BOOL Start(
        CWebServerHandler* pCWebServerHandler,
        LPCSTR lpIP,
        WORD wPort,
        WORD wMaxConnCount,
        WORD wWorkerCount,
        DWORD dwRecvBufferSize,
        DWORD dwTimeout = 0);

    BOOL Start(
        CWebServerHandler* pCWebServerHandler,
        LPCSTR lpIP,
        WORD wPort,
        WORD wMaxConnCount,
        DWORD dwRecvBufferSize,
        DWORD dwTimeout = 0);

    void Stop();
    
    void Send(PCHAR pData, DWORD dwSize, DWORD dwSocketID);

    void CloseSocket(DWORD dwSocketID);

	void CloseSocketSend(DWORD dwSocketID);
    
    WORD GetConnCount();

protected:

    //////////////////////////////////////////////////////////////////////////

    struct TSocketInfo
    {
        SOCKET socket;
        WORD wValue;
        BOOL bValid;
        PCHAR pRecvBuffer;
        DWORD dwRecvSize;
        DWORD dwIP;
            
        DWORD dwLastTickCount;

        CThreadMutex aCThreadMutex;

        WSAOVERLAPPED aOverlapped;
    };
    
    //////////////////////////////////////////////////////////////////////////

    BOOL InitSocketInfoArray();
    
    void ReleaseSocketInfoArray();

    BOOL GetNewSocketInfo(DWORD& dwSocketID, TSocketInfo** pTSocketInfo);
    
    BOOL GetSocketInfo(DWORD dwSocketID, TSocketInfo** pTSocketInfo);

    BOOL GetSocketInfoWithLock(DWORD dwSocketID, TSocketInfo** pTSocketInfo);

    void ReleaseSocketInfo(DWORD dwSocketID);

    void ReleaseSocketInfoWithUnLock(DWORD dwSocketID);
    
    //////////////////////////////////////////////////////////////////////////

    static DWORD WINAPI ProcAccept(LPVOID pParam);

    static DWORD WINAPI ProcCheck(LPVOID pParam);

    static DWORD WINAPI ProcIO(LPVOID pParam);

private:

    //////////////////////////////////////////////////////////////////////////
    //
    
    BOOL m_bStop;

    CWebServerHandler* m_pCWebServerHandler;

    HANDLE m_hIOCP;

    SOCKET m_hListenSocket;

    WORD m_wMaxConnCount;

    WORD m_wWorkerCount;
    
    DWORD m_dwRecvBufferSize;

    DWORD m_dwTimeout;
    
    //////////////////////////////////////////////////////////////////////////
    //工作线程

    HANDLE m_hAcceptThread;

    HANDLE m_hCheckThread;

    vector<HANDLE> m_hIOWorkerThreadArray;

    //////////////////////////////////////////////////////////////////////////
    //SOCKET 资源管理
    
    struct TSocketID
    {
        WORD wIndex;
        WORD wValue;
    };

    CThreadMutex m_FreeSocketThreadMutex;

    TSocketInfo** m_SocketInfoArray;

    queue<DWORD> m_FreeSocketQueue;

    //////////////////////////////////////////////////////////////////////////
};
 
#endif