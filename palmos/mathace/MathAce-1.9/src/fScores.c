/*
 * fScores.c
 */
#include "Include.h"

/* protos */
static void UpdateForm(void);
static UInt16 GadgetHandler(UInt16, UInt16);
static void ShowDetails(UInt16);
static void GetOperations(Char *, UInt16);
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
		UIShowObject(cScoresExport);

	return true;
}

/*
 * fScoresEH
 */
Boolean
fScoresEH(EventType *ev)
{
	UInt16 idx;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		UpdateForm();
		return true;

	case penDownEvent:
		if ((idx=GadgetHandler(ev->screenX, ev->screenY))) {
			FrmGlueNavObjectTakeFocus(currentForm, idx-1+cScores00);
			return true;
		}
		return false;

	case frmObjectFocusTakeEvent:
		if ((ev->data.frmObjectFocusTake.objectID>=cScores00) && (ev->data.frmObjectFocusTake.objectID<=cScores09)) {
			RectangleType rct;

			FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, ev->data.frmObjectFocusTake.objectID), &rct);
			FrmGlueNavDrawFocusRing(currentForm, ev->data.frmObjectFocusTake.objectID, 0, &rct, frmNavFocusRingStyleObjectTypeDefault, true);
			FrmSetFocus(currentForm, FrmGetObjectIndex(currentForm, ev->data.frmObjectFocusTake.objectID));
			return true;
		}
		break;

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
		break;

	case keyDownEvent:
		if (ev->data.keyDown.chr==vchrRockerCenter) {
			idx=FrmGetObjectId(currentForm, FrmGetFocus(currentForm));
			if ((idx>=cScores00) && (idx<=cScores09) && (record[idx-cScores00].started)) {
				ShowDetails(idx-cScores00);
				FrmGlueNavObjectTakeFocus(currentForm, idx);
				return true;
			}
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
			CtlSetEnabled(UIFormGetObject(cScores00+cnt), 1);
		} else {
			WinDrawChars("-", 1, 18, r.topLeft.y);
			CtlSetEnabled(UIFormGetObject(cScores00+cnt), 0);
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
 * Returns index if a gadget was pressed (and handled), 0 otherwise.
 */
static UInt16
GadgetHandler(UInt16 x, UInt16 y)
{
	Boolean wasInBounds, nowInBounds, penDown;
	RectangleType r;
	UInt16 idx;
	
	for (idx=0; idx<MaxRecords; idx++) {
		FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cScores00+idx), &r);
		if (RctPtInRectangle(x, y, &r) && CtlEnabled(UIFormGetObject(cScores00+idx))) {
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
				return (idx+1);
			}

			break;
		}
	}

	return 0;
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

	if (record[idx].timedMins)
		StrPrintF(buf, "Yes, %u minute%s", record[idx].timedMins, (record[idx].timedMins!=1) ? "s" : "");
	else
		StrCopy(buf, "No");

	UIFieldSetText(cDetailsTimed, buf);

	StrPrintF(buf, "%lu of %lu", record[idx].score, record[idx].round);
	UIFieldSetText(cDetailsScore, buf);

	GetOperations(buf, idx);
	UIFieldSetText(cDetailsOperations, buf);

	UIFieldSetText(cDetailsMode, (record[idx].operation&PrfQuizType) ? "Calculate" : "Solve");

	StrPrintF(buf, "%s, %u", (record[idx].operation&PrfMaxType) ? "Operand" : "Answer", record[idx].max);
	UIFieldSetText(cDetailsMax, buf);

	StrPrintF(buf, "%u", record[idx].min);
	UIFieldSetText(cDetailsMin, buf);

	if (record[idx].operation&PrfOperationSigned)
		UIShowObject(cDetailsSigned);

	UIFormEpilogue(&frm, NULL, 0);
}

/*
 * GetOperations
 *
 * Get ASCII representation of Operation.
 *
 * <-  dst		Where to store ASCII.
 *  -> idx		Record index.
 */
static void
GetOperations(Char *dst, UInt16 idx)
{
	UInt16 dstIdx=0;

	if (record[idx].operation&PrfOperationAdd)
		dst[dstIdx++]='+';
	if (record[idx].operation&PrfOperationSub)
		dst[dstIdx++]='-';
	if (record[idx].operation&PrfOperationMul)
		dst[dstIdx++]='*';
	if (record[idx].operation&PrfOperationDiv)
		dst[dstIdx++]='/';

	dst[dstIdx]=0;
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
	CategorySetTriggerLabel(UIFormGetObject(cExportPopup), catName);

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

	StrCopy(dst, "MathAce Export ");
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
		GetOperations(buf, idx);

		StrPrintF(dst, "Operations:\t%s\n", buf);
		dst+=StrLen(dst);

		StrPrintF(dst, "Mode:\t\t%s\n", (record[idx].operation&PrfQuizType) ? "Calculate" : "Solve");
		dst+=StrLen(dst);

		StrPrintF(dst, "Max:\t\t%s, %u\n", (record[idx].operation&PrfMaxType) ? "Operand" : "Answer", record[idx].max);
		dst+=StrLen(dst);

		StrPrintF(dst, "Min:\t\t\t%u\n", record[idx].min);
		dst+=StrLen(dst);

		if (record[idx].operation&PrfOperationSigned) {
			StrCopy(dst, "Negative Numbers were allowed\n");
			dst+=StrLen(dst);
		}
	}

	*dst=0;
}
