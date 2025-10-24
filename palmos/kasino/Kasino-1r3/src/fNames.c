/*
 * fNames.c
 */
#include "Include.h"

/* protos */
static void BuildList(void);
static void DrawList(Int16, RectangleType *, Char **);
static Boolean NamesEH(EventPtr);

/*
 * BuildList
 *
 * Build/Install name list.
 */
static void
BuildList(void)
{
	ListType *lp;
	UInt16 items;

	items=DmNumRecordsInCategory(dbH, dmAllCategories);
	lp=UIFormGetObject(fNamesList);
	LstSetListChoices(lp, NULL, items);
	LstSetDrawFunction(lp, DrawList);
	LstSetSelection(lp, -1);
	LstDrawList(lp);
}

/*
 * DrawList
 */
static void
DrawList(Int16 item, RectangleType *r, Char **unused)
{
	MemHandle mh=DmQueryRecord(dbH, item);
	UInt16 len, pw;
	Char *n;
	Boolean trunc;

	if (mh) {
		n=MemHandleLock(mh);

		len=StrLen(n);
		pw=r->extent.x;
		FntCharsInWidth(n, &pw, &len, &trunc);
		WinDrawChars(n, len, r->topLeft.x, r->topLeft.y);
		MemHandleUnlock(mh);
	}
}

/*
 * fNamesRun
 *
 * Shows the Names form.
 *
 *  -> dstName		Where to store name (if !NULL).
 *
 * Returns true if a name is stored in dstName, false otherwise.
 */
Boolean
fNamesRun(Char *dstName)
{
	FormPtr nFrm=FrmInitForm(fNames), oldForm=currentForm;
	Boolean nameSet=false;
	Char *name;
	ListType *lp;
	Int16 select;
	UInt16 nameLen;
	MemHandle mh;

	ErrFatalDisplayIf(nFrm==NULL, "(NamesForm) Cannot initialize Names form.");
	currentForm=nFrm;
	FrmSetActiveForm(currentForm);
	FrmSetEventHandler(currentForm, (FormEventHandlerPtr)NamesEH);
	FrmDrawForm(currentForm);

	BuildList();

	if ((FrmDoDialog(currentForm))==fNamesOK) {
		if (dstName) {
			lp=UIFormGetObject(fNamesList);
			if ((select=LstGetSelection(lp))!=noListSelection) {
				if ((mh=DmQueryRecord(dbH, select))!=NULL) {
					name=MemHandleLock(mh);
					nameLen=StrLen(name);
					MemMove(dstName, name, nameLen);
					MemHandleUnlock(mh);
					dstName[nameLen]='\x00';
					nameSet=true;
				}
			}
		}
	}

	FrmEraseForm(nFrm);
	FrmDeleteForm(nFrm);
	currentForm=oldForm;
	FrmSetActiveForm(currentForm);

	return nameSet;
}

/*
 * NamesEH
 */
static Boolean
NamesEH(EventPtr ev)
{
	ListType *lp=UIFormGetObject(fNamesList);
	Int16 select;
	UInt16 button;

	switch (ev->eType) {
	case keyDownEvent:
		switch (ev->data.keyDown.chr) {
		case prevFieldChr:
		case pageUpChr:
			LstScrollList(lp, winUp, LinesInNamesList-1);
			return true;

		case nextFieldChr:
		case pageDownChr:
			LstScrollList(lp, winDown, LinesInNamesList-1);
			return true;
		}

		break;

	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==fNamesDelete) {
			if ((select=LstGetSelection(lp))!=noListSelection) {
				if ((button=FrmCustomAlert(aDeleteName, NULL, NULL, NULL))==0) {
					DmRemoveRecord(dbH, select);
					BuildList();
				}
			}
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}
