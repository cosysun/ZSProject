#include "CheckSum.h"

//º”√‹KEY
static DWORD s_dwEncryKey = 0x9d590617;

CCheckSum CCheckSum::s_CCheckSum;

CCheckSum::CCheckSum()
{
	int nIndex = 0;
	
	for (nIndex = 0; nIndex < CHECK_DATA_SIZE; ++nIndex)
	{
		m_CheckSrvBuffer[nIndex] = 'a' + nIndex;
	}
}

CCheckSum::~CCheckSum()
{

}

CCheckSum& CCheckSum::Instance()
{
	return s_CCheckSum;
}

WORD CCheckSum::GetCheckData(PCHAR lpBuffer, WORD wSize)
{
	if (!lpBuffer || wSize < CHECK_DATA_SIZE)
	{
		return 0;
	}
	
	DWORD dwTickCount = GetTickCount();
	int nSize = CHECK_DATA_SIZE;
	int nBlocks = nSize / 4;
	int nRemainByte = nSize % 4;
	int nIndex = 0;
	int nIndex2 = 0;
	
	BYTE* pCurTime = (BYTE*)&dwTickCount;

	for (nIndex=0; nIndex < nBlocks; ++nIndex)
	{
		for (nIndex2 = 0; nIndex2 < 4; ++nIndex2)
		{
			lpBuffer[nIndex * 4 + nIndex2] = m_CheckSrvBuffer[nIndex*4 + nIndex2] ^ pCurTime[nIndex2];
		}
	}

	if (nRemainByte > 0)
	{
		for (nIndex2 = 0; nIndex2 < nRemainByte; ++nIndex2)
		{
			lpBuffer[nBlocks * 4 + nIndex2]  = m_CheckSrvBuffer[nBlocks * 4 + nIndex2] ^ pCurTime[nIndex2];
		}
	}
	
	return CHECK_DATA_SIZE;
}

DWORD CCheckSum::EncryCheckData(CONST PCHAR lpBuffer, WORD wSize)
{
	if (!lpBuffer || wSize < CHECK_DATA_SIZE)
	{	
		return 0;
	}

	DWORD dwValue = 0;
	int nSize  = CHECK_DATA_SIZE;
	int nBlocks = nSize >> 2;
	int nRemainByte = nSize % 4;

	char chCase = (char)lpBuffer[0] % 3;

	BYTE btTmpBlock[4];
	BYTE btDes[4];
	
	memset(btDes,0,4);
	memset(btTmpBlock, 0, 4);
	
	memcpy(btDes, &s_dwEncryKey, 4);

	int nIndex = 0;
	int nIndex2 = 0;

	switch(chCase)
	{
	case 0:
		{
			for (nIndex=0; nIndex < nBlocks; ++nIndex)
			{
				for (nIndex2 = 0; nIndex2 < 4; ++nIndex2)
				{
					btDes[nIndex2] ^= lpBuffer[(nIndex<<2) + nIndex2];
				}
			}

			if (nRemainByte > 0 )
			{
				memcpy(btTmpBlock, lpBuffer + (nBlocks << 2), nRemainByte);
				
				for (nIndex2 = 0; nIndex2 < 4; ++nIndex2)
				{
					btDes[nIndex2] ^= btTmpBlock[nIndex2];
				}
					
			}
		}
		break;
	case 1:
		{
			for (nIndex = 0; nIndex < nBlocks; ++nIndex)
			{
				for( nIndex2 = 0; nIndex2 < 4; ++nIndex2)
				{
					btDes[nIndex2] &= lpBuffer[(nIndex << 2) + nIndex2];
				}
			}	

			if (nRemainByte > 0)
			{
				memcpy(btTmpBlock, lpBuffer + (nBlocks << 2), nRemainByte);
				
				for (nIndex2=0; nIndex2<4; ++nIndex2)
				{
					btDes[nIndex2] &= btTmpBlock[nIndex2];
				}
			}
		}
		break;
	case 2:
		{
			for (nIndex=0; nIndex < nBlocks; ++nIndex)
			{
				for (nIndex2=0; nIndex2 < 4; ++nIndex2)
				{
					btDes[nIndex2] |= lpBuffer[(nIndex << 2) + nIndex2];
				}
			}

			if( nRemainByte > 0 )
			{
				memcpy(btTmpBlock, lpBuffer + (nBlocks << 2), nRemainByte);
				
				for (nIndex2 = 0; nIndex2 < 4; ++nIndex2)
				{
					btDes[nIndex2] |= btTmpBlock[nIndex2];
				}
			}
		}
		break;
	}
	memcpy(&dwValue, btDes, 4);
	
	return dwValue;
}