/*
 * PalmPrint.c
 */
#include "Include.h"

/* Print command defines */
#define cmdPrintChars		32768
#define cmdXmitChars		32769
#define cmdStartPrint		32770
#define cmdStartXmit		32771
#define cmdPrintLine		32772
#define cmdXmitLine		32773
#define cmdEndPrint		32774
#define cmdEndXmit		32775
#define cmdPrintLinePassThru	32776
#define cmdXmitLinePassThru	32777
#define cmdDoFF			32800
#define cmdSetPlain		32802
#define cmdSetBold		32804
#define cmdSetStdFont		32810
#define cmdSetNewFont		32812
#define cmdSetPtSize		32814
#define cmdSetLeftMargin	32820
#define cmdSetTopMargin		32822
#define cmdSetChars		32824
#define cmdSetLines		32826
#define cmdSetPortrait		32828
#define cmdSetLandscape		32830
#define cmdSetIndent		32832
#define cmdSetNumCopies		32834
#define cmdGetChars		33000
#define cmdGetLines		33001
#define sysAppLaunchPrintSetup	40000

/* Font defines */
#define Courier		0
#define Times		1
#define Helvetica	2
#define fontMask	7
#define pt12		0
#define pt10		8
#define pt9		16
#define pointMask	56 

/* protos */
static Boolean PalmPrintFind(UInt16 *, LocalID *);

/* globals */
static UInt16 ppCard;
static LocalID ppId;

/*
 * PalmPrintFind
 *
 * Find Palm Print.
 *
 *  -> dstCard		Where to store card num.
 *  -> dstId		Where to store local id.
 *
 * Returns true if Palm Print was found, false otherwise.
 */
static Boolean
PalmPrintFind(UInt16 *dstCard, LocalID *dstId)
{
	DmSearchStateType s;

	if (DmGetNextDatabaseByTypeCreator(true, &s, 'appl', 'SCSp', false, dstCard, dstId)!=errNone)
		return false;

	return true;
}

/*
 * PalmPrintOpen
 *
 * Open palm print.
 *
 * Returns true if succesful, false otherwise.
 */
Boolean
PalmPrintOpen(void)
{
	return PalmPrintFind(&ppCard, &ppId);
}

/*
 * PalmPrintGetInfo
 *
 * Get chars pr. line and lines from palm print.
 *
 *  -> w		Where to store width.
 *  -> h		Where to store height.
 */
void
PalmPrintGetInfo(UInt32 *w, UInt32 *h)
{
	UInt32 result;

	SysAppLaunch(ppCard, ppId, 0, cmdGetChars, (Char *)w, &result);
	SysAppLaunch(ppCard, ppId, 0, cmdGetLines, (Char *)h, &result);
}

/*
 * PalmPrintLineModeStart
 *
 * Start Line Mode printing.
 */
void
PalmPrintLineModeStart(void)
{
	UInt32 result;

	SysAppLaunch(ppCard, ppId, 0, cmdStartPrint, NULL, &result);
}

/*
 * PalmPrintLineModeStop
 *
 * Stop Line Mode printing.
 */
void
PalmPrintLineModeStop(void)
{
	UInt32 result;

	SysAppLaunch(ppCard, ppId, 0, cmdEndPrint, NULL, &result);
}

/*
 * PalmPrintLineModePrint
 *
 * Print a line.
 *
 *  -> l		Line to print.
 */
void
PalmPrintLineModePrint(Char *l)
{
	UInt32 result;

	SysAppLaunch(ppCard, ppId, 0, cmdPrintLine, l, &result);
}

/*
 * PalmPrintSetBold
 *
 * Set bold formatting.
 */
void
PalmPrintSetBold(void)
{
	UInt32 result;

	SysAppLaunch(ppCard, ppId, 0, cmdSetBold, 0, &result);
}

/*
 * PalmPrintSetPlain
 *
 * Set plain formatting.
 */
void
PalmPrintSetPlain(void)
{
	UInt32 result;

	SysAppLaunch(ppCard, ppId, 0, cmdSetPlain, 0, &result);
}
