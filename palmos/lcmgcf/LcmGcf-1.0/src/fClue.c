/*
 * fClue.c
 */
#include "Include.h"

/* protos */
static Boolean EH(EventType *);
static void SetupClue(void);
static void AddToClue(Char *);
static void PageUp(void);
static void PageDown(void);
static void DrawControls(void);

/* globals */
static Gadget gList[]={ { cClueUp, "\x01" }, { cClueDown, "\x02" }, { 0 } };
static MemHandle clueMH=NULL;
static Char *clue;
static Boolean runLoop;
static UInt16 clueCnt, pos;


/*
 * fClueClose
 *
 * Deallocate Clue form resources.
 */
void
fClueClose(void)
{
	if (clueMH)
		MemHandleFree(clueMH);
}

/*
 * fClueRun
 *
 * Show the "Clue" screen.
 */
void
fClueRun(void)
{
	FormType *saveCurrent=currentForm;
	UInt16 saveID=currentFormID;
	FormActiveStateType frmSave;
	EventType ev;
	UInt16 err;

	session->clues++;
	if (clueMH==NULL) {
		clueMH=MemHandleNew(ClueTextLength);
		ErrFatalDisplayIf(clueMH==NULL, "(fClueRun) Out of memory");
		clue=MemHandleLock(clueMH);
	}

	clueCnt=0;
	pos=0;

	SetupClue();

	FrmSaveActiveState(&frmSave);

	currentFormID=fClue;
	currentForm=FrmInitForm(currentFormID);
	ErrFatalDisplayIf(currentForm==NULL, "(fClueRun) Cannot initialize form.");
	FrmSetActiveForm(currentForm);
	FrmSetEventHandler(currentForm, (FormEventHandlerType *)EH);

	FldSetMaxChars(UIObjectGet(cClueField), clueCnt);
	UIFieldSetText(cClueField, clue);

	MemSet(&ev, sizeof(EventType), 0);
	ev.eType=frmOpenEvent;
	ev.data.frmLoad.formID=currentFormID;
	EvtAddEventToQueue(&ev);
	
	runLoop=true;
	while (runLoop) {
		EvtGetEvent(&ev, evtWaitForever);
		if (ev.eType==appStopEvent) {
			EvtAddEventToQueue(&ev);
			appStopped=true;
			break;
		} else if (ev.eType==frmCloseEvent)
			break;

		if (SysHandleEvent(&ev)==true ||
		    MenuHandleEvent(0, &ev, &err)==true)
			continue;

		FrmDispatchEvent(&ev);
	}

	FrmEraseForm(currentForm);
	FrmDeleteForm(currentForm);
	FrmRestoreActiveState(&frmSave);
	currentForm=saveCurrent;
	currentFormID=saveID;
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	MemHandle mh;
	RectangleType r;
	Char buffer[16];

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		FntSetFont(stdFont);
		RctSetRectangle(&r, 1, 1, 50, 12);
		WinEraseRectangle(&r, 0);
		StrPrintF(buffer, "#%lu", session->clues);
		WinDrawChars(buffer, StrLen(buffer), 1, 1);

		mh=DmGetResource(iconType, 1000);
		ErrFatalDisplayIf(mh==NULL, "(EH) Logo bitmap not found");
		WinDrawBitmap(MemHandleLock(mh), 54, 2);
		MemHandleUnlock(mh);
		DmReleaseResource(mh);


		DrawControls();
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cClueStop:
			runLoop=false;
			return true;
		}
		break;

	case penDownEvent:
		switch (UIGadgetFontHandler(&gList[0], symbol7Font, ev->screenX, ev->screenY)) {
		case cClueUp:
			PageUp();
			return true;
		case cClueDown:
			PageDown();
			return true;
		default:
			break;

		}
		return false;
		
	case keyDownEvent:
		switch (ev->data.keyDown.chr) {
		case pageUpChr:
			PageUp();
			return true;
		case pageDownChr:
			PageDown();
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * SetupClue
 *
 * Setup the clue message.
 */
static void
SetupClue(void)
{
	UInt16 max=(session->defOperations&PrfNumbers ? 3 : 2), cnt;
	Char buffer[16];

	if (session->operation&PrfQuizTypeLcm) {
		AddToClue("LCM: Least Common Multiple\n\nThe Least Common Multiple of two numbers is the smallest number (not zero) that is a multiple of both.\n");

		for (cnt=0; cnt<max; cnt++) {
			Boolean sep=false;
			UInt16 m, c;

			AddToClue("\nMultiples of ");
			StrPrintF(buffer, "%u: ", session->nums[cnt]);
			AddToClue(buffer);

			m=session->nums[cnt];
			for (c=0; c<10; c++) {
				if (sep)
					StrPrintF(buffer, ", %u", m);
				else {
					StrPrintF(buffer, "%u", m);
					sep=true;
				}

				AddToClue(buffer);
				m+=session->nums[cnt];
			}
			AddToClue(", ...");
		}

		AddToClue("\n");
	} else {
		AddToClue("GCF: Greatest Common Factor\n\nThe Greatest Common Factor of two or more whole numbers is the largest whole number that divides evenly into each of the numbers.\n\n");

		for (cnt=0; cnt<max; cnt++) {
			UInt16 m, c;

			AddToClue("\nFactors of ");
			StrPrintF(buffer, "%u: 1", session->nums[cnt]);
			AddToClue(buffer);

			m=session->nums[cnt];
			for (c=2; c<m; c++) {
				if ((m%c)==0) {
					StrPrintF(buffer, ", %u", c);
					AddToClue(buffer);
				}
			}
			if (m!=1) {
				StrPrintF(buffer, ", %u", m);
				AddToClue(buffer);
			}
		}

		AddToClue("\n");
	}
}

/*
 * AddToClue
 *
 * Add text to clue.
 *
 *  -> src		Source text.
 */
static void
AddToClue(Char *src)
{
	UInt16 srcLen=StrLen(src);

	if (clueCnt+srcLen>(ClueTextLength-1))
		return;

	MemMove(clue+clueCnt, src, srcLen);
	clueCnt+=srcLen;
	clue[clueCnt]=0;
}

/*
 * PageUp
 *
 * Scroll page up one line.
 */
static void
PageUp(void)
{
	FldScrollField(UIObjectGet(cClueField), 1, winUp);
	DrawControls();
}

/*
 * PageDown
 *
 * Scroll page down one line.
 */
static void
PageDown(void)
{
	FldScrollField(UIObjectGet(cClueField), 1, winDown);
	DrawControls();
}

/*
 * DrawControls
 *
 * Draw the up/down buttons.
 */
static void
DrawControls(void)
{
	FontID fID=FntSetFont(symbol7Font);
	RectangleType r;

	FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cClueUp), &r);

	if (FldScrollable(UIObjectGet(cClueField), winUp)==true) {
		CtlSetEnabled(UIObjectGet(cClueUp), true);
		WinDrawChars("\x01", 1, r.topLeft.x, r.topLeft.y);
	} else {
		CtlSetEnabled(UIObjectGet(cClueUp), false);
		WinDrawChars("\x03", 1, r.topLeft.x, r.topLeft.y);
	}

	FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cClueDown), &r);
	if (FldScrollable(UIObjectGet(cClueField), winDown)==true) {
		CtlSetEnabled(UIObjectGet(cClueDown), true);
		WinDrawChars("\x02", 1, r.topLeft.x, r.topLeft.y);
	} else {
		CtlSetEnabled(UIObjectGet(cClueDown), false);
		WinDrawChars("\x04", 1, r.topLeft.x, r.topLeft.y);
	}

	FntSetFont(fID);
}
