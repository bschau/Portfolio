/*
 * fWinner.c
 */
#include "Include.h"

/* protos */
static void DrawText(RectangleType *, Char *);
static void DrawPage(void);

/* globals */
static UInt16 gKeepSame=0;

/**
 * Draw text within rctangle.
 *
 * @param r Rectangle.
 * @param t Text.
 */
static void
DrawText(RectangleType *r, Char *t)
{
	UInt16 len=StrLen(t), pW=r->extent.x;
	Boolean trunc;

	FntCharsInWidth(t, &pW, &len, &trunc);
	WinDrawChars(t, len, r->topLeft.x, r->topLeft.y);
}

/**
 * Draw the content of the page.
 */
static void
DrawPage(void)
{
	UInt16 *indices=GetIndices();
	UInt16 h=FntCharHeight(), plc=1, prevPlc, idx, prevPhase, prevScore, this, p, s;
	FontID font=boldFont;
	Char buffer[ScoreLength];
	RectangleType rPos, rName, rScore;

	RctSetRectangle(&rPos, 4, 34, 10, 12);
	RctSetRectangle(&rName, 16, 34, 90, 12);
	RctSetRectangle(&rScore, 108, 34, 52, 12);

	this=*indices++;

	FntSetFont(boldFont);
	DrawText(&rPos, "1.");
	DrawText(&rName, &game.names[this][0]);

	prevPhase=game.phase[this];
	prevScore=game.scores[this];
	StrPrintF(buffer, "(%u) %u", prevPhase, prevScore);
	DrawText(&rScore, buffer);

	rPos.topLeft.y+=h;
	rName.topLeft.y+=h;
	rScore.topLeft.y+=h;

	for (idx=1; idx<game.noOfPlayers; idx++) {
		this=*indices++;
		p=game.phase[this];
		s=game.scores[this];
		prevPlc=plc;
		if (!(p==prevPhase && s==prevScore)) {
			plc++;
			font=stdFont;
		}

		prevPhase=p;
		prevScore=s;

		FntSetFont(font);

		StrPrintF(buffer, "(%u) %u", p, s);
		DrawText(&rScore, buffer);
		DrawText(&rName, &game.names[this][0]);
		
		if (prevPlc!=plc) {
			StrPrintF(buffer, "%u.", plc);
			DrawText(&rPos, buffer);
		}
	}

	FntSetFont(stdFont);
}

/*
 * fWinnerInit
 */
Boolean
fWinnerInit(void)
{
	game.gameEnded=TimGetSeconds();
	CtlSetValue(UIObjectGet(cWinnerSame), gKeepSame);
	return true;
}

/*
 * fWinnerEH
 */
Boolean
fWinnerEH(EventType *ev)
{
	RectangleType r;
	UInt16 fh;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cWinnerWinner), &r);
		fh=FntCharHeight()+r.topLeft.y;
		WinDrawLine(r.topLeft.x-1, fh, r.topLeft.x+r.extent.x+1, fh);

		DrawPage();
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cWinnerSame:
			gKeepSame=CtlGetValue(UIObjectGet(cWinnerSame));
			return true;

		case cWinnerNew:
			if (gKeepSame)
				GameSamePlayers();
			else
				FrmGotoForm(fMain);
			
			return true;

		case cWinnerExport:
			fExportRun();
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}
