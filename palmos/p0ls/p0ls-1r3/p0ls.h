#ifndef P0LS_H
#define P0LS_H

#include <PalmOS.h>
#include "resources.h"

/* macros */
#define EVER ;;
#define DATABASENAME "0L Systems.db"
#define DATABASETYPE "0l0l"
#define RECMAXSIZE (KEYSIZE+2+(12*(PRODRULELEN+2))+AXIOMLEN+2+1+(2*sizeof(Int16)))
#define INTOFF 4	/* how many bytes before key start in packed struct */
#define ROMNONE 0
#define ROMV2 2
#define ROMV3 3
#define TRULE0 1
#define TRULE1 (TRULE0+1)
#define TRULE2 (TRULE1+1)
#define TRULE3 (TRULE2+1)
#define TRULE4 (TRULE3+1)
#define TRULE5 (TRULE4+1)
#define TRULE6 (TRULE5+1)
#define TRULE7 (TRULE6+1)
#define TRULE8 (TRULE7+1)
#define TRULE9 (TRULE8+1)
#define TFORWARDDRAW (TRULE9+1)
#define TFORWARD (TFORWARDDRAW+1)
#define TRIGHT (TFORWARD+1)
#define TLEFT (TRIGHT+1)
#define TREVERSE (TLEFT+1)
#define TPUSH (TREVERSE+1)
#define TPOP (TPUSH+1)
#define DISPW 160
#define DISPH 160
#define LOADFORM 1	/* show load form */
#define SAVEFORM 2	/* show save form */

/* structures */
typedef struct {
	UInt16 depth;
	UInt16 angle;
} p0lspref;

typedef struct {
	float x;
	float y;
	Int16 dir;
} stackentry;

/* display.c */
void displayinit(FormPtr, UInt16);

/* io.c */
void iopack(Char *, Int16, Int16);
void iounpack(Char *);
void ioinit(FormPtr, UInt16);

/* prodrules.c */
void prodrulesinit(FormPtr, UInt16);

/* start.c */
void startinit(FormPtr, UInt16);

/* turtle.c */
void dbloader(Char *, Int16, Int16, Char *, Char *, Char *, Char *, Char *, Char *, Char *, Char *);
void clearvars(void);
void resetdefaultrules(void);
Int16 compileaxiom(void);
Int16 validaterules(void);
void turtleinit(void);
void turtledone(void);
Int16 turtlestep(void);
Int16 turtlestack(void);
Int16 turtlecurvesize(void);
Int16 turtlerender(void);

/* utils.c */
MemPtr findobject(UInt16);
MemPtr objectinform(FormPtr, UInt16);
int setfieldtext(FormPtr, UInt16, Char *);
void getfieldtext(FormPtr, UInt16, Char *, UInt16);
void about(FormPtr, UInt16);
Boolean globalmenu(EventPtr, FormPtr);
void normalizekey(Char *, Char *);

/* working.c */
void working(FormPtr);
void updatevisual(UInt16);

#ifdef MAIN
	UInt16 romv, userinput, ioform;
	p0lspref prefs;
	Char axiom[AXIOMLEN+2], ax[AXIOMLEN+2];
	Char ruleuf[PRODRULELEN+2], rulelf[PRODRULELEN+2];
	Char rule0[PRODRULELEN+2], rule1[PRODRULELEN+2], rule2[PRODRULELEN+2];
	Char rule3[PRODRULELEN+2], rule4[PRODRULELEN+2], rule5[PRODRULELEN+2];
	Char rule6[PRODRULELEN+2], rule7[PRODRULELEN+2], rule8[PRODRULELEN+2];
	Char rule9[PRODRULELEN+2];
	MemHandle buf1h;
	UInt32 bufferlen;
	DmOpenRef gdb;
	Char grec[RECMAXSIZE+2];
#else
	extern UInt16 romv, userinput, ioform;
	extern p0lspref prefs;
	extern Char axiom[AXIOMLEN+2], ax[AXIOMLEN+2];
	extern Char ruleuf[PRODRULELEN+2], rulelf[PRODRULELEN+2];
	extern Char rule0[PRODRULELEN+2], rule1[PRODRULELEN+2];
	extern Char rule2[PRODRULELEN+2], rule3[PRODRULELEN+2];
	extern Char rule4[PRODRULELEN+2], rule5[PRODRULELEN+2];
	extern Char rule6[PRODRULELEN+2], rule7[PRODRULELEN+2];
	extern Char rule8[PRODRULELEN+2], rule9[PRODRULELEN+2];
	extern MemHandle buf1h;
	extern UInt32 bufferlen;
	extern DmOpenRef gdb;
	extern Char grec[RECMAXSIZE+2];
#endif

#endif
