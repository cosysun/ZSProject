#ifndef CHECK_SUM_H
#define CHECK_SUM_H

#include "BaseTypeDef.h"

#define CHECK_DATA_SIZE 20

class CCheckSum
{
public:

	virtual ~CCheckSum();
	
	static CCheckSum& Instance();

	WORD GetCheckData(PCHAR lpBuffer, WORD wSize);

	DWORD EncryCheckData(CONST PCHAR lpBuffer, WORD wSize);

private:
	
	CCheckSum();
	
	static CCheckSum s_CCheckSum;
	
	BYTE m_CheckSrvBuffer[CHECK_DATA_SIZE];
};

#endif