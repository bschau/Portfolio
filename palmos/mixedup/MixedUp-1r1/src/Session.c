/*
 * Session.c
 */
#include "Include.h"

/* protos */
static UInt16 GetBoundedRandom(UInt16);

/* globals */
static UInt16 denominators[]={ 10, 20, 100, 200 };
static Session s;

/*
 * GetBoundedRandom
 *
 * Return a number between 1 and x.
 *
 *  -> x	Max.
 */
static UInt16 GetBoundedRandom(UInt16 x)
{
	UInt16 i, r;

	for (i=0; i<5; i++) {
		r=(UInt16)((((UInt32)SysRandom(0))*x)/(((UInt32)sysRandomMax)));
		if (r<x)
			return (r+1);
	}

	return (x/2)+1;
}

/*
 * SssGet
 *
 * Open or create a new session.
 */
void
SssGet(void)
{
	if (UtilsGetPref(&s, sizeof(Session), PrfSession)==true)
		session=&s;
	else
		session=NULL;
}

/*
 * SssNew
 *
 * Create a new session.
 */
void
SssNew(void)
{
	MemSet(&s, sizeof(Session), 0);
	SssUpdate(0);

	session=&s;
}

/*
 * SssUpdate
 *
 * Update session.
 *
 *  -> p	Points.
 */
void
SssUpdate(UInt16 p)
{
	UInt16 denom=denominators[level&LvlDenomMask], w;

	s.round++;
	s.score+=p;
	s.guesses=0;

	s.whole=GetBoundedRandom(5);
	if (GetBoundedRandom(10)>7)
		s.whole+=GetBoundedRandom(5);	

	s.wNum=GetBoundedRandom(denom-1)+1;
	s.wDenom=GetBoundedRandom(denom-1)+1;
	if (s.wNum==s.wDenom)
		s.wDenom+=GetBoundedRandom(5);

	if (s.wNum>s.wDenom) {
		w=s.wNum;
		s.wNum=s.wDenom;
		s.wDenom=w;
	}

	UtilsReduce(&s.wNum, &s.wDenom);
	s.numerator=(s.whole*s.wDenom)+s.wNum;
	s.denominator=s.wDenom;

	if (level&LvlMixed) {
		// Given fraction, find mixed number.
		if (GetBoundedRandom(10)>5) {
			w=GetBoundedRandom(5);
			s.numerator*=w;
			s.denominator*=w;
		}

		if (level&LvlAdvanced)
			s.mask=1+2;
		else
			s.mask=(1+2+4+8+16)&mask[GetBoundedRandom(3)+1];
	} else {
		// Given mixed number, find fraction
		if (level&LvlAdvanced)
			s.mask=4+8+16;
		else
			s.mask=2+4+8+16;
	}
}

/*
 * SssSet
 *
 * Save session.
 */
void
SssSet(void)
{
	if (session)
		UtilsSetPref(&s, sizeof(Session), PrfSession);
	else
		UtilsSetPref(NULL, 0, PrfSession);
}

/*
 * SssDestroy
 *
 * Destroy session.
 */
void
SssDestroy(void)
{
	session=NULL;
	SssSet();
}
