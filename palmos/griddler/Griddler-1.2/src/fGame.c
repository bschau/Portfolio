/*
 * fGame.c
 */
#include "Include.h"

/* Macros */
#define ScrW 160
#define ScrH 136

/* protos */
static Boolean SetupGrid(UInt8 *);
static void FindOrigin(UInt8 *, UInt16 *, UInt16 *);
static void DrawAxes(UInt8 *);
static void BlitGrid(void);
static void UpdatePen(void);
static void UpdateTitle(void);
static Boolean PuzzleCompleted(void);
static Boolean pc(UInt8 *);
static Boolean CheckAll(UInt16, UInt16, UInt16, UInt16, UInt8 **);

/* globals */
static RGBColorType greyRGB={ 0, 128, 128, 128 };
static WinHandle gridWin;
static Boolean formOk, followPen, scrollable, checkIsX;
static Char *errMsg;
static UInt16 cellSize, fontSizeX, fontSizeY, offender;
static UInt16 pageSizeX, pageSizeY, gridSizeX, gridSizeY;
static UInt16 topX, topY, xOrigin, yOrigin;
static UInt16 penDownX, penDownY, oldPenX, oldPenY, pen;
static UInt16 xOffset, yOffset, puzIdx, limitX, limitY;
static MemHandle puzzleDataH;

/*
 * fGameInit
 */
Boolean
fGameInit(void)
{
	UInt16 cnt=DmNumRecordsInCategory(dbPuzzle.db, dmAllCategories);
	Boolean found=false;

	for (puzIdx=0; puzIdx<cnt; puzIdx++) {
		UInt32 uid;

		DmRecordInfo(dbPuzzle.db, puzIdx, NULL, &uid, NULL);
		if (uid==puzzle.currentPuzzleId) {
			found=true;
			break;
		}
	}

	if (found==false) {
		PMSetPref(NULL, 0, PrfPuzzle, false);
		PMSetPref(NULL, 0, PrfGrid, false);
		ErrFatalDisplay("(fGameInit) Cannot find record data");
	}

	puzzleDataH=DBGetRecord(&dbPuzzle, puzIdx);
	
	pen=1;
	formOk=SetupGrid(grid);

	return true;
}

/*
 * fGameCloseGridWindow
 *
 * Deallocate grid window.
 */
void
fGameCloseGridWindow(void)
{
	if (gridWin) {
		WinDeleteWindow(gridWin, false);
		gridWin=NULL;
	}
}

/*
 * fGameCloseGrid
 *
 * Deallocate grid.
 */
void
fGameCloseGrid(void)
{
	if (grid) {
		MemPtrFree(grid);
		grid=NULL;
	}
}

/*
 * fGameSaveGrid
 *
 * Save grid to preferences.
 */
void
fGameSaveGrid(void)
{
	if (grid) {
		PMSetPref(&puzzle, sizeof(Puzzle), PrfPuzzle, false);
		PMSetPref(grid, cellsX*cellsY, PrfGrid, false);
		fGameCloseGrid();
	}
}

/*
 * fGameLoadGrid
 *
 * Load grid from preferences.
 *
 * Returns true if grid was loaded, false otherwise.
 */
Boolean
fGameLoadGrid(void)
{
	Boolean ret=false;
	MemHandle mh;

	grid=NULL;
	if (PMGetPref(&puzzle, sizeof(Puzzle), PrfPuzzle, false)==false) {
		PMSetPref(NULL, 0, PrfPuzzle, false);
		return false;
	}

	cellsX=(puzzle.gridSize>>8)&0xff;
	cellsY=puzzle.gridSize&0xff;

	PMSetPref(NULL, 0, PrfPuzzle, false);

	if ((mh=MemHandleNew(cellsX*cellsY))) {
		grid=MemHandleLock(mh);
		if (PMGetPref(grid, cellsX*cellsY, PrfGrid, false)==true)
			ret=true;
		else {
			MemPtrFree(grid);
			grid=NULL;
		}
	}

	PMSetPref(NULL, 0, PrfGrid, false);
	return ret;
}

/*
 * fGameEH
 */
Boolean
fGameEH(EventType *ev)
{
	RectangleType rct;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		if (osGood) {
			UIObjectShow(cGameCellSizeG);
			UIObjectShow(cGameBlankG);
			UIObjectShow(cGameFillG);
			UIObjectShow(cGameDotG);
			UIObjectShow(cGameStopG);
			UIObjectShow(cGameArrowLG);
			UIObjectShow(cGameArrowUG);
			UIObjectShow(cGameArrowDG);
			UIObjectShow(cGameArrowRG);
		} else {
			UIObjectShow(cGameCellSizeT);
			UIObjectShow(cGameBlankT);
			UIObjectShow(cGameFillT);
			UIObjectShow(cGameDotT);
			UIObjectShow(cGameStopT);
			UIObjectShow(cGameArrowLT);
			UIObjectShow(cGameArrowUT);
			UIObjectShow(cGameArrowDT);
			UIObjectShow(cGameArrowRT);
		}
	case frmUpdateEvent:
		FrmEraseForm(currentForm);
		FrmDrawForm(currentForm);
		if (formOk) {
			BlitGrid();
			UpdatePen();
		} else {
			FrmCustomAlert(aGameFatal, errMsg, NULL, NULL);
			FrmGotoForm(fMain);
		}

		UpdateTitle();
		return true;

	case frmCloseEvent:
		if (appStopped)
			fGameSaveGrid();
		break;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cGameCellSizeG:
		case cGameCellSizeT:
			prefs.cellSize++;
			prefs.cellSize&=1;

			RctSetRectangle(&rct, 0, 154-ScrH, ScrW, ScrH);
			WinEraseRectangle(&rct, 0);
			if ((formOk=SetupGrid(grid))==false) {
				prefs.cellSize++;
				prefs.cellSize&=1;
				if ((formOk=SetupGrid(grid))==false) {
					errMsg="Cannot resize or backsize cells.";
					UIFormRedraw();
					return true;
				}
			}

			BlitGrid();
			return true;

		case cGameBlankG:
		case cGameBlankT:
			pen=0;
			UpdatePen();
			return true;

		case cGameFillG:
		case cGameFillT:
			pen=1;
			UpdatePen();
			return true;

		case cGameDotG:
		case cGameDotT:
			pen=2;
			UpdatePen();
			return true;

		case cGameStopG:
		case cGameStopT:
			if (FrmAlert(aGameStop)==0) {
				fGameCloseGrid();
				fGameCloseGridWindow();
				FrmGotoForm(fMain);
			} else
				UIFormRedraw();

			return true;

		case cGameArrowLG:
		case cGameArrowLT:
			if (topX<(ScrW/2))
				topX=0;
			else
				topX-=(ScrW/2);

			BlitGrid();
			break;

		case cGameArrowUG:
		case cGameArrowUT:
			if (topY<(ScrH/2))
				topY=0;
			else
				topY-=(ScrH/2);

			BlitGrid();
			break;

		case cGameArrowDG:
		case cGameArrowDT:
			if (topY+(ScrH/2)>limitY)
				topY=limitY;
			else
				topY+=(ScrH/2);

			BlitGrid();
			break;

		case cGameArrowRG:
		case cGameArrowRT:
			if (topX+(ScrW/2)>limitX)
				topX=limitX;
			else
				topX+=(ScrW/2);

			BlitGrid();
			break;
		}
		break;

	case penDownEvent:
		if ((ev->screenX>(xOffset+2)) && (ev->screenX<(ScrW-2)) && (ev->screenY>(yOffset+2)) && (ev->screenY<(ScrH+yOffset-2))) {
			penDownX=ev->screenX;
			penDownY=ev->screenY;
			oldPenX=penDownX;
			oldPenY=penDownY;
			followPen=true;
		} else
			followPen=false;
		break;

	case penMoveEvent:
		if (followPen && scrollable) {
			Boolean reDraw=false;
			UInt16 diff;

			if ((ev->screenX>(xOffset+2)) && (ev->screenX<(ScrW-2)) && (ev->screenY>(yOffset+2)) && (ev->screenY<(ScrH+yOffset-2))) {
				if (ev->screenX<oldPenX) {
					diff=(oldPenX-ev->screenX);
					if ((diff+topX)>limitX)
						topX=limitX;
					else
						topX+=diff;
	
					oldPenX=ev->screenX;
					reDraw=true;
				} else if (ev->screenX>oldPenX) {
					diff=(ev->screenX-oldPenX);
					if (diff>topX)
						topX=0;
					else
						topX-=diff;

					oldPenX=ev->screenX;
					reDraw=true;
				}

				if (ev->screenY<oldPenY) {
					diff=(oldPenY-ev->screenY);
					if ((diff+topY)>limitY)
						topY=limitY;
					else
						topY+=diff;
	
					oldPenY=ev->screenY;
					reDraw=true;
				} else if (ev->screenY>oldPenY) {
					diff=(ev->screenY-oldPenY);
					if (diff>topY)
						topY=0;
					else
						topY-=diff;
	
					oldPenY=ev->screenY;
					reDraw=true;
				}

				if (reDraw) {
					BlitGrid();
					return true;
				}
			} else {
				followPen=false;
				BlitGrid();
			}
		}
		break;


	case penUpEvent:
		if (followPen) {
			followPen=false;

			if ((ev->screenX>=(penDownX-1)) && (ev->screenX<=(penDownX+1)) && (ev->screenY>=(penDownY-1)) && (ev->screenY<=(penDownY+1))) {
				UInt16 x=(penDownX-xOrigin-xOffset+topX)/cellSize, y=(penDownY-yOrigin-yOffset+topY)/cellSize;

				if ((x<cellsX) && (y<cellsY)) {
					WinHandle oldW=WinSetDrawWindow(gridWin);
					UInt16 idx=(y*cellsX)+x;

					RectangleType rct;

					RctSetRectangle(&rct, xOrigin+(x*cellSize)+1, yOrigin+(y*cellSize)+1, cellSize-1, cellSize-1);
					if (!pen || grid[idx]==pen) {
						grid[idx]=0;
						WinEraseRectangle(&rct, 0);
						if (PuzzleCompleted()==true)
							FrmGotoForm(fPuzzleDone);
					} else {
						grid[idx]=pen;
						if (pen==1) {
							WinDrawRectangle(&rct, 0);
							if (PuzzleCompleted()==true)
								FrmGotoForm(fPuzzleDone);
						} else {
							IndexedColorType ict=0;

							WinEraseRectangle(&rct, 0);
							if (ui->colorEnabled)
								ict=WinSetForeColor(WinRGBToIndex(&greyRGB));

							WinDrawLine(xOrigin+(x*cellSize)+cellSize, yOrigin+(y*cellSize), xOrigin+(x*cellSize), yOrigin+(y*cellSize)+cellSize);

							if (ui->colorEnabled)
								WinSetForeColor(ict);
						}
					}

					WinSetDrawWindow(oldW);
					BlitGrid();
				}
			}
		}
		break;
		
	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mGameCheck:
			if (PuzzleCompleted()==true)
				FrmGotoForm(fPuzzleDone);
			else {
				Char buffer[8];

				StrPrintF(buffer, "%u", offender+1);
				FrmCustomAlert(aNotSolved, checkIsX==true ? "Column" : "Row", buffer, NULL);
			}

			return true;
		case mGameClear:
			if (FrmAlert(aClearField)==0) {
				WinHandle oldW=WinSetDrawWindow(gridWin);
				RectangleType rct;
				UInt16 x, y;

				MemSet(grid, cellsX*cellsY, 0);
				for (y=0; y<cellsY; y++) {
					for (x=0; x<cellsX; x++) {
						RctSetRectangle(&rct, xOrigin+(x*cellSize)+1, yOrigin+(y*cellSize)+1, cellSize-1, cellSize-1);
						WinEraseRectangle(&rct, 0);
					}
				}
				WinSetDrawWindow(oldW);
				BlitGrid();
			}
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * SetupGrid
 *
 * Setup new grid.
 *
 *  -> gridTemplate	Grid data (or NULL).
 *
 * Returns true if gridWin was allocated, false otherwise.
 */
static Boolean
SetupGrid(UInt8 *gridTemplate)
{
	UInt8 *quizData=MemHandleLock(puzzleDataH)+1;
	IndexedColorType ict=0;
	UInt16 err, x, y;
	WinHandle oldW;
	RectangleType rct;
	FontID fID;

	fGameCloseGridWindow();

	if (!prefs.cellSize) {
		fID=FntSetFont(smallNumbersFont);
		cellSize=9;
	} else {
		fID=FntSetFont(largeNumbersFont);
		cellSize=13;
	}

	fontSizeX=FntAverageCharWidth();
	fontSizeY=FntCharHeight();
	FntSetFont(fID);

	cellsX=(UInt16)(*quizData++);
	cellsY=(UInt16)(*quizData++);
	if (cellsX>99 || cellsY>99) {
		MemHandleUnlock(puzzleDataH);
		errMsg="One or more invalid sizes detected.";
		return false;
	}

	quizData+=StrLen(quizData)+1;
	quizData+=StrLen(quizData)+1;
	quizData+=StrLen(quizData)+1;
	FindOrigin(quizData, &xOrigin, &yOrigin);

	if (gridTemplate==NULL) {
		MemHandle mh;

		if ((mh=MemHandleNew(cellsX*cellsY))==NULL) {
			MemHandleUnlock(puzzleDataH);
			errMsg="Out of memory.";
			return false;
		}

		grid=MemHandleLock(mh);
		MemSet(grid, cellsX*cellsY, 0);
	} else
		grid=gridTemplate;

	gridSizeX=cellsX*cellSize;
	gridSizeY=cellsY*cellSize;

	pageSizeX=gridSizeX+xOrigin+1;
	pageSizeY=gridSizeY+yOrigin+1;

	gridWin=WinCreateOffscreenWindow(pageSizeX, pageSizeY, genericFormat, &err);
	if (!gridWin) {
		MemHandleUnlock(puzzleDataH);
		return false;
	}

	topX=0;
	topY=0;
	yOffset=(154-ScrH);
	if ((pageSizeX>ScrW) || (pageSizeY>ScrH)) {
		scrollable=true;
		xOffset=0;
	} else {
		scrollable=false;
		xOffset=(ScrW-pageSizeX)/2;
		yOffset+=((ScrH-pageSizeY)/2);
	}

	oldW=WinSetDrawWindow(gridWin);
	RctSetRectangle(&rct, 0, 0, pageSizeX, pageSizeY);
	WinEraseRectangle(&rct, 0);

	if (ui->colorEnabled)
		ict=WinSetForeColor(WinRGBToIndex(&greyRGB));

	x=xOrigin;
	y=yOrigin+gridSizeY;
	for (err=0; err<=cellsX; err++) {
		WinDrawLine(x, yOrigin, x, y);
		x+=cellSize;
	}

	x=xOrigin+gridSizeX;
	y=yOrigin;
	for (err=0; err<=cellsY; err++) {
		WinDrawLine(xOrigin, y, x, y);
		y+=cellSize;
	}

	if (ui->colorEnabled)
		WinSetForeColor(ict);

	for (y=0; y<cellsY; y++) {
		for (x=0; x<cellsX; x++) {
			switch (grid[(y*cellsX)+x]) {
			case 0:
				break;
			case 1:
				RctSetRectangle(&rct, xOrigin+(x*cellSize)+1, yOrigin+(y*cellSize)+1, cellSize-1, cellSize-1);
				WinDrawRectangle(&rct, 0);
				break;
			default:
				if (ui->colorEnabled)
					ict=WinSetForeColor(WinRGBToIndex(&greyRGB));

				WinDrawLine(xOrigin+(x*cellSize)+cellSize, yOrigin+(y*cellSize), xOrigin+(x*cellSize), yOrigin+(y*cellSize)+cellSize);

				if (ui->colorEnabled)
					WinSetForeColor(ict);
				break;
			}
		}
	}

	DrawAxes(quizData);

	WinSetDrawWindow(oldW);
	MemHandleUnlock(puzzleDataH);

	if (pageSizeX>ScrW)
		limitX=pageSizeX-ScrW;
	else
		limitX=0;

	if (pageSizeY>ScrH)
		limitY=pageSizeY-ScrH;
	else
		limitY=0;

	return true;
}

/*
 * FindOrigin
 *
 * Calculate origin.
 *
 *  -> data		Quiz data.
 * <-  oX		Origin, X.
 * <-  oY		Origin, Y.
 */
static void
FindOrigin(UInt8 *data, UInt16 *oX, UInt16 *oY)
{
	UInt16 w=0, max=0, t, i, x;
	Char buffer[4];

	for (t=0; t<cellsX; t++) {
		w=(UInt16)*data++;
		if (w>max)
			max=w;

		data+=w;
	}

	*oY=max*(fontSizeY+1);

	max=0;
	for (t=0; t<cellsY; t++) {
		w=(UInt16)*data++;
		x=0;
		for (i=0; i<w; i++) {
			StrPrintF(buffer, "%u", (UInt16)*data);
			x+=(StrLen(buffer)*fontSizeX)+3;
			data++;
		}

		if (x>max)
			max=x;
	}

	*oX=max;
}

/*
 * DrawAxes
 *
 * Draw the axes.
 *
 *  -> data		Quiz data.
 */
static void
DrawAxes(UInt8 *data)
{
	UInt16 x, y, idx, n, w, t, i;
	Char buffer[4];
	FontID fID;
	UInt8 *p;
			
	if (!prefs.cellSize)
		fID=FntSetFont(smallNumbersFont);
	else
		fID=FntSetFont(largeNumbersFont);

	x=xOrigin+1;
	for (n=0; n<cellsX; n++) {
		t=*data++;
		y=yOrigin-(t*(fontSizeY+1));
		for (; t>0; t--) {
			StrPrintF(buffer, "%u", (UInt16)*data);
			idx=StrLen(buffer);
			w=(cellSize-(FntCharsWidth(buffer, idx)-1))/2;

			WinDrawChars(buffer, idx, x+w, y);
		
			y+=fontSizeY+1;
			data++;
		}

		x+=cellSize;
	}

	y=yOrigin+((cellSize-fontSizeY)/2);
	for (n=0; n<cellsY; n++) {
		t=*data++;
		x=xOrigin+1;
		p=data;
		for (i=0; i<t; i++) {
			StrPrintF(buffer, "%u", (UInt16)*p);
			x-=(StrLen(buffer)*fontSizeX)+3;
			p++;
		}

		for (; t>0; t--) {
			StrPrintF(buffer, "%u", (UInt16)*data);
			idx=StrLen(buffer);

			WinDrawChars(buffer, idx, x, y);
			x+=(idx*fontSizeX)+3;
			data++;
		}

		y+=cellSize;
	}

	FntSetFont(fID);
}

/*
 * BlitGrid
 *
 * Draw the Grid to the screen.
 */
static void
BlitGrid(void)
{
	if (gridWin) {
		RectangleType rct;

		RctSetRectangle(&rct, topX, topY, ScrW, ScrH);
		WinCopyRectangle(gridWin, NULL, &rct, xOffset, yOffset, winPaint);
	}
}

/*
 * UpdatePen
 *
 * Update the "Draw:" pen image
 */
static void
UpdatePen(void)
{
	MemHandle mh=DmGetResource(bitmapRsc, bmpBlank+pen);

	if (!mh)
		return;

	WinDrawBitmap(MemHandleLock(mh), 134, 2);
	MemHandleUnlock(mh);
	DmReleaseResource(mh);
}

/*
 * UpdateTitle
 *
 * Update the title bar.
 */
static void
UpdateTitle(void)
{
	UInt8 *p=MemHandleLock(puzzleDataH);
	FontID fID=FntSetFont(tinyFont);
	Char title[64];
	UInt16 tLen;

	StrNCopy(title, p+3, sizeof(title));
	StrNCat(title, " / ", sizeof(title));
	p+=StrLen(p+3)+4;
	StrNCat(title, p, sizeof(title));

	p=title;
	for (tLen=0; (*p) && (tLen<sizeof(title)); tLen++) {
		if (*p>'Z')
			*p=*p&223;

		p++;
	}
	WinDrawChars(title, tLen, (ScrW-FntCharsWidth(title, tLen))/2, 155);

	FntSetFont(fID);
	MemHandleUnlock(puzzleDataH);
}

/*
 * PuzzleCompleted
 *
 * Is puzzle completed?
 *
 * Returns true if puzzle is completed, false otherwise.
 */
static Boolean
PuzzleCompleted(void)
{
	UInt8 *p=MemHandleLock(puzzleDataH);
	Boolean ret;

	if ((ret=pc(p))==true) {
		if (*p)
			DmSet(p, 0, 1, 0);
	}
	MemHandleUnlock(puzzleDataH);

	return ret;
}

/*
 * pc
 *
 * Piggyback for PuzzleData.
 */
static Boolean
pc(UInt8 *p)
{
	p+=3;
	p+=StrLen(p)+1;
	p+=StrLen(p)+1;
	p+=StrLen(p)+1;
	if (CheckAll(cellsX, cellsY, cellsX, 1, &p)==false) {
		checkIsX=true;
		return false;
	}

	if (CheckAll(cellsY, cellsX, 1, cellsX, &p)==false) {
		checkIsX=false;
		return false;
	}

	return true;
}

/**
 * Check if a puzzle is solved.
 *
 * @param size1 Size of a row/column.
 * @param size2 Size of the other.
 * @param add Add to pointer.
 * @param start Start multiplier.
 * @param pp Puzzle pointer (will be updated).
 * @return True if solved in this direction, false otherwise.
 */
static Boolean
CheckAll(UInt16 size1, UInt16 size2, UInt16 add, UInt16 start, UInt8 **pp)
{
	UInt8 *p=*pp, *c;
	UInt8 count[102];
	UInt16 t, idx, cIdx, nums;

	for (offender=0; offender<size1; offender++) {
		cIdx=0;
		idx=offender*start;
		nums=(UInt16)(*p++);
		MemSet(count, sizeof(count), 0);
		if (*p) {
			for (t=0; t<size2; t++) {
				if (grid[idx]==1)
					count[cIdx]++;
				else
					cIdx++;

				idx+=add;
			}

			c=count;
			t=0;
			for (idx=0; idx<sizeof(count); idx++) {
				if (*c) {
					if (*c!=*p)
						return false;

					t++;
					p++;
				}

				c++;
			}

			if (t!=nums)
				return false;
		} else {
			for (t=0; t<size2; t++) {
				if (grid[idx]==1)
					return false;

				idx+=add;
			}

			p++;
		}
	}

	*pp=p;
	return true;
}
