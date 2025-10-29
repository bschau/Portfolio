/*
 * fWinner.c
 */
#include "Include.h"

/* protos */
static Int16 FWCompare(UInt32 *, UInt32 *, Int16, SortRecordInfoPtr, SortRecordInfoPtr, MemHandle);

/*
 *	FWCompare
 */
static Int16
FWCompare(UInt32 *r1, UInt32 *r2, Int16 unused1, SortRecordInfoPtr unused2, SortRecordInfoPtr unused3, MemHandle unused4)
{
	UInt32 s1=*r1, s2=*r2;

	if (s1<s2)
		return -1;

	if (s1>s2)
		return 1;

	return 0;
}

/*
 * FindWinners
 *
 * Find any winners.  A winner is:
 *
 * 1. The player with the most points after all the rounds has been
 *    played.
 * 2. The player with the most points after the point limit has been
 *    reached.
 *
 * Returns true if the Winner form was shown, false otherwise.
 */
Boolean
FindWinners(void)
{
	UInt16 player3Row[]={ fWinnersPly3Score, fWinnersPly3Name, 0 };
	UInt16 player4Row[]={ fWinnersPly4Score, fWinnersPly4Name, 0 };
	UInt16 pScores[]={ fWinnersPly1Score, fWinnersPly2Score, fWinnersPly3Score, fWinnersPly4Score };
	UInt16 pNames[]={ fWinnersPly1Name, fWinnersPly2Name, fWinnersPly3Name, fWinnersPly4Name };
	Boolean gameFinished=false;
	FormPtr oldForm=currentForm, wFrm;
	UInt32 scores[MaxPlayers], winnerScore;
	UInt16 places[MaxPlayers], seen[MaxPlayers];
	UInt16 idx, nIdx;
	Int16 sIdx;
	Char buffer[maxStrIToALen+2];
	FieldType *fld;

	if (game.noOfRounds==0 && game.noOfPoints==0)
		return false; /* game never stops */

	/* sort player scores */
	for (idx=0; idx<MaxPlayers; idx++)
		scores[idx]=game.playerScore[idx];

	SysInsertionSort(scores, MaxPlayers, sizeof(UInt32), (CmpFuncPtr)FWCompare, 0);

	if (game.noOfRounds) {
		if (game.currentRound>game.noOfRounds)
			gameFinished=true;
	}

	if (game.noOfPoints) {
		if (scores[MaxPlayers-1]>=game.noOfPoints)
			gameFinished=true;
	}

	gameFinished=true;		// FIXME!
	if (gameFinished==false)
		return false;

	/* time to find winner ... */
	for (idx=0; idx<MaxPlayers; idx++)
		seen[idx]=0;

	nIdx=0;
	for (sIdx=MaxPlayers-1; sIdx>-1; sIdx--) {
		winnerScore=scores[sIdx];
		for (idx=0; idx<MaxPlayers; idx++) {
			if (game.playerScore[idx]==winnerScore) {
				if (seen[idx]==0) {
					places[nIdx++]=idx;
					seen[idx]=1;
				}
			}
		}
	}

	wFrm=FrmInitForm(fWinners);
	ErrFatalDisplayIf(wFrm==NULL, "(FindWinners) Cannot initialize Winners form.");
	currentForm=wFrm;
	FrmSetActiveForm(currentForm);
	FrmDrawForm(currentForm);

	UIObjectHideX(player3Row);
	UIObjectHideX(player4Row);
	switch (game.noOfPlayers) {
	case 4:
		UIObjectShowX(player4Row);
	case 3:	/* FALL-THRU */
		UIObjectShowX(player3Row);
		break;
	}

	for (idx=0; idx<game.noOfPlayers; idx++) {
		nIdx=places[idx];
		UIFieldSetText(pScores[idx], StrIToA(buffer, game.playerScore[nIdx]));
		UIFieldSetText(pNames[idx], &game.playerName[nIdx][0]);
	}

	winnerScore=game.playerScore[places[0]];
	for (idx=0; idx<game.noOfPlayers; idx++) {
		if (game.playerScore[idx]==winnerScore) {
			fld=UIFormGetObject(pScores[idx]);
			FldSetFont(fld, boldFont);
			fld=UIFormGetObject(pNames[idx]);
			FldSetFont(fld, boldFont);
		}
	}

	FrmDoDialog(currentForm);
	FrmEraseForm(wFrm);
	FrmDeleteForm(wFrm);
	currentForm=oldForm;
	FrmSetActiveForm(currentForm);
	FrmGotoForm(fNewGame);

	return true;
}
