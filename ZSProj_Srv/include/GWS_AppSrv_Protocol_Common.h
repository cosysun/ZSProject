#ifndef GWS_APPSRV_PROTOCOL_COMMON_H
#define GWS_APPSRV_PROTOCOL_COMMON_H

#include <Windows.h>

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//结果码

enum GWS_APPSRV_RES_CODE
{
	GWS_APPSRV_RES_CODE_SUCCESS = 0,		//成功
	GWS_APPSRV_RES_CODE_LOGIN_FAILED,		//登录失败
};

//////////////////////////////////////////////////////////////////////////
//命令码

enum GWS_APPSRV_CMD
{
	GWS_APPSRV_CMD_LOGIN = 24373,			//登录
	GWS_APPSRV_CMD_LOGOUT,					//登出
	GWS_APPSRV_CMD_KEEPALIVE,				//心跳
	GWS_APPSRV_CMD_FUNC_REQ,				//功能请求
    GWS_APPSRV_CMD_SYSTEM,                  //系统功能
};

enum GWS_PROTOCOL_TYPE
{
	GWS_PROTOCOL_BINARY = 0,
	GWS_PROTOCOL_HTTP,
};

//////////////////////////////////////////////////////////////////////////
//tagGWSAppSrvSystemReq 中的ParamFlag参数值：

//APPSRV 业务服务提供功能
#define GWS_APPSRV_REQ_PARAM_FLAG_NO_SERVICE    1   //不再提供服务[请求不传入该APPSRV]（已经处理请求的可返回到前端）
#define GWS_APPSRV_REQ_PARAM_FLAG_SERVICE       2   //恢复提供业务服务(登录成功后默认提供业务服务)[同时出现 NO_SERVICE 与 SERVICE 标记时，则认为 SERVICE 有效，NO_SERVICE 无效]

//////////////////////////////////////////////////////////////////////////
//协议头

typedef struct tagGWSAppSrvHead
{
    unsigned int nSize;	        //数据总大小
    unsigned short sCmd;        //命令
    unsigned short sResv;       //保留
}GWS_APPSRV_HEAD, *PGWS_APPSRV_HEAD;

//////////////////////////////////////////////////////////////////////////
//登录

//说明：登录请求
//命令：GWS_APPSRV_CMD_LOGIN
//协议：GWS_APPSRV_HEAD + GWS_APPSRV_LOGIN_REQ 
typedef struct tagGWSAppSrvLoginReq
{
    unsigned short sSize;       //本结构体大小
    unsigned short sAppTypeID;  //业务类型编号
    char szAuthKey[50];	        //验证信息(无 \0 结束符号)
    unsigned char btNoServices; //0：正常提供业务服务；>0：只连接，不提供业务服务
    unsigned char btResv;       // 预留
	unsigned short sProjectID;	//项目ID
	unsigned short sRecv;	// 保留
    int nResv[4];		        //保留
}GWS_APPSRV_LOGIN_REQ, *PGWS_APPSRV_LOGIN_REQ;

//说明：登录结果
//命令：GWS_APPSRV_CMD_LOGIN
//协议：GWS_APPSRV_HEAD + GWS_APPSRV_LOGIN_RES 
typedef struct tagGWSAppSrvLoginRes
{
    unsigned short sSize;       //本结构体大小
    unsigned short sResCode;    //结果码
    int nResv[5];		        //保留
}GWS_APPSRV_LOGIN_RES, *PGWS_APPSRV_LOGIN_RES;

//////////////////////////////////////////////////////////////////////////
//登出

//说明：登出请求
//命令：GWS_APPSRV_CMD_LOGOUT
//协议：GWS_APPSRV_HEAD + GWS_APPSRV_LOGOUT_REQ 
typedef struct tagGWSAppSrvLogoutReq
{
    unsigned short sSize;				//本结构体大小
    unsigned short sResv;				//保留
    int nResv[5];		                //保留
}GWS_APPSRV_LOGOUT_REQ, *PGWS_APPSRV_LOGOUT_REQ;

//////////////////////////////////////////////////////////////////////////
//心跳

//说明：心跳请求
//命令：GWS_APPSRV_CMD_KEEPALIVE
//协议：GWS_APPSRV_HEAD + GWS_APPSRV_KEEPALIVE_REQ 
typedef struct tagGWSAppSrvKeepAliveReq
{
    unsigned short sSize;				//本结构体大小
    unsigned short sResv;				//保留
    int nResv[5];		                //保留
}GWS_APPSRV_KEEPALIVE_REQ, *PGWS_APPSRV_KEEPALIVE_REQ;

//////////////////////////////////////////////////////////////////////////
//功能请求

//说明：功能请求
//命令：GWS_APPSRV_CMD_FUNC_REQ
//协议：GWS_APPSRV_HEAD + GWS_APPSRV_FUNC_REQ + 携带数据 + 业务数据 
typedef struct tagGWSAppSrvFuncReq
{
    unsigned short sSize;				//本结构体大小
    unsigned short sTakeSize;			//携带数据
    unsigned int nDataSize;		        //业务数据大小
    unsigned int nIP;                   //请求的客户端IP
    char systemData[64];                //系统内部数据（返回时带回）
    int nResv[5];		                //保留
}GWS_APPSRV_FUNC_REQ, *PGWS_APPSRV_FUNC_REQ;


//说明：功能结果
//命令：GWS_APPSRV_CMD_FUNC_REQ
//协议：GWS_APPSRV_HEAD + GWS_APPSRV_FUNC_RES + 携带数据 + 业务数据
typedef struct tagGWSAppSrvFuncRes
{
    unsigned short sSize;				//本结构体大小
    unsigned short sResCode;			//结果码
    unsigned short sTakeSize;			//携带数据                       
    unsigned int nDataSize;		        //业务数据大小
    char systemData[64];                //系统内部数据（返回时带回）
    int nResv[5];		                //保留
}GWS_APPSRV_FUNC_RES, *PGWS_APPSRV_FUNC_RES;

//////////////////////////////////////////////////////////////////////////
//GWS系统内部功能

//说明：系统功能请求
//命令：GWS_APPSRV_CMD_SYSTEM
//协议：GWS_APPSRV_HEAD + GWS_APPSRV_SYSTEM_REQ + 携带数据
typedef struct tagGWSAppSrvSystemReq
{
    unsigned short sSize;				//本结构体大小
    unsigned short sResv;				//保留
    unsigned short sTakeSize;			//携带数据
    unsigned short sResv2;				//保留
    unsigned int nParamFlag;            //参数标志
                                        //1:只连接，但不提供业务服务（已经处理的业务可返回前端）
                                        //2:恢复提供业务服务
    int nResv[5];		                //保留
}GWS_APPSRV_SYSTEM_REQ, *PGWS_APPSRV_SYSTEM_REQ;

//说明：系统功能请求
//命令：GWS_APPSRV_CMD_SYSTEM
//协议：GWS_APPSRV_HEAD + GWS_APPSRV_SYSTEM_RES + 携带数据
typedef struct tagGWSAppSrvSystemRes
{
    unsigned short sSize;				//本结构体大小
    unsigned short sResv;				//保留
    unsigned short sTakeSize;			//携带数据
    unsigned short sResv2;				//保留
    unsigned int nParamFlag;            //参数标志（REQ 的 dwParamFlag 值一致）
    int nResv[5];		                //保留
}GWS_APPSRV_SYSTEM_RES, *PGWS_APPSRV_SYSTEM_RES;

#pragma pack()

#endif

