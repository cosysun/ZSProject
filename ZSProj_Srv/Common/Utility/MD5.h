#ifndef SYSBASE_TEXTENCODE_MD5_H
#define SYSBASE_TEXTENCODE_MD5_H

#include <stdio.h>
#include <time.h>
#include <string.h>

#include "BaseTypeDef.h"

#define MD5_LENGTH 32

#define MD5_STRING_SIZE 33

class Cmd5
{
public:

    //����:MD5����[ANSII]
    //����:
    //      char *pszOutput OUT �����MD5�ַ������ò��������鳤�ȱ�����ڵ��� MD5_STRING_SIZE [�����ڴ��쳣]
    //      UCHAR *pBytes IN ��Ҫ���б���Ļ�������ַ
    //      UINT uBytesLen IN ��Ҫ���б���Ļ�������С
    //����ֵ: ����ʧ��ʱ����NULL���ɹ�ʱ���ز���pszOutputָ��ĵ�ַָ��
    static char *EncodeBytesA(char *pszOutput, UCHAR *pBytes, UINT uBytesLen);
};

#endif