/*
 * AES.c
 */
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
	x0=fKey[0]^fwdTab0[(UInt8)(y0>>24)] ^ \
		   fwdTab1[(UInt8)(y1>>16)] ^ \
		   fwdTab2[(UInt8)(y2>>8)] ^ \
		   fwdTab3[(UInt8)(y3)]; \
	x1=fKey[1]^fwdTab0[(UInt8)(y1>>24)] ^ \
		   fwdTab1[(UInt8)(y2>>16)] ^ \
		   fwdTab2[(UInt8)(y3>>8)] ^ \
		   fwdTab3[(UInt8)(y0)]; \
	x2=fKey[2]^fwdTab0[(UInt8)(y2>>24)] ^ \
		   fwdTab1[(UInt8)(y3>>16)] ^ \
		   fwdTab2[(UInt8)(y0>>8)] ^ \
		   fwdTab3[(UInt8)(y1)]; \
	x3=fKey[3]^fwdTab0[(UInt8)(y3>>24)] ^ \
		   fwdTab1[(UInt8)(y0>>16)] ^ \
		   fwdTab2[(UInt8)(y1>>8)] ^ \
		   fwdTab3[(UInt8)(y2)];

#define R(x0, x1, x2, x3, y0, y1, y2, y3) \
	rKey+=4; \
	x0=rKey[0]^rvsTab0[(UInt8)(y0>>24)] ^ \
		   rvsTab1[(UInt8)(y3>>16)] ^ \
		   rvsTab2[(UInt8)(y2>>8)] ^ \
		   rvsTab3[(UInt8)(y1)]; \
	x1=rKey[1]^rvsTab0[(UInt8)(y1>>24)] ^ \
		   rvsTab1[(UInt8)(y0>>16)] ^ \
		   rvsTab2[(UInt8)(y3>>8)] ^ \
		   rvsTab3[(UInt8)(y2)]; \
	x2=rKey[2]^rvsTab0[(UInt8)(y2>>24)] ^ \
		   rvsTab1[(UInt8)(y1>>16)] ^ \
		   rvsTab2[(UInt8)(y0>>8)] ^ \
		   rvsTab3[(UInt8)(y3)]; \
	x3=rKey[3]^rvsTab0[(UInt8)(y3>>24)] ^ \
		   rvsTab1[(UInt8)(y2>>16)] ^ \
		   rvsTab2[(UInt8)(y1>>8)] ^ \
		   rvsTab3[(UInt8)(y0)];

/* globals */
static const UInt32 roundConstants[10]={
	0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000,
	0x20000000, 0x40000000, 0x80000000, 0x1b000000, 0x36000000
};
static UInt32 *fwdSbox, *fwdTab0, *fwdTab1, *fwdTab2, *fwdTab3;
static UInt32 *rvsSbox, *rvsTab0, *rvsTab1, *rvsTab2, *rvsTab3;
static struct {
	UInt16 id;
	MemHandle mh;
	UInt32 **dst;
} resTable[]={
	{ AESFwdSBox, NULL, &fwdSbox },
	{ AESFwdTab0, NULL, &fwdTab0 },
	{ AESFwdTab1, NULL, &fwdTab1 },
	{ AESFwdTab2, NULL, &fwdTab2 },
	{ AESFwdTab3, NULL, &fwdTab3 },
	{ AESRvsSBox, NULL, &rvsSbox },
	{ AESRvsTab0, NULL, &rvsTab0 },
	{ AESRvsTab1, NULL, &rvsTab1 },
	{ AESRvsTab2, NULL, &rvsTab2 },
	{ AESRvsTab3, NULL, &rvsTab3 },
	{ 0 },
};

/*
 * AESOpen
 *
 * Open the AES engine.
 */
void
AESOpen(void)
{
	UInt16 idx;

	for (idx=0; resTable[idx].id; idx++) {
		resTable[idx].mh=DmGetResource((UInt32)'AESx', resTable[idx].id);
		if (resTable[idx].mh==NULL) {
			Char buffer[64];

			StrPrintF(buffer, "(AESOpen) Failed to find resource %u", resTable[idx].id);
			ErrDisplayFileLineMsg(__FILE__, __LINE__, buffer);
			SysReset();
		}

		*(resTable[idx].dst)=MemHandleLock(resTable[idx].mh);
	}
}

/*
 * AESClose
 *
 * Close the AES engine.
 */
void
AESClose(void)
{
	UInt16 idx;

	for (idx=0; resTable[idx].id; idx++) {
		if (resTable[idx].mh) {
			MemHandleUnlock(resTable[idx].mh);
			DmReleaseResource(resTable[idx].mh);
		}
	}
}

/*
 * AESCreateKeys
 *
 * Create the AES encryption and decryption keys.
 *
 *  -> ks		AESKeys
 *  -> key		Key.
 */
void
AESCreateKeys(AESKeys *ks, UInt8 *key)
{
	UInt32 *fKey=ks->encKey, *rKey=ks->decKey;
	UInt16 idx;
	MemHandle mh;
	UInt32 *k0, *k1, *k2, *k3;

	for (idx=0; idx<8; idx++)
		fKey[idx]=ToUInt32(key, (idx*4));

	for (idx=0; idx<7; idx++) {
		fKey[8]=fKey[0]^roundConstants[idx]^
			(fwdSbox[(UInt8)(fKey[7]>>16)]<<24) ^
			(fwdSbox[(UInt8)(fKey[7]>>8)]<<16) ^
			(fwdSbox[(UInt8)(fKey[7])]<<8) ^
			(fwdSbox[(UInt8)(fKey[7]>>24)]);

		fKey[9]=fKey[1]^fKey[8];
		fKey[10]=fKey[2]^fKey[9];
		fKey[11]=fKey[3]^fKey[10];

		fKey[12]=fKey[4]^
			(fwdSbox[(UInt8)(fKey[11]>>24)]<<24) ^
			(fwdSbox[(UInt8)(fKey[11]>>16)]<<16) ^
			(fwdSbox[(UInt8)(fKey[11]>>8)]<<8) ^
			(fwdSbox[(UInt8)(fKey[11])]);

		fKey[13]=fKey[5]^fKey[12];
		fKey[14]=fKey[6]^fKey[13];
		fKey[15]=fKey[7]^fKey[14];

		fKey+=8;
	}

	mh=MemHandleNew(sizeof(UInt32)*256*4);
	ErrFatalDisplayIf(mh==NULL, "(AESCreateKeys) Out of memory");
	k0=MemHandleLock(mh);
	k1=k0+256;
	k2=k1+256;
	k3=k2+256;

	for (idx=0; idx<256; idx++) {
		k0[idx]=rvsTab0[fwdSbox[idx]];
		k1[idx]=rvsTab1[fwdSbox[idx]];
		k2[idx]=rvsTab2[fwdSbox[idx]];
		k3[idx]=rvsTab3[fwdSbox[idx]];
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
}

/*
 * AESEncrypt
 *
 * Encrypt 128 bits.
 *
 *  -> ks		AESKeys
 *  -> src		Source input (128 bits).
 *  -> dst		Destination output (128 bits).
 *
 * Note. src and dst may point to the same area.
 */
void
AESEncrypt(AESKeys *ks, UInt8 *src, UInt8 *dst)
{
	UInt32 *fKey=ks->encKey, x0, x1, x2, x3, y0, y1, y2, y3;

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
	x0=fKey[0]^(fwdSbox[(UInt8)(y0>>24)]<<24) ^
		   (fwdSbox[(UInt8)(y1>>16)]<<16) ^
		   (fwdSbox[(UInt8)(y2>>8)]<<8) ^
		   (fwdSbox[(UInt8)(y3)]);

	x1=fKey[1]^(fwdSbox[(UInt8)(y1>>24)]<<24) ^
		   (fwdSbox[(UInt8)(y2>>16)]<<16) ^
		   (fwdSbox[(UInt8)(y3>>8)]<<8) ^
		   (fwdSbox[(UInt8)(y0)]);

	x2=fKey[2]^(fwdSbox[(UInt8)(y2>>24)]<<24) ^
		   (fwdSbox[(UInt8)(y3>>16)]<<16) ^
		   (fwdSbox[(UInt8)(y0>>8)]<<8) ^
		   (fwdSbox[(UInt8)(y1)]);

	x3=fKey[3]^(fwdSbox[(UInt8)(y3>>24)]<<24) ^
		   (fwdSbox[(UInt8)(y0>>16)]<<16) ^
		   (fwdSbox[(UInt8)(y1>>8)]<<8) ^
		   (fwdSbox[(UInt8)(y2)]);

	Put32(x0, dst, 0);
	Put32(x1, dst, 4);
	Put32(x2, dst, 8);
	Put32(x3, dst, 12);
}

/*
 * AESDecrypt
 *
 * Decrypt 128 bits.
 *
 *  -> ks		AESKeys
 *  -> src		Source input (128 bits).
 *  -> dst		Destination output (128 bits).
 *
 * Note. src and dst may point to the same area.
 */
void
AESDecrypt(AESKeys *ks, UInt8 *src, UInt8 *dst)
{
	UInt32 *rKey=ks->decKey, x0, x1, x2, x3, y0, y1, y2, y3;

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
	x0=rKey[0]^(rvsSbox[(UInt8)(y0>>24)]<<24) ^
		   (rvsSbox[(UInt8)(y3>>16)]<<16) ^
		   (rvsSbox[(UInt8)(y2>>8)]<<8) ^
		   (rvsSbox[(UInt8)(y1)]);

	x1=rKey[1]^(rvsSbox[(UInt8)(y1>>24)]<<24) ^
		   (rvsSbox[(UInt8)(y0>>16)]<<16) ^
		   (rvsSbox[(UInt8)(y3>>8)]<<8) ^
		   (rvsSbox[(UInt8)(y2)]);

	x2=rKey[2]^(rvsSbox[(UInt8)(y2>>24)]<<24) ^
		   (rvsSbox[(UInt8)(y1>>16)]<<16) ^
		   (rvsSbox[(UInt8)(y0>>8)]<<8) ^
		   (rvsSbox[(UInt8)(y3)]);

	x3=rKey[3]^(rvsSbox[(UInt8)(y3>>24)]<<24) ^
		   (rvsSbox[(UInt8)(y2>>16)]<<16) ^
		   (rvsSbox[(UInt8)(y1>>8)]<<8) ^
		   (rvsSbox[(UInt8)(y0)]);

	Put32(x0, dst, 0);
	Put32(x1, dst, 4);
	Put32(x2, dst, 8);
	Put32(x3, dst, 12);
}
