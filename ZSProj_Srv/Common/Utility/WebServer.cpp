#include "WebServer.h"

//////////////////////////////////////////////////////////////////////////
//��

#define CWEBSERVER_STOP_FLAG -1 

#define CWEBSERVER_SOCKET_CHECK_UPDATE 1

#define CWEBSERVER_SOCKET_CHECK_DELETE 0

#define CWEBSERVER_MIN_CHECK_TIME 30000

//////////////////////////////////////////////////////////////////////////

CWebServer::CWebServer()
{
    m_bStop = TRUE;
    
    m_SocketInfoArray = NULL;
}

CWebServer::~CWebServer()
{
    this->Stop();
}

BOOL CWebServer::Start(
   CWebServerHandler* pCWebServerHandler,
   LPCSTR lpIP,
   WORD wPort,
   WORD wMaxConnCount,
   WORD wWorkerCount,
   DWORD dwRecvBufferSize,
   DWORD dwTimeout)
{
    if (!m_bStop)
    {
        return TRUE;
    }
    
    if (!pCWebServerHandler)
    {
        return FALSE;
    }
    
    if (!lpIP)
    {
        return FALSE;
    }
    
    if (0 == wPort)
    {
        return FALSE;
    }
    
    if (0 == wMaxConnCount)
    {
        return FALSE;
    }
    
    if (0 == wWorkerCount)
    {
        return FALSE;
    }
    
    if (0 == dwRecvBufferSize)
    {
        return FALSE;
    }
    
    //Ĭ��30��ɨ��һ��
    if (0 == dwTimeout)
    {
        dwTimeout = CWEBSERVER_MIN_CHECK_TIME;
    }
    
    m_bStop = FALSE;
    
    WORD wIndex = 0;
    
    //////////////////////////////////////////////////////////////////////////
    //��ʼ������
    
    m_pCWebServerHandler = pCWebServerHandler;
    
    m_wMaxConnCount = wMaxConnCount;
    
    m_wWorkerCount = wWorkerCount;
    
    m_dwRecvBufferSize = dwRecvBufferSize;
    
    m_dwTimeout = dwTimeout;
    
    //////////////////////////////////////////////////////////////////////////
    //SOCKET������Դ
    
    if (!this->InitSocketInfoArray())
    {
        this->Stop();
        
        return FALSE;
    }

    //////////////////////////////////////////////////////////////////////////
    //IOCP�˿�
    
    m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    
    if (!m_hIOCP)
    {
         this->Stop();

        return FALSE;
    }
    
    //////////////////////////////////////////////////////////////////////////
    //�����˿�
    
    m_hListenSocket =  WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

    if (INVALID_SOCKET == m_hListenSocket)
    {
        this->Stop();
    
        return FALSE;
    }

    SOCKADDR_IN sockAddr = {0};

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.S_un.S_addr = inet_addr(lpIP);
    sockAddr.sin_port = htons(wPort);

    //����Ϊ������ģʽ
    // If iMode = 0, blocking is enabled; 
    // If iMode != 0, non-blocking mode is enabled.	
    int iMode = 1;
    if (SOCKET_ERROR == ioctlsocket(m_hListenSocket, FIONBIO, (u_long FAR*)&iMode))
    {
        this->Stop();

        return FALSE;
    }
    
    if (SOCKET_ERROR == bind(m_hListenSocket, (SOCKADDR* )&sockAddr, sizeof(SOCKADDR_IN)))
    {
        this->Stop();

        return FALSE;
    }

    if (SOCKET_ERROR == listen(m_hListenSocket, wMaxConnCount))
    {
        this->Stop();

        return FALSE;
    }

    //////////////////////////////////////////////////////////////////////////
    //����IO�����߳�]
    
    m_hIOWorkerThreadArray.resize(wWorkerCount);
    
    for (wIndex = 0; wIndex < m_wWorkerCount; ++wIndex)
    {
        m_hIOWorkerThreadArray[wIndex] = CreateThread(NULL, 0, CWebServer::ProcIO, this, 0, NULL);
        
        if (!m_hIOWorkerThreadArray[wIndex])
        {
            this->Stop();

            return FALSE;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //��������߳�
    
    m_hCheckThread = CreateThread(NULL, 0, CWebServer::ProcCheck, this, 0, NULL);
    
    if (!m_hCheckThread)
    {
        this->Stop();

        return FALSE;
    }
    
    //////////////////////////////////////////////////////////////////////////
    //���������߳�
    
    m_hAcceptThread = CreateThread(NULL, 0, CWebServer::ProcAccept, this, 0, NULL);

    if (!m_hCheckThread)
    {
        this->Stop();

        return FALSE;
    }
    
    return TRUE;
}

BOOL CWebServer::Start(
   CWebServerHandler* pCWebServerHandler,
   LPCSTR lpIP,
   WORD wPort,
   WORD wMaxConnCount,
   DWORD dwRecvBufferSize,
   DWORD dwTimeout)
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);

    WORD wWorkerCount = (WORD)(info.dwNumberOfProcessors + info.dwNumberOfProcessors) + 2;

    return this->Start(pCWebServerHandler, lpIP, wPort, wMaxConnCount, wWorkerCount, dwRecvBufferSize, dwTimeout);
}

void CWebServer::Stop()
{
    if (m_bStop)
    {
        return;
    }
    
    m_bStop = TRUE;
    
    //////////////////////////////////////////////////////////////////////////
    //�رս����߳�
    
    if (WaitForSingleObject(m_hAcceptThread, 3000) == WAIT_TIMEOUT)
    {
        TerminateThread(m_hAcceptThread, 0);
    }

    CloseHandle(m_hAcceptThread);
    m_hAcceptThread = NULL;
    
    //////////////////////////////////////////////////////////////////////////
    //�ر��������
    
    if (WaitForSingleObject(m_hCheckThread, 3000) == WAIT_TIMEOUT)
    {
        TerminateThread(m_hCheckThread, 0);
    }
    
    CloseHandle(m_hCheckThread);
    m_hCheckThread = NULL;
    
    //////////////////////////////////////////////////////////////////////////
    //�ر�IO�����߳�
    
    // ֪ͨ����I/O�����߳��˳�

    for(WORD wIndex = 0; wIndex < m_wWorkerCount; ++wIndex)
    {	
        PostQueuedCompletionStatus(m_hIOCP, CWEBSERVER_STOP_FLAG, 0, NULL);
    }

	if (m_hIOWorkerThreadArray.size() > 0)
	{
		WaitForMultipleObjects((DWORD)m_hIOWorkerThreadArray.size(), &m_hIOWorkerThreadArray.front(), TRUE, 3000);

		vector<HANDLE>::iterator entity = m_hIOWorkerThreadArray.begin();
		vector<HANDLE>::iterator endEntity = m_hIOWorkerThreadArray.end();       

		for(; entity != endEntity; ++entity)
		{
			TerminateThread(*entity, 0);

			CloseHandle(*entity);
		}

		m_hIOWorkerThreadArray.clear();
	}


    //////////////////////////////////////////////////////////////////////////
    //��Դ����
    
    CloseHandle(m_hIOCP);
    
    m_hIOCP = NULL;
    
    closesocket(m_hListenSocket);
    
    m_hListenSocket = INVALID_SOCKET;
    
    m_FreeSocketThreadMutex.UnLock();
    
    this->ReleaseSocketInfoArray();
}

void CWebServer::Send(PCHAR pData, DWORD dwSize, DWORD dwSocketID)
{
    if (!pData || 0 == dwSize)
    {
        return;
    }

    TSocketInfo* pTSocketInfo = NULL;
    DWORD dwHasSendSize = 0;
    DWORD dwCurSendSize = 0;

    WORD wSendIndex = 0;
    WORD wMaxSendCount = 5;

    if (this->GetSocketInfoWithLock(dwSocketID, &pTSocketInfo))
    {
        while ((dwSize - dwHasSendSize) > 0)
        {
            dwCurSendSize = send(pTSocketInfo->socket, pData + dwHasSendSize, dwSize - dwHasSendSize, 0);

            if (0 == dwCurSendSize)
            {
                this->ReleaseSocketInfoWithUnLock(dwSocketID);
            
                return;
            }

            if (SOCKET_ERROR == dwCurSendSize)
			{
				DWORD dwError = WSAGetLastError();

				if(dwError != WSAEWOULDBLOCK)
				{
					this->ReleaseSocketInfoWithUnLock(dwSocketID);

					return;
				}
				else
				{
                    //////////////////////////////////////////////////////////////////////////
                    //���Ӵ��������ʱ���޷��ɹ����������ݣ�����ΪSOCKET����

                    if (wSendIndex > wMaxSendCount)
                    {
                        this->ReleaseSocketInfoWithUnLock(dwSocketID);

                        return;
                    }
                    else
                    {
                        ++wSendIndex;
                    }

					continue;
				}
			}

			if (dwCurSendSize > 0)
			{
				dwHasSendSize += dwCurSendSize;
			}
        }
        
        pTSocketInfo->aCThreadMutex.UnLock();
    }
}

void CWebServer::CloseSocket(DWORD dwSocketID)
{
    this->ReleaseSocketInfo(dwSocketID);
}

void CWebServer::CloseSocketSend(DWORD dwSocketID)
{
	TSocketID* pTSocketID = (TSocketID*)&dwSocketID;

	DWORD dwIP = 0;

	if (pTSocketID->wIndex >= m_wMaxConnCount)
	{
		return;
	}

	TSocketInfo* pTSocketInfo = m_SocketInfoArray[pTSocketID->wIndex];

	CREATE_SMART_MUTEX(pTSocketInfo->aCThreadMutex);

	if (pTSocketInfo->wValue != pTSocketID->wValue)
	{
		return;
	}

	shutdown(pTSocketInfo->socket, SD_SEND);
}

WORD CWebServer::GetConnCount()
{
    CREATE_SMART_MUTEX(m_FreeSocketThreadMutex);

    return m_wMaxConnCount - (WORD)m_FreeSocketQueue.size();
}

BOOL CWebServer::InitSocketInfoArray()
{
    m_SocketInfoArray = new(std::nothrow) TSocketInfo*[m_wMaxConnCount];

    if (!m_SocketInfoArray)
    {
        return FALSE;
    }

    ZeroMemory(m_SocketInfoArray, sizeof(TSocketInfo*) * m_wMaxConnCount);

    WORD wIndex = 0;
    
    for (; wIndex < m_wMaxConnCount; ++wIndex)
    {
        m_SocketInfoArray[wIndex] = new(std::nothrow) TSocketInfo();
        
        if (!m_SocketInfoArray[wIndex])
        {
            return FALSE;
        }
        
        TSocketInfo* pTSocketInfo = m_SocketInfoArray[wIndex];
        
        pTSocketInfo->dwIP = 0;
        pTSocketInfo->dwRecvSize = 0;
        pTSocketInfo->wValue = 0;
        pTSocketInfo->bValid = FALSE;
        pTSocketInfo->socket = INVALID_SOCKET;
        
        pTSocketInfo->pRecvBuffer = new(std::nothrow) CHAR[m_dwRecvBufferSize + 1];
        
        if (!pTSocketInfo->pRecvBuffer)
        {
            return FALSE;
        }
        
        ZeroMemory(pTSocketInfo->pRecvBuffer, m_dwRecvBufferSize + 1);

        ZeroMemory(&pTSocketInfo->aOverlapped, sizeof(WSAOVERLAPPED));

        DWORD dwSocketID = 0;
        
        TSocketID* pTSocketID = (TSocketID*)&dwSocketID;
        
        pTSocketID->wIndex = wIndex;
        pTSocketID->wValue = 0;
        
        m_FreeSocketQueue.push(dwSocketID);
    }
    
    return TRUE;
}

void CWebServer::ReleaseSocketInfoArray()
{
    WORD wIndex = 0;

    if (m_SocketInfoArray)
    {
        for (; wIndex < m_wMaxConnCount; ++wIndex)
        {
            if (m_SocketInfoArray[wIndex])
            {
                if (m_SocketInfoArray[wIndex]->pRecvBuffer)
                {
                    delete [] m_SocketInfoArray[wIndex]->pRecvBuffer;

                    m_SocketInfoArray[wIndex]->pRecvBuffer = NULL;
                }
                
                delete m_SocketInfoArray[wIndex];
            }
        }

        delete [] m_SocketInfoArray;
    }

    while (!m_FreeSocketQueue.empty())
    {
        m_FreeSocketQueue.pop();
    }
    
}

BOOL CWebServer::GetNewSocketInfo(DWORD& dwSocketID, TSocketInfo** pTSocketInfo)
{
    CREATE_SMART_MUTEX(m_FreeSocketThreadMutex);
    
    if (m_FreeSocketQueue.empty())
    {
        return FALSE;
    }
    
    dwSocketID = m_FreeSocketQueue.front();
    
    m_FreeSocketQueue.pop();
    
    TSocketID* pTSocketID = (TSocketID*)&dwSocketID;
    
    *pTSocketInfo = m_SocketInfoArray[pTSocketID->wIndex];

    (*pTSocketInfo)->bValid = TRUE;

    return TRUE;
}

BOOL CWebServer::GetSocketInfo(DWORD dwSocketID, TSocketInfo** pTSocketInfo)
{
    TSocketID* pTSocketID = (TSocketID*)&dwSocketID;

    TSocketInfo* pTSocketInfoTemp = m_SocketInfoArray[pTSocketID->wIndex];

    CREATE_SMART_MUTEX(pTSocketInfoTemp->aCThreadMutex);

    if (pTSocketInfoTemp->wValue != pTSocketID->wValue)
    {
        return FALSE;
    }

    *pTSocketInfo = m_SocketInfoArray[pTSocketID->wIndex];

    return TRUE;
}

BOOL CWebServer::GetSocketInfoWithLock(DWORD dwSocketID, TSocketInfo** pTSocketInfo)
{
    TSocketID* pTSocketID = (TSocketID*)&dwSocketID;
    
	if (pTSocketID->wIndex >= m_wMaxConnCount)
	{
		return FALSE;
	}

    TSocketInfo* pTSocketInfoTemp = m_SocketInfoArray[pTSocketID->wIndex];

    pTSocketInfoTemp->aCThreadMutex.Lock();

    if (pTSocketInfoTemp->wValue != pTSocketID->wValue)
    {
        pTSocketInfoTemp->aCThreadMutex.UnLock();
    
        return FALSE;
    }

    *pTSocketInfo = m_SocketInfoArray[pTSocketID->wIndex];

    return TRUE;
}

void CWebServer::ReleaseSocketInfo(DWORD dwSocketID)
{
    TSocketID* pTSocketID = (TSocketID*)&dwSocketID;

    DWORD dwIP = 0;

    if (pTSocketID->wIndex >= m_wMaxConnCount)
    {
        return;
    }

    TSocketInfo* pTSocketInfo = m_SocketInfoArray[pTSocketID->wIndex];

    CREATE_SMART_MUTEX(pTSocketInfo->aCThreadMutex);

    if (pTSocketInfo->wValue != pTSocketID->wValue || !pTSocketInfo->bValid)
    {
        return;
    }

    dwIP = pTSocketInfo->dwIP;

    pTSocketInfo->wValue += 1;

    pTSocketInfo->bValid = FALSE;

    closesocket(pTSocketInfo->socket);

    pTSocketInfo->socket = INVALID_SOCKET;

    ZeroMemory(pTSocketInfo->pRecvBuffer, m_dwRecvBufferSize + 1);

    ZeroMemory(&pTSocketInfo->aOverlapped, sizeof(WSAOVERLAPPED));

    pTSocketInfo->dwRecvSize = 0;

    pTSocketInfo->dwIP = 0;

    //////////////////////////////////////////////////////////////////////////

    DWORD dwNewSocketID = 0;
    TSocketID* pNewTSocketID = (TSocketID*)&dwNewSocketID;
    
    pNewTSocketID->wIndex = pTSocketID->wIndex;
    pNewTSocketID->wValue = pTSocketInfo->wValue;
     
    m_FreeSocketThreadMutex.Lock();
    
    m_FreeSocketQueue.push(dwNewSocketID);
    
    m_FreeSocketThreadMutex.UnLock();

    //////////////////////////////////////////////////////////////////////////

    try
    {
        if (this->m_pCWebServerHandler)
        {
            this->m_pCWebServerHandler->OnClose(dwSocketID, dwIP);
        }
    }
    catch (...)
    {
    }
}

void CWebServer::ReleaseSocketInfoWithUnLock(DWORD dwSocketID)
{
    TSocketID* pTSocketID = (TSocketID*)&dwSocketID;

    DWORD dwIP = 0;

    if (pTSocketID->wIndex >= m_wMaxConnCount)
    {
        return;
    }

    TSocketInfo* pTSocketInfo = m_SocketInfoArray[pTSocketID->wIndex];

    if (pTSocketInfo->wValue != pTSocketID->wValue || !pTSocketInfo->bValid)
    {
        return;
    }

    dwIP = pTSocketInfo->dwIP;

    pTSocketInfo->wValue += 1;

    pTSocketInfo->bValid = FALSE;

    closesocket(pTSocketInfo->socket);

    pTSocketInfo->socket = INVALID_SOCKET;

    ZeroMemory(pTSocketInfo->pRecvBuffer, m_dwRecvBufferSize + 1);

    ZeroMemory(&pTSocketInfo->aOverlapped, sizeof(WSAOVERLAPPED));

    pTSocketInfo->dwRecvSize = 0;

    pTSocketInfo->dwIP = 0;
    
    pTSocketInfo->aCThreadMutex.UnLock();    

    //////////////////////////////////////////////////////////////////////////

    DWORD dwNewSocketID = 0;
    TSocketID* pNewTSocketID = (TSocketID*)&dwNewSocketID;

    pNewTSocketID->wIndex = pTSocketID->wIndex;
    pNewTSocketID->wValue = pTSocketInfo->wValue;

    m_FreeSocketThreadMutex.Lock();

    m_FreeSocketQueue.push(dwNewSocketID);

    m_FreeSocketThreadMutex.UnLock();

    //////////////////////////////////////////////////////////////////////////

    try
    {
        if (this->m_pCWebServerHandler)
        {
            this->m_pCWebServerHandler->OnClose(dwSocketID, dwIP);
        }
    }
    catch (...)
    {
    }
}

DWORD WINAPI CWebServer::ProcAccept(LPVOID pParam)
{
    CWebServer* pCWebServer = (CWebServer*)pParam;
    
    SOCKET acceptSocket = INVALID_SOCKET;
    
    SOCKADDR_IN removeAddr = {0};
    int nSockAddrLen = (int)sizeof(SOCKADDR_IN);
    
	fd_set fdset;

	int nfds = (int)pCWebServer->m_hListenSocket + 1;
	
	timeval aTimeval = {0};
    aTimeval.tv_sec = 1;

    while (!pCWebServer->m_bStop)
    {
		//////////////////////////////////////////////////////////////////////////
		//�ȴ�����

		FD_ZERO(&fdset);

		FD_SET(pCWebServer->m_hListenSocket, &fdset);

		int nRes = select(nfds, &fdset, NULL, NULL, &aTimeval);

		if (SOCKET_ERROR == nRes)	//����SOCKET�Ѿ�������
		{
			break;
		}
		else if (0 == nRes)	//�����ǳ�ʱ
		{
			continue;	
		}
		else
		{
			if (!FD_ISSET(pCWebServer->m_hListenSocket, &fdset))
			{
				continue;	//û�л�ȡ�������¼���
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//��ȡ����

		SOCKET acceptSocket = INVALID_SOCKET;

        acceptSocket = accept(pCWebServer->m_hListenSocket, (struct sockaddr*)&removeAddr, &nSockAddrLen);
        
        if (INVALID_SOCKET == acceptSocket)
        {
            if (WSAEWOULDBLOCK == WSAGetLastError())
            {
                Sleep(1);

                continue;
            }
            else
            {
                break;
            }
        }
        else
        {
            DWORD dwIP = removeAddr.sin_addr.S_un.S_addr;

            DWORD dwSocketID = 0;

            TSocketInfo* pTSocketInfo = NULL;

            if (pCWebServer->GetNewSocketInfo(dwSocketID, &pTSocketInfo))
            {
                pTSocketInfo->dwIP = dwIP;
                pTSocketInfo->socket = acceptSocket;

                try
                {   
                    if (pCWebServer->m_pCWebServerHandler)
                    {
                        pCWebServer->m_pCWebServerHandler->OnAccept(pCWebServer, dwSocketID, dwIP);
                    }
                }
                catch (...)
                {
                }

                //�ж��Ƿ񻹴��ڸ�SocketID������������������OnAccept�¼��ڹر��˸�SocketID
                if (pCWebServer->GetSocketInfoWithLock(dwSocketID, &pTSocketInfo))
                {
                    //�����IOCP�����ȴ�
                    if (NULL == CreateIoCompletionPort((HANDLE)acceptSocket, pCWebServer->m_hIOCP, (ULONG_PTR)dwSocketID, 0))
                    {
                        pCWebServer->ReleaseSocketInfoWithUnLock(dwSocketID);
                        
                        continue;
                    }
                    else
                    {
                        //////////////////////////////////////////////////////////////////////////
                        //Ͷ��һ���µ����ݽ���

                        WSABUF wsaBuf = {0};

                        wsaBuf.buf = pTSocketInfo->pRecvBuffer;

                        wsaBuf.len = pCWebServer->m_dwRecvBufferSize;

                        DWORD dwFlags = 0;
                        DWORD dwNumOfBytes = 0;

                        //�����LPWSAOVERLAPPED lpOverlapped �������ܲ���Ϊ0
                        if (SOCKET_ERROR == WSARecv(pTSocketInfo->socket, 
                            &wsaBuf, 
                            1, 
                            &dwNumOfBytes, 
                            &dwFlags,
                            &pTSocketInfo->aOverlapped, 
                            NULL))
                        {
                            if (WSAGetLastError() != WSA_IO_PENDING)
                            {
                                pCWebServer->ReleaseSocketInfoWithUnLock(dwSocketID);
                                
                                continue;
                            }
                        }
                    }
                    
                    pTSocketInfo->aCThreadMutex.UnLock();
                }
            }
            else
            {
                //��ȡ�������е��ڴ�֤���Ѿ��ﵽ���������

                closesocket(acceptSocket);

                acceptSocket = INVALID_SOCKET;
            }
        }
    }
    
    return 0;
}

DWORD WINAPI CWebServer::ProcCheck(LPVOID pParam)
{
    CWebServer* pCWebServer = (CWebServer*)pParam;
    
    DWORD dwCurTickCount = GetTickCount();
    DWORD dwTickCount = GetTickCount();
    
    while (!pCWebServer->m_bStop)
    {
        //////////////////////////////////////////////////////////////////////////
        //�ж��Ƿ񵽴���ʱ��
        
        dwCurTickCount = GetTickCount();

        //����Ƿ��г�ʱ��SocketID
        if (dwCurTickCount - dwTickCount > pCWebServer->m_dwTimeout)
        {
            WORD wIndex = 0;

            for (wIndex = 0; wIndex < pCWebServer->m_wMaxConnCount; ++wIndex)
            {
                DWORD dwSocketID = 0;

                TSocketInfo* pTSocketInfo = pCWebServer->m_SocketInfoArray[wIndex];

                pTSocketInfo->aCThreadMutex.Lock();

                if (INVALID_SOCKET != pTSocketInfo->socket)
                {
                    if (dwCurTickCount - pTSocketInfo->dwLastTickCount > pCWebServer->m_dwTimeout)
                    {
                        TSocketID* pTSocketID = (TSocketID*)&dwSocketID;

                        pTSocketID->wIndex = wIndex;
                        pTSocketID->wValue = pTSocketInfo->wValue;

                        pCWebServer->ReleaseSocketInfoWithUnLock(dwSocketID);

                        continue;
                    }
                }

                pTSocketInfo->aCThreadMutex.UnLock();
            }

            dwTickCount = GetTickCount();
        }
        else
        {
            Sleep(1000);
        }
        
    }

    return 0;
}

DWORD WINAPI CWebServer::ProcIO(LPVOID pParam)
{
    CWebServer* pCWebServer = (CWebServer*)pParam;
    
    DWORD dwNumberOfBytes = 0;

    ULONG_PTR uReserveInfo = 0;

    LPOVERLAPPED lpOverLapped = NULL;

    BOOL bSuccess = TRUE;

    DWORD dwError = 0;
    
    DWORD dwIP = 0;
    
    DWORD dwDataSize = 0;

	DWORD dwTemp = 0;

	BOOL bSocketClose = FALSE;
    
    PCHAR recvBuffer = new(std::nothrow) CHAR[pCWebServer->m_dwRecvBufferSize + 1];
    
    if (!recvBuffer)
    {
        return 1;
    }
    
    ZeroMemory(recvBuffer, pCWebServer->m_dwRecvBufferSize + 1);

    while (!pCWebServer->m_bStop)
    {
        //////////////////////////////////////////////////////////////////////////
        //IOCP ����IO���

        bSuccess = GetQueuedCompletionStatus(pCWebServer->m_hIOCP,
            &dwNumberOfBytes, &uReserveInfo, &lpOverLapped, 1000);

        //�û�֪ͨ�˳�
        if (CWEBSERVER_STOP_FLAG == dwNumberOfBytes)
        {
            return 0;
        }

        if (!bSuccess)
        {
            dwError = GetLastError();

            if (WAIT_TIMEOUT == dwError)
            {
                continue;
            }
        }
        else
        {
            dwError = 0;
        }

        //////////////////////////////////////////////////////////////////////////
        //��ȡ����
        
        DWORD dwSocketID = (DWORD)uReserveInfo;
        
		bSocketClose = FALSE;

        if (!bSuccess)
        {
			pCWebServer->ReleaseSocketInfo(dwSocketID);

			continue;
		}

        if (0 == dwNumberOfBytes)
        {
            pCWebServer->ReleaseSocketInfo(dwSocketID);

			continue;
        }

		//////////////////////////////////////////////////////////////////////////

        TSocketInfo* pTSocketInfo = NULL;
    
        if (pCWebServer->GetSocketInfoWithLock(dwSocketID, &pTSocketInfo))
        {
            dwIP = pTSocketInfo->dwIP;
        
            pTSocketInfo->dwRecvSize += dwNumberOfBytes;
            
			while (pTSocketInfo->dwRecvSize >= 4)
			{
				//////////////////////////////////////////////////////////////////////////
				//�ж��Ƿ���ȡ������

				PCHAR pEnd = strstr(pTSocketInfo->pRecvBuffer, "\r\n\r\n");

				if (pEnd)
				{
					dwDataSize = (DWORD)(pEnd - pTSocketInfo->pRecvBuffer) + 4;	//������Ч��С

					memcpy(recvBuffer, pTSocketInfo->pRecvBuffer, dwDataSize);

					dwTemp = pTSocketInfo->dwRecvSize - dwDataSize;	//ʣ��Ĵ�С

					memmove(pTSocketInfo->pRecvBuffer, pTSocketInfo->pRecvBuffer + dwDataSize, dwTemp);

					memset(pTSocketInfo->pRecvBuffer + dwTemp, 0, pCWebServer->m_dwRecvBufferSize - dwTemp);

					pTSocketInfo->dwRecvSize = dwTemp;

                    pTSocketInfo->dwLastTickCount = GetTickCount();

					pTSocketInfo->aCThreadMutex.UnLock();

					try
					{
						if (pCWebServer->m_pCWebServerHandler)
						{
							pCWebServer->m_pCWebServerHandler->OnRecv(pCWebServer, recvBuffer, dwDataSize, dwIP, dwSocketID);
						}
					}
					catch (...)
					{
					}

					if (!pCWebServer->GetSocketInfoWithLock(dwSocketID, &pTSocketInfo))
					{
						bSocketClose = TRUE;
						break;
					}
				}
				else
				{
					break;
				}
			}

			//////////////////////////////////////////////////////////////////////////
			//����Ͷ�ݽ�����������

			if (bSocketClose)
			{
				continue;
			}

			if (pTSocketInfo->dwRecvSize >= pCWebServer->m_dwRecvBufferSize)
			{
				//���յ��������Ѿ����������ջ�������û�еõ��������ݣ���Ϊ��Ч

				pCWebServer->ReleaseSocketInfoWithUnLock(dwSocketID);

				continue;
			}

			//////////////////////////////////////////////////////////////////////////
			//��û�н��յ����������ݣ�����Ͷ��һ���µ����ݽ���

			WSABUF wsaBuf = {0};

			wsaBuf.buf = pTSocketInfo->pRecvBuffer + pTSocketInfo->dwRecvSize;

			wsaBuf.len = pCWebServer->m_dwRecvBufferSize - pTSocketInfo->dwRecvSize;

			DWORD dwFlags = 0;
			DWORD dwNumOfBytes = 0;

			//�����LPWSAOVERLAPPED lpOverlapped �������ܲ���Ϊ0
			if (SOCKET_ERROR == WSARecv(pTSocketInfo->socket, 
				&wsaBuf, 
				1, 
				&dwNumOfBytes, 
				&dwFlags, 
				&pTSocketInfo->aOverlapped, 
				NULL))
			{
				if (WSAGetLastError() != WSA_IO_PENDING)
				{
					pCWebServer->ReleaseSocketInfoWithUnLock(dwSocketID);

					continue;
				}
			}

			//////////////////////////////////////////////////////////////////////////

			pTSocketInfo->aCThreadMutex.UnLock();
        }
	}

    return 0;
}
