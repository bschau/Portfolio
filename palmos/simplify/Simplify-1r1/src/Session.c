/*
 * Session.c
 */
#include "Include.h"

/* protos */
static UInt16 GetBoundedRandom(UInt16);

/* globals */
static UInt16 denominators[]={ 20, 100, 200, 1000 };
static UInt16 range[]={ 5, 10, 20, 100 };
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
		r=(UInt16)((((UInt32)SysRandom(0))*x)/(((UInt32)sysRandomMax)+1));
		if (r>0 && r<(x+1))
			return r;
	}

	return (x/2);
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
	s.round=0;
	s.score=0;
	s.numerator=0;
	s.denominator=0;
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
	UInt16 denom=denominators[level], rng=range[level];
	UInt16 lN=s.numerator, lD=s.denominator, cnt=0, oCnt=0, r;

	s.round++;
	s.score+=p;

	do {
		oCnt++;

		switch (GetBoundedRandom(10)) {
		case 1:
		case 3:
		case 5:
		case 7:
			r=GetBoundedRandom(denom/2);
			s.numerator=denom/r;
			s.denominator=s.numerator*r;
			break;
		case 2:
		case 4:
		case 6:
		case 8:
			do {
				r=GetBoundedRandom(denom/rng);
				cnt++;
			} while ((r==1) && (cnt<5));

			cnt=0;
			do {
				s.numerator=GetBoundedRandom(rng)*r;
				s.denominator=GetBoundedRandom(rng)*r;
				cnt++;
			} while ((s.numerator==s.denominator) && (cnt<5));
			break;
		default:
			do {
				s.numerator=GetBoundedRandom(denom);
				s.denominator=GetBoundedRandom(denom);
				cnt++;
			} while ((s.numerator==s.denominator) && (cnt<5));
			break;
		}

		if (s.numerator>s.denominator) {
			if (GetBoundedRandom(10)<8) {
				r=s.denominator;
				s.denominator=s.numerator;
				s.numerator=r;
			}
		}
	} while (s.numerator==lN && s.denominator==lD && oCnt<5);
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
