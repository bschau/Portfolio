/*
 * Session.c
 */
#include "Include.h"

/* protos */
static Int16 NumCmp(void *, void *, Int32);

/* globals */
static UInt16 maxToNum[]={ 10, 12, 15, 20, 25, 50, 100, 200 };
static Session s;

/*
 * NumCmp
 */
static Int16
NumCmp(void *u1, void *u2, Int32 unused)
{
	if (*(UInt16 *)u1>*(UInt16 *)u2)
		return 1;

	if (*(UInt16 *)u1<*(UInt16 *)u2)
		return -1;

	return 0;
}

/*
 * SssGet
 *
 * Open or create a new session.
 */
void
SssGet(void)
{
	if (PMGetPref(&s, sizeof(Session), PrfSession)==true)
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

	s.started=TimGetSeconds();
	s.stopped=0;
	s.timedSecs=prefs.timedMins*60;
	s.defOperations=prefs.operation;
	s.max=maxToNum[prefs.maxOperand];

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
	s.round++;
	s.score+=p;

	s.failed=false;
	if ((s.defOperations&PrfQuizTypeRandom)==PrfQuizTypeRandom)
		s.operation=(UInt16)(SysRandom(0)%2)+1;
	else
		s.operation=s.defOperations&PrfQuizTypeRandom;

	if (s.operation&PrfQuizTypeLcm) {
		UInt16 tries;

		for (tries=0; tries<100; tries++) {
			do {
				s.nums[0]=(UInt16)SysRandom(0)%s.max;
			} while (s.nums[0]<2);

			do {
				s.nums[1]=(UInt16)SysRandom(0)%s.max;
			} while (s.nums[1]<2 || s.nums[1]==s.nums[0]);

			do {
				s.nums[2]=(UInt16)SysRandom(0)%s.max;
			} while (s.nums[2]<2 || s.nums[2]==s.nums[1] || s.nums[2]==s.nums[0]);
			SysQSort(&s.nums, 3, sizeof(UInt16), NumCmp, 0);

			if ((s.prevNums[0]!=s.nums[0]) || (s.prevNums[1]!=s.nums[1]) || (s.prevNums[2]!=s.nums[2]))
				break;
		} 
	} else {
		UInt16 y=s.max-(s.max/4), tries, f;

		for (tries=0; tries<100; tries++) {
			do {
				f=(UInt16)SysRandom(0)%y;
			} while (f<4);

			if (s.defOperations&PrfNumbers) {
				s.nums[0]=f/2;
				s.nums[1]=f;
				s.nums[2]=f+(f/2);
			} else {
				s.nums[0]=f;
				s.nums[1]=f+(f/2);
				s.nums[2]=0;
			}

			if ((s.prevNums[0]!=s.nums[0]) || (s.prevNums[1]!=s.nums[1]) || (s.prevNums[2]!=s.nums[2]))
				break;
		}
	}

	s.prevNums[0]=s.nums[0];
	s.prevNums[1]=s.nums[1];
	s.prevNums[2]=s.nums[2];
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
		PMSetPref(&s, sizeof(Session), PrfSession);
	else
		PMSetPref(NULL, 0, PrfSession);
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
