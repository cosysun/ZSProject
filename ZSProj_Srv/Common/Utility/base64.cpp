#include <stdlib.h>
#include <string.h>
#include "base64.h"


//////////////////////////////////////////////////////////////////////////
//Base64

static char base64_chars[65] ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

//处理特别的解析器(如MYSQL）
//char CBase64Ex::base64_chars[65] ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789*-";

DWORD token_decode(const char* token)
{
    DWORD dwIndex;
    DWORD dwVal = 0;
    DWORD dwMarker = 0;

    if (strlen(token) < 4)
    {
        return BASE64_DECODE_ERROR;
    }

    for (dwIndex = 0; dwIndex < 4; dwIndex++) 
    {
        dwVal *= 64;

        if (token[dwIndex] == '=')
        {
            dwMarker++;
        }
        else if (dwMarker > 0)
        {
            return BASE64_DECODE_ERROR;
        }
        else
        {
			char* pPtr = strchr(base64_chars, token[dwIndex]);

			if (pPtr)
			{	
				dwVal += (DWORD)(pPtr - base64_chars);
			}
			else
			{
				dwVal -= 1;
			}
        }
    }

    if (dwMarker > 2)
    {
        return BASE64_DECODE_ERROR;
    }

    return (dwMarker << 24) | dwVal;
}

DWORD CBase64::GetMinEncodeSize(DWORD dwSize)
{
	return dwSize * 4 / 3 + 1;
}

DWORD CBase64::Encode(PCHAR pOutBuffer, DWORD dwOutSize, LPCVOID pData, DWORD dwSize)
{
	if (!pOutBuffer || 0 == dwOutSize || !pData || 0 == dwSize)
	{
		return 0;
	}
	
	DWORD dwMinOutSize = CBase64::GetMinEncodeSize(dwSize);
	
	if (dwOutSize < dwMinOutSize)
	{
		return 0;
	}

    DWORD dwIndex = 0;
    DWORD dwTemp = 0;
    LPCBYTE pTemp = NULL;
    PCHAR pOutTemp = pOutBuffer;

    pTemp = (LPCBYTE)pData;

    dwIndex = 0;

    for (dwIndex = 0; dwIndex < dwSize;) 
    {
        dwTemp = pTemp[dwIndex++];
        dwTemp *= 256;

        if (dwIndex < dwSize)
        {
            dwTemp += pTemp[dwIndex];
        }

        dwIndex++;
        dwTemp *= 256;

        if (dwIndex < dwSize)
        {
            dwTemp += pTemp[dwIndex];
        }

        dwIndex++;
        pOutTemp[0] = base64_chars[(dwTemp & 0x00fc0000) >> 18];
        pOutTemp[1] = base64_chars[(dwTemp & 0x0003f000) >> 12];
        pOutTemp[2] = base64_chars[(dwTemp & 0x00000fc0) >> 6];
        pOutTemp[3] = base64_chars[(dwTemp & 0x0000003f) >> 0];

        if (dwIndex > dwSize)
        {
            pOutTemp[3] = '=';
        }

        if (dwIndex > dwSize + 1)
        {
            pOutTemp[2] = '=';
        }

        pOutTemp += 4;
    }
    
    *pOutTemp = NULL;

	return (DWORD)(pOutTemp - pOutBuffer);
}

DWORD CBase64::GetMinDecodeSize(DWORD dwSize)
{
	return (dwSize - 1) * 3 / 4;
}

DWORD CBase64::Decode(LPVOID pOutBuffer, DWORD dwOutSize, LPCSTR szBase64)
{
	if (!pOutBuffer || 0 == dwOutSize || !szBase64)
	{
		return 0;
	}

    LPCSTR pTemp = szBase64;
    LPBYTE pOutTemp = (LPBYTE)pOutBuffer;

	DWORD dwMinDecodeSize = CBase64::GetMinDecodeSize((DWORD)strlen(szBase64));
	
	if (dwOutSize < dwMinDecodeSize)
	{
		return 0;
	}

    for (pTemp = szBase64;*pTemp && (*pTemp == '=' || strchr(base64_chars, *pTemp)); pTemp += 4) 
    {
        DWORD dwVal = token_decode(pTemp);

        DWORD dwMarker = (dwVal >> 24) & 0xff;

        if (dwVal == BASE64_DECODE_ERROR)
        {
            return 0;
        }

       *pOutTemp++ = (unsigned char)((dwVal >> 16) & 0xff);

        if (dwMarker < 2)
        {
           *pOutTemp++ = (unsigned char)((dwVal >> 8) & 0xff);
        }
        if (dwMarker < 1)
        {
           *pOutTemp++ = (unsigned char)(dwVal & 0xff);
        }
    }

    dwOutSize = (DWORD)(pOutTemp - (unsigned char* )pOutBuffer);
    
    return dwOutSize;
}