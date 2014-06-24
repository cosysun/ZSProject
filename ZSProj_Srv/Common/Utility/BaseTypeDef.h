#ifndef UTILITY_TYPE_DEF_H
#define UTILITY_TYPE_DEF_H

#ifndef _LINUX
#include <windows.h>
#else
typedef unsigned int DWORD;
typedef unsigned short WORD;

typedef char CHAR;
typedef CONST CHAR *LPCSTR, *PCSTR;

typedef void* LPVOID;
typedef unsigned char UCHAR;
#endif

#endif