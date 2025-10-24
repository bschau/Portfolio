/*
 * fNewGame.c
 */
#include "Include.h"

/* macros */
#define MAXLABELLENGTH 32
#define ROUNDENTRIES 7
#define POINTENTRIES 6

/* protos */
static void InitPopup(UInt16, UInt16, UInt32, UInt32 *, UInt16, Char *);
static Boolean GadgetHandler(EventPtr);
static void StartNewGame(void);

/* globals */
static UInt16 focusBand[]={ fNewGameName4, fNewGameName1, fNewGameName2, fNewGameName3, fNewGameName4, 0 };
static UInt32 roundTable[]={ 0, 5, 10, 15, 20, 25, 50 };
static UInt32 pointTable[]={ 0, 25, 50, 75, 100, 150 };
static Char roundLabel[MAXLABELLENGTH+2], pointLabel[MAXLABELLENGTH+2];

/*
 * InitPopup
 *
 * Initializes popup reflecting current preferences.
 *
 *  -> lstId		List ID.
 *  -> popId		Popup ID.
 *  -> value		Find this value ...
 *  -> table		... in this table.
 *  -> len		Table length.
 *  -> buffer		Where to store label text.
 */
static void
InitPopup(UInt16 lstId, UInt16 popId, UInt32 value, UInt32 *table, UInt16 len, Char *buffer)
{
	ListType *lst=UIFormGetObject(lstId);
	Boolean found=false;
	Int16 select=0;
	UInt16 idx;

	for (idx=0; idx<len; idx++) {
		if (table[idx]==value) {
			select=idx;
			found=true;
			break;
		}
	}
	
	if (found==true) {
		LstSetSelection(lst, select);
		CtlSetLabel(UIFormGetObject(popId), LstGetSelectionText(lst, select));
	} else {
		LstSetSelection(lst, len);
		CtlSetLabel(UIFormGetObject(popId), StrIToA(buffer, value));
	}
}

/*
 * fNewGameInit
 */
Boolean
fNewGameInit(void)
{
	InitPopup(fNewGameRoundsList, fNewGameRoundsPopup, pref.noOfRounds, &roundTable[0], ROUNDENTRIES, roundLabel);
	InitPopup(fNewGamePointsList, fNewGamePointsPopup, pref.noOfPoints, &pointTable[0], POINTENTRIES, pointLabel);

	switch (game.noOfPlayers) {
	case 4:
		UIFieldSetText(fNewGameName4, game.playerName[3]);
	case 3: /* FALL-THRU */
		UIFieldSetText(fNewGameName3, game.playerName[2]);
		break;
	}

	UIFieldSetText(fNewGameName2, game.playerName[1]);
	UIFieldSetText(fNewGameName1, game.playerName[0]);

	FrmSetFocus(currentForm, FrmGetObjectIndex(currentForm, fNewGameName1));
	return true;
}

/*
 * fNewGameEH
 */
Boolean
fNewGameEH(EventPtr ev)
{
	Char name[MaxNameLength+2];
	ControlType *pop;
	ListType *lst;
	Boolean new;
	UInt32 value;
	Int16 select;
	Char *txt;
	UInt16 toSet;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case fNewGameSelect1:
		case fNewGameSelect2:
		case fNewGameSelect3:
		case fNewGameSelect4:
			if ((fNamesRun(&name[0]))==false)
				return true;

			toSet=(ev->data.ctlSelect.controlID-fNewGameSelect1)+fNewGameName1;
			UIFieldSetText(toSet, name);
			FrmSetFocus(currentForm, FrmGetObjectIndex(currentForm, toSet));
			return true;

		case fNewGameBegin:
			StartNewGame();
			return true;
		case fNewGameNames:
			fNamesRun(NULL);
			return true;
		case fNewGameRules:
			fRulesRun();
			return true;
		}
		break;

	case popSelectEvent:
		switch (ev->data.popSelect.controlID) {
		case fNewGameRoundsPopup:
			select=ev->data.popSelect.selection;
			if (select>=0 && select<ROUNDENTRIES) {
				pref.noOfRounds=roundTable[select];
				return false;
			} else if (select==ROUNDENTRIES) {
				lst=UIFormGetObject(fNewGameRoundsList);
				pop=UIFormGetObject(fNewGameRoundsPopup);
				value=pref.noOfRounds;
				if ((new=UIIntegerDialog(ifRound, ifRoundField, ifRoundOk, &value))==false) {
					select=ev->data.popSelect.priorSelection;
					txt=LstGetSelectionText(lst, select);
					CtlSetLabel(pop, txt);
				} else {
					StrIToA(roundLabel, value);
					CtlSetLabel(pop, roundLabel);
					pref.noOfRounds=value;
				}
			}
			return true;

		case fNewGamePointsPopup:
			select=ev->data.popSelect.selection;
			if (select>=0 && select<POINTENTRIES) {
				pref.noOfPoints=pointTable[select];
				return false;
			} else if (select==POINTENTRIES) {
				lst=UIFormGetObject(fNewGamePointsList);
				pop=UIFormGetObject(fNewGamePointsPopup);
				value=pref.noOfPoints;
				if ((new=UIIntegerDialog(ifPoint, ifPointField, ifPointOk, &value))==false) {
					select=ev->data.popSelect.priorSelection;
					txt=LstGetSelectionText(lst, select);
					CtlSetLabel(pop, txt);
				} else {
					StrIToA(pointLabel, value);
					CtlSetLabel(pop, pointLabel);
					pref.noOfPoints=value;
				}
			}
			return true;
		}
		break;

	case keyDownEvent:
		return UIFieldReFocus(focusBand, ev->data.keyDown.chr);

	case penDownEvent:
		return GadgetHandler(ev);
		
	default:
		break;
	}

	return false;
}

/*
 * GadgetHandler
 *
 * Handle possible taps on the custom gadgets.
 *
 *  -> ev		Event.
 *
 * Returns true if event is handled, false otherwise.
 */
static Boolean
GadgetHandler(EventPtr ev)
{
	UInt16 cutGadgets[]={ fNewGameClear1, fNewGameClear2, fNewGameClear3, fNewGameClear4, 0 };
	UInt16 sX=ev->screenX, sY=ev->screenY;
	UInt16 toClear;
	UInt16 *g;
	Char buffer[2];
	RectangleType rect;

	for (g=cutGadgets; *g; g++) {
		FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, *g), &rect);
		if (RctPtInRectangle(sX, sY, &rect)) {
			toClear=*g-fNewGameClear1+fNewGameName1;
			*buffer='\x00';
			UIFieldSetText(toClear, buffer);
			FrmSetFocus(currentForm, FrmGetObjectIndex(currentForm, toClear));
			return true;
		}
	}

	return false;
}

/*
 * StartNewGame
 *
 * Setup new game if at least two player names has been given.
 */
static void
StartNewGame(void)
{
	UInt16 nameFields[]={ fNewGameName1, fNewGameName2, fNewGameName3, fNewGameName4 };
	UInt16 idx, len;
	Char *name;
	Char *dst;

	MemSet(&game, sizeof(KasinoGame), 0);
	game.noOfRounds=pref.noOfRounds;
	game.noOfPoints=pref.noOfPoints;

	for (idx=0; idx<MaxPlayers; idx++) {
		if ((name=UIFieldGetText(nameFields[idx]))==NULL)
			continue;

		/* trim name */
		for (; *name && TxtGlueCharIsSpace(*name); name++);
		if (*name!='\x00') {
			dst=&game.playerName[game.noOfPlayers][0];
			for (len=0; *name && len<MaxNameLength; len++)
				*dst++=*name++;

			dst=&game.playerName[game.noOfPlayers][MaxNameLength-1];
			for (len=0; len<MaxNameLength; len++) {
				if (*dst) {
					if (TxtGlueCharIsSpace(*dst))
						*dst='\x00';
					else
						break;
				}

				dst--;
			}

			dst=&game.playerName[game.noOfPlayers][0];
			TxtGlueLowerStr(dst, StrLen(dst));
			*dst=(Char)TxtGlueUpperChar((WChar)*dst);

			DBAddName(dst);
		
			game.noOfPlayers++;
		}
	}

	if (game.noOfPlayers<2)
		FrmCustomAlert(aNotEnoughPlayers, NULL, NULL, NULL);
	else {
		game.currentRound=1;
		FrmGotoForm(fMain);
	}
}
