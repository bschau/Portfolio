/*
 * fScores.c
 */
#include "Include.h"

/* protos */
static void InitPopup(UInt16, UInt16);
static Boolean ValidateList(UInt16, Int16 *, UInt16);
static void ShortSummary(void);
static void HandleCheckBox(UInt16, UInt16, UInt16, UInt16 *);
static void HandleField(UInt16, UInt16, UInt16 *);

/* globals */
static Char *notSet="-";
static Char *names[]={ NULL, NULL, NULL, NULL, NULL };
static UInt16 points[MaxPlayers];

/*
 * InitPopup
 *
 * Initializes popup reflecting current player pool.
 *
 *  -> lstId		List ID.
 *  -> popId		Popup ID.
 */
static void
InitPopup(UInt16 lstId, UInt16 popId)
{
	ListType *lst=UIFormGetObject(lstId);

	LstSetHeight(lst, game.noOfPlayers+1);
	LstSetListChoices(lst, &names[0], game.noOfPlayers+1);
	LstSetSelection(lst, 0);
	CtlSetLabel(UIFormGetObject(popId), LstGetSelectionText(lst, 0));
}

/*
 * fScoresInit
 */
Boolean
fScoresInit(void)
{
	UInt16 player3Row[]={ fScoresSpades3, fScoresCards3, fScoresAces3, fScoresSweeps3, fScoresName3, 0 };
	UInt16 player4Row[]={ fScoresSpades4, fScoresCards4, fScoresAces4, fScoresSweeps4, fScoresName4, 0 };

	names[4]=NULL;
	names[3]=NULL;
	UIObjectHideX(player3Row);
	UIObjectHideX(player4Row);

	switch (game.noOfPlayers) {
	case 4:
		names[4]=&game.playerName[3][0];
		UIFieldSetText(fScoresName4, game.playerName[3]);
		UIObjectShowX(player4Row);
	case 3:	/* FALL-THRU */
		names[3]=&game.playerName[2][0];
		UIFieldSetText(fScoresName3, game.playerName[2]);
		UIObjectShowX(player3Row);
		break;
	}

	names[2]=&game.playerName[1][0];
	names[1]=&game.playerName[0][0];
	UIFieldSetText(fScoresName2, game.playerName[1]);
	UIFieldSetText(fScoresName1, game.playerName[0]);
	names[0]=notSet;

	InitPopup(fScoresLastSweepList, fScoresLastSweepPopup);
	InitPopup(fScoresTenOfDiamondsList, fScoresTenOfDiamondsPopup);
	InitPopup(fScoresTwoOfSpadesList, fScoresTwoOfSpadesPopup);
	
	return true;
}

/*
 * fScoresEH
 */
Boolean
fScoresEH(EventPtr ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==fScoresUpdate) {
			ShortSummary();
			return true;
		}
		break;

	default:
		break;
	}

	return false;
}

/*
 * ValidateList
 *
 * Retrieves selected item in list. Pops up a message if no entry is
 * selected or if the first entry is selected ('-').
 *
 *  -> lstId	List ID.
 *  -> dst		Where to store selection.
 *  -> aId		Alert ID.
 *
 * Returns true if *dst is to be believed, false otherwise.
 */
static Boolean
ValidateList(UInt16 lstId, Int16 *dst, UInt16 aId)
{
	ListType *lst=UIFormGetObject(lstId);

	*dst=LstGetSelection(lst);
	if (*dst==noListSelection || *dst==0) {
		FrmCustomAlert(aId, NULL, NULL, NULL);
		return false;
	}

	(*dst)--;
	return true;
}

/*
 * ShortSummary
 *
 * Shows the Summary screen.  If user selects ok, updates scores and
 * go to main form.
 */
static void
ShortSummary(void)
{
	FormType *oldForm=currentForm, *sFrm;
	UInt16 sweepCtls[]={ fScoresSweeps1, fScoresSweeps2, fScoresSweeps3, fScoresSweeps4 };
	UInt16 player3Row[]={ fStatusName3, fStatusColon3, fStatusScore3, 0 };
	UInt16 player4Row[]={ fStatusName4, fStatusColon4, fStatusScore4, 0 };
	UInt16 spades=0, cards=0, aces=0, idx;
	Char buffer[maxStrIToALen+2];
	Int16 selected;
	Char *txt;

	for (idx=0; idx<MaxPlayers; idx++)
		points[idx]=0;

	if (ValidateList(fScoresLastSweepList, &selected, aLastSweep)==false)
		return;
	points[(UInt16)selected]++;

	if (ValidateList(fScoresTenOfDiamondsList, &selected, aTenOfDiamonds)==false)
		return;
	points[(UInt16)selected]+=2;

	if (ValidateList(fScoresTwoOfSpadesList, &selected, aTwoOfSpades)==false)
		return;
	points[(UInt16)selected]++;

	switch (game.noOfPlayers) {
	case 4:
		HandleCheckBox(fScoresSpades4, 3, 2, &spades);
		HandleCheckBox(fScoresCards4, 3, 1, &cards);
		HandleField(fScoresAces4, 3, &aces);
	case 3:	/* FALL-THRU */
		HandleCheckBox(fScoresSpades3, 2, 2, &spades);
		HandleCheckBox(fScoresCards3, 2, 1, &cards);
		HandleField(fScoresAces3, 2, &aces);
		break;
	}

	HandleCheckBox(fScoresSpades2, 1, 2, &spades);
	HandleCheckBox(fScoresCards2, 1, 1, &cards);
	HandleField(fScoresAces2, 1, &aces);

	HandleCheckBox(fScoresSpades1, 0, 2, &spades);
	HandleCheckBox(fScoresCards1, 0, 1, &cards);
	HandleField(fScoresAces1, 0, &aces);

	if (spades==0) {
		FrmCustomAlert(aSpades, NULL, NULL, NULL);
		return;
	}

	if (cards==0) {
		FrmCustomAlert(aCards, NULL, NULL, NULL);
		return;
	}

	if (aces!=4) {
		FrmCustomAlert(aAces, NULL, NULL, NULL);
		return;
	}
	
	for (idx=0; idx<game.noOfPlayers; idx++) {
		if ((txt=UIFieldGetText(sweepCtls[idx])))
			points[idx]+=(UInt16)StrAToI(txt);
	}

	sFrm=FrmInitForm(fStatus);
	ErrFatalDisplayIf(sFrm==NULL, "(ShortSummary) Cannot initialize Status form.");

	currentForm=sFrm;
	FrmSetActiveForm(currentForm);
	FrmDrawForm(currentForm);
	UIObjectHideX(player3Row);
	UIObjectHideX(player4Row);
	switch (game.noOfPlayers) {
	case 4:
		UIFieldSetText(fStatusName4, &game.playerName[3][0]);
		UIFieldSetText(fStatusScore4, StrIToA(buffer, points[3]));
		UIObjectShowX(player4Row);
	case 3:	/* FALL-THRU */
		UIFieldSetText(fStatusName3, &game.playerName[2][0]);
		UIFieldSetText(fStatusScore3, StrIToA(buffer, points[2]));
		UIObjectShowX(player3Row);
		break;
	}

	UIFieldSetText(fStatusName2, &game.playerName[1][0]);
	UIFieldSetText(fStatusScore2, StrIToA(buffer, points[1]));
	UIFieldSetText(fStatusName1, &game.playerName[0][0]);
	UIFieldSetText(fStatusScore1, StrIToA(buffer, points[0]));

	idx=FrmDoDialog(sFrm);
	FrmEraseForm(sFrm);
	FrmDeleteForm(sFrm);
	currentForm=oldForm;
	FrmSetActiveForm(currentForm);
	switch (idx) {
	case fStatusOk:
		for (idx=0; idx<game.noOfPlayers; idx++)
			game.playerScore[idx]+=(UInt32)points[idx];

		game.currentRound++;
		if (FindWinners()==false)
			FrmGotoForm(fMain);

	case fStatusCancel:	/* FALL-THRU */
	default:	/* FALL-THRU */
		break;
	}
}

/*
 * HandleCheckBox
 *
 * Bump scores based on contents of checkbox.
 *
 *  -> chkId		Checkbox ID.
 *  -> sIdx		Score index.
 *  -> pts		Points given.
 *  -> optInc		Optionally increment this.
 */
static void
HandleCheckBox(UInt16 chkId, UInt16 sIdx, UInt16 pts, UInt16 *optInc)
{
	if ((CtlGetValue(UIFormGetObject(chkId)))) {
		points[sIdx]+=pts;
		if (optInc)
			(*optInc)++;
	}
}

/*
 * HandleField
 *
 * Bump scores based on contents of field.
 *
 *  -> fldId		Field ID.
 *  -> sIdx		Score index.
 *  -> optInc		Optionally increment this.
 */
static void
HandleField(UInt16 fldId, UInt16 sIdx, UInt16 *optInc)
{
	Char *txt;
	UInt16 score;

	if ((txt=UIFieldGetText(fldId))!=NULL) {
		score=(UInt16)StrAToI(txt);
		points[sIdx]+=score;
		if (optInc)
			*optInc+=score;
	}
}
