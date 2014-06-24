#pragma once

#ifndef _LINUX

#include <assert.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#if _MSC_VER < 1300
#define DECLSPEC_DEPRECATED
// VC6: change this path to your Platform SDK headers
#include "M:\\dev7\\vs\\devtools\\common\\win32sdk\\include\\dbghelp.h"			// must be XP version of file
#else
// VC7: ships with updated headers

#endif


// based on dbghelp.h


class MiniDumper
{
private:
	static LONG WINAPI TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo );

public:
	MiniDumper( LPCSTR szDumpPath, LPCSTR szAppName, HWND hWnd = NULL );

	static HWND m_hWnd;
	static char m_szDumpName[_MAX_PATH + _MAX_FNAME];
};

#endif