#include "p0ls.h"

/* macros */
#define NUMRULES 12
#define MAX(x, y) ((x>=y) ? x : y)
#define MIN(x, y) ((x<=y) ? x : y)
#define SCRX(x) (Coord)(scale*(x-xmin))
#define SCRY(y) (Coord)(scale*(y-ymin))

/* globals */
static Char ruf[PRODRULELEN+2], rlf[PRODRULELEN+2], r0[PRODRULELEN+2];
static Char r1[PRODRULELEN+2], r2[PRODRULELEN+2], r3[PRODRULELEN+2];
static Char r4[PRODRULELEN+2], r5[PRODRULELEN+2], r6[PRODRULELEN+2];
static Char r7[PRODRULELEN+2], r8[PRODRULELEN+2], r9[PRODRULELEN+2];
static Char *rulearr[NUMRULES]={ r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, ruf, rlf };
static Char *exsrc;
static UInt16 running;
static UInt32 curstrlen, stackidx;
static float turx, tury, xmin, xmax, ymin, ymax;
static Int16 turdir, turstep;
static MemHandle stackh;
static stackentry *stackp;
extern float costab[], sintab[];

/* protos */
static Int16 compile(Char *, Char *);
static Int16 valrule(Char *, Char *, Char *);
static Int16 rulecheck(void);
static Int16 turtlebrain(Char);
 
/*******************************************************************************
*
*	dbloader
*
*	Load database example.
*
*	Input:
*		dbkey - key.
*		dbd - depth.
*		dba - angle.
*		dbax - axiom.
*		dbuf - F.
*		dblf - f.
*		dbr0 - rule 0.
*		dbr1 - rule 1.
*		dbr2 - rule 2.
*		dbr3 - rule 3.
*		dbr4 - rule 4.
*/
void
dbloader(Char *dbkey,
         Int16 dbd,
         Int16 dba,
         Char *dbax,
         Char *dbuf,
         Char *dblf,
         Char *dbr0,
         Char *dbr1,
         Char *dbr2,
         Char *dbr3,
         Char *dbr4)
{
	UInt16 recindex=dmMaxRecordIndex;
	MemHandle rec;
	unsigned char *kptr;
	Char key[KEYSIZE+2];

	clearvars();

	StrCopy(axiom, dbax);
	if (dbuf)
		StrCopy(ruleuf, dbuf);

	if (dblf)
		StrCopy(rulelf, dblf);

	if (dbr0)
		StrCopy(rule0, dbr0);

	if (dbr1)
		StrCopy(rule1, dbr1);

	if (dbr2)
		StrCopy(rule2, dbr2);

	if (dbr3)
		StrCopy(rule3, dbr3);

	if (dbr4)
		StrCopy(rule4, dbr4);

	normalizekey(key, dbkey);
	iopack(key, dbd, dba);

	rec=DmNewRecord(gdb, &recindex, StrLen(grec+INTOFF)+INTOFF+1);
	if (rec) {
		kptr=MemHandleLock(rec);
		DmWrite(kptr, 0, grec, StrLen(grec+INTOFF)+INTOFF+1);
		MemHandleUnlock(rec);
		DmReleaseRecord(gdb, recindex, true);
	}
}

/*******************************************************************************
*
*	clearvars
*
*	Clear all variables.
*/
void
clearvars(void)
{
	MemSet(axiom, AXIOMLEN, 0);
	MemSet(ruleuf, PRODRULELEN, 0);
	MemSet(rulelf, PRODRULELEN, 0);
	MemSet(rule0, PRODRULELEN, 0);
	MemSet(rule1, PRODRULELEN, 0);
	MemSet(rule2, PRODRULELEN, 0);
	MemSet(rule3, PRODRULELEN, 0);
	MemSet(rule4, PRODRULELEN, 0);
	MemSet(rule5, PRODRULELEN, 0);
	MemSet(rule6, PRODRULELEN, 0);
	MemSet(rule7, PRODRULELEN, 0);
	MemSet(rule8, PRODRULELEN, 0);
	MemSet(rule9, PRODRULELEN, 0);
}

/*******************************************************************************
*
*	compile
*
*	Compiles command string.
*
*	Input:
*		dst - destination.
*		src - source.
*	Output:
*		s: 0.
*		f: !0.
*/
static Int16
compile(Char *dst,
        Char *src)
{
	Char token;

	while (*src) {
		switch (*src) {
		case 'F':
			token=TFORWARDDRAW;
			break;
		case 'f':
			token=TFORWARD;
			break;
		case '+':
			token=TRIGHT;
			break;
		case '-':
			token=TLEFT;
			break;
		case '|':
			token=TREVERSE;
			break;
		case '[':
			token=TPUSH;
			break;
		case ']':
			token=TPOP;
			break;
		default:
			if (*src<'0' || *src>'9')
				return 1;

			token=(*src-'0')+TRULE0;
			break;
		}

		*dst++=token;
		src++;
	}

	return 0;
}

/*******************************************************************************
*
*	compileaxiom
*
*	Special compile axiom sub.   The compiled axiom is used just about
*	everywhere in this program :)
*
*	Output:
*		s: 0.
*		f: !0.
*/
Int16
compileaxiom(void)
{
	MemSet(ax, AXIOMLEN, 0);
	return compile(ax, axiom);
}

/*******************************************************************************
*
*	resetdefaultrules
*
*	Reset the two override (default) rules - if they're not set.
*/
void
resetdefaultrules(void)
{
	if (!*ruleuf) {
		*ruleuf='F';
		*(ruleuf+1)='\x00';
	}

	if (!*rulelf) {
		*rulelf='f';
		*(rulelf+1)='\x00';
	}
}

/*******************************************************************************
*
*	validaterules
*
*	Validate all rules.   This will also prepare the rules.
*
*	Output:
*		s: 0.
*		f: !0.
*/
Int16
validaterules(void)
{
	Int16 ret=0;

	/* preset */
	resetdefaultrules();

	/* validate all rules */
	ret=valrule(ruf, ruleuf, "F");
	ret|=valrule(rlf, rulelf, "f");
	ret|=valrule(r0, rule0, "0");
	ret|=valrule(r1, rule1, "1");
	ret|=valrule(r2, rule2, "2");
	ret|=valrule(r3, rule3, "3");
	ret|=valrule(r4, rule4, "4");
	ret|=valrule(r5, rule5, "5");
	ret|=valrule(r6, rule6, "6");
	ret|=valrule(r7, rule7, "7");
	ret|=valrule(r8, rule8, "8");
	ret|=valrule(r9, rule9, "9");

	ret|=rulecheck();
	return ret;
}

/*******************************************************************************
*
*	valrule
*
*	Validate and compile a rule.
*
*	Input:
*		dst - where to store compiled rule.
*		src - rule.
*		rn - rule name.
*	Output:
*		s: 0.
*		f: !0.
*/
static Int16
valrule(Char *dst,
        Char *src,
        Char *rn)
{
	Int16 err;

	MemSet(dst, PRODRULELEN, 0);
	err=compile(dst, src);
	if (err) {
		FrmCustomAlert(alertruleerr, rn, NULL, NULL);
		return 1;
	}

	return 0;
}

/*******************************************************************************
*
*	rulecheck
*
*	Check consistency of rules.
*
*	Output:
*		consistent: 0.
*		inconsistent: !0.
*/
static Int16
rulecheck(void)
{
	/* "F" and "f" always seen */
	Char seen[NUMRULES]={ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	Char *names[NUMRULES]={ "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "F", "f" };
	Char *ap=ax, *r;
	UInt16 idx, ri;

	/* set defined rules */
	for (ri=0; ri<NUMRULES; ri++) {
		r=rulearr[ri];
		if (*r)
			seen[ri]=1;
	}

	/* check rules used in axiom */
	while (*ap) {
		if ((*ap<=TFORWARD) && (*ap>=TRULE0)) {
			idx=*ap-TRULE0;
			r=rulearr[idx];
			if (!*r) {
				if (seen[idx]!=2) {
					seen[idx]=2;
					FrmCustomAlert(alertconaxerr, names[idx], NULL, NULL);
				}
			}
		}
		ap++;
	}

	/* check cross references */
	for (ri=0; ri<NUMRULES; ri++) {
		if (seen[ri]==1) {
			r=rulearr[ri];
			while (*r) {
				if ((*r<=TFORWARD) && (*r>=TRULE0)) {
					idx=*r-TRULE0;
					ap=rulearr[idx];
					if (*ap)
						seen[idx]=1;
					else {
						if (seen[idx]!=2) {
							seen[idx]=2;
							FrmCustomAlert(alertconruleerr, names[ri], names[idx], NULL);
						}
					}
				}
				r++;
			}
		}
	}

	for (ri=0; ri<NUMRULES; ri++) {
		if (seen[ri]==2)
			return 1;
	}

	return 0;
}

/*******************************************************************************
*
*	turtleinit
*
*	Initializes turtle.
*/
void
turtleinit(void)
{
	exsrc=MemHandleLock(buf1h);
	running=1;
	StrCopy(exsrc, ax);
	curstrlen=StrLen(exsrc);
	turstep=prefs.angle;
	stackp=NULL;
}

/*******************************************************************************
*
*	turtledone
*
*	Stop turtle.
*/
void
turtledone(void)
{
	if (running) {
		if (stackp) {
			MemHandleUnlock(stackh);
			stackp=0;
		}

		if (buf1h) {
			MemHandleUnlock(buf1h);
			exsrc=NULL;
		}

		running=0;
		userinput=1;
	}
}
	
/*******************************************************************************
*
*	turtlestep
*
*	Do one step of turtle action.
*
*	Output:
*		s: 0.
*		f: !0.
*/
Int16
turtlestep(void)
{
	Char *d=exsrc, *s, *cs;
	UInt16 err=0, idx;

	if (userinput)
		return -1;

	if (curstrlen>(bufferlen-2))
		return -1;

	s=d+(bufferlen-curstrlen-1);
	MemMove(s, d, curstrlen);
	*(d+bufferlen-1)='\x00';

	curstrlen=0;
	while (*s) {
		if (err) {
			FrmCustomAlert(alertexerr, NULL, NULL, NULL);
			curstrlen=bufferlen;
			return 1;
		}

		if ((*s>=TRULE0) && (*s<=TFORWARD)) {
			idx=(*s)-TRULE0;
			cs=rulearr[idx];
			while (*cs) {
				if (d<s) {
					*d++=*cs++;
					curstrlen++;
				} else {
					err=1;
					break;
				}
			}
		} else {
			if (d<s) {
				*d++=*s;
				curstrlen++;
			} else
				err=1;
		}
		s++;
	}

	*d='\x00';
	return 0;
}

/*******************************************************************************
*
*	turtlebrain
*
*	Handle turtle command.
*
*	Input:
*		tc - turtle command.
*	Output:
*		s: 0.
*		f: !0.
*/
static Int16
turtlebrain(Char tc)
{
	switch (tc) {
	case TFORWARDDRAW:
	case TFORWARD:
		turx+=costab[turdir];
		tury+=sintab[turdir];
		break;
	case TRIGHT:
		turdir+=turstep;
		if (turdir>359)
			turdir=turdir-360;
		break;
	case TLEFT:
		turdir-=turstep;
		if (turdir<0)
			turdir=360+turdir;
		break;
	case TREVERSE:
		turdir+=180;
		if (turdir>360)
			turdir-=360;
		break;
	case TPUSH:
		stackp[stackidx].x=turx;
		stackp[stackidx].y=tury;
		stackp[stackidx].dir=turdir;
		stackidx++;
		break;
	case TPOP:
		stackidx--;
		turx=stackp[stackidx].x;
		tury=stackp[stackidx].y;
		turdir=stackp[stackidx].dir;
		/* FALL-THROUGH */
	default:
		break;
	}

	return 0;
}

/*******************************************************************************
*
*	turtlestack
*
*	Get and balance stack.
*
*	Output:
*		s: 0.
*		f: !0.
*/
Int16
turtlestack(void)
{
	Char *s=exsrc;
	Int32 top=0, idx=0;

	if (userinput)
		return -1;

	while (*s) {
		if (*s==TPUSH) {
			idx++;
			if (idx>top)
				top=idx;
		} else if (*s==TPOP) {
			idx--;
			if (idx<0) {
				FrmCustomAlert(alertstackunbal, NULL, NULL, NULL);
				return -1;
			}
		}

		s++;
	}

	if (!top)
		return 0;

	stackh=MemHandleNew(top*sizeof(stackentry));
	if (stackh) {
		stackp=MemHandleLock(stackh);
		return 0;
	}

	FrmCustomAlert(alertoom, NULL, NULL, NULL);
	return -1;
}

/*******************************************************************************
*
*	turtlecurvesize
*
*	Calculate size of output.
*
*	Output:
*		s: 0.
*		f: !0.
*/
Int16
turtlecurvesize(void)
{
	Char *turcom=exsrc;
	Int16 err=0;
	Char c;

	if (userinput)
		return -1;

	turdir=0;
	turx=0.0;
	tury=0.0;
	xmin=0.0;
	xmax=0.0;
	ymin=0.0;
	ymax=0.0;
	stackidx=0;
	
	while ((c=*turcom++)) {
		err=turtlebrain(c);
		if (err)
			break;

		if (c==TFORWARDDRAW || c==TFORWARD) {
			xmax=MAX(turx, xmax);
			xmin=MIN(turx, xmin);
			ymax=MAX(tury, ymax);
			ymin=MIN(tury, ymin);
		}
	}

	return err;
}

/*******************************************************************************
*
*	turtlerender
*
*	Render turtle graphics to memory screen.
*
*	Output:
*		s: 0.
*		f: !0.
*/
Int16
turtlerender(void)
{
	Char *turcom=exsrc;
	Int16 err=0;
	Int16 offx, offy;
	Char c;
	Coord sx, sy, x, y;
	float dx, dy, scale;

	if (userinput)
		return -1;

	WinEraseWindow();

	turdir=0;
	turx=0.0;
	tury=0.0;
	stackidx=0;
	dx=(xmax-xmin) ? xmax-xmin : 1.0;
	dy=(ymax-ymin) ? ymax-ymin : 1.0;
	scale=MIN(((float)DISPW-1)/dx, ((float)DISPH-1)/dy);

	offx=((Int16)(((float)DISPW-1)-(scale*dx)))/2;
	if (offx<0)
		offx=0;

	offy=((Int16)(((float)DISPH-1)-(scale*dy)))/2;
	if (offy<0)
		offy=0;
	
	sx=offx+SCRX(turx);
	sy=offy+SCRY(tury);
	while ((c=*turcom++)) {
		err=turtlebrain(c);
		if (err)
			break;

		switch (c) {
		case TFORWARDDRAW:
			x=offx+SCRX(turx);
			y=offy+SCRY(tury);
			WinDrawLine(sx, sy, x, y);
			sx=x;
			sy=y;
			break;
		case TFORWARD:
		case TPOP:
			sx=offx+SCRX(turx);
			sy=offy+SCRY(tury);
			break;
		}
	}

	return 0;
}
