/*
 * SHA256.c
 */
#include "Include.h"

/* protos */
static void Transform(SHA256 *, UInt8 *);

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

/* globals */
static UInt8 padding[64]={
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * SHA256Init
 *
 * Initialize context.
 *
 *  -> ctx		SHA256 Context.
 */
void
SHA256Init(SHA256 *ctx)
{
	UInt32 seeds[8]={ 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };

	MemSet(&ctx->cnt, 2*sizeof(UInt32), 0);
	MemMove(&ctx->state, seeds, 8*sizeof(UInt32));
}

/*
 * Transform
 *
 * Transform 512 bits.
 *
 *  -> ctx		SHA256 Context.
 *  -> src		Source data.
 */
static void
Transform(SHA256 *ctx, UInt8 *src)
{
	UInt16 idx=0, i;
	UInt32 w[64];
	UInt32 t1, t2, a, b, c, d, e, f, g, h;

	for (i=0; i<16; i++) {
		w[i]=ToUInt32(src, idx);
		idx+=4;
	}

	a=ctx->state[0];
	b=ctx->state[1];
	c=ctx->state[2];
	d=ctx->state[3];
	e=ctx->state[4];
	f=ctx->state[5];
	g=ctx->state[6];
	h=ctx->state[7];

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

	ctx->state[0]+=a;
	ctx->state[1]+=b;
	ctx->state[2]+=c;
	ctx->state[3]+=d;
	ctx->state[4]+=e;
	ctx->state[5]+=f;
	ctx->state[6]+=g;
	ctx->state[7]+=h;
}

/*
 * SHA256Transform
 *
 * Transform a block of data.
 *
 *  -> ctx		SHA256 Context.
 *  -> src		Source data.
 *  -> srcLen		Source length.
 */
void
SHA256Transform(SHA256 *ctx, UInt8 *src, UInt32 srcLen)
{
	UInt32 l, f;

	if (!srcLen)
		return;

	l=ctx->cnt[0]&0x3f;
	f=64-l;

	ctx->cnt[0]+=srcLen;
	ctx->cnt[0]&=0xffffffff;

	if (ctx->cnt[0]<srcLen)
		ctx->cnt[1]++;

	if ((l) && (srcLen>=f)) {
		MemMove(ctx->buffer+l, src, f);
		Transform(ctx, ctx->buffer);
		src+=f;
		srcLen-=f;
		l=0;
	}

	while (srcLen>=64) {
		Transform(ctx, src);
		src+=64;
		srcLen-=64;
	}

	if (srcLen)
		MemMove(ctx->buffer+l, src, srcLen);
}

/*
 * SHA256Final
 *
 * Finalize digest.
 *
 *  -> ctx		SHA256 Context.
 *
 * The digest will be stored in ctx->digest.
 */
void
SHA256Final(SHA256 *ctx)
{
	UInt32 hi=((ctx->cnt[0]>>29)|(ctx->cnt[1]<<3)), lo=ctx->cnt[0]<<3;
	UInt32 last, pad;
	UInt8 msgLen[8];

	Put32(hi, msgLen, 0);
	Put32(lo, msgLen, 4);

	last=ctx->cnt[0]&0x3f;
	pad=(last<56) ? (56-last) : (120-last);

	SHA256Transform(ctx, padding, pad);
	SHA256Transform(ctx, msgLen, 8);

	Put32(ctx->state[0], ctx->digest, 0);
	Put32(ctx->state[1], ctx->digest, 4);
	Put32(ctx->state[2], ctx->digest, 8);
	Put32(ctx->state[3], ctx->digest, 12);
	Put32(ctx->state[4], ctx->digest, 16);
	Put32(ctx->state[5], ctx->digest, 20);
	Put32(ctx->state[6], ctx->digest, 24);
	Put32(ctx->state[7], ctx->digest, 28);
}
