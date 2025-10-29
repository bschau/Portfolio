#include "sit.h"

/* Constants for MD5Transform routine. */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

/* protos */
#define RP(rn) static void rn(unsigned long *a, unsigned long b, unsigned long c, unsigned long d, unsigned long x, unsigned short s, unsigned long ac)
RP(round1);
RP(round2);
RP(round3);
RP(round4);

/*******************************************************************************
 *
 *	rounds
 *
 *	Calculations done in the various rounds.
 */
RP(round1)
{
	*a+=((b&c)|(~b&d))+x+ac;
	*a=(*a<<s)|(*a>>(32-s));
	*a+=b;
}

RP(round2)
{
	*a+=((b&d)|(c&~d))+x+ac;
	*a=(*a<<s)|(*a>>(32-s));
	*a+=b;
}

RP(round3)
{
	*a+=(b^c^d)+x+ac;
	*a=(*a<<s)|(*a>>(32-s));
	*a+=b;
}

RP(round4)
{
	*a+=(c^(b|~d))+x+ac;
	*a=(*a<<s)|(*a>>(32-s));
	*a+=b;
}

/*******************************************************************************
 *
 *	md5
 *
 *	Calculate md5 sum for max 256 bits.
 *
 *	Input:
 *		dst - where to store md5sum (4 unsigned longs).
 *		src - source to calculate md5 sum for.
 */
void
md5(unsigned long *dst,
    unsigned char *src)
{
	unsigned long a=0x67452301, b=0xefcdab89, c=0x98badcfe, d=0x10325476, i, j;
	unsigned long x[16];
	unsigned char buffer[64];

	dst[0]=a;
	dst[1]=b;
	dst[2]=c;
	dst[3]=d;

	/* prepare buffer */
	MemSet(buffer, 64, 0);	/* PalmOS assbackwards */
	StrNCopy(buffer, src, 32);

	/* step 1 - pad: this is easy, as we do not have more than one buffer
	   full of data.    So we can cheat. */
	i=StrLen(buffer);
	buffer[i]='\x80';

	/* step 2 - pad length: cheating again.  The length must be expressed
	   as a quadword (64 bits), low order word first, starting at bit 448.
	   The highorder word will _always_ be 0.    The low order word is
	   represented with low order octet first. */
	i<<=3;
	buffer[56]=i&0xff;
	buffer[57]=(i>>8)&0xff;
	buffer[58]=(i>>16)&0xff;
	buffer[59]=(i>>24)&0xff;

	/* prepare workspace */
	i=0;
	for (j=0; j<64; j+=4) {
		x[i]=(unsigned long)buffer[j];
		x[i]|=((unsigned long)buffer[j+1])<<8;
		x[i]|=((unsigned long)buffer[j+2])<<16;
		x[i]|=((unsigned long)buffer[j+3])<<24;
		i++;
	}

	/* zap buffer */
	MemSet(buffer, 64, 0);

	/* rounds ... */
	round1(&a, b, c, d, x[0], S11, 0xd76aa478);	/* 1 */
	round1(&d, a, b, c, x[1], S12, 0xe8c7b756);	/* 2 */
	round1(&c, d, a, b, x[2], S13, 0x242070db);	/* 3 */
	round1(&b, c, d, a, x[3], S14, 0xc1bdceee);	/* 4 */
	round1(&a, b, c, d, x[4], S11, 0xf57c0faf);	/* 5 */
	round1(&d, a, b, c, x[5], S12, 0x4787c62a);	/* 6 */
	round1(&c, d, a, b, x[6], S13, 0xa8304613);	/* 7 */
	round1(&b, c, d, a, x[7], S14, 0xfd469501);	/* 8 */
	round1(&a, b, c, d, x[8], S11, 0x698098d8);	/* 9 */
	round1(&d, a, b, c, x[9], S12, 0x8b44f7af);	/* 10 */
	round1(&c, d, a, b, x[10], S13, 0xffff5bb1);	/* 11 */
	round1(&b, c, d, a, x[11], S14, 0x895cd7be);	/* 12 */
	round1(&a, b, c, d, x[12], S11, 0x6b901122);	/* 13 */
	round1(&d, a, b, c, x[13], S12, 0xfd987193);	/* 14 */
	round1(&c, d, a, b, x[14], S13, 0xa679438e);	/* 15 */
	round1(&b, c, d, a, x[15], S14, 0x49b40821);	/* 16 */

	round2(&a, b, c, d, x[1], S21, 0xf61e2562);	/* 17 */
	round2(&d, a, b, c, x[6], S22, 0xc040b340);	/* 18 */
	round2(&c, d, a, b, x[11], S23, 0x265e5a51);	/* 19 */
	round2(&b, c, d, a, x[0], S24, 0xe9b6c7aa);	/* 20 */
	round2(&a, b, c, d, x[5], S21, 0xd62f105d);	/* 21 */
	round2(&d, a, b, c, x[10], S22,  0x2441453);	/* 22 */
	round2(&c, d, a, b, x[15], S23, 0xd8a1e681);	/* 23 */
	round2(&b, c, d, a, x[4], S24, 0xe7d3fbc8);	/* 24 */
	round2(&a, b, c, d, x[9], S21, 0x21e1cde6);	/* 25 */
	round2(&d, a, b, c, x[14], S22, 0xc33707d6);	/* 26 */
	round2(&c, d, a, b, x[3], S23, 0xf4d50d87);	/* 27 */
	round2(&b, c, d, a, x[8], S24, 0x455a14ed);	/* 28 */
	round2(&a, b, c, d, x[13], S21, 0xa9e3e905);	/* 29 */
	round2(&d, a, b, c, x[2], S22, 0xfcefa3f8);	/* 30 */
	round2(&c, d, a, b, x[7], S23, 0x676f02d9);	/* 31 */
	round2(&b, c, d, a, x[12], S24, 0x8d2a4c8a);	/* 32 */

	round3(&a, b, c, d, x[5], S31, 0xfffa3942);	/* 33 */
	round3(&d, a, b, c, x[8], S32, 0x8771f681);	/* 34 */
	round3(&c, d, a, b, x[11], S33, 0x6d9d6122);	/* 35 */
	round3(&b, c, d, a, x[14], S34, 0xfde5380c);	/* 36 */
	round3(&a, b, c, d, x[1], S31, 0xa4beea44);	/* 37 */
	round3(&d, a, b, c, x[4], S32, 0x4bdecfa9);	/* 38 */
	round3(&c, d, a, b, x[7], S33, 0xf6bb4b60);	/* 39 */
	round3(&b, c, d, a, x[10], S34, 0xbebfbc70);	/* 40 */
	round3(&a, b, c, d, x[13], S31, 0x289b7ec6);	/* 41 */
	round3(&d, a, b, c, x[0], S32, 0xeaa127fa);	/* 42 */
	round3(&c, d, a, b, x[3], S33, 0xd4ef3085);	/* 43 */
	round3(&b, c, d, a, x[6], S34,  0x4881d05);	/* 44 */
	round3(&a, b, c, d, x[9], S31, 0xd9d4d039);	/* 45 */
	round3(&d, a, b, c, x[12], S32, 0xe6db99e5);	/* 46 */
	round3(&c, d, a, b, x[15], S33, 0x1fa27cf8);	/* 47 */
	round3(&b, c, d, a, x[2], S34, 0xc4ac5665);	/* 48 */

	round4(&a, b, c, d, x[0], S41, 0xf4292244);	/* 49 */
	round4(&d, a, b, c, x[7], S42, 0x432aff97);	/* 50 */
	round4(&c, d, a, b, x[14], S43, 0xab9423a7);	/* 51 */
	round4(&b, c, d, a, x[5], S44, 0xfc93a039);	/* 52 */
	round4(&a, b, c, d, x[12], S41, 0x655b59c3);	/* 53 */
	round4(&d, a, b, c, x[3], S42, 0x8f0ccc92);	/* 54 */
	round4(&c, d, a, b, x[10], S43, 0xffeff47d);	/* 55 */
	round4(&b, c, d, a, x[1], S44, 0x85845dd1);	/* 56 */
	round4(&a, b, c, d, x[8], S41, 0x6fa87e4f);	/* 57 */
	round4(&d, a, b, c, x[15], S42, 0xfe2ce6e0);	/* 58 */
	round4(&c, d, a, b, x[6], S43, 0xa3014314);	/* 59 */
	round4(&b, c, d, a, x[13], S44, 0x4e0811a1);	/* 60 */
	round4(&a, b, c, d, x[4], S41, 0xf7537e82);	/* 61 */
	round4(&d, a, b, c, x[11], S42, 0xbd3af235);	/* 62 */
	round4(&c, d, a, b, x[2], S43, 0x2ad7d2bb);	/* 63 */
	round4(&b, c, d, a, x[9], S44, 0xeb86d391);	/* 64 */

	dst[0]+=a;
	dst[1]+=b;
	dst[2]+=c;
	dst[3]+=d;
}
