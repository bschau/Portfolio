/*
 * Session.c
 */
#include "Include.h"

/* macros */
#define SizeEasyLevel 41	// base 2-8 and 10 (8), exp 2 - 6 (5) +1
#define SizeHardLevel 64	// base 4-9 11-12 (8), exp 5 - 12 (8)

/* protos */
static void Shuffle(UInt8 *, UInt16);

/* globals */
static MemHandle elH, hlH, tblH;
static UInt8 *el, *hl, *tbl;
static Char strQuiz[30];
static Session s;

/*
 * Shuffle
 *
 * Shuffle the level data.
 *
 *  -> lvl		Level data.
 *  -> len		Length of level data.
 */
static void
Shuffle(UInt8 *lvl, UInt16 len)
{
	UInt16 idx, pos;
	UInt8 b;

	for (idx=0; idx<(len-1); idx++) {
		pos=SysRandom(0)%(len-idx);
		b=lvl[idx];
		lvl[idx]=lvl[pos];
		lvl[pos]=b;
	}
}

/*
 * SssInit
 *
 * Initialize session data.
 */
void
SssInit(void)
{
	UInt16 idx;

	elH=MemHandleNew(SizeEasyLevel);
	el=MemHandleLock(elH);
	ErrFatalDisplayIf(elH==NULL, "(SssInit) Out of memory");
	hlH=MemHandleNew(SizeHardLevel);
	hl=MemHandleLock(hlH);
	ErrFatalDisplayIf(hlH==NULL, "(SssInit) Out of memory");

	if (PMGetPref(el, SizeEasyLevel, PrfEasyLevel, false)==false) {
		UInt8 *p=el;
		UInt16 b, e;

		*p++=(1<<4|1);

		for (b=2; b<9; b++) {
			for (e=2; e<7; e++)
				*p++=(b<<4)|e;
		}

		for (e=2; e<7; e++)
			*p++=(10<<4)|e;

		DPrint("EasyLevel:\n");
		D(HexDump(el, SizeEasyLevel));
	}

	for (idx=0; idx<SizeEasyLevel; idx++) {
		if ((el[idx]&0x0f)==1) {
			el[idx]=((SysRandom(0)%8)+1)<<1;
			break;
		}
	}

	if (PMGetPref(hl, SizeHardLevel, PrfHardLevel, false)==false) {
		UInt8 *p=hl;
		UInt16 b, e;

		for (b=4; b<13; b++) {
			if (b==10)
				continue;

			for (e=5; e<13; e++)
				*p++=(b<<4)|e;
		}

		DPrint("HardLevel:\n");
		D(HexDump(hl, SizeHardLevel));
	}

	tblH=DmGetResource((UInt32)'PPLY', BaseExpTableID);
	ErrFatalDisplayIf(tblH==NULL, "(SssInit) Base/Exponent table not found");
	tbl=MemHandleLock(tblH);
}

/*
 * SssFree
 *
 * Close session data.
 */
void
SssFree(void)
{
	MemHandleUnlock(tblH);
	DmReleaseResource(tblH);

	MemHandleFree(elH);
	MemHandleFree(hlH);
}

/*
 * SssGet
 *
 * Open or create a new session.
 */
void
SssGet(void)
{
	if (PMGetPref(&s, sizeof(Session), PrfSession, false)==true)
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
	Shuffle(el, SizeEasyLevel);
	PMSetPref(el, SizeEasyLevel, PrfEasyLevel, false);
	DPrint("EasyLevel:\n");
	D(HexDump(el, SizeEasyLevel));

	Shuffle(hl, SizeHardLevel);
	PMSetPref(hl, SizeHardLevel, PrfHardLevel, false);
	DPrint("HardLevel:\n");
	D(HexDump(hl, SizeHardLevel));

	MemSet(&s, sizeof(Session), 0);
	if (prefs.flags&PFlgLevel)
		s.quizIdx=SysRandom(0)%SizeHardLevel;
	else
		s.quizIdx=SysRandom(0)%SizeEasyLevel;

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

	s.quizIdx+=(SysRandom(0)%6)+1;
	if (prefs.flags&PFlgLevel) {
		if (s.quizIdx>=SizeHardLevel)
			s.quizIdx-=SizeHardLevel;

		s.quiz=hl[s.quizIdx];
	} else {
		if (s.quizIdx>=SizeEasyLevel)
			s.quizIdx-=SizeEasyLevel;

		s.quiz=el[s.quizIdx];
	}

	DPrint("Session Quiz: %u\n", (UInt16)s.quiz);
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
		PMSetPref(&s, sizeof(Session), PrfSession, false);
	else
		PMSetPref(NULL, 0, PrfSession, false);
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

/*
 * SssQuizToAscii
 *
 * Convert the quiz answer to ascii.
 *
 * Return string.
 */
Char *
SssQuizToAscii(void)
{
	UInt8 *t=tbl;
	Char sep;

	switch (PrefGetPreference(prefNumberFormat)) {
	case nfPeriodComma:
		sep='.';
		break;
	case nfSpaceComma:
		sep=' ';
		break;
	case nfApostrophePeriod:
	case nfApostropheComma:
		sep='\'';
		break;
	default:
		sep=',';
		break;
	}

	for (; *t; ) {
		if (*t==s.quiz) {
			Int16 idx=0, dIdx=sizeof(strQuiz)-1, m=3;
			UInt8 b, c;

			for (t++; *t; t++) {
				b=*t;
				c=(b>>4);
				if (!c)
					break;

				strQuiz[idx++]=(c-5)+'0';

				c=b&0x0f;
				if (!c)
					break;
				
				strQuiz[idx++]=(c-5)+'0';
			}

			if (!idx)
				break;

			if (idx<4) {
				strQuiz[idx]=0;
				return strQuiz;
			}

			strQuiz[dIdx]=0;
			for (idx--; idx>=0; idx--) {
				if (m==0) {
					m=3;
					strQuiz[--dIdx]=sep;
				}

				strQuiz[--dIdx]=strQuiz[idx];
				m--;
			}

			return strQuiz+dIdx;
		}

		for (; *t; t++);
		t++;
	}

	/* should never happen ... */
	strQuiz[0]='0';
	strQuiz[1]=0;

	return strQuiz;
}
