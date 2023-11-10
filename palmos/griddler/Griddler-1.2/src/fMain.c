/*
 * fMain.c
 */
#include "Include.h"

/* protos */
static void DrawPuzzleList(Int16, RectangleType *, Char **);
static void DrawPCList(Int16, RectangleType *, Char **);
static void fInfoRun(UInt8 *);

/*
 * DrawPuzzleList
 */
static void
DrawPuzzleList(Int16 itemNum, RectangleType *rct, Char **unused)
{
	if (dbPuzzle.db) {
		MemHandle mh=DBGetRecord(&dbPuzzle, itemNum);
		Int16 width=rct->extent.x, sLen;
		Char buffer[16];
		FontID fID;
		Boolean fit;
		UInt8 *p;

		WinEraseRectangle(rct, 0);

		p=MemHandleLock(mh);
		fID=FntSetFont(stdFont+(UInt16)*p);
		sLen=StrLen(p+3);
		FntCharsInWidth(p+3, &width, &sLen, &fit);
		WinDrawChars(p+3, sLen, rct->topLeft.x, rct->topLeft.y);
		FntSetFont(fID);

		StrPrintF(buffer, " (%u x %u)", (UInt16)*(p+1), (UInt16)*(p+2));
		sLen=StrLen(buffer);
		width=FntCharsWidth(buffer, sLen);
		WinDrawChars(buffer, sLen, rct->topLeft.x+rct->extent.x-width, rct->topLeft.y);

		MemHandleUnlock(mh);
	}
}

/*
 * DrawPCList
 */
static void
DrawPCList(Int16 itemNum, RectangleType *rct, Char **unused)
{
	WinEraseRectangle(rct, 0);
	WinDrawChars(pcList[itemNum].name, StrLen(pcList[itemNum].name), rct->topLeft.x, rct->topLeft.y);
}

/*
 * fMainInit
 */
Boolean
fMainInit(void)
{
	Int16 idx;

	PCBuildList();

	if (state.lsPCList>pcCnt) {
		state.lsPCList=noListSelection;
		state.lsPuzzleList=noListSelection;
	}

	UIListInit(cMainPCList, DrawPCList, pcCnt, state.lsPCList);

	if (state.lsPCList==noListSelection)
		UIListInit(cMainPuzzleList, DrawPuzzleList, 0, noListSelection);
	else {
		UInt32 recs=0;

		DmDatabaseSize(pcList[state.lsPCList].cardNo, pcList[state.lsPCList].dbID, &recs, NULL, NULL);
		DBClose(&dbPuzzle);
		DBInit(&dbPuzzle, pcList[state.lsPCList].name);
		if (DBOpen(&dbPuzzle, dmModeReadWrite, true)==false)
			recs=0;

		if (recs) {
			if (state.lsPuzzleList>recs)
				idx=noListSelection;
			else
				idx=state.lsPuzzleList;
		} else
			idx=0;

		UIListInit(cMainPuzzleList, DrawPuzzleList, (Int16)recs, idx);
	}

	return true;
}

/*
 * fMainEH
 */
Boolean
fMainEH(EventType *ev)
{
	Int16 idx;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cMainPlay:
			if ((idx=LstGetSelection(UIObjectGet(cMainPCList)))!=noListSelection) {
				Int16 pIdx;

				if ((pIdx=LstGetSelection(UIObjectGet(cMainPuzzleList)))!=noListSelection) {
					MemHandle mh=DBGetRecord(&dbPuzzle, pIdx);
					UInt8 *p;

					state.lsPCList=idx;
					state.lsPuzzleList=pIdx;

					MemSet(&puzzle, sizeof(Puzzle), 0);
					StrCopy(puzzle.currentPuzzle, pcList[idx].name);
					DmRecordInfo(dbPuzzle.db, pIdx, NULL, &puzzle.currentPuzzleId, NULL);
					p=MemHandleLock(mh);
					puzzle.gridSize=((*(p+1))<<8)|(*(p+2));
					MemHandleUnlock(mh);

					PMSetPref(NULL, 0, PrfGrid, false);
					FrmGotoForm(fGame);
					return true;
				}
			}
			FrmAlert(aSelectPuzzle);
			break;
		case cMainInfo:
			if ((idx=LstGetSelection(UIObjectGet(cMainPCList)))!=noListSelection) {
				Int16 pIdx;

				if ((pIdx=LstGetSelection(UIObjectGet(cMainPuzzleList)))!=noListSelection) {
					MemHandle mh=DBGetRecord(&dbPuzzle, pIdx);

					fInfoRun(MemHandleLock(mh));
					MemHandleUnlock(mh);
					UIFormRedraw();
					return true;
				}
			}

			FrmAlert(aSelectPuzzle);
			break;
		}
		break;

	case lstSelectEvent:
		if (ev->data.lstSelect.selection==noListSelection)
			break;

		if (ev->data.lstSelect.listID==cMainPCList) {
			UInt32 recs=0;

			idx=ev->data.lstSelect.selection;
			DmDatabaseSize(pcList[idx].cardNo, pcList[idx].dbID, &recs, NULL, NULL);
			DBClose(&dbPuzzle);
			DBInit(&dbPuzzle, pcList[idx].name);
			if (DBOpen(&dbPuzzle, dmModeReadWrite, true)==false)
				recs=0;

			UIListInit(cMainPuzzleList, DrawPuzzleList, (Int16)recs, recs ? 0 : noListSelection);
			LstDrawList(UIObjectGet(cMainPuzzleList));
			return true;
		}
		break;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mMainDelete:
			if ((idx=LstGetSelection(UIObjectGet(cMainPCList)))!=noListSelection) {
				if (FrmAlert(aCollectionDelete)==0) {
					DBClose(&dbPuzzle);
					LstSetSelection(UIObjectGet(cMainPCList), noListSelection);
					DmDeleteDatabase(pcList[idx].cardNo, pcList[idx].dbID);

					PCBuildList();

					UIListInit(cMainPCList, DrawPCList, pcCnt, noListSelection);
					UIListInit(cMainPuzzleList, DrawPuzzleList, 0, noListSelection);

				}

				UIFormRedraw();
				return true;
			}
			break;

		case mMainAbout:
			fAboutRun();
			UIFormRedraw();
			return true;
		}
		break;
	case keyDownEvent:
		return UIListKeyHandler(ev, cMainPCList);
	default:
		break;
	}

	return false;
}

/*
 * fInfoRun
 *
 * Show the info screen.
 *
 *  -> data		Puzzle data.
 */
static void
fInfoRun(UInt8 *data)
{
	Char buffer[65];
	FormSave frm;

	UIFormPrologue(&frm, fInfo, NULL);

	data+=3;
	StrNCopy(buffer, data, 64);
	data+=StrLen(data)+1;
	if (StrLen(buffer)>0)
		UIFieldSetText(cInfoName, buffer);
	else
		UIFieldSetText(cInfoName, "???");

	StrNCopy(buffer, data, 64);
	data+=StrLen(data)+1;
	if (StrLen(buffer)>0)
		UIFieldSetText(cInfoAuthor, buffer);
	else
		UIFieldSetText(cInfoAuthor, "???");

	StrNCopy(buffer, data, 64);
	if (StrLen(buffer)>0)
		UIFieldSetText(cInfoSite, buffer);
	else
		UIFieldSetText(cInfoSite, "???");

	UIFormEpilogue(&frm, NULL, 0);
}
