/*
 * fMain.c
 */
#include "Include.h"

/* protos */
_FRM(static void DrawList(Int16, RectangleType *, Char **));
_FRM(static Int16 fMainFindClosest(Char));

/* globals */
static Int16 saveIdx;
static DmOpenRef db;
static UInt16 dbCard, topicsCnt;
static LocalID dbID;
static MemHandle topicsH, crossH, bodyH;
static Char *topics, *body;
static UInt16 *cross;

/**
 * Draw the topics list.
 *
 * @param item Item num.
 * @param rct Rectangle.
 * @param unused Just that ...
 */
static void
DrawList(Int16 item, RectangleType *rct, Char **unused)
{
	WinEraseRectangle(rct, 0);

	if (crossH) {
		UInt16 pW=rct->extent.x, len;
		Char *n;
		Boolean trunc;

		n=topics+cross[item*2];
		len=StrLen(n);
		FntCharsInWidth(n, &pW, &len, &trunc);
		WinDrawChars(n, len, rct->topLeft.x, rct->topLeft.y);
	}
}

/**
 * Open a Plua help file.
 *
 * @param v Plua help file version.
 */
void
fMainOpen(UInt16 v)
{
	Char *name;

	state.helpFile=v;
	if (!v) {
		dbCard=ph1Card;
		dbID=ph1ID;
		name=ph1Name;
	} else {
		dbCard=ph2Card;
		dbID=ph2ID;
		name=ph2Name;
	}

	db=DmOpenDatabase(dbCard, dbID, dmModeReadOnly);
	ErrFatalDisplayIf(db==NULL, "(fMainOpen) Cannot open database");

	topicsH=DmGetResource((UInt32)'hlpW', 0x03e9);
	ErrFatalDisplayIf(topicsH==NULL, "(fMainOpen) Cannot open topics word list from help file");
	topics=MemHandleLock(topicsH);

	bodyH=DmGetResource((UInt32)'hlpD', 0x03e9);
	ErrFatalDisplayIf(topicsH==NULL, "(fMainOpen) Cannot open description from help file");
	body=MemHandleLock(bodyH);

	crossH=DmGetResource((UInt32)'hlpI', 0x03e9);
	ErrFatalDisplayIf(topicsH==NULL, "(fMainOpen) Cannot open cross-index from help file");
	cross=MemHandleLock(crossH);

	topicsCnt=MemHandleSize(crossH)/4;

	FrmGotoForm(fMain);
}

/**
 * Close the database.
 */
void
fMainClose(void)
{
	if (crossH) {
		MemHandleUnlock(crossH);
		DmReleaseResource(crossH);
		crossH=NULL;
	}

	if (bodyH) {
		MemHandleUnlock(bodyH);
		DmReleaseResource(bodyH);
		bodyH=NULL;
	}

	if (topicsH) {
		MemHandleUnlock(topicsH);
		DmReleaseResource(topicsH);
		topicsH=NULL;
	}

	if (db) {
		DmCloseDatabase(db);
		db=NULL;
	}
}

/*
 * fMainInit
 */
Boolean
fMainInit(void)
{
	ListType *l=UIObjectGet(cMainList);

	LstSetSelection(l, state.helpFile);
	CtlSetLabel(UIObjectGet(cMainPopup), LstGetSelectionText(l, state.helpFile));

	UIListInit(cMainTopics, DrawList, topicsCnt, saveIdx);
	return true;
}

/*
 *	fMainEH
 */
Boolean
fMainEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case lstSelectEvent:
		if (ev->data.lstSelect.selection!=noListSelection) {
			saveIdx=ev->data.lstSelect.selection;
			fTopicRun(topics+cross[saveIdx*2], body+cross[(saveIdx*2)+1]);
			return true;
		}
		break;

	case popSelectEvent:
		if (ev->data.popSelect.listID==cMainList) {
			if ((ev->data.popSelect.selection!=ev->data.popSelect.priorSelection) && (ev->data.popSelect.selection!=noListSelection)) {
				if (!ev->data.popSelect.selection) {
					if (ph1ID) {
						fMainClose();
						fMainOpen(0);
					} else {
						LstSetSelection(ev->data.popSelect.listP, 0);
						FrmAlert(aNotInstalled);
						return true;
					}
				} else {
					if (ph2ID) {
						fMainClose();
						fMainOpen(1);
					} else {
						LstSetSelection(ev->data.popSelect.listP, 1);
						FrmAlert(aNotInstalled);
						return true;
					}
				}
			}
		}
		break;

	case keyDownEvent:
		if (UIListKeyHandler(ev, cMainTopics, ui->navSupported)==false) {
			if (TxtGlueCharIsPrint(ev->data.keyDown.chr)) {
				UIListUpdateFocus(cMainTopics, fMainFindClosest(ev->data.keyDown.chr));
				return true;
			}
		} else
			return true;

		return false;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mMainAbout:
			fAboutRun();
			return true;
		}
		break;

	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/**
 * Find the closest matching topic.
 *
 * @param ltr First letter.
 * @return index.
 */
static Int16
fMainFindClosest(Char ltr)
{
	Char str[2];
	UInt16 idx;
	Int16 ret;

	str[0]=ltr;
	str[1]=0;

	for (idx=0; idx<topicsCnt; idx++) {
		if ((ret=StrNCaselessCompare(str, topics+cross[idx*2], 1))<=0)
			return idx;
	}

	return idx;
}
