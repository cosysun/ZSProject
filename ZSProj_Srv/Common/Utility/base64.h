#ifndef SYSBASE_TEXTENCODE_BASE64_H
#define SYSBASE_TEXTENCODE_BASE64_H

#include <Windows.h>

#define BASE64_DECODE_ERROR 0xffffffff

class CBase64
{
public:

	//功能:获取编码后需要的最小缓冲区大小
	//参数:
	//		DWORD dwSize IN 需要编码的数据大小
	//返回值:编码后需要的最小缓冲区大小
	static DWORD GetMinEncodeSize(DWORD dwSize);

    //功能:编码[ANSII]
    //参数:
    //      PCHAR pOutBuffer OUT 接收转换后得到的BASE64编码的缓冲区
    //		DWORD dwOutSize IN pOutBuffer 参数缓冲区的大小
    //      LPCVOID pData IN 需要编码的缓冲区地址
    //      DWORD dwSize IN 需要编码的缓冲区大小
    //返回值: 0 错误; >0 BASE64编码后的有效大小
    static DWORD Encode(PCHAR pOutBuffer, DWORD dwOutSize, LPCVOID pData, DWORD dwSize);

	//功能:获取解码后需要的最小缓冲区大小
	//参数:
	//		DWORD dwSize IN 需要解码的数据大小
	//返回值:解码后需要的最小缓冲区大小
	static DWORD GetMinDecodeSize(DWORD dwSize);

    //功能:解码[ANSII]
    //参数:
    //      LPVOID pOutBuffer OUT 解码的缓冲区地址
    //		DWORD dwOutSize IN OUT 接收解密后数据的缓冲区的大小
    //      LPCSTR szBase64 IN 需要解码的BASE64字符串
    //返回值: 0 错误; >0 BASE64编码前的有效大小
    static DWORD Decode(LPVOID pOutBuffer, DWORD dwOutSize, LPCSTR szBase64);
};

#endif