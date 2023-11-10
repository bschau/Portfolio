/*
 * fMain.c
 */
#include "Include.h"

/*
 * fMainInit
 */
Boolean
fMainInit(void)
{
	FindWinners();
	return true;
}

/*
 * fMainEH
 */
Boolean
fMainEH(EventPtr ev)
{
	UInt16 player3Row[]={ fMainPlayer3Name, fMainPlayer3Colon, fMainPlayer3Score, 0 };
	UInt16 player4Row[]={ fMainPlayer4Name, fMainPlayer4Colon, fMainPlayer4Score, 0 };
	Char buffer[maxStrIToALen+2];
	FormPtr aFrm;

	switch (ev->eType) {
	case frmOpenEvent:
		UIObjectHideX(player3Row);
		UIObjectHideX(player4Row);
		switch (game.noOfPlayers) {
		case 4:
			UIFieldSetText(fMainPlayer4Name, game.playerName[3]);
			UIFieldSetText(fMainPlayer4Score, StrIToA(buffer, game.playerScore[3]));
			UIObjectShowX(player4Row);
		case 3:	/* FALL-THRU */
			UIFieldSetText(fMainPlayer3Name, game.playerName[2]);
			UIFieldSetText(fMainPlayer3Score, StrIToA(buffer, game.playerScore[2]));
			UIObjectShowX(player3Row);
			break;
		}

		UIFieldSetText(fMainPlayer2Name, game.playerName[1]);
		UIFieldSetText(fMainPlayer2Score, StrIToA(buffer, game.playerScore[1]));
		UIFieldSetText(fMainPlayer1Name, game.playerName[0]);
		UIFieldSetText(fMainPlayer1Score, StrIToA(buffer, game.playerScore[0]));

		if (game.noOfRounds)
			UIFieldSetText(fMainMaxRounds, StrIToA(buffer, game.noOfRounds));
		else
			UIFieldSetText(fMainMaxRounds, "-");

		if (game.noOfPoints)
			UIFieldSetText(fMainMaxPoints, StrIToA(buffer, game.noOfPoints));
		else
			UIFieldSetText(fMainMaxPoints, "-");

		UIFieldSetText(fMainCurrentRound, StrIToA(buffer, game.currentRound));
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==fMainEndRound) {
			FrmGotoForm(fScores);
			return true;
		}
		break;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mMainNew:
			FrmGotoForm(fNewGame);
			return true;
		case mMainNames:
			fNamesRun(NULL);
			return true;
		case mMainRules:
			fRulesRun();
			return true;
		case mMainAbout:
			aFrm=FrmInitForm(fAbout);
			ErrFatalDisplayIf(aFrm==NULL, "(MainFormEventHandler) Cannot initialize About form.");
			FrmSetActiveForm(aFrm);
			FrmDoDialog(aFrm);
			FrmSetActiveForm(currentForm);
			FrmEraseForm(aFrm);
			FrmDeleteForm(aFrm);
			return true;
		default:
			break;
		}
		break;

	default:
		break;
	}

	return false;
}
