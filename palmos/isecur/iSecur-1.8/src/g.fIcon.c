/*
 * fIcon.c
 */
#include "Include.h"

/* protos */
_UTL(static Boolean EH(EventType *));
_UTL(static void DrawIcons(void));
_UTL(static void PageUp(void));
_UTL(static void PageDown(void));
_UTL(static void LeaveIcons(void));

/* globals */
extern Char *tArrUp, *tArrDown, *tArrUpDis, *tArrDownDis;
extern Int32 sIdx, iIdx, maxLines;
extern Boolean bIconSelected;
extern DmResID *dstIconID;
extern DmOpenRef dbRefIcons;
extern UInt16 cntIcons, inIcons, iX, iY;

/*
 * fIconOpen
 *
 * Open the iSecur Icons set.
 */
void
fIconOpen(void)
{
	LocalID id=DmFindDatabase(0, "iSecur Icons");

	dbRefIcons=NULL;
	if (id) {
		UInt32 type=0, creator=0;

		DmDatabaseInfo(0, id, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &type, &creator);
		if ((type=='Rsrc') && (creator=='Actn'))
			dbRefIcons=DmOpenDatabase(0, id, dmModeReadOnly);
	}

	/* Try the default icon database .. */
	if (!dbRefIcons)
		dbRefIcons=DmOpenDatabaseByTypeCreator('Rsrc', 'Actn', dmModeReadOnly);

	if (dbRefIcons)
		cntIcons=DmNumResources(dbRefIcons);
	else
		cntIcons=0;

	maxLines=cntIcons/IMW;
	if (cntIcons%IMW)
		maxLines++;

//	maxLines--;
//	if (maxLines<0)
//		maxLines=0;
}

/*
 * fIconClose
 *
 * Close the iSecur Icons set.
 */
void
fIconClose(void)
{
	if (dbRefIcons)
		DmCloseDatabase(dbRefIcons);
}

/*
 * fIconRun
 *
 * Show the Select Icon window.
 *
 * <-> iid		Original Icon ID & where to store new Icon ID.
 *
 * Returns true if an icon was selected, false otherwise.
 */
Boolean
fIconRun(DmResID *iid)
{
	FormType *saveCurrent=currentForm;
	UInt16 saveID=currentFormID;
	FormActiveStateType frmSave;
	EventType ev;
	UInt16 err;

	dstIconID=iid;
	if (*iid!=noIconSelected) {
		if ((sIdx=DmFindResource(dbRefIcons, bitmapRsc, *iid, NULL))==dmMaxRecordIndex)
			sIdx=0;
	} else
		sIdx=0;

	iIdx=sIdx/IMW;
	iIdx-=(IMH/2);
	if (iIdx<0)
		iIdx=0;

	inIcons=0;
	FrmSaveActiveState(&frmSave);

	currentFormID=fIcon;
	currentForm=FrmInitForm(currentFormID);
	ErrFatalDisplayIf(currentForm==NULL, "(fIconRun) Cannot initialize form.");
	FrmSetActiveForm(currentForm);
	FrmSetEventHandler(currentForm, (FormEventHandlerType *)EH);

	MemSet(&ev, sizeof(EventType), 0);
	ev.eType=frmOpenEvent;
	ev.data.frmLoad.formID=currentFormID;
	EvtAddEventToQueue(&ev);
	
	bIconSelected=false;
	while (!bIconSelected) {
		EvtGetEvent(&ev, evtWaitForever);
		if (ev.eType==appStopEvent) {
			EvtAddEventToQueue(&ev);
			appStopped=true;
			break;
		} else if (ev.eType==frmCloseEvent)
			break;

		if (PMHardPower(&ev)==true)
			continue;

		if (SysHandleEvent(&ev)==true ||
		    MenuHandleEvent(0, &ev, &err)==true)
			continue;

		FrmDispatchEvent(&ev);
	}

	if (inIcons)
		LeaveIcons();

	FrmEraseForm(currentForm);
	FrmDeleteForm(currentForm);
	FrmRestoreActiveState(&frmSave);
	currentForm=saveCurrent;
	currentFormID=saveID;
	return bIconSelected;
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	RectangleType r;

	EntropyAdd(pool, ev);

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		DrawIcons();
		if (inIcons) {
			RectangleType rct;

			rct.topLeft.x=(iX*18)+IconX-1;
			rct.topLeft.y=((iY-iIdx)*12)+IconY-1;
			rct.extent.x=12;
			rct.extent.y=11;

			FrmGlueNavDrawFocusRing(currentForm, cIconIcon, 0, &rct, frmNavFocusRingStyleObjectTypeDefault, true);
		}
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cIconNone:
			*dstIconID=noIconSelected;
			bIconSelected=true;
			EvtWakeup();
			return true;
		case cIconUp:
			PageUp();
			return true;
		case cIconDown:
			PageDown();
			return true;
		}
		break;

	case penDownEvent:
		RctSetRectangle(&r, IconX, IconY, IconW, IconH);
		if (RctPtInRectangle(ev->screenX, ev->screenY, &r)) {
			UInt16 x=ev->screenX-IconX, y=ev->screenY-IconY;

			if (((x%18)<10) && ((y%12)<9)) {
				UInt16 idx=((y/12)*8)+(x/18)+(iIdx*IMW);
				DmResType type;

				if (idx<cntIcons) {
					if ((DmResourceInfo(dbRefIcons, idx, &type, dstIconID, NULL)==errNone) && (type==bitmapRsc)) {
						bIconSelected=true;
						EvtWakeup();
					}
				}
			}
			return true;
		}
		break;

	case frmObjectFocusTakeEvent:
		if (ev->data.frmObjectFocusTake.objectID==cIconIcon) {
			RectangleType rct;

			FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cIconIcon), &rct);
			FrmGlueNavDrawFocusRing(currentForm, cIconIcon, 0, &rct, frmNavFocusRingStyleObjectTypeDefault, true);
			FrmSetFocus(currentForm, FrmGetObjectIndex(currentForm, cIconIcon));
			return true;
		}
		break;

	case frmObjectFocusLostEvent:
		if (ev->data.frmObjectFocusLost.objectID==cIconIcon) {
			FrmSetFocus(currentForm, noFocus);
			return true;
		}
		break;

	case keyDownEvent:
		switch (ev->data.keyDown.chr) {
		case pageUpChr:
			PageUp();
			return true;
		case pageDownChr:
			PageDown();
			return true;
		case vchrHardRockerCenter:	// vchrRockerCenter?
		case vchrRockerCenter:
			if (FrmGetObjectId(currentForm, FrmGetFocus(currentForm))==cIconIcon) {
				if (inIcons) {
					UInt16 idx=(iY*IMW)+iX;
					DmResType type;

					if ((DmResourceInfo(dbRefIcons, idx, &type, dstIconID, NULL)==errNone) && (type==bitmapRsc)) {
						bIconSelected=true;
						EvtWakeup();
					}
				} else {
					iX=0;
					iY=iIdx;
					inIcons=1;
					FrmUpdateForm(fIcon, frmRedrawUpdateCode);
				}

				return true;
			}
			break;
		case vchrRockerUp:
			if (inIcons) {
				if (iY>0) {
					iY--;
					if (iY<iIdx)
						iIdx=iY;

					FrmUpdateForm(fIcon, frmRedrawUpdateCode);
				} else
					LeaveIcons();

				return true;
			}
			break;
		case vchrRockerDown:
			if (inIcons) {
				if (iY<(maxLines-1)) {
					iY++;
					if (iY>(iIdx+(IMH-1)))
						iIdx=iY-(IMH-1);

					FrmUpdateForm(fIcon, frmRedrawUpdateCode);
				} else
					LeaveIcons();

				return true;
			}
			break;
		case vchrRockerLeft:
			if (inIcons) {
				if (iX>0) {
					iX--;
					FrmUpdateForm(fIcon, frmRedrawUpdateCode);
				}
				return true;
			}
			break;
		case vchrRockerRight:
			if (inIcons) {
				if (iX<(IMW-1)) {
					iX++;
					FrmUpdateForm(fIcon, frmRedrawUpdateCode);
				}
				return true;
			}
		default:	/* FALL-THRU */
			break;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * DrawIcons
 *
 * Draw icons.
 */
static void
DrawIcons(void)
{
	UInt16 yPt=IconY, xPt, y, x;
	Int32 rIdx;
	ControlType *ctl;
	MemHandle mh;
	UInt32 type;
	RectangleType r;

	ctl=UIObjectGet(cIconUp);
	if (iIdx>0) {
		CtlSetEnabled(ctl, true);
		CtlSetLabel(ctl, tArrUp);
	} else {
		CtlSetEnabled(ctl, false);
		CtlSetLabel(ctl, tArrUpDis);
	}

	RctSetRectangle(&r, IconX-2, IconY-2, IconW+4, IconH+4);
	WinEraseRectangle(&r, 0);

	rIdx=iIdx*IMW;
	r.extent.x=12;
	r.extent.y=11;
	for (y=0; y<IMH; y++) {
		xPt=IconX;
		r.topLeft.y=yPt-1;
		for (x=0; x<8; x++) {
			r.topLeft.x=xPt-1;
			if (rIdx<cntIcons) {
				if ((DmResourceInfo(dbRefIcons, (UInt16)rIdx, &type, NULL, NULL)==errNone) && (type==bitmapRsc)) {
					if ((mh=DmGetResourceIndex(dbRefIcons, (UInt16)rIdx))) {
						WinDrawBitmap(MemHandleLock(mh), xPt, yPt);
						MemHandleUnlock(mh);
						DmReleaseResource(mh);
					}
				}

				if (rIdx==sIdx && *dstIconID!=noIconSelected)
					WinDrawRectangleFrame(simpleFrame, &r);

				rIdx++;
			}
			xPt+=18;
		}

		yPt+=12;
	}

	ctl=UIObjectGet(cIconDown);
	if (iIdx<(maxLines-(IMH))) {
		CtlSetEnabled(ctl, true);
		CtlSetLabel(ctl, tArrDown);
	} else {
		CtlSetEnabled(ctl, false);
		CtlSetLabel(ctl, tArrDownDis);
	}
}

/*
 * PageUp
 *
 * Scroll page up.
 */
static void
PageUp(void)
{
	if (iIdx>0) {
		iIdx--;
		if (inIcons)
			iY--;

		FrmUpdateForm(fIcon, frmRedrawUpdateCode);
	}
}

/*
 * PageDown
 *
 * Scroll page down.
 */
static void
PageDown(void)
{
	if (iIdx<(maxLines-IMH)) {
		iIdx++;
		if (inIcons)
			iY++;

		FrmUpdateForm(fIcon, frmRedrawUpdateCode);
	}
}

/**
 * Nav leave icons.
 */
static void
LeaveIcons(void)
{
	inIcons=0;
	FrmGlueNavObjectTakeFocus(currentForm, cIconNone);
}
