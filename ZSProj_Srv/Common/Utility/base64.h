#ifndef SYSBASE_TEXTENCODE_BASE64_H
#define SYSBASE_TEXTENCODE_BASE64_H

#include <Windows.h>

#define BASE64_DECODE_ERROR 0xffffffff

class CBase64
{
public:

	//����:��ȡ�������Ҫ����С��������С
	//����:
	//		DWORD dwSize IN ��Ҫ��������ݴ�С
	//����ֵ:�������Ҫ����С��������С
	static DWORD GetMinEncodeSize(DWORD dwSize);

    //����:����[ANSII]
    //����:
    //      PCHAR pOutBuffer OUT ����ת����õ���BASE64����Ļ�����
    //		DWORD dwOutSize IN pOutBuffer �����������Ĵ�С
    //      LPCVOID pData IN ��Ҫ����Ļ�������ַ
    //      DWORD dwSize IN ��Ҫ����Ļ�������С
    //����ֵ: 0 ����; >0 BASE64��������Ч��С
    static DWORD Encode(PCHAR pOutBuffer, DWORD dwOutSize, LPCVOID pData, DWORD dwSize);

	//����:��ȡ�������Ҫ����С��������С
	//����:
	//		DWORD dwSize IN ��Ҫ��������ݴ�С
	//����ֵ:�������Ҫ����С��������С
	static DWORD GetMinDecodeSize(DWORD dwSize);

    //����:����[ANSII]
    //����:
    //      LPVOID pOutBuffer OUT ����Ļ�������ַ
    //		DWORD dwOutSize IN OUT ���ս��ܺ����ݵĻ������Ĵ�С
    //      LPCSTR szBase64 IN ��Ҫ�����BASE64�ַ���
    //����ֵ: 0 ����; >0 BASE64����ǰ����Ч��С
    static DWORD Decode(LPVOID pOutBuffer, DWORD dwOutSize, LPCSTR szBase64);
};

#endif