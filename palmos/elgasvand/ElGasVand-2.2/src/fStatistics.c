/*
 * fMain.c
 */
#include "Include.h"

/* macros */
typedef struct {
	UInt16 cnt;
	Int32 usage;
	Int32 hi;
	UInt16 hiDate;
	Int32 lo;
	UInt16 loDate;
} Statistics;
/* protos */
static void DrawWaiting(void);
static void GatherStatistics(void);
static void DrawPage(Int16);
static void RightString(UInt16, UInt16, UInt16);
static void LeftString(UInt16, UInt16, UInt16);
static void LeftInt32(Int32, UInt16, UInt16);
static void DrawSubStat(UInt16, Statistics *);

/* globals */
static Int16 pageNo;
static Statistics sE, sG, sW;

/*
 * fStatisticsRun
 *
 * Show the statistics form.
 *
 *  -> pNo		Start page.
 */
void
fStatisticsRun(Int16 pNo)
{
	pageNo=pNo;
	FrmGotoForm(fStatistics);
}

/*
 * fStatisticsEH
 */
Boolean
fStatisticsEH(EventType *ev)
{
	ListType *l;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		DrawWaiting();
		GatherStatistics();
		UIObjectShow(cStatisticsOK);
		l=UIObjectGet(cStatisticsList);

		LstSetSelection(l, pageNo);
		CtlSetLabel(UIObjectGet(cStatisticsPopup), LstGetSelectionText(l, pageNo));
		UIObjectShow(cStatisticsPopup);
	case frmUpdateEvent:
		DrawPage(pageNo);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cStatisticsOK:
			FrmGotoForm(fMain);
			return true;
		}
		break;

	case popSelectEvent:
		if (ev->data.popSelect.listID==cStatisticsList) {
			if ((ev->data.popSelect.selection!=ev->data.popSelect.priorSelection) && (ev->data.popSelect.selection!=noListSelection)) {
				pageNo=ev->data.popSelect.selection;
				DrawPage(pageNo);
			}
		}
		break;

	default:
		break;
	}

	return false;
}

/*
 * DrawWaiting
 *
 * Draw the Please Wait banner.
 */
static void
DrawWaiting(void)
{
	MemHandle mh=DmGetResource(strRsc, strPleaseWait);
	FontID fID=FntSetFont(boldFont);
	Char *str;
	UInt16 strLen, pW;
	RectangleType r;

	str=MemHandleLock(mh);
	strLen=StrLen(str);
	pW=FntCharsWidth(str, strLen);

	
	RctSetRectangle(&r, (160-(pW+8))/2, (160-(FntCharHeight()+8))/2, pW+8, FntCharHeight()+8);
	WinDrawRectangleFrame(boldRoundFrame, &r);

	WinDrawChars(str, strLen, (160-pW)/2, (160-FntCharHeight())/2);

	MemHandleUnlock(mh);
	DmReleaseResource(mh);
	FntSetFont(fID);
}

/*
 * GatherStatistics
 *
 * Collect statistics ...
 */
static void
GatherStatistics(void)
{
	UInt16 recIdx=0;
	MemHandle mh;
	Record oldRec;
	Record *r;
	UInt32 v;

	MemSet(&sE, sizeof(Statistics), 0);
	MemSet(&sG, sizeof(Statistics), 0);
	MemSet(&sW, sizeof(Statistics), 0);
	sE.lo=0xffffffff;
	sG.lo=0xffffffff;
	sW.lo=0xffffffff;

	if ((mh=DmQueryNextInCategory(dbData.db, &recIdx, dmAllCategories))==NULL) 
		return;

	MemMove(&oldRec, MemHandleLock(mh), sizeof(Record));
	MemHandleUnlock(mh);

	recIdx++;

	while ((mh=DmQueryNextInCategory(dbData.db, &recIdx, dmAllCategories))!=NULL) {
		r=MemHandleLock(mh);

		if (!(oldRec.flags&RFlgNewEMeter)) {
			v=(oldRec.el-r->el);
			sE.usage+=v;

			if (v>sE.hi) {
				sE.hi=v;
				sE.hiDate=r->date;
			} else if (v<sE.lo) {
				sE.lo=v;
				sE.loDate=r->date;
			}

			sE.cnt++;
		}

		if (!(oldRec.flags&RFlgNewGMeter)) {
			v=(oldRec.gas-r->gas);
			sG.usage+=v;

			if (v>sG.hi) {
				sG.hi=v;
				sG.hiDate=r->date;
			} else if (v<sG.lo) {
				sG.lo=v;
				sG.loDate=r->date;
			}

			sG.cnt++;
		}

		if (!(oldRec.flags&RFlgNewWMeter)) {
			v=(oldRec.water-r->water);
			sW.usage+=v;

			if (v>sW.hi) {
				sW.hi=v;
				sW.hiDate=r->date;
			} else if (v<sW.lo) {
				sW.lo=v;
				sW.loDate=r->date;
			}

			sW.cnt++;
		}

		MemMove(&oldRec, r, sizeof(Record));
		MemHandleUnlock(mh);
		recIdx++;
	}
}

/*
 * DrawPage
 *
 * Draw the statistics page X.
 *
 *  -> pageNo		Page number.
 */
static void
DrawPage(Int16 pageNo)
{
	RectangleType r;
	FontID fID;

	RctSetRectangle(&r, 0, 15, 160, 130);
	WinEraseRectangle(&r, 0);

	switch (pageNo) {
	case 1:
		DrawSubStat(strMeterE, &sE);
		break;

	case 2:
		DrawSubStat(strMeterG, &sG);
		break;

	case 3:
		DrawSubStat(strMeterW, &sW);
		break;

	default:
		fID=FntSetFont(boldFont);
		RightString(strSMeter, 60, 30);
		LeftString(strSUsage, 64, 30);
		LeftString(strSAverage, 120, 30);
		FntSetFont(fID);

		RightString(strMeterE, 60, 44);
		LeftInt32(sE.usage, 64, 44);
		LeftInt32(sE.usage/sE.cnt, 120, 44);

		RightString(strMeterG, 60, 58);
		LeftInt32(sG.usage, 64, 58);
		LeftInt32(sG.usage/sG.cnt, 120, 58);

		RightString(strMeterW, 60, 72);
		LeftInt32(sW.usage, 64, 72);
		LeftInt32(sW.usage/sW.cnt, 120, 72);

		break;
	}
}

/*
 * RightString
 *
 * Output string, right aligned at X.
 *
 *  -> strId		String ID.
 *  -> x		X pos.
 *  -> y		Y pos.
 */
static void
RightString(UInt16 strId, UInt16 x, UInt16 y)
{
	MemHandle mh=DmGetResource(strRsc, strId);
	Char *str;
	UInt16 sLen, w;

	str=MemHandleLock(mh);

	sLen=StrLen(str);
	w=FntCharsWidth(str, sLen);
	WinDrawChars(str, sLen, x-w, y);

	MemHandleUnlock(mh);
	DmReleaseResource(mh);
}

/*
 * LeftString
 *
 * Output string at (x, y).
 *
 *  -> strId		String ID.
 *  -> x		X pos.
 *  -> y		Y pos.
 */
static void
LeftString(UInt16 strId, UInt16 x, UInt16 y)
{
	MemHandle mh=DmGetResource(strRsc, strId);
	Char *str;

	str=MemHandleLock(mh);

	WinDrawChars(str, StrLen(str), x, y);

	MemHandleUnlock(mh);
	DmReleaseResource(mh);
}

/*
 * LeftInt32
 *
 * Output Int32 at (x, y).
 *
 *  -> v		Value
 *  -> x		X pos.
 *  -> y		Y pos.
 */
static void
LeftInt32(Int32 v, UInt16 x, UInt16 y)
{
	Char buffer[16];

	StrPrintF(buffer, "%ld", v);

	WinDrawChars(buffer, StrLen(buffer), x, y);
}

/*
 * DrawSubStat
 *
 * Draw sub statistics page.
 *
 *  -> strId		String ID.
 *  -> stat		Statistics block.
 */
static void
DrawSubStat(UInt16 strId, Statistics *stat)
{
	MemHandle mh=DmGetResource(strRsc, strId);
	FontID fID=FntSetFont(boldFont);
	Char *str;
	UInt16 sLen;
	Char dateBuf[32];
	
	str=MemHandleLock(mh);

	sLen=StrLen(str);
	WinDrawChars(str, sLen, (160-FntCharsWidth(str, sLen))/2, 30);

	MemHandleUnlock(mh);
	DmReleaseResource(mh);

	FntSetFont(fID);

	RightString(strSUsage, 78, 44);
	WinDrawChars(":", 1, 78, 44);
	LeftInt32(stat->usage, 82, 44);

	RightString(strSCount, 78, 56);
	WinDrawChars(":", 1, 78, 56);
	LeftInt32((Int32)stat->cnt, 82, 56);

	RightString(strSAverage, 78, 68);
	WinDrawChars(":", 1, 78, 68);
	LeftInt32(stat->usage/stat->cnt, 82, 68);

	RightString(strSHi, 78, 80);
	WinDrawChars(":", 1, 78, 80);
	LeftInt32(stat->hi, 82, 80);

	RightString(strSDate, 78, 92);
	WinDrawChars(":", 1, 78, 92);
	PMFormatDate(dateBuf, ((stat->hiDate>>9)&0x7f)+1904, (stat->hiDate>>5)&0x0f, stat->hiDate&0x1f);
	WinDrawChars(dateBuf, StrLen(dateBuf), 82, 92);
	
	RightString(strSLo, 78, 104);
	WinDrawChars(":", 1, 78, 104);
	LeftInt32(stat->lo, 82, 104);

	RightString(strSDate, 78, 116);
	WinDrawChars(":", 1, 78, 116);
	PMFormatDate(dateBuf, ((stat->loDate>>9)&0x7f)+1904, (stat->loDate>>5)&0x0f, stat->loDate&0x1f);
	WinDrawChars(dateBuf, StrLen(dateBuf), 82, 116);
}
