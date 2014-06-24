#ifndef __NETSERVICES_H__
#define __NETSERVICES_H__

#include "../config.h"
#include "../tool/LogFile.h"

GV_BEGIN

class CStreamServices;
/*------------------------------------
*     define error code
*
--------------------------------------*/
enum ERROR_NETSERVICES
{
	ERROR_NETSRV_SUCC = 0, //succ
	ERROR_NETSRV_PARAMS,   //param error
	ERROR_NETSRV_SYSTEM,   //system error:close socket
	ERROR_NETSRV_SRV_CLOSE, //srv close the socket:close socket
	ERROR_NETSRV_REMOTE_CLOSE, //remote close the socket:close socket
	ERROR_NETSRV_BUFFER_FULL,  //socket buffer is full
	ERROR_NETSRV_SOCKET_NONE,   //can't find the socket:,close socket
	ERROR_NETSRV_NOTINIT,    //netservices not init
	ERROR_NETSRV_NEWMEMORY,   //new memory faild:close socket
	ERROR_NETSRV_ERRORPACK,    //recv error pack:close socket
	ERROR_NETSRV_EXCEPTION,     //system exception
	ERROR_NETSRV_SENDDATA,       //send data error:close socket
	ERROR_NETSRV_SOCKETSTATUS,     //socket status error:not close socket
	ERROR_NETSRV_CONNECTSRV,      //connect srv faild
	ERROR_NETSRV_SOCKETTIMEOUT,           //socket timeout
	ERROR_NETSRV_SRVSTOP,                   //system stop
	ERROR_NETSRV_CLOSESOCKET_CALLER,         //caller close socket
	ERROR_NETSRV_BINDPORT         //bind port error ----add by gavin on 2012.7.17
};

/********************************
*   define protocol type
*  add by gavin on 2012.9.25
********************************/
enum PROTOCOL_TYPE
{
	PROTOCOL_TYPE_CUSTOM=0, //custom
	PROTOCOL_TYPE_HTTP,     //http
	PROTOCOL_TYPE_FTP      //ftp
};

/*---------------------------------------
*	     the callback interface
*   warning:
*        all the interface,can't direct call
*   the CNetServices's interface,maybe occour deadlock
*
------------------------------------------*/
class BASECLASS_API CCallback_NetServices
{
public:
	/*------------------------------
	*   notify recv data
	*  return value,0(succ),else :errorcode
	------------------------------*/
	virtual Int NotifyRecvData(Void* pData,Long nDataLen,ULong ulSourceIP,Int nSourcePort,Long  nSocketID) = 0;
	/*------------------------------
	*   notify new client connected
	*  return value,0(succ),else :errorcode
	------------------------------*/
	virtual Int NotifyNewClientConnected(ULong   ulRemoteIP,Int  nRemotePort,Long nSocketID)=0;
	/*------------------------------
	*   notify client closed
	*  return value,0(succ),else :errorcode
	------------------------------*/
	virtual Int NotifyClientClosed(ULong   ulRemoteIP,Int  nRemotePort,Long nSocketID,ERROR_NETSERVICES   errorCode)=0;
	/*------------------------------
	*   notify the status of connected remote srv
	* return value,0(succ),else :errorcode
	------------------------------*/
	virtual Int NotifyConnectRemoteSrv(ULong   ulRemoteIP,Int  nRemotePort,Long nSocketID,ERROR_NETSERVICES   errorCode)=0;
};



class BASECLASS_API CNetServices
{
public:
	CNetServices(Void);
	~CNetServices(Void);
	//start net services(return value---0: succ,>0:error code)
	Int NetServices_Start(ULong           ulServiceIP,//srv ip
	                        Int             nListenPort,//listen Port(0:not listen any port)
	                        Long            lMaxCount_ConnectClient,//max connect client
	                        ULong           ulMaxDataLen_OnePack_Recv,//max len of one recv pack
	                        ULong           ulMaxDataLen_OnePack_Send,//max len of one sed pack 
	                        Int             nMaxCount_RecvPack,//max sav recv pack count
	                        Int             nMaxCount_SendPack,//max save send pack count 
							Short           nThreadCount_HandleMsg,//thread count(default value:cpu num) ,hand msg
							ULong           ulTimeout_Socket,//socket timeout value (second)
							CCallback_NetServices*   pCallback);//the callback ptr (user define)
	//stop net services
	Void NetServices_Stop();
	//send data(return value---0:succ,>0:error code)
	Int SendData(Long lSocketID,Void*  pData,Long  lDataLen);
	//close one socket
	Void CloseSocket(Long   lSocketID);
	//get connected socket count 
	Long  GetSocketCount_Connected();
	//connect remote srv(return value---0:faild,>0 socket id;ulWaitTime:wait time millsecond)
	Long  Connect_RemoteSrv(Char*    pszRemoteIP,Int    nRemotePort,ULong   ulWaitTime = 500);
	//set pack msghead info
	Void  SetPackageHeadInfo(Int  nOffset=0,Int   nLen=4,Int nMsgheadLen=8);
	//get memory size (k)
	ULong  GetSize_Memory();
	//set log level 
	Void SetLogLevel(LEVEL_LOG     logLevel=LEVEL_LOG_WARNING);//add by gavin on 2012.3.11
	//set protocol type
	Void SetProtocolType(PROTOCOL_TYPE      protocoltype);//add by gavin on 2012.9.25
private:
	CStreamServices*         m_pStreamService;
};


GV_END

#endif
