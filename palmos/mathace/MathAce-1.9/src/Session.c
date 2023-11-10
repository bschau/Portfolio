/*
 * Session.c
 */
#include "Include.h"

/* protos */
static Int32 GetRandom(UInt16);

/* globals */
static Session s;

/*
 * GetRandom
 *
 * Return a number x>=0.
 *
 *  -> x	Max.
 */
static Int32
GetRandom(UInt16 x)
{
	if (s.defOperations&PrfOperationSigned)
		return (((Int32)(SysRandom(0)%(x+x))-(Int32)x));

	return ((Int32)(SysRandom(0)%x));
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
	s.min=prefs.minOperand;
	s.defOperations=prefs.operation;
	if (s.defOperations&PrfMaxType)
		s.max=prefs.maxOperand;
	else
		s.max=prefs.maxAnswer;

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
	UInt16 bufIdx=0, rand=4, cnt, max;
	UInt16 buf[4];

	s.round++;
	s.score+=p;

	s.failed=false;
	if (s.defOperations&PrfOperationAdd)
		buf[bufIdx++]=PrfOperationAdd;

	if (s.defOperations&PrfOperationSub)
		buf[bufIdx++]=PrfOperationSub;

	if ((s.defOperations&(PrfOperationMul|PrfMaxType))==(PrfOperationMul|PrfMaxType))
		buf[bufIdx++]=PrfOperationMul;

	if ((s.defOperations&(PrfOperationDiv|PrfMaxType))==(PrfOperationDiv|PrfMaxType))
		buf[bufIdx++]=PrfOperationDiv;

	if (!bufIdx)
		buf[bufIdx++]=PrfOperationAdd;

	if (!(s.defOperations&PrfMaxType))
		rand=(bufIdx<2) ? bufIdx : 2;
	else
		rand=bufIdx;

	s.operation=buf[SysRandom(0)%rand];

	s.mask=SMlhs1|SMlhs2;
	if (!(s.defOperations&PrfQuizType)) {
		s.mask|=SMrhs;

		switch (SysRandom(0)%((s.defOperations&PrfSolveType) ? 3 : 2)) {
		case 0:
			s.mask&=~SMlhs1;
			break;
		case 1:
			s.mask&=~SMlhs2;
			break;
		default:
			s.mask&=~SMrhs;
			break;
		}
	}

	if (!s.max)
		s.max=10;

	if (s.min>s.max)
		s.min=0;

	if (s.min)
		max=s.max-s.min;
	else
		max=s.max;

	if (s.operation==PrfOperationDiv) {
		Int32 t;

		s.lhs1=GetRandom(max)+s.min;
		s.lhs2=0;
		for (cnt=0; cnt<5; cnt++) {
			if (s.lhs2)
				break;

			s.lhs2=GetRandom(max)+s.min;
		}

		if (!s.lhs2)
			s.lhs2=s.max/2;

		s.rhs=s.lhs1*s.lhs2;

		t=s.lhs1;
		s.lhs1=s.rhs;
		s.rhs=t;
	} else {
		switch (s.operation) {
		case PrfOperationAdd:
			if (!(s.defOperations&PrfMaxType)) {
				s.lhs1=GetRandom(max)+s.min;
				s.lhs2=0;
				for (cnt=0; cnt<5; cnt++) {
					if (s.lhs2)
						break;

					s.lhs2=GetRandom(max-s.lhs1)+s.min;
				}
				if (!s.lhs2)
					s.lhs2=(max-s.lhs1)/2;
			} else {
				s.lhs1=GetRandom(max)+s.min;
				s.lhs2=GetRandom(max)+s.min;
			}
			s.rhs=s.lhs1+s.lhs2;
			break;
		case PrfOperationSub:
			if (!(s.defOperations&PrfMaxType)) {
				s.lhs1=GetRandom(max)+s.min;
				s.lhs2=0;
				for (cnt=0; cnt<5; cnt++) {
					if (s.lhs2)
						break;

					s.lhs2=GetRandom(max-s.lhs1)+s.min;
				}
				if (!s.lhs2)
					s.lhs2=(max-s.lhs1)/2;
			} else {
				s.lhs1=GetRandom(max)+s.min;
				s.lhs2=GetRandom(max)+s.min;
			}

			if (!(s.defOperations&PrfOperationSigned)) {
				if ((s.lhs1-s.lhs2)<0) {
					Int32 t=s.lhs1;

					s.lhs1=s.lhs2;
					s.lhs2=t;
				}
			}
			s.rhs=s.lhs1-s.lhs2;
			break;
		default:
			s.lhs1=GetRandom(max)+s.min;
			s.lhs2=GetRandom(max)+s.min;

			s.rhs=s.lhs1*s.lhs2;
			break;
		}
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
