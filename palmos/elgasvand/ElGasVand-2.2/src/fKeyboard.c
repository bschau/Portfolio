/*
 * fKeyboard.c
 */
#include "Include.h"

/* protos */
static Boolean EH(EventType *);
static void Insert(WChar);
static void HandleOK(void);

/* globals */
static Boolean runLoop, cancelled;
static Char integer[16];

/*
 * fKeyboardRun
 *
 * Show the Numeric keyboard.
 *
 * <-> dstUInt32	Where to store destination integer and where to read.
 *  -> titleID		Form title resource id.
 *
 * Returns true if the OK button was pressed, false if keyboard was cancelled.
 */
Boolean
fKeyboardRun(UInt32 *dstUInt32, UInt16 titleID)
{
	FormType *saveCurrent=currentForm;
	UInt16 saveID=currentFormID;
	MemHandle titMH=DmGetResource(strRsc, titleID);
	FormActiveStateType frmSave;
	EventType ev;
	UInt16 err;

	FrmSaveActiveState(&frmSave);

	currentFormID=fKeyboard;
	currentForm=FrmInitForm(currentFormID);
	ErrFatalDisplayIf(currentForm==NULL, "(fKeyboardRun) Cannot initialize form.");
	FrmSetActiveForm(currentForm);
	FrmSetEventHandler(currentForm, (FormEventHandlerType *)EH);

	FrmSetTitle(currentForm, MemHandleLock(titMH));

	if (*dstUInt32)
		StrPrintF(integer, "%lu", *dstUInt32);
	else
		*integer=0;

	MemSet(&ev, sizeof(EventType), 0);
	ev.eType=frmOpenEvent;
	ev.data.frmLoad.formID=currentFormID;
	EvtAddEventToQueue(&ev);

	runLoop=true;
	cancelled=false;
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

	MemHandleUnlock(titMH);
	DmReleaseResource(titMH);

	if (cancelled)
		return false;

	*dstUInt32=StrAToI(integer);
	return true;
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	UInt16 id;
	WChar key;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		if (*integer)
			UIFieldSetText(cKeyboardField, integer);
		UIFieldFocus(cKeyboardField);
		return true;

	case ctlSelectEvent:
		id=ev->data.ctlSelect.controlID;
		if ((id>=cKeyboardBut0) && (id<=cKeyboardBut9)) {
			Insert((id-cKeyboardBut0)+'0');
			return true;
		}
	       
		switch (id) {
		case cKeyboardButBS:
			Insert(8);
			return true;
		case cKeyboardButC:
			Insert(1);
			return true;
		case cKeyboardButCancel:
			cancelled=true;
			runLoop=false;
			return true;
		case cKeyboardButOK:	/* FALL-THRU */
			HandleOK();
			return true;
		}
		break;

		
	case keyDownEvent:
		key=ev->data.keyDown.chr;
		if ((key>='0') && (key<='9'))
			Insert(key);
		else if (key==8)
			Insert(key);
		else if (key==10)
			HandleOK();

		return true;

	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * Insert
 *
 * Insert the char into the field.
 *
 *  -> c		Char.
 */
static void
Insert(WChar c)
{
	Char buffer[16];
	Char *p;
	UInt16 l;

	MemSet(buffer, sizeof(buffer), 0);
	if ((c>='0') && (c<='9')) {
		if ((p=UIFieldGetText(cKeyboardField))==NULL)
			*buffer=c;
		else {
			if ((l=StrLen(p))>0)
				MemMove(buffer, p, l);

			buffer[l]=c;
		}
	} else if (c==8) {
		if ((p=UIFieldGetText(cKeyboardField))) {
			if ((l=StrLen(p))>0)
				MemMove(buffer, p, l-1);
		}
	}
	
	UIFieldSetText(cKeyboardField, buffer);
}

/*
 * HandleOK
 *
 * Things to do when OK is pressed.
 */
static void
HandleOK(void)
{
	Char *p=UIFieldGetText(cKeyboardField);
	UInt32 len;

	runLoop=false;
	if (p && ((len=StrLen(p))>0)) {
		MemMove(integer, p, len);
		integer[len]=0;
	} else
		cancelled=true;
}
