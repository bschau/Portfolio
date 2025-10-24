/*
 * AES.c
 */
#include <PalmOS.h>
#include "iSecurLib.h"
#include "Include.h"

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

#define F(x0, x1, x2, x3, y0, y1, y2, y3) \
	fKey+=4; \
	x0=fKey[0]^lib->fwdTab0[(UInt8)(y0>>24)] ^ \
		   lib->fwdTab1[(UInt8)(y1>>16)] ^ \
		   lib->fwdTab2[(UInt8)(y2>>8)] ^ \
		   lib->fwdTab3[(UInt8)(y3)]; \
	x1=fKey[1]^lib->fwdTab0[(UInt8)(y1>>24)] ^ \
		   lib->fwdTab1[(UInt8)(y2>>16)] ^ \
		   lib->fwdTab2[(UInt8)(y3>>8)] ^ \
		   lib->fwdTab3[(UInt8)(y0)]; \
	x2=fKey[2]^lib->fwdTab0[(UInt8)(y2>>24)] ^ \
		   lib->fwdTab1[(UInt8)(y3>>16)] ^ \
		   lib->fwdTab2[(UInt8)(y0>>8)] ^ \
		   lib->fwdTab3[(UInt8)(y1)]; \
	x3=fKey[3]^lib->fwdTab0[(UInt8)(y3>>24)] ^ \
		   lib->fwdTab1[(UInt8)(y0>>16)] ^ \
		   lib->fwdTab2[(UInt8)(y1>>8)] ^ \
		   lib->fwdTab3[(UInt8)(y2)];

#define R(x0, x1, x2, x3, y0, y1, y2, y3) \
	rKey+=4; \
	x0=rKey[0]^lib->rvsTab0[(UInt8)(y0>>24)] ^ \
		   lib->rvsTab1[(UInt8)(y3>>16)] ^ \
		   lib->rvsTab2[(UInt8)(y2>>8)] ^ \
		   lib->rvsTab3[(UInt8)(y1)]; \
	x1=rKey[1]^lib->rvsTab0[(UInt8)(y1>>24)] ^ \
		   lib->rvsTab1[(UInt8)(y0>>16)] ^ \
		   lib->rvsTab2[(UInt8)(y3>>8)] ^ \
		   lib->rvsTab3[(UInt8)(y2)]; \
	x2=rKey[2]^lib->rvsTab0[(UInt8)(y2>>24)] ^ \
		   lib->rvsTab1[(UInt8)(y1>>16)] ^ \
		   lib->rvsTab2[(UInt8)(y0>>8)] ^ \
		   lib->rvsTab3[(UInt8)(y3)]; \
	x3=rKey[3]^lib->rvsTab0[(UInt8)(y3>>24)] ^ \
		   lib->rvsTab1[(UInt8)(y2>>16)] ^ \
		   lib->rvsTab2[(UInt8)(y1>>8)] ^ \
		   lib->rvsTab3[(UInt8)(y0)];

/* structs */
typedef struct {
	UInt32 *fwdSbox;
	UInt32 *rvsSbox;
	UInt32 *fwdTab0;
	UInt32 *fwdTab1;
	UInt32 *fwdTab2;
	UInt32 *fwdTab3;
	UInt32 *rvsTab0;
	UInt32 *rvsTab1;
	UInt32 *rvsTab2;
	UInt32 *rvsTab3;
	UInt32 encKey[64];
	UInt32 decKey[64];
} AESLib;

/*
 * iSecurLibAESOpen
 *
 * Open the AES engine.
 *
 *  -> refNum		Ref num.
 *
 * Returns handle to AESLib or NULL.
 */
MemHandle
iSecurLibAESOpen(UInt16 refNum)
{
	iSecurLibGlobals *g=LockGlobals(refNum);
	MemHandle mh=MemHandleNew(sizeof(AESLib));

	if (mh) {
		AESLib *lib=MemHandleLock(mh);

		MemSet(lib, sizeof(AESLib), 0);

		lib->fwdSbox=MemHandleLock(g->mhs[0]);
		lib->rvsSbox=MemHandleLock(g->mhs[1]);
		lib->fwdTab0=MemHandleLock(g->mhs[2]);
		lib->fwdTab1=MemHandleLock(g->mhs[3]);
		lib->fwdTab2=MemHandleLock(g->mhs[4]);
		lib->fwdTab3=MemHandleLock(g->mhs[5]);
		lib->rvsTab0=MemHandleLock(g->mhs[6]);
		lib->rvsTab1=MemHandleLock(g->mhs[7]);
		lib->rvsTab2=MemHandleLock(g->mhs[8]);
		lib->rvsTab3=MemHandleLock(g->mhs[9]);
		MemHandleUnlock(mh);
	}

	MemPtrUnlock(g);
	return mh;
}

/*
 * iSecurLibAESClose
 *
 * Close the AES engine.
 *
 *  -> refNum		Ref num.
 *  -> library		AESLib.
 */
void
iSecurLibAESClose(UInt16 refNum, MemHandle library)
{
	if (library) {
		iSecurLibGlobals *g=LockGlobals(refNum);
		AESLib *lib=MemHandleLock(library);
		UInt16 idx;

		for (idx=0; idx<10; idx++) {
			if (g->mhs[idx])
				MemHandleUnlock(g->mhs[idx]);
		}

		MemSet(lib->encKey, 64*sizeof(UInt32), 0);
		MemSet(lib->decKey, 64*sizeof(UInt32), 0);
		MemHandleFree(library);
		MemPtrUnlock(g);
	}
}

/*
 * iSecurLibAESCreateKeys
 *
 * Create the AES encryption and decryption keys.
 *
 *  -> refNum		Ref num.
 *  -> library		AESLib.
 *  -> key		Key.
 */
void
iSecurLibAESCreateKeys(UInt16 refNum, MemHandle library, UInt8 *key)
{
	UInt32 roundConstants[10]={
		0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000,
		0x20000000, 0x40000000, 0x80000000, 0x1b000000, 0x36000000
	};
	AESLib *lib=MemHandleLock(library);
	UInt32 *fKey, *rKey, *k0, *k1, *k2, *k3;
	UInt16 idx;
	MemHandle mh;

	fKey=lib->encKey;
	rKey=lib->decKey;

	for (idx=0; idx<8; idx++)
		fKey[idx]=ToUInt32(key, (idx*4));

	for (idx=0; idx<7; idx++) {
		fKey[8]=fKey[0]^roundConstants[idx]^
			(lib->fwdSbox[(UInt8)(fKey[7]>>16)]<<24) ^
			(lib->fwdSbox[(UInt8)(fKey[7]>>8)]<<16) ^
			(lib->fwdSbox[(UInt8)(fKey[7])]<<8) ^
			(lib->fwdSbox[(UInt8)(fKey[7]>>24)]);

		fKey[9]=fKey[1]^fKey[8];
		fKey[10]=fKey[2]^fKey[9];
		fKey[11]=fKey[3]^fKey[10];

		fKey[12]=fKey[4]^
			(lib->fwdSbox[(UInt8)(fKey[11]>>24)]<<24) ^
			(lib->fwdSbox[(UInt8)(fKey[11]>>16)]<<16) ^
			(lib->fwdSbox[(UInt8)(fKey[11]>>8)]<<8) ^
			(lib->fwdSbox[(UInt8)(fKey[11])]);

		fKey[13]=fKey[5]^fKey[12];
		fKey[14]=fKey[6]^fKey[13];
		fKey[15]=fKey[7]^fKey[14];

		fKey+=8;
	}

	mh=MemHandleNew(sizeof(UInt32)*256*4);
	ErrFatalDisplayIf(mh==NULL, "(iSecurLibAESCreateKeys) Out of memory");

	k0=MemHandleLock(mh);
	k1=k0+256;
	k2=k1+256;
	k3=k2+256;

	for (idx=0; idx<256; idx++) {
		k0[idx]=lib->rvsTab0[lib->fwdSbox[idx]];
		k1[idx]=lib->rvsTab1[lib->fwdSbox[idx]];
		k2[idx]=lib->rvsTab2[lib->fwdSbox[idx]];
		k3[idx]=lib->rvsTab3[lib->fwdSbox[idx]];
	}

	*rKey++=*fKey++;
	*rKey++=*fKey++;
	*rKey++=*fKey++;
	*rKey++=*fKey++;

	for (idx=1; idx<14; idx++) {
		fKey-=8;

		*rKey++=k0[(UInt8)(*fKey>>24)] ^
			k1[(UInt8)(*fKey>>16)] ^
			k2[(UInt8)(*fKey>>8)] ^
			k3[(UInt8)(*fKey)];

		fKey++;

		*rKey++=k0[(UInt8)(*fKey>>24)] ^
			k1[(UInt8)(*fKey>>16)] ^
			k2[(UInt8)(*fKey>>8)] ^
			k3[(UInt8)(*fKey)];

		fKey++;

		*rKey++=k0[(UInt8)(*fKey>>24)] ^
			k1[(UInt8)(*fKey>>16)] ^
			k2[(UInt8)(*fKey>>8)] ^
			k3[(UInt8)(*fKey)];

		fKey++;

		*rKey++=k0[(UInt8)(*fKey>>24)] ^
			k1[(UInt8)(*fKey>>16)] ^
			k2[(UInt8)(*fKey>>8)] ^
			k3[(UInt8)(*fKey)];

		fKey++;
	}

	fKey-=8;

	*rKey++=*fKey++;
	*rKey++=*fKey++;
	*rKey++=*fKey++;
	*rKey++=*fKey++;

	MemHandleFree(mh);
	MemHandleUnlock(library);
}

/*
 * iSecurLibAESEncrypt
 *
 * Encrypt 128 bits.
 *
 *  -> refNum		Ref num.
 *  -> library		AESLib.
 *  -> src		Source input (128 bits).
 *  -> dst		Destination output (128 bits).
 *
 * Note. src and dst may point to the same area.
 */
void
iSecurLibAESEncrypt(UInt16 refNum, MemHandle library, UInt8 *src, UInt8 *dst)
{
	AESLib *lib=MemHandleLock(library);
	UInt32 *fKey, x0, x1, x2, x3, y0, y1, y2, y3;

	fKey=lib->encKey;

	x0=(ToUInt32(src, 0))^fKey[0];
	x1=(ToUInt32(src, 4))^fKey[1];
	x2=(ToUInt32(src, 8))^fKey[2];
	x3=(ToUInt32(src, 12))^fKey[3];

	F(y0, y1, y2, y3, x0, x1, x2, x3);
	F(x0, x1, x2, x3, y0, y1, y2, y3);
	F(y0, y1, y2, y3, x0, x1, x2, x3);
	F(x0, x1, x2, x3, y0, y1, y2, y3);
	F(y0, y1, y2, y3, x0, x1, x2, x3);
	F(x0, x1, x2, x3, y0, y1, y2, y3);
	F(y0, y1, y2, y3, x0, x1, x2, x3);
	F(x0, x1, x2, x3, y0, y1, y2, y3);
	F(y0, y1, y2, y3, x0, x1, x2, x3);
	F(x0, x1, x2, x3, y0, y1, y2, y3);
	F(y0, y1, y2, y3, x0, x1, x2, x3);
	F(x0, x1, x2, x3, y0, y1, y2, y3);
	F(y0, y1, y2, y3, x0, x1, x2, x3);

	fKey+=4;
	x0=fKey[0]^(lib->fwdSbox[(UInt8)(y0>>24)]<<24) ^
		   (lib->fwdSbox[(UInt8)(y1>>16)]<<16) ^
		   (lib->fwdSbox[(UInt8)(y2>>8)]<<8) ^
		   (lib->fwdSbox[(UInt8)(y3)]);

	x1=fKey[1]^(lib->fwdSbox[(UInt8)(y1>>24)]<<24) ^
		   (lib->fwdSbox[(UInt8)(y2>>16)]<<16) ^
		   (lib->fwdSbox[(UInt8)(y3>>8)]<<8) ^
		   (lib->fwdSbox[(UInt8)(y0)]);

	x2=fKey[2]^(lib->fwdSbox[(UInt8)(y2>>24)]<<24) ^
		   (lib->fwdSbox[(UInt8)(y3>>16)]<<16) ^
		   (lib->fwdSbox[(UInt8)(y0>>8)]<<8) ^
		   (lib->fwdSbox[(UInt8)(y1)]);

	x3=fKey[3]^(lib->fwdSbox[(UInt8)(y3>>24)]<<24) ^
		   (lib->fwdSbox[(UInt8)(y0>>16)]<<16) ^
		   (lib->fwdSbox[(UInt8)(y1>>8)]<<8) ^
		   (lib->fwdSbox[(UInt8)(y2)]);

	Put32(x0, dst, 0);
	Put32(x1, dst, 4);
	Put32(x2, dst, 8);
	Put32(x3, dst, 12);
	MemHandleUnlock(library);
}

/*
 * iSecurLibAESDecrypt
 *
 * Decrypt 128 bits.
 *
 *  -> refNum		Ref num.
 *  -> library		AESLib.
 *  -> src		Source input (128 bits).
 *  -> dst		Destination output (128 bits).
 *
 * Note. src and dst may point to the same area.
 */
void
iSecurLibAESDecrypt(UInt16 refNum, MemHandle library, UInt8 *src, UInt8 *dst)
{
	AESLib *lib=MemHandleLock(library);
	UInt32 *rKey, x0, x1, x2, x3, y0, y1, y2, y3;

	rKey=lib->decKey;
	x0=(ToUInt32(src, 0))^rKey[0];
	x1=(ToUInt32(src, 4))^rKey[1];
	x2=(ToUInt32(src, 8))^rKey[2];
	x3=(ToUInt32(src, 12))^rKey[3];

	R(y0, y1, y2, y3, x0, x1, x2, x3);
	R(x0, x1, x2, x3, y0, y1, y2, y3);
	R(y0, y1, y2, y3, x0, x1, x2, x3);
	R(x0, x1, x2, x3, y0, y1, y2, y3);
	R(y0, y1, y2, y3, x0, x1, x2, x3);
	R(x0, x1, x2, x3, y0, y1, y2, y3);
	R(y0, y1, y2, y3, x0, x1, x2, x3);
	R(x0, x1, x2, x3, y0, y1, y2, y3);
	R(y0, y1, y2, y3, x0, x1, x2, x3);
	R(x0, x1, x2, x3, y0, y1, y2, y3);
	R(y0, y1, y2, y3, x0, x1, x2, x3);
	R(x0, x1, x2, x3, y0, y1, y2, y3);
	R(y0, y1, y2, y3, x0, x1, x2, x3);

	rKey+=4;
	x0=rKey[0]^(lib->rvsSbox[(UInt8)(y0>>24)]<<24) ^
		   (lib->rvsSbox[(UInt8)(y3>>16)]<<16) ^
		   (lib->rvsSbox[(UInt8)(y2>>8)]<<8) ^
		   (lib->rvsSbox[(UInt8)(y1)]);

	x1=rKey[1]^(lib->rvsSbox[(UInt8)(y1>>24)]<<24) ^
		   (lib->rvsSbox[(UInt8)(y0>>16)]<<16) ^
		   (lib->rvsSbox[(UInt8)(y3>>8)]<<8) ^
		   (lib->rvsSbox[(UInt8)(y2)]);

	x2=rKey[2]^(lib->rvsSbox[(UInt8)(y2>>24)]<<24) ^
		   (lib->rvsSbox[(UInt8)(y1>>16)]<<16) ^
		   (lib->rvsSbox[(UInt8)(y0>>8)]<<8) ^
		   (lib->rvsSbox[(UInt8)(y3)]);

	x3=rKey[3]^(lib->rvsSbox[(UInt8)(y3>>24)]<<24) ^
		   (lib->rvsSbox[(UInt8)(y2>>16)]<<16) ^
		   (lib->rvsSbox[(UInt8)(y1>>8)]<<8) ^
		   (lib->rvsSbox[(UInt8)(y0)]);

	Put32(x0, dst, 0);
	Put32(x1, dst, 4);
	Put32(x2, dst, 8);
	Put32(x3, dst, 12);
	MemHandleUnlock(library);
}
