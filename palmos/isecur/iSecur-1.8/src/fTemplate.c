/*
 * fTemplate.c
 */
#include "Include.h"

/* protos */
_UTL(static void DrawList(Int16, RectangleType *, Char **));
_UTL(static Boolean EH(EventType *));
_UTL(static UInt16 TemplatesInCategory(DmOpenRef, UInt16));
_UTL(static UInt16 TemplateSeekTo(DmOpenRef, UInt16, UInt16));
_UTL(static void SetupView(Boolean, Int16));

/* globals */
extern Char catName[dmCategoryLength];
extern UInt16 *dstRecIdx;
extern UInt16 tmplIdx;
extern Boolean tReturn;
extern Prefs prefs;
extern DB dbData;

/*
 * DrawList
 */
static void
DrawList(Int16 itemNum, RectangleType *rct, Char **unused)
{
	UInt16 rIdx=0;
	MemHandle mh;
	RecordHdr *h;
	Char *p;

	if ((rIdx=TemplateSeekTo(dbData.db, itemNum, tmplIdx))==dmMaxRecordIndex)
		return;

	WinEraseRectangle(rct, 0);

	mh=DBGetRecord(&dbData, rIdx);
	h=MemHandleLock(mh);

	fMainDrawIcon(h->iconID, rct->topLeft.x, rct->topLeft.y+1);

	if (h->flags&RHHasTitle) {
       		UInt16 pW=rct->extent.x, len;
		Boolean trunc;

		p=((Char *)h)+sizeof(RecordHdr);
		len=StrLen(p);
		FntCharsInWidth(p, &pW, &len, &trunc);
		WinDrawChars(p, len, rct->topLeft.x, rct->topLeft.y);
	}

	MemHandleUnlock(mh);
}

/*
 * fTemplateRun
 *
 * Launch the Template screen.
 *
 *  -> catIdx		Category index.
 * <-  idx		Where to store selected template index.
 * 
 * Returns true if a template was selected, false otherwise.
 */
Boolean
fTemplateRun(UInt16 catIdx, UInt16 *idx)
{
	FormSave frm;

	tReturn=false;
	tmplIdx=catIdx;
	dstRecIdx=idx;

	UIFormPrologue(&frm, fTemplate, EH);

	UIPopupSet(dbData.db, tmplIdx, catName, cTemplatePopup);

	SetupView(true, noListSelection);

	UIFormEpilogue(&frm, NULL, 0);
	return tReturn;
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	UInt16 cat;

	EntropyAdd(pool, ev);

	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cTemplatePopup:
			cat=tmplIdx;
			CategorySelect(dbData.db, currentForm, cTemplatePopup, cTemplateList, true, &tmplIdx, catName, 1, categoryHideEditCategory);
			if (cat!=tmplIdx)
				SetupView(true, 0);
			return true;
		}
		break;

	case lstSelectEvent:
		if (ev->data.lstSelect.selection!=noListSelection) {
			*dstRecIdx=TemplateSeekTo(dbData.db, ev->data.lstSelect.selection, tmplIdx);
			tReturn=true;
			CtlHitControl(UIObjectGet(cTemplateCancel));
			return true;
		}
		break;

	case keyDownEvent:
		if (UIListKeyHandler(ev, cTemplateItems, prefs.flags&PFlg5Way)==false) {
			if (TxtGlueCharIsPrint(ev->data.keyDown.chr)) {
				UIListUpdateFocus(cTemplateItems, fMainFindByLetter(ev->data.keyDown.chr, 0, tmplIdx));
				return true;
			}
		} else
			return true;

	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * TemplatesInCategory
 *
 * Count the numbers of templates in this category.
 *
 *  -> db		DmOpenRef.
 *  -> cat		Category.
 */
static UInt16
TemplatesInCategory(DmOpenRef db, UInt16 cat)
{
	UInt16 cnt=0, rIdx=0;
	MemHandle mh;

	while ((mh=DmQueryNextInCategory(db, &rIdx, cat))) {
		rIdx++;

		if ((((RecordHdr *)(MemHandleLock(mh)))->flags)&RHTemplate)
			cnt++;

		MemHandleUnlock(mh);
	}

	return cnt;
}

/*
 * TemplateSeekTo
 *
 * Seek to the Xth template.
 *
 *  -> db		DmOpenRef.
 *  -> X		Template to seek to.
 *  -> cIdx		Category index.
 *
 * Returns index of record or dmMaxRecordIndex if out of range.
 */
static UInt16
TemplateSeekTo(DmOpenRef db, UInt16 X, UInt16 cIdx)
{
	UInt16 rIdx=0;
	MemHandle mh;

	while ((mh=DmQueryNextInCategory(db, &rIdx, cIdx))) {
		if ((((RecordHdr *)(MemHandleLock(mh)))->flags)&RHTemplate) {
			if (!X) {
				MemHandleUnlock(mh);
				return rIdx;
			} else
				X--;
		}

		MemHandleUnlock(mh);
		rIdx++;
	}

	return dmMaxRecordIndex;
}

/*
 * SetupView
 *
 * Setup the template items list.
 *
 *  -> redraw		Redraw list.
 *  -> active		Active item (or noListSelection).
 */
static void
SetupView(Boolean redraw, Int16 active)
{
	UIListInit(cTemplateItems, DrawList, TemplatesInCategory(dbData.db, tmplIdx), active);
	if (redraw)
		LstDrawList(UIObjectGet(cTemplateItems));
}
