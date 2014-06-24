
#ifndef __FENCRYPTTOOL_H__
#define __FENCRYPTTOOL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseTypeDef.h"

class fEncryptTool
{
public:
	fEncryptTool();
	virtual ~fEncryptTool();

public:
	void Decode(BYTE * buffer,DWORD dwSize,BYTE * pOut,DWORD *pSize,char * pwd);
	void Encode(BYTE * buffer,DWORD dwSize,BYTE * pOut,DWORD *pSize,char * pwd);
	
	void Atohex(char *instr, int inlen, char *outstr, int* outlen);
	void Hextoa(char *instr, int inlen, char *outstr, int *outlen);
private:

	int desinit(int mode);

	void desdone();

	int setKey(char (*kn)[8], char *key);

	int endes(char (*kn)[8], char *block);

	int dedes(char (*kn)[8], char *block);

	void permute(char *inblock, char perm[16][16][8], char *outblock);

	long f(register long r, register char *subkey);

	void perminit(char perm[16][16][8], char p[64]);

	void spinit();

#ifdef	LITTLE_ENDIAN
	static long byteswap(unsigned long x);
#endif

	void chorder(char* str);

	char* addstr(char *str1, char *str2);

	void freestr(char *str);

	long doencrypt(char* pszstr, long nlen,char * pOut,char * pwd);

	long dodecrypt(char* pszstr, long nlen,char * pOut,char * pwd);

	void atohex(char *instr, int inlen, char *outstr, int* outlen);

	unsigned char BtoH(char ch);

	void hextoa(char *instr, int inlen, char *outstr, int *outlen);

private:
	/* Lookup tables initialized once only at startup by desinit() */
	long (*sp)[64];		/* Combined S and P boxes */
	char (*iperm)[16][8];	/* Initial and final permutations */
	char (*fperm)[16][8];


	int desmode;
};

#endif
