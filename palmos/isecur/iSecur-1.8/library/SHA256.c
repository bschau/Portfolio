/*
 * SHA256.c
 */
#include <SystemMgr.h>
#include "iSecurLib.h"

/* macros */
#define ToUInt32(src, idx) \
	(((UInt32)(src)[(idx)]<<24) \
	|((UInt32)(src)[(idx)+1]<<16) \
	|((UInt32)(src)[(idx)+2]<<8) \
	|((UInt32)(src)[(idx)+3]))

#define Put32(src, dst, idx) \
	(dst)[(idx)]=(UInt8)((src)>>24); \
	(dst)[(idx)+1]=(UInt8)((src)>>16); \
	(dst)[(idx)+2]=(UInt8)((src)>>8); \
	(dst)[(idx)+3]=(UInt8)(src);

#define SHR(x, n) ((x&0xffffffff)>>n)

#define ROTR(x, n) (((x&0xffffffff)>>n)|(x<<(32-n)))

#define S0(x) (ROTR(x,7)^ROTR(x,18)^ SHR(x,3))
#define S1(x) (ROTR(x,17)^ROTR(x,19)^ SHR(x,10))
#define S2(x) (ROTR(x,2)^ROTR(x,13)^ROTR(x,22))
#define S3(x) (ROTR(x,6)^ROTR(x,11)^ROTR(x,25))

#define F0(x, y, z) ((x&y)|(z&(x|y)))
#define F1(x, y, z) (z^(x&(y^z)))

#define R(n) (w[n]=S1(w[n-2])+w[n-7]+S0(w[n-15])+w[n-16])

#define P(x, k, a, b, c, d, e, f, g, h) \
	t1=h+S3(e)+F1(e, f, g)+k+x; \
	t2=S2(a)+F0(a, b, c); \
	d+=t1; \
	h=t1+t2;

/* structs */
typedef struct {
	UInt8 digest[SHA256DigestLength];
	UInt32 state[8];
	UInt32 cnt[2];
	UInt8 buffer[64];
	UInt8 padding[64];
} SHA256Lib;

/* protos */
static void Transform(SHA256Lib *, UInt8 *);

/*
 * iSecurLibSHA256Open
 *
 * Opens the SHA256 engine.
 *
 *  -> refNum		Ref num.
 *
 * Returns SHA256Lib or NULL.
 */
MemHandle
iSecurLibSHA256Open(UInt16 refNum)
{
	MemHandle mh=MemHandleNew(sizeof(SHA256Lib));

	if (mh) {
		SHA256Lib *lib=MemHandleLock(mh);
		MemSet(lib, sizeof(SHA256Lib), 0);
		lib->padding[0]=0x80;

		MemHandleUnlock(mh);
	}

	return mh;
}

/*
 * iSecurLibSHA256Close
 *
 * Close the SHA256 engine.
 *
 *  -> refNum		Ref num.
 *  -> library		SHA256Lib.
 */
void
iSecurLibSHA256Close(UInt16 refNum, MemHandle library)
{
	if (library) {
		SHA256Lib *lib=MemHandleLock(library);

		MemSet(lib, sizeof(SHA256Lib), 0);
		MemHandleFree(library);
	}
}

/*
 * iSecurLibSHA256Init
 *
 * Initialize context.
 *
 *  -> refNum		Ref num.
 *  -> library		SHA256Lib.
 */
void
iSecurLibSHA256Init(UInt16 refNum, MemHandle library)
{
	UInt32 seeds[8]={ 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };
	SHA256Lib *lib=MemHandleLock(library);

	MemSet(lib->digest, SHA256DigestLength, 0);

	lib->cnt[0]=0;
	lib->cnt[1]=0;

	MemMove(&lib->state, seeds, 8*sizeof(UInt32));
	MemHandleUnlock(library);
}

/*
 * Transform
 *
 * Transform 512 bits.
 *
 *  -> lib		Locked library.
 *  -> src		Source data.
 */
static void
Transform(SHA256Lib *lib, UInt8 *src)
{
	UInt16 idx=0, i;
	UInt32 w[64];
	UInt32 t1, t2, a, b, c, d, e, f, g, h;

	for (i=0; i<16; i++) {
		w[i]=ToUInt32(src, idx);
		idx+=4;
	}

	a=lib->state[0];
	b=lib->state[1];
	c=lib->state[2];
	d=lib->state[3];
	e=lib->state[4];
	f=lib->state[5];
	g=lib->state[6];
	h=lib->state[7];

	P(w[0], 0x428a2f98, a, b, c, d, e, f, g, h);
	P(w[1], 0x71374491, h, a, b, c, d, e, f, g);
	P(w[2], 0xb5c0fbcf, g, h, a, b, c, d, e, f);
	P(w[3], 0xe9b5dba5, f, g, h, a, b, c, d, e);
	P(w[4], 0x3956c25b, e, f, g, h, a, b, c, d);
	P(w[5], 0x59f111f1, d, e, f, g, h, a, b, c);
	P(w[6], 0x923f82a4, c, d, e, f, g, h, a, b);
	P(w[7], 0xab1c5ed5, b, c, d, e, f, g, h, a);
	P(w[8], 0xd807aa98, a, b, c, d, e, f, g, h);
	P(w[9], 0x12835b01, h, a, b, c, d, e, f, g);
	P(w[10], 0x243185be, g, h, a, b, c, d, e, f);
	P(w[11], 0x550c7dc3, f, g, h, a, b, c, d, e);
	P(w[12], 0x72be5d74, e, f, g, h, a, b, c, d);
	P(w[13], 0x80deb1fe, d, e, f, g, h, a, b, c);
	P(w[14], 0x9bdc06a7, c, d, e, f, g, h, a, b);
	P(w[15], 0xc19bf174, b, c, d, e, f, g, h, a);
	P(R(16), 0xe49b69c1, a, b, c, d, e, f, g, h);
	P(R(17), 0xefbe4786, h, a, b, c, d, e, f, g);
	P(R(18), 0x0fc19dc6, g, h, a, b, c, d, e, f);
	P(R(19), 0x240ca1cc, f, g, h, a, b, c, d, e);
	P(R(20), 0x2de92c6f, e, f, g, h, a, b, c, d);
	P(R(21), 0x4a7484aa, d, e, f, g, h, a, b, c);
	P(R(22), 0x5cb0a9dc, c, d, e, f, g, h, a, b);
	P(R(23), 0x76f988da, b, c, d, e, f, g, h, a);
	P(R(24), 0x983e5152, a, b, c, d, e, f, g, h);
	P(R(25), 0xa831c66d, h, a, b, c, d, e, f, g);
	P(R(26), 0xb00327c8, g, h, a, b, c, d, e, f);
	P(R(27), 0xbf597fc7, f, g, h, a, b, c, d, e);
	P(R(28), 0xc6e00bf3, e, f, g, h, a, b, c, d);
	P(R(29), 0xd5a79147, d, e, f, g, h, a, b, c);
	P(R(30), 0x06ca6351, c, d, e, f, g, h, a, b);
	P(R(31), 0x14292967, b, c, d, e, f, g, h, a);
	P(R(32), 0x27b70a85, a, b, c, d, e, f, g, h);
	P(R(33), 0x2e1b2138, h, a, b, c, d, e, f, g);
	P(R(34), 0x4d2c6dfc, g, h, a, b, c, d, e, f);
	P(R(35), 0x53380d13, f, g, h, a, b, c, d, e);
	P(R(36), 0x650a7354, e, f, g, h, a, b, c, d);
	P(R(37), 0x766a0abb, d, e, f, g, h, a, b, c);
	P(R(38), 0x81c2c92e, c, d, e, f, g, h, a, b);
	P(R(39), 0x92722c85, b, c, d, e, f, g, h, a);
	P(R(40), 0xa2bfe8a1, a, b, c, d, e, f, g, h);
	P(R(41), 0xa81a664b, h, a, b, c, d, e, f, g);
	P(R(42), 0xc24b8b70, g, h, a, b, c, d, e, f);
	P(R(43), 0xc76c51a3, f, g, h, a, b, c, d, e);
	P(R(44), 0xd192e819, e, f, g, h, a, b, c, d);
	P(R(45), 0xd6990624, d, e, f, g, h, a, b, c);
	P(R(46), 0xf40e3585, c, d, e, f, g, h, a, b);
	P(R(47), 0x106aa070, b, c, d, e, f, g, h, a);
	P(R(48), 0x19a4c116, a, b, c, d, e, f, g, h);
	P(R(49), 0x1e376c08, h, a, b, c, d, e, f, g);
	P(R(50), 0x2748774c, g, h, a, b, c, d, e, f);
	P(R(51), 0x34b0bcb5, f, g, h, a, b, c, d, e);
	P(R(52), 0x391c0cb3, e, f, g, h, a, b, c, d);
	P(R(53), 0x4ed8aa4a, d, e, f, g, h, a, b, c);
	P(R(54), 0x5b9cca4f, c, d, e, f, g, h, a, b);
	P(R(55), 0x682e6ff3, b, c, d, e, f, g, h, a);
	P(R(56), 0x748f82ee, a, b, c, d, e, f, g, h);
	P(R(57), 0x78a5636f, h, a, b, c, d, e, f, g);
	P(R(58), 0x84c87814, g, h, a, b, c, d, e, f);
	P(R(59), 0x8cc70208, f, g, h, a, b, c, d, e);
	P(R(60), 0x90befffa, e, f, g, h, a, b, c, d);
	P(R(61), 0xa4506ceb, d, e, f, g, h, a, b, c);
	P(R(62), 0xbef9a3f7, c, d, e, f, g, h, a, b);
	P(R(63), 0xc67178f2, b, c, d, e, f, g, h, a);

	lib->state[0]+=a;
	lib->state[1]+=b;
	lib->state[2]+=c;
	lib->state[3]+=d;
	lib->state[4]+=e;
	lib->state[5]+=f;
	lib->state[6]+=g;
	lib->state[7]+=h;
}

/*
 * iSecurLibSHA256Transform
 *
 * Transform a block of data.
 *
 *  -> refNum		Ref num.
 *  -> library		SHA256Lib.
 *  -> src		Source data.
 *  -> srcLen		Source length.
 */
void
iSecurLibSHA256Transform(UInt16 refNum, MemHandle library, UInt8 *src, UInt32 srcLen)
{
	SHA256Lib *lib;
	UInt32 l, f;

	if (!srcLen)
		return;

	lib=MemHandleLock(library);
	l=lib->cnt[0]&0x3f;
	f=64-l;

	lib->cnt[0]+=srcLen;
	lib->cnt[0]&=0xffffffff;

	if (lib->cnt[0]<srcLen)
		lib->cnt[1]++;

	if ((l) && (srcLen>=f)) {
		MemMove(lib->buffer+l, src, f);
		Transform(lib, lib->buffer);
		src+=f;
		srcLen-=f;
		l=0;
	}

	while (srcLen>=64) {
		Transform(lib, src);
		src+=64;
		srcLen-=64;
	}

	if (srcLen)
		MemMove(lib->buffer+l, src, srcLen);

	MemHandleUnlock(library);
}

/*
 * iSecurLibSHA256Final
 *
 * Finalize digest.
 *
 *  -> refNum		Ref num.
 *  -> library		SHA256Lib.
 *
 * The digest will be stored in lib->digest.
 */
void
iSecurLibSHA256Final(UInt16 refNum, MemHandle library)
{
	SHA256Lib *lib=MemHandleLock(library);
	UInt32 hi, lo, last, pad;
	UInt8 msgLen[8];

	hi=((lib->cnt[0]>>29)|(lib->cnt[1]<<3));
	lo=lib->cnt[0]<<3;

	Put32(hi, msgLen, 0);
	Put32(lo, msgLen, 4);

	last=lib->cnt[0]&0x3f;
	pad=(last<56) ? (56-last) : (120-last);

	iSecurLibSHA256Transform(refNum, library, lib->padding, pad);
	iSecurLibSHA256Transform(refNum, library, msgLen, 8);

	Put32(lib->state[0], lib->digest, 0);
	Put32(lib->state[1], lib->digest, 4);
	Put32(lib->state[2], lib->digest, 8);
	Put32(lib->state[3], lib->digest, 12);
	Put32(lib->state[4], lib->digest, 16);
	Put32(lib->state[5], lib->digest, 20);
	Put32(lib->state[6], lib->digest, 24);
	Put32(lib->state[7], lib->digest, 28);

	MemHandleUnlock(library);
}
