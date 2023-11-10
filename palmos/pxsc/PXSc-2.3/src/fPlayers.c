/*
 * Players.c
 */
#include "Include.h"

/* protos */
_FRM(static void DrawList(Int16, RectangleType *, Char **));
_FRM(static Boolean EH(EventPtr));
_FRM(static void PlayersEdit(UInt16));
_FRM(static Boolean PlayersEditEH(EventPtr));
_FRM(static void SetupView(Int16));

/* globals */
static UInt16 plySelected, gIdx;
static Boolean gNeedSel;

/*
 * DrawTable
 */
static void
DrawList(Int16 itemNum, RectangleType *r, Char **unused)
{
	UInt16 item=0, pw=r->extent.x, sLen;
	MemHandle mh;
	Char *n;
	Boolean trunc;

	if (DmSeekRecordInCategory(dbData.db, &item, itemNum, dmSeekForward, dmAllCategories)==errNone) {
		WinEraseRectangle(r, 0);

		mh=DBGetName(item);

		n=MemHandleLock(mh);
		sLen=StrLen(n);
		FntCharsInWidth(n, &pw, &sLen, &trunc);
		WinDrawChars(n, sLen, r->topLeft.x, r->topLeft.y);

		MemHandleUnlock(mh);
	}
}

/*
 * Players
 *
 * Handle a player selection.
 *
 *  -> t		Title.
 *  -> mustSelect	True if user must select a player before returning.
 *
 * Returns index in database to player, dmMaxRecordIndex otherwise.
 */
UInt16
fPlayersRun(Char *t, Boolean mustSelect)
{
	FormSave frm;

	gNeedSel=mustSelect;
	plySelected=dmMaxRecordIndex;
	UIFormPrologue(&frm, fPlayers, EH);
	FrmSetTitle(currentForm, t);
	SetupView(noListSelection);

	UIFormEpilogue(&frm, NULL, 0);

	return plySelected;
}

/*
 *      EH
 */
static Boolean
EH(EventPtr ev)
{
	Int16 idx;

	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cPlayersOK:
			if (gNeedSel==true) {
				if ((idx=LstGetSelection(UIObjectGet(cPlayersList)))==noListSelection) {
					FrmAlert(aSelectPlayer);
					return true;
				}
				plySelected=idx;
			}
			break;

		case cPlayersNew:
			PlayersEdit(dmMaxRecordIndex);
			SetupView(noListSelection);
			return true;
		case cPlayersEdit:
			if ((idx=LstGetSelection(UIObjectGet(cPlayersList)))==noListSelection)
				FrmAlert(aNeedsPlayer);
			else {
				PlayersEdit(idx);
				SetupView(idx);
			}
			return true;
		}
		break;

	default:	/* FALL-THRU */
		return UIListKeyHandler(ev, cPlayersList, 1);
	}
	return false;
}

/*
 * PlayersEdit
 *
 * Rename or create new player.
 *
 *  -> idx		Existing index or dmMaxRecordIndex.
 */
static void
PlayersEdit(UInt16 idx)
{
	FormSave frm;
	MemHandle mh;
	FieldType *f;

	gIdx=idx;
	UIFormPrologue(&frm, fPlayersEdit, PlayersEditEH);
	UIFieldFocus(cPlayersEditName);
	if (idx!=dmMaxRecordIndex) {
		f=UIObjectGet(cPlayersEditName);
		mh=DBGetName(idx);
		UIFieldSetText(cPlayersEditName, MemHandleLock(mh));
		MemHandleUnlock(mh);
		FldSetSelection(f, 0, FldGetTextLength(f));
		UIObjectShow(cPlayersEditDelete);
	}
	UIFormEpilogue(&frm, NULL, cPlayersEditCancel);
}

/*
 * PlayersEditEH
 */
static Boolean
PlayersEditEH(EventPtr ev)
{
	Char *s;

	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cPlayersEditOK:
			if ((s=UIFieldGetText(cPlayersEditName))==NULL)
				return true;

			if (StrLen(s)==0)
				return true;

			DBAddName(gIdx, s);
			return false;
		case cPlayersEditDelete:
			if (FrmAlert(aDeletePlayer)==0) {
				DmDeleteRecord(dbData.db, gIdx);
				return false;
			}
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}
	return false;
}

/**
 * Setup view.
 */
static void
SetupView(Int16 selected)
{
	UIListInit(cPlayersList, DrawList, DmNumRecordsInCategory(dbData.db, dmAllCategories), selected);
	LstDrawList(UIObjectGet(cPlayersList));
}
