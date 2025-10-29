/*
 * fPuzzleDone.c
 */
#include "Include.h"

/* protos */
static void DrawPicture(void);

/* globals */
static UInt16 cellSize, width, height;

/*
 * fPuzzleDoneInit
 */
Boolean
fPuzzleDoneInit(void)
{
	UInt16 w, h;

	fGameCloseGridWindow();

	w=100/cellsX;
	if (w>8)
		w=8;

	h=100/cellsY;
	if (h>8)
		h=8;

	if (w<h)
		cellSize=w;
	else
		cellSize=h;

	width=cellSize*cellsX;
	height=cellSize*cellsY;
	return true;
}

/*
 * fPuzzleDoneEH
 */
Boolean
fPuzzleDoneEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		DrawPicture();
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cPuzzleDoneDone:
			PMSetPref(NULL, 0, PrfPuzzle, false);
			PMSetPref(NULL, 0, PrfGrid, false);
			fGameCloseGrid();
			FrmGotoForm(fMain);
			return true;
		}
	default: /* FALL-THRU */
		break;
	}

	return false;
}

/*
 * DrawPicture
 *
 * Draws the puzzle picture.
 */
static void
DrawPicture(void)
{
	Char *txt="Congratulations!";
	UInt16 x, y, idx;
	FontID fID;

	RectangleType rct;

	RctSetRectangle(&rct, (160-width)/2, 38+((100-height)/2), width, height);
	WinEraseRectangle(&rct, 0);
	WinDrawRectangleFrame(simpleFrame, &rct);

	fID=FntSetFont(largeBoldFont);
	y=rct.topLeft.y-5-FntCharHeight();
	x=StrLen(txt);
	WinDrawChars(txt, x, (160-FntCharsWidth(txt, x))/2, y);
	FntSetFont(fID);

	rct.extent.x=cellSize;
	rct.extent.y=cellSize;
	for (y=0; y<cellsY; y++) {
		idx=y*cellsX;

		rct.topLeft.x=(160-width)/2;
		for (x=0; x<cellsX; x++) {
			if (grid[idx]==1)
				WinDrawRectangle(&rct, 0);

			rct.topLeft.x+=cellSize;
			idx++;
		}

		rct.topLeft.y+=cellSize;
	}
}
