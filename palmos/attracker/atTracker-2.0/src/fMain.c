/*
 * fMain.c
 */
#include "Include.h"

/* protos */
static Char *BumpBuffer(UInt32);
static void DrawTable(void *, Int16, Int16, RectangleType *);
static void DrawTableD(DmOpenRef, UInt16, TTask *, RectangleType *);
static void DrawTableN(TTask *, RectangleType *);
static void Print(void);
static void FilterSetup(Int16, Int16);
static void FilterUpdate(void);
static UInt16 FilterRecordCount(void);
static void SetupMainTable(void);

/* globals */
static UInt16 fLo, fHi;
static Table tblMain;
static Char filterTitle[128];
static Int16 returnTop;
static UInt32 tblBufSize;
static MemHandle tblBuf;
static Boolean isDoubled, notFirstDraw;

/*
 * fMainFirstTime
 */
void
fMainFirstTime(void)
{
	FilterSetup(0, 0);
	returnTop=0;
	tblBufSize=64;
	tblBuf=MemHandleNew(tblBufSize);
}

/*
 * fMainLastTime
 */
void
fMainLastTime(void)
{
	if (tblBuf)
		MemHandleFree(tblBuf);
}

/*
 * BumpBuffer
 *
 * Increase buffer.
 *
 *   -> newSize	New size.
 *
 * Returns locked pointer to buffer.
 */
static Char *
BumpBuffer(UInt32 newSize)
{
	if (newSize>tblBufSize) {
		MemHandleFree(tblBuf);
		tblBuf=MemHandleNew(newSize+1);
		ErrFatalDisplayIf(tblBuf==NULL, "(BumpBuffer) Out of memory.");
		tblBufSize=newSize;
	}

	return MemHandleLock(tblBuf);
}

/*
 * DrawTable
 *
 * Table draw dispatch function.
 */
static void
DrawTable(void *tbl, Int16 row, Int16 column, RectangleType *r)
{
	Int32 recCnt=tblMain.top+row;
	UInt16 recIdx=0;
	DmOpenRef db=dbData.db;
	IndexedColorType ict=0;
	Boolean useIct=false;
	MemHandle mh;
	TTask *t;

	// Fast forward to the first entry in the category
	if (DmSeekRecordInCategory(db, &recIdx, 0, dmSeekForward, prefs.categoryIdx)!=errNone)
		return;

	// Pick up our record.
	for (EVER) {
		mh=DBGetRecord(recIdx);
		t=MemHandleLock(mh);
		if ((t->date>=fLo) && (t->date<=fHi)) {
			if (!recCnt)
				break;
			else
				recCnt--;
		}
		MemHandleUnlock(mh);
		if (DmSeekRecordInCategory(db, &recIdx, 1, dmSeekForward, prefs.categoryIdx)!=errNone)
			return;
	}

	TblSetRowData(tbl, row, (UInt32)recIdx);

	if (canColor==true && (prefs.flags&PFlagAlternate) && ((tblMain.top+row)&1)) {
		useIct=true;
		ict=WinSetBackColor(prefs.barColor);
	}

	WinEraseRectangle(r, 0);

	if (isDoubled)
		DrawTableD(db, recIdx, t, r);
	else
		DrawTableN(t, r);

	MemHandleUnlock(mh);
	if (useIct==true)
		WinSetBackColor(ict);
}

/*
 * DrawTableD
 *
 * Draw the table contents in "double line" mode.
 */
static void
DrawTableD(DmOpenRef db, UInt16 recIdx, TTask *t, RectangleType *r)
{
	UInt16 f=FntCharHeight(), pw, origLen, sLen, attrs;
	Boolean trunc;
	Char buffer[64];
	Char *s, *dst;

	s=StringSkipLeading((UInt8 *)t+sizeof(TTask));
	pw=r->extent.x;
	origLen=StrLen(s);
	sLen=FntWidthToOffset(s, origLen, pw, NULL, NULL);
	dst=BumpBuffer(sLen);
	StringWhiteSpaceConvert(dst, s, sLen);
	WinDrawChars(dst, sLen, r->topLeft.x, r->topLeft.y+f);
	MemPtrUnlock(dst);

	if (origLen!=sLen)
		WinDrawBitmap(bmpBullet, 145, r->topLeft.y+3+f);

	DTDateFormatShort(buffer, t->date);
	pw=DATEFIELDWIDTH;
	sLen=StrLen(buffer);
	FntCharsInWidth(buffer, &pw, &sLen, &trunc);
	WinDrawChars(buffer, sLen, r->topLeft.x+DATEFIELDWIDTH-pw, r->topLeft.y);

	r->topLeft.x+=DATEFIELDWIDTH+SPACING;
	DTTimeFormat(buffer, t->time);
	pw=TIMEFIELDWIDTH;
	sLen=StrLen(buffer);
	FntCharsInWidth(buffer, &pw, &sLen, &trunc);
	WinDrawChars(buffer, sLen, r->topLeft.x+TIMEFIELDWIDTH-pw, r->topLeft.y);

	r->topLeft.x+=TIMEFIELDWIDTH+SPACING;
	DmRecordInfo(db, recIdx, &attrs, NULL, NULL);
	attrs&=dmRecAttrCategoryMask;
	CategoryGetName(db, attrs, buffer);
	pw=TASKFIELDWIDTH;
	CategoryTruncateName(buffer, pw);
	sLen=StrLen(buffer);
	FntCharsInWidth(buffer, &pw, &sLen, &trunc);
	WinDrawChars(buffer, sLen, r->topLeft.x, r->topLeft.y);
}

/*
 * DrawTableN
 *
 * Draw the table contents in "normal" mode.
 */
static void
DrawTableN(TTask *t, RectangleType *r)
{
	UInt16 pw, origLen, sLen;
	Boolean trunc;
	Char buffer[64];
	Char *s, *dst;

	DTDateFormatShort(buffer, t->date);
	pw=DATEFIELDWIDTH;
	sLen=StrLen(buffer);
	FntCharsInWidth(buffer, &pw, &sLen, &trunc);
	WinDrawChars(buffer, sLen, r->topLeft.x+DATEFIELDWIDTH-pw, r->topLeft.y);

	r->topLeft.x+=DATEFIELDWIDTH+SPACING;
	DTTimeFormat(buffer, t->time);
	pw=TIMEFIELDWIDTH;
	sLen=StrLen(buffer);
	FntCharsInWidth(buffer, &pw, &sLen, &trunc);
	WinDrawChars(buffer, sLen, r->topLeft.x+TIMEFIELDWIDTH-pw, r->topLeft.y);

	r->topLeft.x+=TIMEFIELDWIDTH+SPACING;
	s=StringSkipLeading((UInt8 *)t+sizeof(TTask));
	pw=TASKFIELDWIDTH;
	origLen=StrLen(s);
	sLen=FntWidthToOffset(s, origLen, pw, NULL, NULL);
	dst=BumpBuffer(sLen);
	StringWhiteSpaceConvert(dst, s, sLen);
	WinDrawChars(dst, sLen, r->topLeft.x, r->topLeft.y);
	MemPtrUnlock(dst);

	if (origLen!=sLen)
		WinDrawBitmap(bmpBullet, 145, r->topLeft.y+3);
}

/*
 * fMainInit
 */
Boolean
fMainInit(void)
{
	TableStyle style[]={ {0, customTableItem }, { -1 } };

	isDoubled=false;
	notFirstDraw=false;
	UITableInit(NULL, &tblMain, cMainTable, cMainScrollBar, DrawTable, style);
	SetupMainTable();
	tblMain.records=FilterRecordCount();
	tblMain.top=returnTop;
	UITableUpdateValues(&tblMain, false);

	UIPopupSet(&dbData, prefs.categoryIdx, categoryName, cMainCategoryPopup);
	FilterUpdate();
	return true;
}

/*
 * fMainEH
 */
Boolean
fMainEH(EventType *ev)
{
	Boolean edited;
	UInt16 category;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		if (notFirstDraw) {
			SetupMainTable();
			TblDrawTable(tblMain.tbl);
		} else
			notFirstDraw=true;

		WinDrawLine(1, 30, 159, 30);
		return true;

	case tblSelectEvent:
		editCmd=(UInt16)TblGetRowData(tblMain.tbl, ev->data.tblSelect.row)|EditMaskIdx;
		returnTop=tblMain.top;
		FrmGotoForm(fEdit);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cMainCategoryPopup:
			category=prefs.categoryIdx;
			edited=CategorySelect(dbData.db, currentForm, cMainCategoryPopup, cMainCategoryList, true, &prefs.categoryIdx, categoryName, CATNONEDITABLES, 0);
			if (edited || (category!=prefs.categoryIdx))
			{
				SetupMainTable();
				tblMain.records=FilterRecordCount();
				UITableUpdateValues(&tblMain, true);
			}
			return true;

		case cMainNew:
			editCmd=dmMaxRecordIndex|EditMaskNew;
			FrmGotoForm(fEdit);
			return true;

		default:
			break;
		}
		break;

	case popSelectEvent:
		if (ev->data.popSelect.controlID==cMainFilterPopup) {
			FilterSetup(ev->data.popSelect.selection, ev->data.popSelect.priorSelection);
			FilterUpdate();
			tblMain.records=FilterRecordCount();
			returnTop=0;
			tblMain.top=returnTop;
			UITableUpdateValues(&tblMain, true);
			return true;
		}
		break;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mMainAbout:
			fAboutRun();
			return true;
		case mMainPreferences:
			fPreferencesRun();
			FrmGotoForm(fMain);
			return true;
		case mMainCleanUp:
			FrmGotoForm(fCleanUp);
			return true;
		case mMainSummary:
			FrmGotoForm(fSummary);
			return true;
		case mMainPrint:
			Print();
			return true;
		case mMainArchive:
			FrmGotoForm(fArchive);
			return true;
		case mMainRestore:
			FrmGotoForm(fRestore);
			return true;
		case mMainManager:
			fManagerRun("Archive Manager", NULL, NULL);
			return true;
		case mMainQuickText:
			QuickTextManager();
			return true;
		default:
			break;
		}
		break;

	case keyDownEvent:
		if (EvtKeydownIsVirtual(ev)==false) {
			if (TxtGlueCharIsPrint(ev->data.keyDown.chr)) {
				MemMove(&editEvent, ev, sizeof(EventType));
				editCmd=dmMaxRecordIndex|EditMaskNew|EditMaskEvt;
				FrmGotoForm(fEdit);
				return true;
			}
		}
	default:	/* FALL-THRU */
		UITableEvents(&tblMain, ev);
		break;
	}

	return false;
}

/*
 * FilterSetup
 *
 * Calculate upper/lower bounds of filter.
 *
 *  -> sel		Selection (from list).
 *  -> prevSel		Prior selection.
 */
static void
FilterSetup(Int16 sel, Int16 prevSel)
{
	UInt16 oldLo=fLo, oldHi=fHi;
	UInt16 y, m, d;
	Int16 t;

	if (sel==noListSelection)
		sel=0;
	else if (sel>7)
		sel=0;

	switch (sel) {
	case 0:	// All
		fLo=0;
		fHi=0xffff;
		break;
	case 1: // Today
		fLo=DTToday();
		fHi=fLo;
		break;
	case 2: // Yesterday
		fLo=DTToday();
		DateAdjust((DateType *)&fLo, -1);
		fHi=fLo;
		break;
	case 3: // Last 7 days
		fLo=DTToday();
		fHi=fLo;
		DateAdjust((DateType *)&fLo, -7);
		break;
	case 4: // Last Week
		// week runs man-sun, not sun-sat as Palm wants it :-)
		fLo=DTToday();
		DTDateUnpack(fLo, &y, &m, &d);
		if ((t=DayOfWeek(m, d, y))==0)
			t=6;		// sunday
		else
			t--;

		t+=7;
		DateAdjust((DateType *)&fLo, -t);
		fHi=fLo;
		DateAdjust((DateType *)&fHi, 6);
		break;
	case 5: // Last 31 Days
		fLo=DTToday();
		fHi=fLo;
		DateAdjust((DateType *)&fLo, -31);
		break;
	case 6: // Last Month
		fLo=DTToday();
		DTDateUnpack(fLo, &y, &m, &d);
		m--;
		if (m<1) {
			m=12;
			y--;
			if (y<MAGICYEAR)
				y=MAGICYEAR;
			
		}
		d=DaysInMonth(m, y);
		fHi=DTDatePack(y, m, d);
		d=1;
		fLo=DTDatePack(y, m, d);
		break;
	case 7: // Range ...
		if (fRangeRun(&fLo, &fHi)==false) {
			fLo=oldLo;
			fHi=oldHi;
			LstSetSelection(UIObjectGet(cMainFilterList), prevSel);
		}
		break;
	}
}

/*
 * FilterUpdate
 *
 * Update selection text.
 */
static void
FilterUpdate(void)
{
	ListType *lst=UIObjectGet(cMainFilterList);
	ControlType *pop=UIObjectGet(cMainFilterPopup);
	Int16 sel;
	Char *txt, *p;

	if ((sel=LstGetSelection(lst))==noListSelection) {
		sel=0;
		LstSetSelection(lst, 0);
	}

	if (sel<7) {
		txt=LstGetSelectionText(lst, sel);
		CtlSetLabel(pop, txt);
	} else {
		DTDateFormatLong(filterTitle, fLo);
		for (p=filterTitle; *p; p++);
		*p=' ';
		p[1]='-';
		p[2]=' ';
		p+=3;
		DTDateFormatLong(p, fHi);
		CtlSetLabel(pop, filterTitle);
	}
}

/*
 * FilterRecordCount
 *
 * Iterate through the database and count lines to be shown.
 *
 * Returns number of lines selected by filter.
 */
static UInt16
FilterRecordCount(void)
{
	UInt16 recIdx=0, recCnt=0;
	DmOpenRef db=dbData.db;
	TTask *t;
	MemHandle mh;

	for (EVER) {
		if ((mh=DmQueryNextInCategory(db, &recIdx, prefs.categoryIdx))==NULL)
			break;

		t=MemHandleLock(mh);
		if ((t->date>=fLo) && (t->date<=fHi))
			recCnt++;

		MemHandleUnlock(mh);
		recIdx++;
	}

	return recCnt;
}

/*
 * Print
 *
 * Send Event list to Palm Print.
 */
static void
Print(void)
{
	DmOpenRef db=dbData.db;
	UInt16 printCnt=0, recIdx=0;
	Boolean blankLine=false;
	UInt32 len;
	MemHandle mh;
	TTask *t;

	if (PalmPrintOpen()==false) {
		FrmAlert(aNoPalmPrint);
		return;
	}
	
	for (EVER) {
		if ((mh=DmQueryNextInCategory(db, &recIdx, prefs.categoryIdx))==NULL)
			break;

		t=MemHandleLock(mh);
		if ((t->date>=fLo) && (t->date<=fHi)) {
			printCnt++;
			if (printCnt==1)
				PalmPrintLineModeStart();

			if (blankLine==true)
				PalmPrintLineModePrint("\n");
		
			DTDateFormatLong(selector1, t->date);
			len=StrLen(selector1)&1;
			DTTimeFormat(selector2, t->time);
			StrNCat(selector1, (len) ? "    " : "   ", SelectorLabelLength);
			StrNCat(selector1, selector2, SelectorLabelLength);
			PalmPrintSetBold();
			PalmPrintLineModePrint(selector1);
			PalmPrintSetPlain();
			PalmPrintLineModePrint((UInt8 *)t+sizeof(TTask));
			blankLine=true;
		}

		MemHandleUnlock(mh);
		recIdx++;
	}

	if (!printCnt)
		FrmAlert(aNothingToPrint);
	else
		PalmPrintLineModeStop();
}

/*
 * SetupMainTable
 *
 * Setup table to be double lined (if possible) if category is All.
 */
static void
SetupMainTable(void)
{
	UInt16 halfSize=MainTableLines/2, f, r;

	if (prefs.categoryIdx==dmAllCategories) {
		if (canDoubleTable==true && (prefs.flags&PFlagDoubleTable)) {
			isDoubled=true;
			tblMain.rows=halfSize;
			f=FntCharHeight()<<1;
			for (r=0; r<halfSize; r++) {
				TblSetRowHeight(tblMain.tbl, r, f);
				TblSetItemStyle(tblMain.tbl, r, 0, tallCustomTableItem);
				TblRemoveRow(tblMain.tbl, r+halfSize);
			}
		} else
			isDoubled=false;
	} else {
		if (isDoubled==true) {
			isDoubled=false;
			tblMain.rows=MainTableLines;
			f=FntCharHeight();
			for (r=0; r<halfSize; r++) {
				TblSetRowHeight(tblMain.tbl, r, f);
				TblSetItemStyle(tblMain.tbl, r, 0, customTableItem);
				TblInsertRow(tblMain.tbl, r+halfSize);
				TblSetRowUsable(tblMain.tbl, r+halfSize, true);
			}
		}
	}
}
