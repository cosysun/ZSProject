#ifndef GWS_WEB_PROTOCOL_COMMON_H
#define GWS_WEB_PROTOCOL_COMMON_H

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//HTTP 协议相关

#define GWS_HTTP_RETURN_FORMAT "HTTP/1.1 200 OK \r\nServer: GWS/1.0 \r\nDate: Thu,08 Mar 2099 00:00:00 GMT \r\nExpries:0 \r\nContent-Type:text/html; charset=UTF-8 \r\nContent-Length:%lu \r\n\r\n"

#define GWS_HTTP_RETURN_EMPTY "HTTP/1.1 200 OK \r\nServer: GWS/1.0 \r\nDate: Thu,08 Mar 2099 00:00:00 GMT \r\nExpries:0 \r\nContent-Type:text/html; charset=UTF-8 \r\nContent-Length:7 \r\n\r\nsuccess"

//#define GWS_HTTP_ERROR_AUTH "HTTP/1.1 403 Forbidden \r\nServer: GWS/1.0 \r\nDate: Thu,08 Mar 2099 00:00:00 GMT \r\nExpries:0 \r\nContent-Type:text/html;"

#define GWS_HTTP_ERROR_DATA "HTTP/1.1 200 OK \r\nServer: GWS/1.0 \r\nDate: Thu,08 Mar 2099 00:00:00 GMT \r\nExpries:0 \r\nContent-Type:text/html; charset=UTF-8 \r\nContent-Length:9 \r\n\r\nDataError"

#define GWS_HTTP_ERROR_LOGIN "HTTP/1.1 200 OK \r\nServer: GWS/1.0 \r\nDate: Thu,08 Mar 2099 00:00:00 GMT \r\nExpries:0 \r\nContent-Type:text/html; charset=UTF-8 \r\nContent-Length:11 \r\n\r\nLoginFailed"

#define GWS_HTTP_ERROR_AUTH "HTTP/1.1 200 OK \r\nServer: GWS/1.0 \r\nDate: Thu,08 Mar 2099 00:00:00 GMT \r\nExpries:0 \r\nContent-Type:text/html; charset=UTF-8 \r\nContent-Length:10 \r\n\r\nAuthFailed"

#define GWS_HTTP_CHANNEL_ERROR_AUTH "HTTP/1.1 200 OK \r\nServer: GWS/1.0 \r\nDate: Thu,08 Mar 2099 00:00:00 GMT \r\nExpries:0 \r\nContent-Type:text/html; charset=UTF-8 \r\nContent-Length:13 \r\n\r\nChannelFailed"

#define GWS_HTTP_IP_ERROR_AUTH "HTTP/1.1 200 OK \r\nServer: GWS/1.0 \r\nDate: Thu,08 Mar 2099 00:00:00 GMT \r\nExpries:0 \r\nContent-Type:text/html; charset=UTF-8 \r\nContent-Length:8 \r\n\r\nIPFailed"

#define GWS_HTTP_CMD_ERROR_AUTH "HTTP/1.1 200 OK \r\nServer: GWS/1.0 \r\nDate: Thu,08 Mar 2099 00:00:00 GMT \r\nExpries:0 \r\nContent-Type:text/html; charset=UTF-8 \r\nContent-Length:9 \r\n\r\nCmdFailed"

#define GWS_HTTP_ERROR_NO_APP "HTTP/1.1 200 OK \r\nServer: GWS/1.0 \r\nDate: Thu,08 Mar 2099 00:00:00 GMT \r\nExpries:0 \r\nContent-Type:text/html; charset=UTF-8 \r\nContent-Length:5 \r\n\r\nNoApp"

#define GWS_HTTP_ERROR_NO_APPTYPE "HTTP/1.1 200 OK \r\nServer: GWS/1.0 \r\nDate: Thu,08 Mar 2099 00:00:00 GMT \r\nExpries:0 \r\nContent-Type:text/html; charset=UTF-8 \r\nContent-Length:9 \r\n\r\nNoAppType"

#define GWS_HTTP_ERROR_DISABLE_APPTYPE "HTTP/1.1 200 OK \r\nServer: GWS/1.0 \r\nDate: Thu,08 Mar 2099 00:00:00 GMT \r\nExpries:0 \r\nContent-Type:text/html; charset=UTF-8 \r\nContent-Length:14 \r\n\r\nDisableAppType"

#pragma pack()

#endif