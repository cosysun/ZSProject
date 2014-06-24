#ifndef GWS_APPSRV_PROTOCOL_COMMON_H
#define GWS_APPSRV_PROTOCOL_COMMON_H

#include <Windows.h>

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//�����

enum GWS_APPSRV_RES_CODE
{
	GWS_APPSRV_RES_CODE_SUCCESS = 0,		//�ɹ�
	GWS_APPSRV_RES_CODE_LOGIN_FAILED,		//��¼ʧ��
};

//////////////////////////////////////////////////////////////////////////
//������

enum GWS_APPSRV_CMD
{
	GWS_APPSRV_CMD_LOGIN = 24373,			//��¼
	GWS_APPSRV_CMD_LOGOUT,					//�ǳ�
	GWS_APPSRV_CMD_KEEPALIVE,				//����
	GWS_APPSRV_CMD_FUNC_REQ,				//��������
    GWS_APPSRV_CMD_SYSTEM,                  //ϵͳ����
};

enum GWS_PROTOCOL_TYPE
{
	GWS_PROTOCOL_BINARY = 0,
	GWS_PROTOCOL_HTTP,
};

//////////////////////////////////////////////////////////////////////////
//tagGWSAppSrvSystemReq �е�ParamFlag����ֵ��

//APPSRV ҵ������ṩ����
#define GWS_APPSRV_REQ_PARAM_FLAG_NO_SERVICE    1   //�����ṩ����[���󲻴����APPSRV]���Ѿ���������Ŀɷ��ص�ǰ�ˣ�
#define GWS_APPSRV_REQ_PARAM_FLAG_SERVICE       2   //�ָ��ṩҵ�����(��¼�ɹ���Ĭ���ṩҵ�����)[ͬʱ���� NO_SERVICE �� SERVICE ���ʱ������Ϊ SERVICE ��Ч��NO_SERVICE ��Ч]

//////////////////////////////////////////////////////////////////////////
//Э��ͷ

typedef struct tagGWSAppSrvHead
{
    unsigned int nSize;	        //�����ܴ�С
    unsigned short sCmd;        //����
    unsigned short sResv;       //����
}GWS_APPSRV_HEAD, *PGWS_APPSRV_HEAD;

//////////////////////////////////////////////////////////////////////////
//��¼

//˵������¼����
//���GWS_APPSRV_CMD_LOGIN
//Э�飺GWS_APPSRV_HEAD + GWS_APPSRV_LOGIN_REQ 
typedef struct tagGWSAppSrvLoginReq
{
    unsigned short sSize;       //���ṹ���С
    unsigned short sAppTypeID;  //ҵ�����ͱ��
    char szAuthKey[50];	        //��֤��Ϣ(�� \0 ��������)
    unsigned char btNoServices; //0�������ṩҵ�����>0��ֻ���ӣ����ṩҵ�����
    unsigned char btResv;       // Ԥ��
	unsigned short sProjectID;	//��ĿID
	unsigned short sRecv;	// ����
    int nResv[4];		        //����
}GWS_APPSRV_LOGIN_REQ, *PGWS_APPSRV_LOGIN_REQ;

//˵������¼���
//���GWS_APPSRV_CMD_LOGIN
//Э�飺GWS_APPSRV_HEAD + GWS_APPSRV_LOGIN_RES 
typedef struct tagGWSAppSrvLoginRes
{
    unsigned short sSize;       //���ṹ���С
    unsigned short sResCode;    //�����
    int nResv[5];		        //����
}GWS_APPSRV_LOGIN_RES, *PGWS_APPSRV_LOGIN_RES;

//////////////////////////////////////////////////////////////////////////
//�ǳ�

//˵�����ǳ�����
//���GWS_APPSRV_CMD_LOGOUT
//Э�飺GWS_APPSRV_HEAD + GWS_APPSRV_LOGOUT_REQ 
typedef struct tagGWSAppSrvLogoutReq
{
    unsigned short sSize;				//���ṹ���С
    unsigned short sResv;				//����
    int nResv[5];		                //����
}GWS_APPSRV_LOGOUT_REQ, *PGWS_APPSRV_LOGOUT_REQ;

//////////////////////////////////////////////////////////////////////////
//����

//˵������������
//���GWS_APPSRV_CMD_KEEPALIVE
//Э�飺GWS_APPSRV_HEAD + GWS_APPSRV_KEEPALIVE_REQ 
typedef struct tagGWSAppSrvKeepAliveReq
{
    unsigned short sSize;				//���ṹ���С
    unsigned short sResv;				//����
    int nResv[5];		                //����
}GWS_APPSRV_KEEPALIVE_REQ, *PGWS_APPSRV_KEEPALIVE_REQ;

//////////////////////////////////////////////////////////////////////////
//��������

//˵������������
//���GWS_APPSRV_CMD_FUNC_REQ
//Э�飺GWS_APPSRV_HEAD + GWS_APPSRV_FUNC_REQ + Я������ + ҵ������ 
typedef struct tagGWSAppSrvFuncReq
{
    unsigned short sSize;				//���ṹ���С
    unsigned short sTakeSize;			//Я������
    unsigned int nDataSize;		        //ҵ�����ݴ�С
    unsigned int nIP;                   //����Ŀͻ���IP
    char systemData[64];                //ϵͳ�ڲ����ݣ�����ʱ���أ�
    int nResv[5];		                //����
}GWS_APPSRV_FUNC_REQ, *PGWS_APPSRV_FUNC_REQ;


//˵�������ܽ��
//���GWS_APPSRV_CMD_FUNC_REQ
//Э�飺GWS_APPSRV_HEAD + GWS_APPSRV_FUNC_RES + Я������ + ҵ������
typedef struct tagGWSAppSrvFuncRes
{
    unsigned short sSize;				//���ṹ���С
    unsigned short sResCode;			//�����
    unsigned short sTakeSize;			//Я������                       
    unsigned int nDataSize;		        //ҵ�����ݴ�С
    char systemData[64];                //ϵͳ�ڲ����ݣ�����ʱ���أ�
    int nResv[5];		                //����
}GWS_APPSRV_FUNC_RES, *PGWS_APPSRV_FUNC_RES;

//////////////////////////////////////////////////////////////////////////
//GWSϵͳ�ڲ�����

//˵����ϵͳ��������
//���GWS_APPSRV_CMD_SYSTEM
//Э�飺GWS_APPSRV_HEAD + GWS_APPSRV_SYSTEM_REQ + Я������
typedef struct tagGWSAppSrvSystemReq
{
    unsigned short sSize;				//���ṹ���С
    unsigned short sResv;				//����
    unsigned short sTakeSize;			//Я������
    unsigned short sResv2;				//����
    unsigned int nParamFlag;            //������־
                                        //1:ֻ���ӣ������ṩҵ������Ѿ������ҵ��ɷ���ǰ�ˣ�
                                        //2:�ָ��ṩҵ�����
    int nResv[5];		                //����
}GWS_APPSRV_SYSTEM_REQ, *PGWS_APPSRV_SYSTEM_REQ;

//˵����ϵͳ��������
//���GWS_APPSRV_CMD_SYSTEM
//Э�飺GWS_APPSRV_HEAD + GWS_APPSRV_SYSTEM_RES + Я������
typedef struct tagGWSAppSrvSystemRes
{
    unsigned short sSize;				//���ṹ���С
    unsigned short sResv;				//����
    unsigned short sTakeSize;			//Я������
    unsigned short sResv2;				//����
    unsigned int nParamFlag;            //������־��REQ �� dwParamFlag ֵһ�£�
    int nResv[5];		                //����
}GWS_APPSRV_SYSTEM_RES, *PGWS_APPSRV_SYSTEM_RES;

#pragma pack()

#endif

