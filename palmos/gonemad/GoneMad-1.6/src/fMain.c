/*
 * fMain.c
 */
#include "Include.h"

/* protos */
static void GetTitle(Char *, Char*, UInt16 *);
static void DrawTable(void *, Int16, Int16, RectangleType *);
static void ImportMemo(UInt16);

/* globals */
static Table tblMain;
static Int16 returnTop;

/*
 * GetTitle
 *
 * Get title from memo. If title is blank, get first line. Removes [..] and
 * [*..]. Breaks on '\n'.
 *
 * <-  dst		Where to store title.
 *  -> src		Source string.
 * <-> length		Destination length.
 *
 * Updates length to contain the actual length.
 */
static void
GetTitle(Char *dst, Char *src, UInt16 *length)
{
	UInt16 maxLength=(*length)-1, len=0;
	
	while (len<maxLength) {
		if (!*src || *src=='\n')
			break;

		if (*src=='[') {
			src++;
			for (; *src && *src!=']'; src++);
			if (!*src)
				break;
			else
				src++;
		} else {
			*dst++=*src++;
			len++;
		}
	}

	dst[len]='\x00';
	*length=len;
}

/*
 * DrawTable
 *
 * Draw the main table.
 *
 *  -> tbl		Table.
 *  -> row		Current row.
 *  -> column		Current column.
 *  -> r		Rectangle.
 */
static void
DrawTable(void *tbl, Int16 row, Int16 column, RectangleType *r)
{
	UInt16 recIdx=0;

	if (DmSeekRecordInCategory(dbTemplate.db, &recIdx, tblMain.top+row, dmSeekForward, prefs.category)!=errNone)
		return;

	TblSetRowData(tbl, row, (UInt32)recIdx);
	WinEraseRectangle(r, 0);

	if (!column) {
		FontID fId=FntSetFont(symbolFont);

		WinDrawChars("\x09", 1, r->topLeft.x+1, r->topLeft.y);
		FntSetFont(fId);
	} else {
		Boolean gotChar=false, trunc;
		UInt16 pW=138, len, i;
		Char buffer[128];
		MemHandle mh;
		Char *p, *s;

		mh=DBGetRecord(&dbTemplate, recIdx);
		p=MemHandleLock(mh);
		len=sizeof(buffer);
		GetTitle(buffer, p, &len);
		s=p;
		for (i=0; i<len; i++) {
			if (*s>' ') {
				gotChar=true;
				break;
			} else
				s++;
		}

		if (gotChar==false) {
			p="--- no title ---";
			len=16;
		} else
			p=buffer;

		FntCharsInWidth(p, &pW, &len, &trunc);
		WinDrawChars(p, len, r->topLeft.x, r->topLeft.y);

		MemHandleUnlock(mh);
	}
}

/*
 * fMainInit
 */
Boolean
fMainInit(void)
{
	TableStyle style[]={ { 0, customTableItem }, { 1, customTableItem }, { -1 } };

	PMSetPref(NULL, 0, PrfKeywords);
	UIPopupSet(&dbTemplate, prefs.category, catName1, cMainPopup);

	UITableInit(&dbTemplate, &tblMain, cMainTable, cMainScrollBar, DrawTable, style);
	tblMain.top=returnTop;
	fMainRethink();
	return true;
}

/*
 * fMainEH
 */
Boolean
fMainEH(EventType *ev)
{
	Boolean edited;
	UInt16 category, rIdx;
	MemHandle mh;
	Boolean color;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		if (WinScreenMode(winScreenModeGetSupportsColor, NULL, NULL, NULL, &color)!=errNone)
			color=false;

		mh=DmGetResource(iconType, 1000);	//color ? bmpLogo8 : bmpLogo1);
		ErrFatalDisplayIf(mh==NULL, "(fMainEH) Logo bitmap not found.");
		WinPaintBitmap(MemHandleLock(mh), 64, 29);
		MemHandleUnlock(mh);
		DmReleaseResource(mh);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cMainNew:
			fEditTemplateRun(dmMaxRecordIndex);
			FrmUpdateForm(fMain, frmRedrawUpdateCode);
			return true;

		case cMainImport:
			if ((rIdx=fMemoBrowserRun())!=dmMaxRecordIndex) {
				ImportMemo(rIdx);
				FrmUpdateForm(fMain, frmRedrawUpdateCode);
				fMainRethink();
			}
			return true;

		case cMainPopup:
			category=prefs.category;
			edited=CategorySelect(dbTemplate.db, currentForm, cMainPopup, cMainList, true, &prefs.category, catName1, 1, 0);
			if (edited || (category!=prefs.category))
				fMainRethink();

			return true;
		}
		break;

	case tblSelectEvent:
		returnTop=tblMain.top;
		rIdx=(UInt16)TblGetRowData(tblMain.tbl, ev->data.tblSelect.row);
		if (!ev->data.tblSelect.column)
			fEditTemplateRun(rIdx); 
		else
			fKeywordsRun(rIdx);

		return true;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mMainImport:
			fTemplateImportRun();
			FrmUpdateForm(fMain, frmRedrawUpdateCode);
			fMainRethink();
			return true;
		case mMainExport:
			fTemplateExportRun();
			FrmUpdateForm(fMain, frmRedrawUpdateCode);
			fMainRethink();
			return true;
		case mMainKeywordHelp:
			fHelpRun("Keyword Help", strKeywordHelpText);
			FrmUpdateForm(fMain, frmRedrawUpdateCode);
			return true;
		case mMainHelp:
			fHelpRun("Help", strHelpText);
			FrmUpdateForm(fMain, frmRedrawUpdateCode);
			return true;
		case mMainAbout:
			fAboutRun();
			FrmUpdateForm(fMain, frmRedrawUpdateCode);
			return true;
		}
		break;
	default:
		UITableEvents(&tblMain, ev);
		break;
	}

	return false;
}

/*
 * ImportMemo
 *
 * Import the selected memo.
 *
 *  -> rIdx		Record Index.
 */
static void
ImportMemo(UInt16 rIdx)
{
	MemHandle mh, rh;
	Char *r;
	UInt32 size;

	DBClear(&dbMemo);
	if (DBOpen(&dbMemo, dmModeReadWrite, true)==false)
		return;

	mh=DBGetRecord(&dbMemo, rIdx);
	size=MemHandleSize(mh);
	rh=MemHandleNew(size+1);
	ErrFatalDisplayIf(rh==NULL, "(ImportMemo) Out of memory.");

	r=MemHandleLock(rh);
	MemMove(r, MemHandleLock(mh), size);
	MemHandleUnlock(mh);
	r[size]='\x00';

	if (prefs.category==dmAllCategories)
		prefs.category=dmUnfiledCategory;

	DBSetRecord(&dbTemplate, dmMaxRecordIndex, prefs.category, r, size+1, SFString);
	MemHandleFree(rh);
	DBClose(&dbMemo);
}

/*
 * fMainRethink
 *
 * Update table values.
 */
void
fMainRethink(void)
{
	if (FrmGetActiveFormID()==fMain) {
		tblMain.records=DmNumRecordsInCategory(dbTemplate.db, prefs.category);
		UITableUpdateValues(&tblMain);
		UIPopupSet(&dbTemplate, prefs.category, catName1, cMainPopup);
	}
}
