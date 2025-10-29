#include "sit.h"

/* protos */
static void buildlist(void);
static void listdrawfunc(Int16, RectangleType *, Char **);
static Boolean startevh(EventPtr);
static Boolean startselev(EventPtr, FormPtr);
static Boolean startmenuev(EventPtr, FormPtr);

/*******************************************************************************
*
*	startinit
*
*	Initializes the start form.
*
*	Input:
*		form - form.
*/
void
startinit(FormPtr form)
{
	ControlType *ctl;

	buildlist();

	ctl=FrmGetObjectPtr(form, FrmGetObjectIndex(form, sfcatpopup));
	CategoryGetName(sitdb, catlist.idx, catlist.name);
	CategorySetTriggerLabel(ctl, catlist.name);

	FrmSetEventHandler(form, (FormEventHandlerPtr)startevh);
}

/*******************************************************************************
*
*	buildlist
*	
*	Build the mainlist.
*/
static void
buildlist(void)
{
	ListPtr lp;
	UInt16 items;

	items=DmNumRecordsInCategory(sitdb, catlist.idx);
	lp=(ListPtr)findobject(sfmainlist);
	LstSetListChoices(lp, NULL, items);
	LstSetSelection(lp, 0);
	LstSetDrawFunction(lp, listdrawfunc);
}

/*******************************************************************************
*
*	listdrawfunc
*
*	Updates main list.
*
*	Input:
*		itemnum - item number to be drawn.
*		rect - bounding rectangle.
*		strings - data.
*/
static void
listdrawfunc(Int16 itemnum,
             RectangleType *rect,
	     Char **data)
{
	UInt16 slen, pw;
	Boolean trunc;
	MemHandle mh;
	unsigned char rec[EKEYLEN+1];
	MemPtr k;
	Err err;

	pw=0;
	err=DmSeekRecordInCategory(sitdb, &pw, itemnum, dmSeekForward, catlist.idx);
	if (err!=errNone)
		return;

	mh=DmQueryRecord(sitdb, pw);
	if (mh) {
		k=MemHandleLock(mh);
		MemMove(rec, (char *)k, EKEYLEN);
		MemHandleUnlock(mh);
		*(rec+EKEYLEN)='\x00';

		slen=StrLen(rec);
		pw=rect->extent.x;

		FntCharsInWidth(rec, &pw, &slen, &trunc);
		WinDrawChars(rec, slen, rect->topLeft.x, rect->topLeft.y);
	}
}

/*******************************************************************************
*
*	startevh
*
*	Start form event handler.
*
*	Input:
*		ev - event.
*	Output:
*		handled: true.
*		not handled: false.
*/
static Boolean
startevh(EventPtr ev)
{
	Boolean handled=false;
	FormPtr form;
	ListPtr lp;
	Int16 rn, nxt;

	form=FrmGetActiveForm();
	switch (ev->eType) {
		case frmOpenEvent:
			FrmDrawForm(form);
			return true;

		case lstSelectEvent:
			lp=(ListPtr)findobject(sfmainlist);
			rn=LstGetSelection(lp);
			nxt=showrecord(rn, catlist.idx, form);
			switch (nxt) {
				case 1:	/* edit */
					editflag=1;
					StrNCopy(formtitle, "Edit entry", FORMTITLELEN);
					FrmGotoForm(formeditentry);
					break;
				case 2:
					buildlist();
					LstDrawList(lp);
					break;

			}
			handled=true;
			break;

		case ctlSelectEvent:
			handled=startselev(ev, form);
			break;

		case menuEvent:
			handled=startmenuev(ev, form);
			break;

		default:
			break;
	}

	return handled;
}

/*******************************************************************************
*
*	startselev
*
*	Handle select events.
*
*	Input:
*		ev - event.
*		form - form.
*	Output:
*		handled: true.
*		not handled: false.
*/
static Boolean
startselev(EventPtr ev,
           FormPtr form)
{
	Boolean handled=false, catedit;
	UInt16 ccat;
	ListPtr lp;

	switch (ev->data.ctlSelect.controlID) {
		case sfcatpopup:
			ccat=catlist.idx;
			catedit=CategorySelect(sitdb, form, sfcatpopup, sfcatlist, false, &catlist.idx, catlist.name, CATNONEDITABLES, 0);
			if (catedit || (ccat!=catlist.idx)) {
				buildlist();
				lp=(ListPtr)findobject(sfmainlist);
				LstDrawList(lp);
			}
			handled=true;
			break;
			
		case sfnew:
			StrNCopy(formtitle, "New entry", FORMTITLELEN);
			FrmGotoForm(formeditentry);
			break;
	}

	return handled;
}

/*******************************************************************************
*
*	startmenuev
*
*	Handle menu events.
*
*	Input:
*		ev - event.
*		form - form.
*	Output:
*		handled: true.
*		not handled: false.
*/
static Boolean
startmenuev(EventPtr ev,
            FormPtr form)
{
	switch (ev->data.menu.itemID) {
		case sfmvisual:
			visual(form);
			break;
		case sfmsecurity:
			security(form);
			break;
		case sfmchpwd:
			about(form, formaboutchangepwd);
			getpassword(PASSDOUBLE);
			break;
		case sfmabout:
			MenuEraseStatus(NULL);
			about(form, formaboutsit);
			break;
		case sfmalgorithms:
			MenuEraseStatus(NULL);
			about(form, formaboutalgorithms);
			break;
		case sfmsupport:
			MenuEraseStatus(NULL);
			about(form, formaboutsupport);
			break;
		case sfmthanks:
			MenuEraseStatus(NULL);
			about(form, formaboutthanks);
			break;
		default:
			return false;
	}

	return true;
}

/*******************************************************************************
 *
 *	about
 *
 *	Handle abouts.
 *
 * 	Input:
 * 		parform - parent form.
 * 		adf - about dialog form.
 */
void
about(FormPtr parform,
      UInt16 adf)
{
	FormPtr tfrm;
	
	tfrm=FrmInitForm(adf);
	FrmSetActiveForm(tfrm);

	FrmDoDialog(tfrm);
	if (parform)
		FrmSetActiveForm(parform);

	FrmDeleteForm(tfrm);
}

/*******************************************************************************
 *
 *	gotostart
 *
 *	Goto start form.
 */
void
gotostart(void)
{
	if (prefs.quickaccess)
		FrmGotoForm(formstart);
	else
		FrmGotoForm(formstartnqa);
}
