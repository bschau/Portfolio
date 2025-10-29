/*
 * fMain.c
 */
#include "Include.h"

/* protos */
_FRM(static void SetPlayerSelector(UInt16, Char *));

/* globals */
static Char plySel[MaxPlayers][PlayerNameLength+1];
static UInt8 plyAct[MaxPlayers];

/*
 * SetPlayerSelector
 *
 * Update the specified Player Name selector.
 *
 *  -> id		Selector ID.
 *  -> value		Value (or NULL).
 */
static void
SetPlayerSelector(UInt16 id, Char *value)
{
	UInt16 idx=id-cMainPlayer1;
	Char *dst;

	dst=&plySel[idx][0];
	if (!value) {
		value=StringGet(Str02);
		plyAct[idx]=0;
	} else
		plyAct[idx]=1;

	MemSet(dst, PlayerNameLength+1, 0);
	StrNCopy(dst, value, PlayerNameLength);
	CtlSetLabel(UIObjectGet(id), dst);
}

/*
 * fMainInit
 */
Boolean
fMainInit(void)
{
	SetPlayerSelector(cMainPlayer1, NULL);
	SetPlayerSelector(cMainPlayer2, NULL);
	SetPlayerSelector(cMainPlayer3, NULL);
	SetPlayerSelector(cMainPlayer4, NULL);
	SetPlayerSelector(cMainPlayer5, NULL);
	SetPlayerSelector(cMainPlayer6, NULL);
	valid=0;
	return true;
}

/*
 *	fMainEH
 */
Boolean
fMainEH(EventType *ev)
{
	UInt16 idx, gIdx;
	MemHandle mh;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cMainPlayer1:
		case cMainPlayer2:
		case cMainPlayer3:
		case cMainPlayer4:
		case cMainPlayer5:
		case cMainPlayer6:
			if ((idx=fPlayersRun(StringGet(Str04), true))==dmMaxRecordIndex)
				return true;

			mh=DBGetName(idx);
			SetPlayerSelector(ev->data.ctlSelect.controlID, MemHandleLock(mh));
			MemHandleUnlock(mh);
			return true;
		case cMainStart:
			gIdx=0;
			MemSet(&game, sizeof(PXScGame), 0);
			for (idx=0; idx<6; idx++) {
				if (plyAct[idx]) {
					MemMove(&game.names[gIdx][0], &plySel[idx][0], PlayerNameLength);
					game.noOfPlayers++;
					gIdx++;
				}
			}
			if (game.noOfPlayers<2) {
				FrmAlert(aNeedsMorePlayers);
				return true;
			}

			valid=1;
			fInGameRun();
			break;
		}
		break;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mMainAbout:
			About();
			return true;
		case mMainPlayers:
			fPlayersRun(StringGet(Str03), false);
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}
