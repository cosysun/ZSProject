#ifndef INI_FILE_H_
#define INI_FILE_H_

class CIniFile
{
public:
	
	static int ReadString(
		const char* lpAppName,
		const char* lpKeyName,
		const char* lpDefault,
		char* lpReturnedString,
		int nSize,
		const char* lpFileName);
	
	static int ReadInt(
		const char* lpAppName,
		const char* lpKeyName,
		int nDefault,
		const char* lpFileName);
		
	static int WriteString(
		const char* lpAppName,
		const char* lpKeyName,
		const char* lpString,
		const char* lpFileName);
	
};

#endif

