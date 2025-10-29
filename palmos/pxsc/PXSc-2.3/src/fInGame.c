/*
 * fInGame.c
 */
#include "Include.h"

/* macros */
#define WScoreIndent 10
#define WScoreSpace 4

/* protos */
_FRM(static void DrawScores(void));
_FRM(static Boolean GadgetHandler(UInt16, UInt16));
_FRM(static void Sort(void));
_FRM(static Int16 SortCompare(UInt16, UInt16));
_FRM(static void HandleScore(UInt16));
_FRM(static Boolean Score(UInt16));
_FRM(static Boolean ScoreEH(EventType *));
_FRM(static Boolean NewGame(void));
_FRM(static Boolean NewGameEH(EventType *));

/* globals */
static UInt16 gSamePlayers=0, gPhaseDone;
static Int32 gPoints;
static UInt16 indices[MaxPlayers];
static PXScGame origGame;

/**
 * Draw players, phases and scores.
 */
static void
DrawScores(void)
{
	UInt16 pw, sLen, idx, ply, addX, addY;
	RectangleType r;
	Boolean trunc;
	Char *n;
	Char buffer[12];

	for (ply=0; (ply<MaxPlayers) && (ply<game.noOfPlayers); ply++) {
		FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cInGame1+ply), &r);
		WinEraseRectangle(&r, 0);

		r.topLeft.x++;
		r.extent.x-=2;

		idx=indices[ply];
		n=&game.names[idx][0];
		sLen=StrLen(n);
		pw=r.extent.x;
		FntCharsInWidth(n, &pw, &sLen, &trunc);
		WinDrawChars(n, sLen, r.topLeft.x , r.topLeft.y);

		StrPrintF(buffer, "%u", game.scores[idx]);
		sLen=StrLen(buffer);
		pw=r.extent.x;
		FntCharsInWidth(buffer, &pw, &sLen, &trunc);
		WinDrawChars(buffer, sLen, r.topLeft.x+r.extent.x-pw, r.topLeft.y);

		addY=r.topLeft.y+FntCharHeight();
		StrPrintF(buffer, "%u. ", game.oldPhase[idx]+1);
		sLen=StrLen(buffer);
		WinDrawChars(buffer, sLen, r.topLeft.x+WScoreIndent, addY);

		addX=FntCharsWidth(buffer, sLen)+WScoreIndent+WScoreSpace;
		n=StringGet(Str05+game.oldPhase[idx]);
		sLen=StrLen(n);
		pw=r.extent.x-addX;
		FntCharsInWidth(n, &pw, &sLen, &trunc);
		WinDrawChars(n, sLen, r.topLeft.x+addX, addY);

		if (game.scoreDone[idx]) {
			WinDrawLine(r.topLeft.x, r.topLeft.y, r.topLeft.x+r.extent.x, r.topLeft.y+(2*FntCharHeight())-1);
			WinDrawLine(r.topLeft.x, r.topLeft.y+(2*FntCharHeight())-1, r.topLeft.x+r.extent.x, r.topLeft.y);

			if (game.oldPhase[idx]!=game.phase[idx]) {
				FontID fID=FntSetFont(symbolFont);

				WinDrawChars("\x16", 1, r.topLeft.x, addY+2);
				FntSetFont(fID);
			}
		}
	}
}

/**
 * Run the game score card.
 */
void
fInGameRun(void)
{
	game.gameStarted=TimGetSeconds();
	MemMove(&origGame, &game, sizeof(PXScGame));
	FrmGotoForm(fInGame);
}

/*
 * fInGameInit
 */
Boolean
fInGameInit(void)
{
	UInt16 idx;

	Sort();
	for (idx=0; idx<game.noOfPlayers; idx++) {
		if (game.phase[idx]==10) {
			FrmGotoForm(fWinner);
			return true;
		}
		CtlSetUsable(UIObjectGet(cInGame1+idx), true);
	}

	MemMove(&game.oldPhase[0], &game.phase[0], sizeof(UInt8)*MaxPlayers);
	return true;
}

/*
 * fInGameEH
 */
Boolean
fInGameEH(EventType *ev)
{
	Int16 idx;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		DrawScores();
		{
			UInt16 id;
			Int16 extraInfo;
			RectangleType rct;
			FrmNavFocusRingStyleEnum style;

			if (FrmGlueNavGetFocusRingInfo(currentForm, &id, &extraInfo, &rct, &style)==errNone)
				FrmGlueNavDrawFocusRing(currentForm, id, extraInfo, &rct, style, true);
		}
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cInGameNext:
			for (idx=0; idx<game.noOfPlayers; idx++) {
				if (!game.scoreDone[idx]) {
					FrmAlert(aScoreMissing);
					return true;
				}
			}

			Sort();
			MemSet(&game.scoreDone, sizeof(UInt8)*MaxPlayers, 0);
			MemMove(&origGame, &game, sizeof(PXScGame));
			MemMove(&game.oldPhase[0], &game.phase[0], sizeof(UInt8)*MaxPlayers);
			for (idx=0; idx<game.noOfPlayers; idx++) {
				if (game.phase[idx]==10) {
					FrmGotoForm(fWinner);
					return false;
				}
			}

			FrmUpdateForm(currentFormID, frmRedrawUpdateCode);
			return true;
		}
		break;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mInGameNew:
			NewGame();
			return true;
		case mInGameAbout:
			About();
			return true;
		}
		break;

	case penDownEvent:
		return GadgetHandler(ev->screenX, ev->screenY);

	case frmObjectFocusTakeEvent:
		if (ui->navSupported) {
			if ((ev->data.frmObjectFocusTake.objectID>=cInGame1) && (ev->data.frmObjectFocusTake.objectID<=cInGame6)) {
				RectangleType r;

				FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, ev->data.frmObjectFocusTake.objectID), &r);
				FrmGlueNavDrawFocusRing(currentForm, ev->data.frmObjectFocusTake.objectID, frmNavFocusRingNoExtraInfo, &r, frmNavFocusRingStyleSquare, true);
			}
		}
		break;

	case keyDownEvent:
		if ((ev->data.keyDown.chr==vchrHardRockerCenter) || (ev->data.keyDown.chr==vchrRockerCenter)) {
			UInt16 id;
			Int16 extraInfo;
			RectangleType rct;
			FrmNavFocusRingStyleEnum style;

			if (FrmGlueNavGetFocusRingInfo(currentForm, &id, &extraInfo, &rct, &style)==errNone) {
				if ((id>=cInGame1) && (id<=cInGame6)) {
					HandleScore(id-cInGame1);
					FrmUpdateForm(currentFormID, frmRedrawUpdateCode);
					return true;
				}
			}
		}
		break;

	default:
		break;
	}

	return false;
}


/**
 * Handle a gadget tap.
 *
 * @param x point X
 * @param y point Y
 * @return true if gadget was handled, false otherwise.
 */
static Boolean
GadgetHandler(UInt16 x, UInt16 y)
{
	IndexedColorType b=0, f=0;
	WinDrawOperation w=winPaint;
	RectangleType r;
	Boolean wasInBounds, nowInBounds, penDown;
	UInt16 idx;

	for (idx=0; (idx<MaxPlayers) && (idx<game.noOfPlayers); idx++) {
		FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cInGame1+idx), &r);
		if (RctPtInRectangle(x, y, &r)) {
			wasInBounds=true;
			
			if (ui->colorEnabled) {
				b=WinSetBackColor(ui->backColor);
				f=WinSetForeColor(ui->foreColor);
				w=WinSetDrawMode(winSwap);
			}

			wasInBounds=false;
			do {
				PenGetPoint(&x, &y, &penDown);
				nowInBounds=RctPtInRectangle(x, y, &r);
				if (nowInBounds!=wasInBounds) {
					if (ui->colorEnabled)
						WinPaintRectangle(&r, 0);
					else
						WinInvertRectangle(&r, 0);

					wasInBounds=nowInBounds;
				}
			} while (penDown);

			WinEraseRectangle(&r, 0);

			if (ui->colorEnabled) {
				WinSetDrawMode(w);
				WinSetForeColor(f);
				WinSetBackColor(b);
			}

			if (wasInBounds)
				HandleScore(idx);

			FrmUpdateForm(currentFormID, frmRedrawUpdateCode);
			return true;
		}
	}

	return false;
}

/*
 * Sort
 *
 * Sort players based on phase + scores.
 */
static void
Sort(void)
{
	UInt16 this, that, outer, inner;

	for (inner=0; inner<game.noOfPlayers; inner++)
		indices[inner]=inner;

	for (outer=0; outer<game.noOfPlayers; outer++) {
		this=indices[outer];
		for (inner=outer+1; inner<game.noOfPlayers; inner++) {
			that=indices[inner];
			if (SortCompare(this, that)<1) {
				indices[outer]=that;
				indices[inner]=this;
				this=that;
			}
		}
	}
}

/*
 * SortCompare
 */
static Int16
SortCompare(UInt16 this, UInt16 that)
{
	UInt16 d1, d2;
	Int16 d;

	/* Signalling the inverse of the _normal_ logic below, will put
	   the entries in the right order. */

	/* The order of the compare signs here _must_ be  < >
	   A "larger" phase must occur towards the top of the list. */
	if (game.phase[this]<game.phase[that])
		return -1;
	else if (game.phase[this]>game.phase[that])
		return 1;

	/* The order of the compare signs here _must_ be  > <
	   A "smaller" score must occur towards the top of the list */
	if (game.scores[this]>game.scores[that])
		return -1;
	else if (game.scores[this]<game.scores[that])
		return 1;

	d=TxtGlueCaselessCompare(&game.names[this][0], StrLen(&game.names[this][0]), &d1, &game.names[that][0], StrLen(&game.names[that][0]), &d2);
	if (d<0)
		return 1;
	if (d>0)
		return -1;

	return 0;
}

/**
 * Handle score for this player.
 *
 * @param ply Player index.
 */
static void
HandleScore(UInt16 ply)
{
	UInt16 idx=indices[ply];

	if (Score(idx)==true) {
		if (game.scoreDone[idx]) {
			game.scores[idx]=origGame.scores[idx];
			game.phase[idx]=origGame.phase[idx];
		}
		game.scores[idx]+=gPoints;
		if (gPhaseDone) {
			if (game.phase[idx]<10)
				game.phase[idx]++;
		}
		game.scoreDone[idx]=1;
	}
}

/*
 * Score
 *
 * Get score for this player.
 *
 *  -> ply		Player index.
 */
static Boolean
Score(UInt16 ply)
{
	FormSave frm;

	gPoints=0;
	gPhaseDone=0;
	UIFormPrologue(&frm, fScore, ScoreEH);
	UIFieldSetText(cScorePlayer, &game.names[ply][0]);
	UIFieldFocus(cScorePoints);
	return UIFormEpilogue(&frm, NULL, cScoreCancel);
}

/*
 * ScoreEH
 */
static Boolean
ScoreEH(EventType *ev)
{
	Char *s;

	if (ev->eType==ctlSelectEvent) {
	       switch (ev->data.ctlSelect.controlID) {
		case cScoreOK:
			if ((s=UIFieldGetText(cScorePoints))==NULL) {
				FrmAlert(aNeedsPoints);
				return true;
			}

			gPoints=StrAToI(s);
			gPhaseDone=CtlGetValue(UIObjectGet(cScorePhase));
			break;

		case cScoreKeyboard:
			{
				UInt16 pts=0;
				Char buffer[12];

				fKeyboardRun(&pts);
				StrPrintF(buffer, "%u", pts);
				UIFieldSetText(cScorePoints, buffer);
			}
			return true;
		
		case cScorePhase:
			if (CtlGetValue(UIObjectGet(cScorePhase))) {
				Char *p=UIFieldGetText(cScorePoints);

				if (!p || (StrLen(p)==0))
					UIFieldSetText(cScorePoints, "0");
			}
			return true;
		}
	}

	return false;
}

/*
 * NewGame
 *
 * New game form.
 *
 * Return true if new game was selected.
 */
static Boolean
NewGame(void)
{
	FormSave frm;

	UIFormPrologue(&frm, fNewGame, NewGameEH);
	CtlSetValue(UIObjectGet(cNewGameSame), gSamePlayers);
	if (UIFormEpilogue(&frm, NULL, cNewGameCancel)==false)
		return false;

	if (gSamePlayers)
		GameSamePlayers();
	else
		FrmGotoForm(fMain);

	return true;
}

/*
**	NewGameEH
*/
static Boolean
NewGameEH(EventType *ev)
{
	if (ev->eType==ctlSelectEvent && ev->data.ctlSelect.controlID==cNewGameSame) {
		gSamePlayers=CtlGetValue(UIObjectGet(cNewGameSame));
		return true;
	}

	return false;
}

/*
 *	GetIndices
 */
UInt16 *
GetIndices(void)
{
	return &indices[0];
}

/*
 * GameSamePlayers
 *
 * Setup a game with the same players.
 */
void
GameSamePlayers(void)
{
	MemSet(game.phase, sizeof(UInt8)*MaxPlayers, 0);
	MemSet(game.oldPhase, sizeof(UInt8)*MaxPlayers, 0);
	MemSet(game.scores, sizeof(UInt16)*MaxPlayers, 0);
	fInGameRun();
}
