/*
 * Entropy.c
 */
#include "Include.h"

/*
 * EntropyOpen
 *
 * Open the entropy pool.
 *
 * Returns pointer to pool.
 */
UInt8 *
EntropyOpen(void)
{
	MemHandle mh=MemHandleNew(EntropyPoolLength);
	UInt8 *p, *pool;

	ErrFatalDisplayIf(mh==NULL, "(EntropyOpen) Out of memory");
	pool=MemHandleLock(mh);
	p=pool;

	if (PMGetPref(pool, EntropyPoolLength, PrfEntropy, false)==false) {
		UInt16 cnt;

		Random(TimGetSeconds());
		for (cnt=0; cnt<EntropyPoolLength; cnt++)
			*p++=(UInt8)(Random(0)&0xff);
	}

	return pool;
}

/*
 * EntropyClose
 *
 * Close the entropy pool.
 *
 *  -> pool		Entropy pool.
 */
void
EntropyClose(UInt8 *pool)
{
	if (pool) {
		PMSetPref(pool, EntropyPoolLength, PrfEntropy, false);

		MemPtrFree(pool);
	}
}

/*
 * EntropyAdd
 *
 * Add event to entropy pool.
 *
 *  -> pool		Entropy pool.
 *  -> ev		EventType.
 */
void
EntropyAdd(UInt8 *pool, EventType *ev)
{
	UInt8 f=*pool;
	UInt16 sIdx=TimGetSeconds()%EntropyPoolLength, dIdx=Random(0)%EntropyPoolLength;

	MemMove(pool, pool+1, EntropyPoolLength-1);
	*(pool+EntropyPoolLength-1)=f+(UInt8)((TimGetSeconds()+Random(0))&0xff);

	pool[sIdx]^=ev->screenY;
	pool[dIdx]+=ev->screenX+ev->tapCount+(ev->penDown ? ev->eType : ev->screenY);
	pool[(sIdx+dIdx)%EntropyPoolLength]=pool[sIdx]^pool[dIdx]^TimGetTicks();
}

/*
 * EntropyGet
 *
 * Get a byte from the entropy pool.
 *
 *  -> pool		Entropy pool.
 */
UInt8
EntropyGet(UInt8 *pool)
{
	UInt8 c=*pool;

	MemMove(pool, pool+1, EntropyPoolLength-1);
	*(pool+EntropyPoolLength-1)=c+(UInt8)((TimGetSeconds()^Random(0))&0xff);

	return c;
}
