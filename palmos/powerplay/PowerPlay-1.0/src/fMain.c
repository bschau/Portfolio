/*
 * fMain.c
 */
#include "Include.h"

/* protos */
static void UpdateDisplay(void);
static void fExportRun(void);
static Boolean ExportEH(EventType *);
static void BuildMemoRecord(Char *);
static Char *DumpBestScorersTable(Char *, Char *, UInt16);
static void fClearRun(void);
static Boolean EH(EventType *);

/* globals */
static Char catName[dmCategoryLength+2];
static DB dbMemo;
static UInt16 lbIdx, catTo;

/*
 * fMainInit
 */
Boolean
fMainInit(void)
{
	ListType *l=UIObjectGet(cMainList);

	lbIdx=prefs.flags;
	LstSetSelection(l, lbIdx);
	CtlSetLabel(UIObjectGet(cMainPopup), LstGetSelectionText(l, lbIdx));
	return true;
}

/*
 * fMainEH
 */
Boolean
fMainEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		UpdateDisplay();
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cMainPlay:
			prefs.flags=lbIdx;
			SssNew();
			FrmGotoForm(fGame);
			return true;
		case cMainLevel:
			FrmGotoForm(fPreferences);
			return true;
		case cMainExport:
			fExportRun();
			return true;
		}
		break;

	case popSelectEvent:
		if (ev->data.popSelect.listID==cMainList) {
			if ((ev->data.popSelect.selection!=ev->data.popSelect.priorSelection) && (ev->data.popSelect.selection!=noListSelection)) {
				lbIdx=ev->data.popSelect.selection;
				UpdateDisplay();
			}
		}
		break;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mMainAbout:
			fAboutRun();
			UIFormRedraw();
			return true;
		case mMainClear:
			fClearRun();
			UIFormRedraw();
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * UpdateDisplay
 */
static void
UpdateDisplay(void)
{
	UInt16 offset=lbIdx*5, pD=PrefGetPreference(prefDateFormat), pT=PrefGetPreference(prefTimeFormat), cnt, pw, len;
	FontID fID;
	RectangleType rct;
	Char buffer[64];
	DateTimeType dtt;

	fID=FntSetFont(boldFont);
	StrPrintF(buffer, "Guess the %s", LstGetSelectionText(UIObjectGet(cMainList), lbIdx));
	len=StrLen(buffer);
	pw=FntCharsWidth(buffer, len);
	
	RctSetRectangle(&rct, 0, 16, 160, 11);
	WinEraseRectangle(&rct, 0);
	WinDrawChars(buffer, len, (160-pw)/2, 16);
	FntSetFont(fID);

	WinDrawLine(0, 27, 159, 27);
	RctSetRectangle(&rct, 0, 29, 160, 22);
	for (cnt=0; cnt<5; cnt++) {
		WinEraseRectangle(&rct, 0);
		fID=FntSetFont(boldFont);
		StrPrintF(buffer, "%u.", cnt+1);
		len=StrLen(buffer);
		pw=FntCharsWidth(buffer, len);
		WinDrawChars(buffer, len, 15-pw, rct.topLeft.y);
		FntSetFont(fID);

		if (bestScorers[offset].time) {
			StrPrintF(buffer, "%u", bestScorers[offset].score);
			WinDrawChars(buffer, StrLen(buffer), 30, rct.topLeft.y);

			TimSecondsToDateTime(bestScorers[offset].time, &dtt);
			DateToAscii(dtt.month, dtt.day, dtt.year, pD, buffer);
			StrCat(buffer, " ");
			TimeToAscii(dtt.hour, dtt.minute, pT, buffer+StrLen(buffer));
			len=StrLen(buffer);
			pw=FntCharsWidth(buffer, len);
			WinDrawChars(buffer, len, 160-pw, rct.topLeft.y);

			fID=FntSetFont(boldFont);
			WinDrawChars(bestScorers[offset].name, StrLen(bestScorers[offset].name), 30, rct.topLeft.y+11);
			FntSetFont(fID);
		} else {
			pw=FntCharsWidth("-", 1);
			WinDrawChars("-", 1, 30, rct.topLeft.y);
			WinDrawChars("-", 1, 160-pw, rct.topLeft.y);
			WinDrawChars("-", 1, 30, rct.topLeft.y+11);
		}

		rct.topLeft.y+=22;
		offset++;
	}
}

/*
 * fExportRun
 *
 * Export Best Scorers tables to Memo pad.
 */
static void
fExportRun(void)
{
	FormSave frm;
	MemHandle mh;
	Char *memo;

	DBInit(&dbMemo, "MemoDB");
	if (DBOpen(&dbMemo, dmModeReadWrite, true)==false)
		return;

	mh=MemHandleNew(4096);
	ErrFatalDisplayIf(mh==NULL, "(fExportRun) Out of memory.");
	memo=MemHandleLock(mh);
	MemSet(memo, 4096, 0);
	BuildMemoRecord(memo);

	UIFormPrologue(&frm, fExport, ExportEH);

	if (prefs.memoCategory==dmAllCategories)
		catTo=dmUnfiledCategory;
	else
		catTo=prefs.memoCategory;

	CategoryGetName(dbMemo.db, catTo, catName);
	CategorySetTriggerLabel(UIObjectGet(cExportPopup), catName);

	if (UIFormEpilogue(&frm, NULL, cExportCancel)==true) {
		prefs.memoCategory=catTo;

		DBSetRecord(&dbMemo, dmMaxRecordIndex, catTo, memo, StrLen(memo)+1, NULL);
		FrmAlert(aExported);
	}

	MemHandleFree(mh);
	DBClose(&dbMemo);
}

/*
 * ExportEH
 */
static Boolean
ExportEH(EventType *ev)
{
	if (ev->eType==ctlSelectEvent && ev->data.ctlSelect.controlID==cExportPopup) {
		CategorySelect(dbMemo.db, currentForm, cExportPopup, cExportList, false, &catTo, catName, 0, categoryHideEditCategory);
		return true;
	}

	return false;
}

/*
 * BuildMemoRecord
 *
 * Build the Memo record to export.
 *
 * <-  dst		Destination.
 */
static void
BuildMemoRecord(Char *dst)
{
	DateTimeType dtt;

	StrCopy(dst, "PowerPlay Best Scorers tables\nExport: ");
	dst+=StrLen(dst);

	TimSecondsToDateTime(TimGetSeconds(), &dtt);
	DateToAscii(dtt.month, dtt.day, dtt.year, PrefGetPreference(prefDateFormat), dst);
	dst+=StrLen(dst);
	*dst++=' ';
	TimeToAscii(dtt.hour, dtt.minute, PrefGetPreference(prefTimeFormat), dst);
	dst+=StrLen(dst);
	*dst++='\n';
	*dst++='\n';
	*dst++='\n';

	dst=DumpBestScorersTable(dst, "Guess the Base - Easy", 0);
	dst=DumpBestScorersTable(dst, "Guess the Base - Hard", 5);
	dst=DumpBestScorersTable(dst, "Guess the Exponent - Easy", 10);
	dst=DumpBestScorersTable(dst, "Guess the Exponent - Hard", 15);
}

/*
 * DumpBestScorersTable
 *
 * Dump the Best Scorer table to the memo record.
 *
 *  -> dst		Memo Record.
 *  -> title		Title.
 *  -> offset		offset.
 *
 * Returns new *dst.
 */
static Char * 
DumpBestScorersTable(Char *dst, Char *title, UInt16 offset)
{
	UInt16 idx, pD=PrefGetPreference(prefDateFormat), pT=PrefGetPreference(prefTimeFormat);
	DateTimeType dtt;

	StrCopy(dst, title);
	dst+=StrLen(dst);
	*dst++='\n';
	*dst++='\n';
	for (idx=0; idx<5; idx++) {
		StrPrintF(dst, "%u.\t", idx+1);
		dst+=StrLen(dst);

		TimSecondsToDateTime(bestScorers[offset].time, &dtt);
		DateToAscii(dtt.month, dtt.day, dtt.year, pD, dst);
		dst+=StrLen(dst);
		*dst++=' ';
		TimeToAscii(dtt.hour, dtt.minute, pT, dst);
		dst+=StrLen(dst);
		*dst++='\n';
		StrPrintF(dst, "\tScore: %u\n\tName: %s\n\n", bestScorers[offset].score, (*bestScorers[offset].name ? bestScorers[offset].name : "-"));
		dst+=StrLen(dst);
		offset++;
	}

	*dst++='\n';
	*dst++='\n';

	return dst;
}

/*
 * fClearRun
 *
 * Show the Clear Best Scorers form.
 */
void
fClearRun(void)
{
	FormSave frm;

	UIFormPrologue(&frm, fClear, EH);
	UIFormEpilogue(&frm, NULL, 0);
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	if (ev->eType==ctlSelectEvent && ev->data.ctlSelect.controlID==cClearClear) {
		UInt16 mask=0;

		if (CtlGetValue(UIObjectGet(cClearBE)))
			mask=1;

		if (CtlGetValue(UIObjectGet(cClearBH)))
			mask|=2;

		if (CtlGetValue(UIObjectGet(cClearEE)))
			mask|=4;

		if (CtlGetValue(UIObjectGet(cClearEH)))
			mask|=8;

		if (mask) {
			if (FrmAlert(aClear)==0) {
				if (mask&1)
					MemSet(&bestScorers[5*0], sizeof(BestScorerEntry)*5, 0);
				if (mask&2)
					MemSet(&bestScorers[5*1], sizeof(BestScorerEntry)*5, 0);
				if (mask&4)
					MemSet(&bestScorers[5*2], sizeof(BestScorerEntry)*5, 0);
				if (mask&8)
					MemSet(&bestScorers[5*3], sizeof(BestScorerEntry)*5, 0);
			} else
				return true;
		}
	}

	return false;
}
