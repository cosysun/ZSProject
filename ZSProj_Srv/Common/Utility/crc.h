#pragma once

#include "BaseTypeDef.h"

DWORD GetCrc32(DWORD dwCrc, void * szBuff, int nLen);

#ifndef _LINUX
BOOL GetFileCrc32(LPCTSTR lpszFileName, DWORD& dwCrc32);
#endif
