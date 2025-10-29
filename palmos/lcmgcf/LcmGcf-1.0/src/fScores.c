/*
 * fScores.c
 */
#include "Include.h"

/* protos */
static void UpdateForm(void);
static Boolean GadgetHandler(UInt16, UInt16);
static void ShowDetails(UInt16);
static void SecondsToAscii(Char *, UInt32);
static void Export(void);
static Boolean ExportEH(EventType *);
static void BuildMemoRecord(Char *);

/* globals */
static Char catName[dmCategoryLength+2];
static DB dbMemo;
static UInt16 catTo;

/*
 * fScoresInit
 */
Boolean
fScoresInit(void)
{
	if (record->started)
		UIObjectShow(cScoresExport);

	return true;
}

/*
 * fScoresEH
 */
Boolean
fScoresEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		UpdateForm();
		return true;

	case penDownEvent:
		return GadgetHandler(ev->screenX, ev->screenY);

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cScoresDone:
			FrmGotoForm(fMain);
			return true;
		case cScoresExport:
			Export();
			FrmUpdateForm(fScores, frmRedrawUpdateCode);
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * UpdateForm
 *
 * Redraw Hi Score table.
 */
static void
UpdateForm(void)
{
	FontID fID;
	RectangleType r;
	UInt16 cnt, pw, len;
	Char buffer[32];
	Boolean trunc;

	WinDrawLine(0, 27, 159, 27);
	RctSetRectangle(&r, 0, 29, 160, 12);
	for (cnt=0; cnt<MaxRecords; cnt++) {
		WinEraseRectangle(&r, 0);
		fID=FntSetFont(boldFont);
		StrPrintF(buffer, "%u.", cnt+1);
		len=StrLen(buffer);
		pw=FntCharsWidth(buffer, len);
		WinDrawChars(buffer, len, 15-pw, r.topLeft.y);
		FntSetFont(fID);

		if (record[cnt].started) {
			len=StrLen(record[cnt].name);
			pw=100;
			FntCharsInWidth(record[cnt].name, &pw, &len, &trunc);
			WinDrawChars(record[cnt].name, len, 18, r.topLeft.y);

			StrPrintF(buffer, "%lu/%lu", record[cnt].score, record[cnt].round);
			pw=40;
			len=StrLen(buffer);
			FntCharsInWidth(buffer, &pw, &len, &trunc);
			WinDrawChars(buffer, len, 159-pw, r.topLeft.y);
			CtlSetEnabled(UIObjectGet(cScores00+cnt), 1);
		} else {
			WinDrawChars("-", 1, 18, r.topLeft.y);
			CtlSetEnabled(UIObjectGet(cScores00+cnt), 0);
		}

		r.topLeft.y+=11;
	}
}

/*
 * GadgetHandler
 *
 * Handle a gadget tap.
 *
 *  -> x		Point X.
 *  -> y		Point Y.
 *
 * Returns true if a gadget was pressed (and handled), false otherwise.
 */
static Boolean
GadgetHandler(UInt16 x, UInt16 y)
{
	Boolean wasInBounds, nowInBounds, penDown;
	RectangleType r;
	UInt16 idx;
	
	for (idx=0; idx<MaxRecords; idx++) {
		FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cScores00+idx), &r);
		if (RctPtInRectangle(x, y, &r) && CtlEnabled(UIObjectGet(cScores00+idx))) {
			wasInBounds=true;
			WinInvertRectangle(&r, 0);
			do {
				PenGetPoint(&x, &y, &penDown);
				nowInBounds=RctPtInRectangle(x, y, &r);
				if (nowInBounds!=wasInBounds) {
					WinInvertRectangle(&r, 0);
					wasInBounds=nowInBounds;
				}
			} while (penDown);

			if (wasInBounds) {
				WinInvertRectangle(&r, 0);
				ShowDetails(idx);
				return true;
			}

			break;
		}
	}

	return false;
}

/*
 * ShowDetails
 *
 * Show details about the selected score.
 *
 *  -> idx		Score index.
 */
static void
ShowDetails(UInt16 idx)
{
	FormSave frm;
	Char buf[DateTimeLength+1];

	UIFormPrologue(&frm, fDetails, NULL);

	UIFieldSetText(cDetailsName, record[idx].name);

	SecondsToAscii(buf, record[idx].started);
	UIFieldSetText(cDetailsStarted, buf);

	SecondsToAscii(buf, record[idx].stopped);
	UIFieldSetText(cDetailsStopped, buf);

	if ((record[idx].operation&PrfQuizTypeRandom)==PrfQuizTypeRandom)
		UIFieldSetText(cDetailsQuizType, "Random");
	else if (record[idx].operation&PrfQuizTypeGcf)
		UIFieldSetText(cDetailsQuizType, "GCF");
	else
		UIFieldSetText(cDetailsQuizType, "LCM");

	if (record[idx].timedMins)
		StrPrintF(buf, "Yes, %u minute%s", record[idx].timedMins, (record[idx].timedMins!=1) ? "s" : "");
	else
		StrCopy(buf, "No");

	UIFieldSetText(cDetailsTimed, buf);

	StrPrintF(buf, "%lu of %lu", record[idx].score, record[idx].round);
	UIFieldSetText(cDetailsScore, buf);

	UIFieldSetText(cDetailsScoring, (record[idx].operation&PrfScoreType) ? "Correct Guess" : "First Guess");

	StrPrintF(buf, "%u", record[idx].max);
	UIFieldSetText(cDetailsMax, buf);

	UIFieldSetText(cDetailsNumbers, (record[idx].operation&PrfNumbers) ? "3" : "2");

	StrPrintF(buf, "%lu", record[idx].clues);
	UIFieldSetText(cDetailsClues, buf);

	UIFormEpilogue(&frm, NULL, 0);
}

/*
 * SecondsToAscii
 *
 * Convert the specified seconds to ascii and store the result in dst.
 *
 * <-  dst		Where to store Ascii representation of tick.
 *  -> secs		The seconds to convert.
 */
static void
SecondsToAscii(Char *dst, UInt32 secs)
{
	DateTimeType dt;

	MemSet(dst, DateTimeLength, 0);
	TimSecondsToDateTime(secs, &dt);
	DateToAscii(dt.month, dt.day, dt.year, PrefGetPreference(prefLongDateFormat), dst);
	dst+=StrLen(dst);
	*dst++=' ';
	TimeToAscii(dt.hour, dt.minute, PrefGetPreference(prefTimeFormat), dst);
}

/*
 * Export
 *
 * Export list to Memo pad.
 */
static void
Export(void)
{
	FormSave frm;
	MemHandle mh;
	Char *memo;

	DBInit(&dbMemo, "MemoDB");
	if (DBOpen(&dbMemo, dmModeReadWrite, true)==false)
		return;

	mh=MemHandleNew(4096);
	ErrFatalDisplayIf(mh==NULL, "(Export) Out of memory.");
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
	DateTimeType dt;
	UInt16 idx;
	Char buf[DateTimeLength+1];

	StrCopy(dst, "LcmGcf Export ");
	dst+=StrLen(dst);
	TimSecondsToDateTime(TimGetSeconds(), &dt);
	DateToAscii(dt.month, dt.day, dt.year, PrefGetPreference(prefDateFormat), dst);
	dst+=StrLen(dst);
	*dst++=' ';
	TimeToAscii(dt.hour, dt.minute, PrefGetPreference(prefTimeFormat), dst);
	dst+=StrLen(dst);
	for (idx=0; idx<MaxRecords; idx++) {
		if (!record[idx].started)
			break;

		*dst++='\n';
		*dst++='\n';
		StrPrintF(dst, "%u:\t%s\n", idx+1, record[idx].name);
		dst+=StrLen(dst);

		SecondsToAscii(buf, record[idx].started);
		StrPrintF(dst, "Started:\t\t%s\n", buf);
		dst+=StrLen(dst);

		SecondsToAscii(buf, record[idx].stopped);
		StrPrintF(dst, "Stopped:\t\t%s\n", buf);
		dst+=StrLen(dst);

		if (record[idx].timedMins)
			StrPrintF(dst, "Timed:\t\tYes, %u minute%s\n", record[idx].timedMins, (record[idx].timedMins!=1) ? "s" : "");
		else
			StrCopy(dst, "Timed:\t\tNo\n");

		dst+=StrLen(dst);

		StrPrintF(dst, "Score:\t\t%lu of %lu\n", record[idx].score, record[idx].round);
		dst+=StrLen(dst);

		StrPrintF(dst, "Scoring:\t\t%s\n", (record[idx].operation&PrfScoreType) ? "Correct Guess" : "First Guess");
		dst+=StrLen(dst);

		StrPrintF(dst, "Numbers:\t\t%s\n", (record[idx].operation&PrfNumbers) ? "3" : "2");
		dst+=StrLen(dst);

		StrPrintF(dst, "Quiz Type:\t");
		dst+=StrLen(dst);

		if ((record[idx].operation&PrfQuizTypeRandom)==PrfQuizTypeRandom)
			StrPrintF(dst, "Random\n");
		else if (record[idx].operation&PrfQuizTypeGcf)
			StrPrintF(dst, "GCF\n");
		else
			StrPrintF(dst, "LCM\n");

		dst+=StrLen(dst);

		StrPrintF(dst, "Max:\t\t%u\n", record[idx].max);
		dst+=StrLen(dst);

		StrPrintF(dst, "Clues:\t\t%lu\n", record[idx].clues);
		dst+=StrLen(dst);
	}

	*dst=0;
}
