// fEncryptTool.cpp: implementation of the fEncryptTool class.
//
//////////////////////////////////////////////////////////////////////

#include "fEncryptTool.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#undef TRACE
#undef DEBUG

#define PRIV_KEY 	"abcd1234"

/* Tables defined in the Data Encryption Standard documents */

/* initial permutation IP */
static char ip[] = {
	58, 50, 42, 34, 26, 18, 10,  2,
	60, 52, 44, 36, 28, 20, 12,  4,
	62, 54, 46, 38, 30, 22, 14,  6,
	64, 56, 48, 40, 32, 24, 16,  8,
	57, 49, 41, 33, 25, 17,  9,  1,
	59, 51, 43, 35, 27, 19, 11,  3,
	61, 53, 45, 37, 29, 21, 13,  5,
	63, 55, 47, 39, 31, 23, 15,  7
};

/* final permutation IP^-1 */
static char fp[] = {
	40,  8, 48, 16, 56, 24, 64, 32,
	39,  7, 47, 15, 55, 23, 63, 31,
	38,  6, 46, 14, 54, 22, 62, 30,
	37,  5, 45, 13, 53, 21, 61, 29,
	36,  4, 44, 12, 52, 20, 60, 28,
	35,  3, 43, 11, 51, 19, 59, 27,
	34,  2, 42, 10, 50, 18, 58, 26,
	33,  1, 41,  9, 49, 17, 57, 25
};

/* expansion operation matrix
 * This is for reference only; it is unused in the code
 * as the f() function performs it implicitly for speed
 */
#ifdef notdef
static char ei[] = {
	32,  1,  2,  3,  4,  5,
	 4,  5,  6,  7,  8,  9,
	 8,  9, 10, 11, 12, 13,
	12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21,
	20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29,
	28, 29, 30, 31, 32,  1 
};
#endif

/* permuted choice table (key) */
static char pc1[] = {
	57, 49, 41, 33, 25, 17,  9,
	 1, 58, 50, 42, 34, 26, 18,
	10,  2, 59, 51, 43, 35, 27,
	19, 11,  3, 60, 52, 44, 36,

	63, 55, 47, 39, 31, 23, 15,
	 7, 62, 54, 46, 38, 30, 22,
	14,  6, 61, 53, 45, 37, 29,
	21, 13,  5, 28, 20, 12,  4
};

/* number left rotations of pc1 */
static char totrot[] = {
	1,2,4,6,8,10,12,14,15,17,19,21,23,25,27,28
};

/* permuted choice key (table) */
static char pc2[] = {
	14, 17, 11, 24,  1,  5,
	 3, 28, 15,  6, 21, 10,
	23, 19, 12,  4, 26,  8,
	16,  7, 27, 20, 13,  2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32
};

/* The (in)famous S-boxes */
static char si[8][64] = {
	/* S1 */
	14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
	 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
	 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
	15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,

	/* S2 */
	15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
	 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
	 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
	13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9,

	/* S3 */
	10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
	13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
	13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
	 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12,

	/* S4 */
	 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
	13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
	10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
	 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14,

	/* S5 */
	 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
	14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
	 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
	11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3,

	/* S6 */
	12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
	10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
	 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
	 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,

	/* S7 */
	 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
	13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
	 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
	 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12,

	/* S8 */
	13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
	 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
	 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
	 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
};

/* 32-bit permutation function P used on the output of the S-boxes */
static char p32i[] = {	
	16,  7, 20, 21,
	29, 12, 28, 17,
	 1, 15, 23, 26,
	 5, 18, 31, 10,
	 2,  8, 24, 14,
	32, 27,  3,  9,
	19, 13, 30,  6,
	22, 11,  4, 25
};
/* End of DES-defined tables */


/* bit 0 is left-most in byte */
static int bytebit[] = {
	0200,0100,040,020,010,04,02,01
};

static int nibblebit[] = {
	 010,04,02,01
};


fEncryptTool::fEncryptTool()
{

}

fEncryptTool::~fEncryptTool()
{

}

void fEncryptTool::Encode(BYTE *buffer, DWORD dwSize, BYTE *pOut, DWORD *pSize, char *pwd)
{
	(*pSize) = doencrypt((char *)buffer,dwSize,(char *)pOut,pwd);
//	pOut[*pSize] = 0;
}

void fEncryptTool::Decode(BYTE *buffer, DWORD dwSize, BYTE *pOut, DWORD *pSize, char *pwd)
{
	(*pSize) = dodecrypt((char *)buffer,dwSize,(char *)pOut,pwd);
//	pOut[*pSize] = 0;
}

void fEncryptTool::Atohex(char *instr, int inlen, char *outstr, int* outlen)
{
	atohex(instr,inlen,outstr,outlen);
//	outstr[*outlen] = 0;
}

void fEncryptTool::Hextoa(char *instr, int inlen, char *outstr, int *outlen)
{
	hextoa(instr,inlen,outstr,outlen);
	outstr[*outlen] = 0;
}

//---------------------------------------------------------- 
// Function:   desinit
// Descrption: Allocate space and initialize DES lookup arrays
//	       mode == 0: standard Data Encryption Algorithm
//             mode == 1: DEA without initial and final
//             permutations for speed.
// Input:      
//	       mode --- the value of mode
// Return:     VOID
//----------------------------------------------------------
int fEncryptTool::desinit(int mode)
{
	//---------------------------------
	/*
	char *malloc();
		
	if(sp != NULL){
		/* Already initialized */
		/*
		return 0;
	}
	*/
	//---------------------------------
	
	desmode = mode;
	
	if((sp = (long (*)[64])malloc(sizeof(long) * 8 * 64)) == NULL){
		return 0;
	}
	spinit();
	if(mode == 1)	/* No permutations */
		return 0;

	iperm = (char (*)[16][8])malloc(sizeof(char) * 16 * 16 * 8);
	if(iperm == NULL){
		free((char *)sp);
		return -1;
	}
	perminit(iperm,ip);

	fperm = (char (*)[16][8])malloc(sizeof(char) * 16 * 16 * 8);
	if(fperm == NULL){
		free((char *)sp);
		free((char *)iperm);
		return -1;
	}
	perminit(fperm,fp);
	
	return 0;
}

//---------------------------------------------------------- 
// Function:   desdone
// Descrption: Free up storage used by DES 
// Input:      NULL
// Return:     VOID
//----------------------------------------------------------
void fEncryptTool::desdone()
{
	if(sp == NULL)
		return;	/* Already done */

	free((char *)sp);
	if(iperm != NULL)
		free((char *)iperm);
	if(fperm != NULL)
		free((char *)fperm);

	sp = NULL;
	iperm = NULL;
	fperm = NULL;
}

//---------------------------------------------------------- 
// Function:   setKey
// Descrption: Set key (initialize key schedule array) 
// Input:
//	       char (*kn)[8] --- Key schedule
//	       char *key     --- 64 bits (will use only 56)
// Return:     int
//----------------------------------------------------------
int fEncryptTool::setKey(char (*kn)[8], char *key)
{
	char pc1m[56];		/* place to modify pc1 into */
	char pcr[56];		/* place to rotate pc1 into */
	register int i,j,l;
	int m;

	if(kn == NULL){
		return -1;
	}

	/* Clear key schedule */
	memset((char *)kn,0,16*8);

	for (j=0; j<56; j++) {		/* convert pc1 to bits of key */
		l=pc1[j]-1;		/* integer bit location	 */
		m = l & 07;		/* find bit		 */
		pc1m[j]=(key[l>>3] &	/* find which key byte l is in */
			bytebit[m])	/* and which bit of that byte */
			? 1 : 0;	/* and store 1-bit result */
	}
	for (i=0; i<16; i++) {		/* key chunk for each iteration */
		for (j=0; j<56; j++)	/* rotate pc1 the right amount */
			pcr[j] = pc1m[(l=j+totrot[i])<(j<28? 28 : 56) ? l: l-28];
			/* rotate left and right halves independently */
		for (j=0; j<48; j++){	/* select bits individually */
			/* check bit that goes to kn[j] */
			if (pcr[pc2[j]-1]){
				/* mask it in if it's there */
				l=j % 6;
				kn[i][j/6] |= bytebit[l] >> 2;
			}
		}
	}
	return 0;
}

//---------------------------------------------------------- 
// Function:   endes
// Descrption: In-place encryption of 64-bit block 
// Input:      
//	       char (*kn)[8] --- Key schedule
//	       char *block
// Return:     VOID
//----------------------------------------------------------
int fEncryptTool::endes(char (*kn)[8], char *block)
{
	register long left,right;
	register char *knp;
	long work[2]; 		/* Working data storage */

	if(kn == NULL || block == NULL)
	{
		return -1;
	}
	
	permute(block,iperm,(char *)work);	/* Initial Permutation */
			
#ifdef	LITTLE_ENDIAN
	left = byteswap(work[0]);
	right = byteswap(work[1]);
#else
	left = work[0];
	right = work[1];
#endif

	/* Do the 16 rounds.
	 * The rounds are numbered from 0 to 15. On even rounds
	 * the right half is fed to f() and the result exclusive-ORs
	 * the left half; on odd rounds the reverse is done.
	 */
	knp = &kn[0][0];
	left ^= f(right,knp);
	knp += 8;
	right ^= f(left,knp);
	knp += 8;
	left ^= f(right,knp);
	knp += 8;
	right ^= f(left,knp);
	knp += 8;
	left ^= f(right,knp);
	knp += 8;
	right ^= f(left,knp);
	knp += 8;
	left ^= f(right,knp);
	knp += 8;
	right ^= f(left,knp);
	knp += 8;
	left ^= f(right,knp);
	knp += 8;
	right ^= f(left,knp);
	knp += 8;
	left ^= f(right,knp);
	knp += 8;
	right ^= f(left,knp);
	knp += 8;
	left ^= f(right,knp);
	knp += 8;
	right ^= f(left,knp);
	knp += 8;
	left ^= f(right,knp);
	knp += 8;
	right ^= f(left,knp);

	/* Left/right half swap, plus byte swap if little-endian */
#ifdef	LITTLE_ENDIAN
	work[1] = byteswap(left);
	work[0] = byteswap(right);
#else
	work[0] = right;
	work[1] = left;
#endif
	
	permute((char *)work,fperm,block);	/* Inverse initial permutation */
	
	return 0;
}

//---------------------------------------------------------- 
// Function:   dedes
// Descrption: In-place decryption of 64-bit block. This 
//	       function is the mirror image of encryption;
//             exactly the same steps are taken, but in
//	       reverse order.
// Input:      
//	       char (*kn)[8] --- Key schedule
//	       char *block
// Return:     int
//----------------------------------------------------------
int fEncryptTool::dedes(char (*kn)[8], char *block)
{
	register long left,right;
	register char *knp;
	long work[2];	/* Working data storage */

	if(kn == NULL || block == NULL)
		return -1;
	permute(block,iperm,(char *)work);	/* Initial permutation */

	/* Left/right half swap, plus byte swap if little-endian */
#ifdef	LITTLE_ENDIAN
	right = byteswap(work[0]);
	left = byteswap(work[1]);
#else
	right = work[0];
	left = work[1];
#endif
	/* Do the 16 rounds in reverse order.
	 * The rounds are numbered from 15 to 0. On even rounds
	 * the right half is fed to f() and the result exclusive-ORs
	 * the left half; on odd rounds the reverse is done.
	 */
	knp = &kn[15][0];
	right ^= f(left,knp);
	knp -= 8;
	left ^= f(right,knp);
	knp -= 8;
	right ^= f(left,knp);
	knp -= 8;
	left ^= f(right,knp);
	knp -= 8;
	right ^= f(left,knp);
	knp -= 8;
	left ^= f(right,knp);
	knp -= 8;
	right ^= f(left,knp);
	knp -= 8;
	left ^= f(right,knp);
	knp -= 8;
	right ^= f(left,knp);
	knp -= 8;
	left ^= f(right,knp);
	knp -= 8;
	right ^= f(left,knp);
	knp -= 8;
	left ^= f(right,knp);
	knp -= 8;
	right ^= f(left,knp);
	knp -= 8;
	left ^= f(right,knp);
	knp -= 8;
	right ^= f(left,knp);
	knp -= 8;
	left ^= f(right,knp);

#ifdef	LITTLE_ENDIAN
	work[0] = byteswap(left);
	work[1] = byteswap(right);
#else
	work[0] = left;
	work[1] = right;
#endif
	permute((char *)work,fperm,block);	/* Inverse initial permutation */
	return 0;
}

//---------------------------------------------------------- 
// Function:   permute
// Descrption: Permute inblock with perm
// Input:      
//	       char *inblock, *outblock --- sult into 
//					    outblock,64 bits
//	       char perm[16][16][8] ---  2K bytes defining perm.
// Return:     VOID
//----------------------------------------------------------
void fEncryptTool::permute(char *inblock, char perm[16][16][8], char *outblock)
{
	register char *ib, *ob;		/* ptr to input or output block */
	register char *p, *q;
	register int j;

	if(perm == NULL)
	{
		/* No permutation, just copy */
		memcpy(outblock,inblock,8);
		return;
	}
	
	/* Clear output block */
	memset(outblock,'\0',8);

	ib = inblock;
	for (j = 0; j < 16; j += 2, ib++)
	{ /* for each input nibble */
		ob = outblock;
		p = perm[j][(*ib >> 4) & 0xf];
		q = perm[j + 1][*ib & 0xf];
		/* and each output byte, OR the masks together */
		*ob++ |= *p++ | *q++;
		*ob++ |= *p++ | *q++;
		*ob++ |= *p++ | *q++;
		*ob++ |= *p++ | *q++;
		*ob++ |= *p++ | *q++;
		*ob++ |= *p++ | *q++;
		*ob++ |= *p++ | *q++;
		*ob++ |= *p++ | *q++;
	}
}

//---------------------------------------------------------- 
// Function:   f
// Descrption: The nonlinear function f(r,k), the heart of DES
// Input:      
//             register long r --- 32 bits
//             register char *subkey --- 48-bit key for this round
// Return:     long
//----------------------------------------------------------
long fEncryptTool::f(register long r, register char *subkey)
{
	register long *spp;
	register long rval,rt;
	register int er;

#ifdef	TRACE
	//printf("f(%08lx, %02x %02x %02x %02x %02x %02x %02x %02x) = ",
	//	r,
	//	subkey[0], subkey[1], subkey[2],
	//	subkey[3], subkey[4], subkey[5],
	//	subkey[6], subkey[7]);
#endif
	/* Run E(R) ^ K through the combined S & P boxes.
	 * This code takes advantage of a convenient regularity in
	 * E, namely that each group of 6 bits in E(R) feeding
	 * a single S-box is a contiguous segment of R.
	 */
	subkey += 7;

	/* Compute E(R) for each block of 6 bits, and run thru boxes */
	er = ((int)r << 1) | ((r & 0x80000000) ? 1 : 0);
	spp = &sp[7][0];
	rval = spp[(er ^ *subkey--) & 0x3f];
	spp -= 64;
	rt = (unsigned long)r >> 3;
	rval |= spp[((int)rt ^ *subkey--) & 0x3f];
	spp -= 64;
	rt >>= 4;
	rval |= spp[((int)rt ^ *subkey--) & 0x3f];
	spp -= 64;
	rt >>= 4;
	rval |= spp[((int)rt ^ *subkey--) & 0x3f];
	spp -= 64;
	rt >>= 4;
	rval |= spp[((int)rt ^ *subkey--) & 0x3f];
	spp -= 64;
	rt >>= 4;
	rval |= spp[((int)rt ^ *subkey--) & 0x3f];
	spp -= 64;
	rt >>= 4;
	rval |= spp[((int)rt ^ *subkey--) & 0x3f];
	spp -= 64;
	rt >>= 4;
	rt |= (r & 1) << 5;
	rval |= spp[((int)rt ^ *subkey) & 0x3f];
#ifdef	TRACE
	//printf(" %08lx\n",rval);
#endif
	return rval;
}

//---------------------------------------------------------- 
// Function:   perminit
// Descrption: initialize a perm array
// Input:      
// 	       char perm[16][16][8] --- 64-bit, either init or final
// 	       char p[64] --- 
// Return:     VOID
//----------------------------------------------------------
void fEncryptTool::perminit(char perm[16][16][8], char p[64])
{
	register int l, j, k;
	int i,m;

	/* Clear the permutation array */
	memset((char *)perm,0,16*16*8);

	for (i=0; i<16; i++)		/* each input nibble position */
		for (j = 0; j < 16; j++)/* each possible input nibble */
		for (k = 0; k < 64; k++)/* each output bit position */
		{   l = p[k] - 1;	/* where does this bit come from*/
			if ((l >> 2) != i)  /* does it come from input posn?*/
			continue;	/* if not, bit k is 0	 */
			if (!(j & nibblebit[l & 3]))
			continue;	/* any such bit in input? */
			m = k & 07;	/* which bit is this in the byte*/
			perm[i][j][k>>3] |= bytebit[m];
		}
}

//---------------------------------------------------------- 
// Function:   spinit
// Descrption: Initialize the lookup table for the combined S and P boxes
// Input:      NULL
// Return:     VOID
//----------------------------------------------------------
void fEncryptTool::spinit()
{
	char pbox[32];
	int p,i,s,j,rowcol;
	long val;

	/* Compute pbox, the inverse of p32i.
	 * This is easier to work with
	 */
	for(p=0;p<32;p++){
		for(i=0;i<32;i++){
			if(p32i[i]-1 == p){
				pbox[p] = i;
				break;
			}
		}
	}
	for(s = 0; s < 8; s++){			/* For each S-box */
		for(i=0; i<64; i++){		/* For each possible input */
			val = 0;
			/* The row number is formed from the first and last
			 * bits; the column number is from the middle 4
			 */
			rowcol = (i & 32) | ((i & 1) ? 16 : 0) | ((i >> 1) & 0xf);
			for(j=0;j<4;j++){	/* For each output bit */
				if(si[s][rowcol] & (8 >> j)){
				 val |= 1L << (31 - pbox[4*s + j]);
				}
			}
			sp[s][i] = val;

#ifdef	DEBUG
			//printf("sp[%d][%2d] = %08lx\n",s,i,sp[s][i]);
#endif
		}
	}
}

#ifdef	LITTLE_ENDIAN

//---------------------------------------------------------- 
// Function:   byteswap
// Descrption: Byte swap a long
// Input:      
//             unsigned long x ---
// Return:     VOID
//----------------------------------------------------------
static long fEncryptTool::byteswap(unsigned long x)
{
	register char *cp,tmp;

	cp = (char *)&x;
	tmp = cp[3];
	cp[3] = cp[0];
	cp[0] = tmp;

	tmp = cp[2];
	cp[2] = cp[1];
	cp[1] = tmp;

	return x;
}

#endif

// the following code is added by richard zeng 2001/08/15

//----------------------------------------------------------
// Function:   chorder
// Descrption: Change string's order
// Input:
//             char *str --- inputed string
// Return:     VOID
//----------------------------------------------------------
void fEncryptTool::chorder(char* str)
{
        int i = 0;
        int len = 0;

        if (str != NULL)
        {
                len = (int)strlen(str);

                while (str[i] != '\0')
                {
                        str[i] = (str[i] + len) % 128;
                        i++;
                }
        }
}

/*
  add two string to one string
*/
char* fEncryptTool::addstr(char *str1, char *str2)
{
        char *s, *s1, *s2;
        int len1 = (int)strlen(str1);
        int len2 = (int)strlen(str2);

        char * p = (char *)malloc(len1 + len2 + 1);
        if (p != NULL)
        {
                memset(p, 0, len1 + len2 + 1);

                //--------------------------------
                /*
                memcpy(p, str1, len1);
                memcpy(p + len1, str2, len2);
                */
                // replaced by richard zeng 2001/08/16
                s = p;
                s1 = str1;
                s2 = str2;

                while (len1 > 0 && len2 > 0)
                {
                        *s++ = *s1++;
                        *s++ = *s2++;

                        len1--;
                        len2--;
                }

                if (len1 > 0)
                {
                        for ( ; len1 > 0; len1--)
                        {
                                *s++ = *s1++;
                        }
                }
                else if (len2 > 0)
                {
                        for ( ; len2 > 0; len2--)
                        {
                                *s++ = *s2++;
                        }
                }
                //--------------------------------
        }

        return p;
}

/*
  free the string's memory
*/
void fEncryptTool::freestr(char *str)
{
        if (str != NULL)
        {
                free(str);
        }
}

//----------------------------------------------------------
// Function:   doencrypt
// Descrption: Encrypt two inputed string to one string
// Input:
//             char *pszstr --- inputed strings and outputed string
// Return:     VOID
//----------------------------------------------------------
long fEncryptTool::doencrypt(char* pszstr, long nlen,char * pOut,char * pwd)
{
	char ks[16][8];
    char key[65] = PRIV_KEY;
    char *data = pszstr;
    char work[8];
    long  i, j;
	int cnt;

    if (pszstr == NULL)
    {
		//printf("the encrrypt string cann't be NULL!");
		return 0;
    }

	//change to check pwd
	if(pwd != NULL && strlen(pwd) != 0)
	{
		memset(key,0,sizeof(key));
		//memcpy(key,pwd,64);
		strncpy(key, pwd, sizeof(key)-1);
	}

    chorder(key);
    
    desinit(0);
    setKey(ks, key);

    i = j = 0;

    do
    {
        /* read 8 bytes data to the buffer */
		cnt = 0;
        memset(work, 0, 8);
        while (i < nlen && cnt < 8)
        {
            work[cnt++] = data[i++];
        }

        if (cnt != 8)
        {
            /* Put residual byte count in the last block.
             * Note that garbage is left in the other bytes,
             * if any; this is a feature, not a bug, since it'll
             * be stripped out at decrypt time.
             */
            work[7] = cnt;
        }

        /* Encrypt block */
        endes(ks, work);

        /* Save the outgoing ciphertext */
        memcpy(pOut + j, work, 8);
        j += 8;

    }while (cnt == 8);

    desdone();

	return j;
}

//----------------------------------------------------------
// Function:   dodecrypt
// Descrption: Dencrypt two inputed string to one string
// Input:
//             char *pszstr --- inputed strings and outputed string
// Return:     VOID
//----------------------------------------------------------
long fEncryptTool::dodecrypt(char* pszstr, long nlen,char * pOut,char * pwd)
{
	char ks[16][8];
    char key[65] = PRIV_KEY;
	char work[8],nwork[8];
	char* data = pszstr;
	long  i, j;
	int cnt;

	//change by Hongkun Qiao 03.11.21
	if(pwd != NULL && strlen(pwd) != 0)
	{
		memset(key,0,sizeof(key));
		//memcpy(key,pwd,64);
		strncpy(key, pwd, sizeof(key)-1);
	}
	chorder(key);
    
    desinit(0);
    setKey(ks, key);

	i = j = 0;

	/* read 8 bytes data to the buffer */
	cnt = 0;
    memset(work, 0, 8);
    while (i < nlen && cnt < 8)
    {
        work[cnt++] = data[i++];
    }

	while(1)
	{
		dedes(ks, work);
		
		/* Save buffer pending next read */
		memcpy(nwork, work, 8);

		/* read 8 bytes data to the buffer */
		cnt = 0;
		memset(work, 0, 8);
		while (i < nlen && cnt < 8)
		{
			work[cnt++] = data[i++];
		}

		if(cnt != 8)
		{	
			/* Can "only" be 0 if not 8 */
			/* Prev block was last one, write appropriate number
			 * of bytes
			 */
			cnt = nwork[7];
			if(cnt < 0 || cnt > 7)
			{
				//printf("maybe wrong key\n");
			}
			else if(cnt != 0)
			{
				memcpy(pOut + j, nwork, cnt);
				j += cnt;
			}
			break;
		} 
		else
		{
			/* Now okay to write previous buffer */
			memcpy(pOut + j, nwork, 8);
			j += 8;
		}
	}

	desdone();

	return j;
}

//---------------------------------------------------------- 
// Function:   atohex
// Descrption: Convert ascii to hex string
// Input:      
//             char *instr  --- inputed strings
//             int  inlen ---  inputed string's length
//             char *instr  --- outputed strings
//             int  *outlen ---  outputed string's length
// Return:     VOID
//----------------------------------------------------------
void fEncryptTool::atohex(char *instr, int inlen, char *outstr, int* outlen)
{
	int i = 0, j = 0;
	unsigned char szChar = 0;

	if (instr == NULL || inlen <= 0 || outstr == NULL || *outlen <= 0)
	{
		return;
	}
	
	for (i = 0, j = 0; i < inlen && j < *outlen; i++)
	{
		szChar = *((unsigned char*)(instr + i));
		if (szChar < 16)
		{
			*(outstr + j++) = '0';
			sprintf(outstr + j++, "%X", szChar);
		}
		else
		{
			sprintf(outstr + j, "%X", szChar);
			j += 2;
		}
	}
	*outlen = j;
}

//---------------------------------------------------------- 
// Function:   hextoa
// Descrption: Convert hex string to ascii
// Input:      
//             char *instr  --- inputed strings
//             int  inlen ---  inputed string's length
// Return:     VOID
//----------------------------------------------------------
unsigned char fEncryptTool::BtoH(char ch)
{
	if ((ch >= '0') && (ch <= '9'))
	{ 
		return (ch - '0');
	}
	if ((ch >= 'A') && (ch <= 'F'))
	{
		return (ch - 'A' + 0xA);
	}
	if ((ch >= 'a') && (ch <= 'f'))
	{
		return (ch - 'a' + 0xA);
	}

	return ch;
}

void fEncryptTool::hextoa(char *instr, int inlen, char *outstr, int *outlen)
{
	int i = 0, j = 0;

	if (instr == NULL || inlen <= 0 || outstr == NULL || *outlen <= 0)
	{
		return;
	}
	
	for (i = 0; i < inlen && j < *outlen; j++)
	{
		outstr[j] = (BtoH(*instr++) << 4);
		outstr[j] += BtoH(*instr++);
		i += 2;
	}
	*outlen = j;
}
