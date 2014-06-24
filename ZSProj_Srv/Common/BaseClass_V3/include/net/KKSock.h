
#ifndef __KKSOCKET_H__
#define __KKSOCKET_H__

//
#include    "../config.h"


GV_BEGIN


/*--------------------------------------
*    error code
-----------------------------------------*/
#define ERROR_SOCKET_SUCC                       0 //su
#define ERROR_SOCKET_NET                       -1 //network error
#define ERROR_SOCKET_INVALID_PARAMS            -2 //param error


/*--------------------------------------
*       define const value
-----------------------------------------*/
#define  CAN_READ	     101
#define  CAN_WRITE       102
#define  CAN_CONNECT     103
#define  CAN_ACCEPT      104

/*--------------------------------------
*    error code
-----------------------------------------*/
class BASECLASS_API CKKSock  
{
public:
	CKKSock();
	virtual ~CKKSock();
	//get socket name
	Void GetSockName(ULong& ulIP,UShort& usPort);
	//set socket buffer
	Void SetSockBuff(Long lSockRecvBufferLen = 5*1024*1024,Long lSockSendBufferLen=5*1024*1024);
	//attach socket
	Void Attach(SOCKET sock);
	//set block
	Void SetBlock(Bool block);
	//connect
	Int Connect(Char* pszHostAddress, UShort usHostPort, Bool bBlock);
	//check socket is valid
	Bool IsInvaild();
	//recv data
	Int Receive(Void* lpBuf, Int nBufLen, Int nFlags);
	//send data
	Int Send(CVoid* lpBuf, Int nBufLen, Int nFlags);
	//accept
	Bool Accept(SOCKET &socket, SOCKADDR *lpSockAddr, socklen_t *lpSockAddrLen);
	//accept
	Bool Accept(CKKSock &rConnectedSocket, SOCKADDR* lpSockAddr, socklen_t* lpSockAddrLen);
	//listen
	Bool Listen(Int nConnectionBacklog = 5);
	//wait
	Int Wait(Long millis, Int flag);	
	//close
	Void Close(Int   nHow=0);
	//recv data
	Int ReceiveFrom(Void* lpBuf, Int nBufLen, ULong&   ulIP, UShort& usPort, Int nFlags);
	//send data
	Int SendTo(CVoid *lpBuf, Int nBufLen, ULong   ulIP, UShort  usPort, Int nFlags);
	//send data
	Int SendTo(CVoid* lpBuf, Int nBufLen, Char*  pszIP, UShort  usPort, Int nFlags);
	//bind
	Bool Bind(UShort  usPort, Char*  pszIP = Null);
	//create socket
	Bool Create(Int af, Int type, Int protocol);
	//init socket lib
	static Bool InitSock();
	//release socket lib
	static ULong CleanSock();
	//connect using proxy .0(succ) ,!=0(error code)
	Int  Connect_UserProxy_SOCK5(Char* pszProxySrvIP, UShort usProxySrvPort,Char*  pszHostAddress, UShort usHostPort,Char* pszProxyUser,Char* pszProxyPwd, Bool bBlock);
	//
	Int  SetSockOpt(Int  nLevel,Int nOptName,CChar* optvalue,Int nOptlen);
	//get last error code
	Int  GetLastError();
protected:
	SOCKET                 m_sock;//socket
};

GV_END


#endif 
