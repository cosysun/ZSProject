#ifndef _LINUX

//
#   define STA_CAST(TYPE, EXPR) static_cast<TYPE>(EXPR)
#   define CON_CAST(TYPE, EXPR) const_cast<TYPE>(EXPR)
#   define REI_CAST(TYPE, EXPR) reinterpret_cast<TYPE>(EXPR)
#   define DYN_CAST(TYPE, EXPR) dynamic_cast<TYPE>(EXPR)
//
#include "mdump.h"
#include "dbghelp.h"
//
#define MAKE_POINTER(val) REI_CAST(void*, REI_CAST(char*, 0) + (val))
#define MAKE_VALUE(pointer) STA_CAST(unsigned int, REI_CAST(char*, (pointer)) - REI_CAST(char*, 0))
//

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
										 CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
										 CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
										 CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
										 );

#if defined _M_X64 || defined _M_IX86
LPTOP_LEVEL_EXCEPTION_FILTER WINAPI 
MyDummySetUnhandledExceptionFilter(
								   LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
	return NULL;
}
#else
#error "This code works only for x86 and x64!"
#endif

BOOL PreventSetUnhandledExceptionFilter()
{
	HMODULE hKernel32 = LoadLibrary(_T("kernel32.dll"));
	if (hKernel32 == NULL) return FALSE;
	void *pOrgEntry = GetProcAddress(hKernel32, 
		"SetUnhandledExceptionFilter");
	if(pOrgEntry == NULL) return FALSE;

	DWORD dwOldProtect = 0;
	SIZE_T jmpSize = 5;
#ifdef _M_X64
	jmpSize = 13;
#endif
	BOOL bProt = VirtualProtect(pOrgEntry, jmpSize, 
		PAGE_EXECUTE_READWRITE, &dwOldProtect);
	BYTE newJump[20];
	void *pNewFunc = &MyDummySetUnhandledExceptionFilter;
#ifdef _M_IX86
	DWORD dwOrgEntryAddr = (DWORD) MAKE_VALUE(pOrgEntry);
	dwOrgEntryAddr += (DWORD)jmpSize; // add 5 for 5 op-codes for jmp rel32
	DWORD dwNewEntryAddr = (DWORD) MAKE_VALUE(pNewFunc);
	DWORD dwRelativeAddr = dwNewEntryAddr - dwOrgEntryAddr;
	// JMP rel32: Jump near, relative, displacement relative to next instruction.
	newJump[0] = 0xE9;  // JMP rel32
	memcpy(&newJump[1], &dwRelativeAddr, sizeof(pNewFunc));
#elif _M_X64
	// We must use R10 or R11, because these are "scratch" registers 
	// which need not to be preserved accross function calls
	// For more info see: Register Usage for x64 64-Bit
	// http://msdn.microsoft.com/en-us/library/ms794547.aspx
	// Thanks to Matthew Smith!!!
	newJump[0] = 0x49;  // MOV R11, ...
	newJump[1] = 0xBB;  // ...
	memcpy(&newJump[2], &pNewFunc, sizeof (pNewFunc));
	//pCur += sizeof (ULONG_PTR);
	newJump[10] = 0x41;  // JMP R11, ...
	newJump[11] = 0xFF;  // ...
	newJump[12] = 0xE3;  // ...
#endif
	SIZE_T bytesWritten;
	BOOL bRet = WriteProcessMemory(GetCurrentProcess(),
		pOrgEntry, newJump, jmpSize, &bytesWritten);

	if (bProt != FALSE)
	{
		DWORD dwBuf;
		VirtualProtect(pOrgEntry, jmpSize, dwOldProtect, &dwBuf);
	}
	return bRet;
}

char MiniDumper::m_szDumpName[] = "";
HWND   MiniDumper::m_hWnd = NULL;

MiniDumper::MiniDumper( LPCSTR szDumpPath, LPCSTR szAppName, HWND hWnd )
{
	// if this assert fires then you have two instances of MiniDumper
	// which is not allowed
	assert( szDumpPath!=NULL );

	if (szDumpPath)
	{
		strcat(m_szDumpName, szDumpPath);
		strcat(m_szDumpName, "\\");
	}
	if (szAppName)
	{
		strcat(m_szDumpName, szAppName);
		strcat(m_szDumpName, ".dmp");
	}

	::SetUnhandledExceptionFilter( TopLevelFilter );
	PreventSetUnhandledExceptionFilter();

	m_hWnd = hWnd;
}

LONG MiniDumper::TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo )
{
	LONG retval = EXCEPTION_CONTINUE_SEARCH;
	HWND hParent = NULL;						// find a better value for your app

	// firstly see if dbghelp.dll is around and has the function we need
	// look next to the EXE first, as the one in System32 might be old 
	// (e.g. Windows 2000)
	HMODULE hDll = NULL;
	char szDbgHelpPath[_MAX_PATH];

	if (GetModuleFileName( NULL, szDbgHelpPath, _MAX_PATH ))
	{
		char *pSlash = _tcsrchr( szDbgHelpPath, '\\' );
		if (pSlash)
		{
			_tcscpy( pSlash+1, "DBGHELP.DLL" );
			hDll = ::LoadLibrary( szDbgHelpPath );
		}
	}

	if (hDll==NULL)
	{
		// load any version we can
		hDll = ::LoadLibrary( "DBGHELP.DLL" );
	}

	LPCTSTR szResult = NULL;

	if (hDll)
	{
		MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );
		if (pDump)
		{
			char szScratch [_MAX_PATH];

			// work out a good place for the dump file
			//if (!GetTempPath( _MAX_PATH, szDumpPath ))
            /*
			char szAppInfo[512];
			g_runing_info.get_info(szAppInfo, 512);
			LG("run_info", "%s\n", szAppInfo);
			*/

			// ask the user if they want to save a dump file
			{
				// create the file
				HANDLE hFile = ::CreateFile( m_szDumpName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
											FILE_ATTRIBUTE_NORMAL, NULL );

				if (hFile!=INVALID_HANDLE_VALUE)
				{
					_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

					ExInfo.ThreadId = ::GetCurrentThreadId();
					ExInfo.ExceptionPointers = pExceptionInfo;
					ExInfo.ClientPointers = NULL;

					// write the dump
					BOOL bOK = pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, (_MINIDUMP_TYPE)(MiniDumpNormal/* | MiniDumpWithFullMemory*/), &ExInfo, NULL, NULL );
					if (bOK)
					{
						sprintf( szScratch, "成功保存故障记录文件(dump file) 到 '%s'", m_szDumpName );
						szResult = szScratch;
						retval = EXCEPTION_EXECUTE_HANDLER;
					}
					else
					{
						sprintf( szScratch, "保存故障文件(dump file)失败 '%s' (error %d)", m_szDumpName, GetLastError() );
						szResult = szScratch;
					}
					::CloseHandle(hFile);
				}
				else
				{
					sprintf( szScratch, "不能创建故障记录文件(dump file) '%s' (error %d)", m_szDumpName, GetLastError() );
					szResult = szScratch;
				}
			}
		}
		else
		{
			szResult = "DBGHELP.DLL too old";
		}
	}
	else
	{
		szResult = "DBGHELP.DLL not found";
	}

	return retval;
}

#endif