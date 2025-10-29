/*
 * fStory.c
 */
#include "Include.h"

/* protos */
static void UpdateLock(void);
static void Export(void);
static Boolean EH(EventType *);

/* globals */
static Boolean locked;

/*
 * fStoryRun
 *
 * Show the finished story.
 *
 *  -> tIdx		Template index.
 */
void
fStoryRun(UInt16 tIdx)
{
	recordIdx=tIdx;
	locked=true;
	FrmGotoForm(fStory);
}

/*
 * fStoryInit
 */
Boolean
fStoryInit(void)
{
	MemHandle mh=StoryGetFinal();

	UIFieldSetText(cStoryField, MemHandleLock(mh));
	MemHandleFree(mh);

	UIFieldUpdateScrollBar(cStoryField, cStoryScrollBar);
	UIFieldFocus(cStoryField);
	return true;
}

/*
 * fStoryEH
 */
Boolean
fStoryEH(EventType *ev)
{
	RectangleType r;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		UpdateLock();
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cStoryBack:
			fKeywordsRun(recordIdx);
			return true;
		case cStoryTemplates:
			FrmGotoForm(fMain);
			return true;
		case cStoryExport:
			Export();
			return true;
		}
		break;

	case penDownEvent:
		FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cStoryLock), &r);
		if (RctPtInRectangle(ev->screenX, ev->screenY, &r)) {
			locked=!locked;
			UpdateLock();
			return true;
		}
		break;

	case keyDownEvent:
		UIFieldScrollBarKeyHandler(ev, cStoryField, cStoryScrollBar);
	default:	/* FALL-THRU */
		UIFieldScrollBarHandler(ev, cStoryField, cStoryScrollBar);
		break;
	}

	return false;
}

/*
 * UpdateLock
 *
 * Draw lock picture.
 */
static void
UpdateLock(void)
{
	FieldType *fld=UIObjectGet(cStoryField);
	RectangleType r;
	MemHandle mh;
	BitmapType *b;
	FieldAttrType attr;

	mh=DmGetResource(bitmapRsc, locked ? bmpLock : bmpUnlock);
	ErrFatalDisplayIf(mh==NULL, "(UpdateLock) Lock/unlock bitmap not found.");

	b=MemHandleLock(mh);
	FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cStoryLock), &r);

	WinDrawBitmap(b, r.topLeft.x, r.topLeft.y);

	MemHandleUnlock(mh);
	DmReleaseResource(mh);

	FldGetAttributes(fld, &attr);
	if (locked) {
		attr.editable=0;
		attr.underlined=0;
	} else {
		attr.editable=1;
		attr.underlined=1;
	}
	FldSetAttributes(fld, &attr);
	FldDrawField(fld);
	if (!locked) {
		FldSetInsPtPosition(fld, 0);
		UIFieldFocus(cStoryField);
	} else
		FldReleaseFocus(fld);

	UIFieldUpdateScrollBar(cStoryField, cStoryScrollBar);
}

/*
 * Export
 *
 * Export story to Memo Pad.
 */
static void
Export(void)
{
	Char *p=UIFieldGetText(cStoryField), *m;
	FormSave frm;
	Int32 len;
	MemHandle mh;
	UInt16 t;

	if (p==NULL || StrLen(p)==0) {
		FrmAlert(aNoStoryToExport);
		return;
	}

	DBClear(&dbMemo);
	if (DBOpen(&dbMemo, dmModeReadWrite, true)==false)
		return;

	UIFormPrologue(&frm, fStoryExport, EH);

	catTo=prefs.catExchangeIdx;
	UIPopupSet(&dbMemo, catTo, catName2, cStoryExportPopupTo);

	if (UIFormEpilogue(&frm, NULL, cStoryExportCancel)==false) {
		DBClose(&dbMemo);
		return;
	}

	prefs.catExchangeIdx=catTo;

	mh=MemHandleNew(MemoSize);
	ErrFatalDisplayIf(mh==NULL, "(Export) Out of memory.");
	m=MemHandleLock(mh);
	len=StrLen(p);

	while (len>0) {
		if (len>(MemoSize-1))
			t=MemoSize-1;
		else
			t=len;

		MemMove(m, p, t);
		m[t]='\x00';

		DBSetRecord(&dbMemo, dmMaxRecordIndex, catTo, m, t+1, NULL);
		len-=t;
		p+=t;
	}

	MemHandleFree(mh);
	DBClose(&dbMemo);
	FrmAlert(aStoryExported);
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cStoryExportPopupTo:
			CategorySelect(dbMemo.db, currentForm, cStoryExportPopupTo, cStoryExportListTo, false, &catTo, catName2, 0, categoryHideEditCategory);
			return true;
		}

	default:	/* FALL-THRU */
		break;
	}

	return false;
}
