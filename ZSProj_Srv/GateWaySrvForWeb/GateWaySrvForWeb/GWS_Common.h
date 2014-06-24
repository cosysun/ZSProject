#ifndef GWS_COMMON_H
#define GWS_COMMON_H

//////////////////////////////////////////////////////////////////////////
//忽略的编译警告

#pragma warning(disable:4275)
#pragma warning(disable:4018)
#pragma warning(disable:4200)
#pragma warning(disable:4244)
#pragma warning(disable:4267)

//////////////////////////////////////////////////////////////////////////
//宏定义

#define APP_VERSION_NUM 1010102

#define GWS_TEMP_VALUE_SIZE 1024

#define SOCKET_IP_MAX_SIZE 50
#define SOCKET_IP_MAX_LENGTH 49

#define STOP_WAIT_MILLISECOND 3000

#define MD5_STRING_SIZE 33
#define MD5_STRING_LENGTH 32

#define GWS_DB_STR_MAX_SIZE 52
#define GWS_DB_STR_MAX_LENGTH 51

#define CLIENT_MIN_RECV_BUFFER_SIZE 2 * 1024
#define CLIENT_MIN_SEND_BUFFER_SIZE 5 * 1024

#define APPSRV_MIN_RECV_BUFFER_SIZE 5 * 1024
#define APPSRV_MIN_SEND_BUFFER_SIZE 5 * 1024

#define GWS_APP_TYPE_SNMP_COUNT 200

#define GWS_OMAS_TYPE_DEFAULT	1
#define GWS_OMAS_TYPE_CLIENT	2
#define GWS_OMAS_TYPE_BL		3

//////////////////////////////////////////////////////////////////////////
//系统库

#ifdef WIN32

#include <Winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

#include <windows.h>
#include <shlobj.h>
#pragma comment(lib, "shell32.lib")

#endif

#include <map>
#include <vector>
#include <list>
#include <string>
#include <new>

using namespace std;

//////////////////////////////////////////////////////////////////////////
//数据类型兼容

#include "../../Common/Utility/BaseTypeDef.h"

//////////////////////////////////////////////////////////////////////////
//BaseClass

#define _USELIB
#define BASECLASS_EXPORTS

#ifdef _DEBUG

#pragma comment(lib, "../../Common/BaseClass_V3/lib/debug/util_d.lib")
#pragma comment(lib, "../../Common/BaseClass_V3/lib/debug/BaseClass_d.lib")

#else

#pragma comment(lib, "../../Common/BaseClass_V3/lib/release/util_x32.lib")
#pragma comment(lib, "../../Common/BaseClass_V3/lib/release/Baseclass_x32.lib")

#endif

#include "../../Common/BaseClass_V3/include/util/gvapp.h"
#include "../../Common/BaseClass_V3/include/net/KKSock.h"
#include "../../Common/BaseClass_V3/include/net/NetServices.h"
#include "../../Common/BaseClass_V3/include/tool/LogFile.h"
#include "../../Common/BaseClass_V3/include/tool/packagequeue.h"
#include "../../Common/BaseClass_V3/include/tool/AutoLock.h"
#include "../../Common/BaseClass_V3/include/util/adoDatabase.h"

GV_USING

//////////////////////////////////////////////////////////////////////////
//Utility

#include "../../Common/Utility/Base64.h"
#include "../../Common/Utility/MD5.h"
#include "../../Common/Utility/tinyxml.h"
#include "../../Common/Utility/IniFile.h"
#include "../../Common/Utility/mdump.h"
#include "../../Common/Utility/Singleton.h"
#include "../../Common/Utility/ThreadSync.h"
#include "../../Common/Utility/Thread.h"
#include "../../Common/Utility/Utility.h"
#include "../../Common/Utility/WebServer.h"
#include "../../Common/Utility/tinyxml.h"

//////////////////////////////////////////////////////////////////////////
//协议

#include "../../Include/GWS_AppSrv_Protocol_Common.h"
#include "../../Include/GWS_WEB_Protocol_Common.h"

//////////////////////////////////////////////////////////////////////////
//Json
#include "../../Common/Json/include/json.h"

#ifdef _DEBUG
#pragma comment(lib, "../../Common/Json/bin/json_vc71_libmtd.lib")
#else
#pragma comment(lib, "../../Common/Json/bin/json_vc71_libmt.lib")
#endif

//////////////////////////////////////////////////////////////////////////
//数据结构

//连接信息结构
struct TSocketInfo
{
	DWORD dwIP;
	LONG lSocketID;
	DWORD dwDataSize;
    DWORD dwOp;
};

#pragma pack(1)

struct TGWSSystemData
{
    long lSocketID;                     //客户端SocketID
    unsigned char btSyncCall;           //是否同步调用
    unsigned char btResv2[3];           //保留
    long lSyncID;                       //同步编号
    unsigned int nCallTickCount;        //请求时间
    unsigned int nCallTimeout;          //请求超时
};

#pragma pack()

#endif