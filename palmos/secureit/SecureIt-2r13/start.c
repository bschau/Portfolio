#include "sit.h"

/* globals */
static Int16 listsel=noListSelection, scrollspeed;

/* protos */
static void listdrawfunc(Int16, RectangleType *, Char **);
static Boolean startevh(EventPtr);
static Boolean startselev(EventPtr, FormPtr);
static Boolean startmenuev(EventPtr, FormPtr);
static void rethinkscrollspeed(void);

/*******************************************************************************
*
*	startinit
*
*	Initializes the start form.
*/
void
startinit(FormPtr form)
{
	ControlType *ctl;

	buildlist(form, sfmainlist, sitdb, catlist.idx, listdrawfunc);

	ctl=FrmGetObjectPtr(form, FrmGetObjectIndex(form, sfcatpopup));
	CategoryGetName(sitdb, catlist.idx, catlist.name);
	CategorySetTriggerLabel(ctl, catlist.name);

	rethinkscrollspeed();
	FrmSetEventHandler(form, (FormEventHandlerPtr)startevh);
}

/*******************************************************************************
*
*	listdrawfunc
*
*	Updates main list.
*/
static void
listdrawfunc(Int16 itemnum,
             RectangleType *rect,
	     Char **data)
{
	UInt16 item=0;
	Err err;

	err=DmSeekRecordInCategory(sitdb, &item, itemnum, dmSeekForward, catlist.idx);
	if (err==errNone)
		dynamiclistdraw(sitdb, item, rect);
}

/*******************************************************************************
*
*	startevh
*
*	Start form event handler.
*/
static Boolean
startevh(EventPtr ev)
{
	UInt16 item=0;
	FormPtr form;
	ListPtr lp;
	Int16 err;

	form=FrmGetActiveForm();
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(form);
		lp=objectinform(form, sfmainlist);
		LstSetSelection(lp, listsel);
		return true;

	case lstSelectEvent:
		lp=objectinform(form, sfmainlist);
		listsel=LstGetSelection(lp);
		if (listsel==noListSelection)
			return true;

		err=DmSeekRecordInCategory(sitdb, &item, listsel, dmSeekForward, catlist.idx);
		if (err!=errNone)
			return true;

		showrecord(item);
		return true;

	case ctlSelectEvent:
		return startselev(ev, form);

	case keyDownEvent:
		switch (ev->data.keyDown.chr) {
		case vchrPageUp:
			lp=objectinform(form, sfmainlist);
			LstScrollList(lp, winUp, scrollspeed);
			return true;
		case vchrPageDown:
			lp=objectinform(form, sfmainlist);
			LstScrollList(lp, winDown, scrollspeed);
			return true;

		default:
			break;
		}
		break;
	case menuEvent:
		return startmenuev(ev, form);

	default:
		break;
	}

	return false;
}

/*******************************************************************************
*
*	startselev
*
*	Handle select events.
*/
static Boolean
startselev(EventPtr ev,
           FormPtr form)
{
	Boolean catedit;
	UInt16 ccat;

	switch (ev->data.ctlSelect.controlID) {
	case sfcatpopup:
		ccat=catlist.idx;
		catedit=CategorySelect(sitdb, form, sfcatpopup, sfcatlist, true, &catlist.idx, catlist.name, CATNONEDITABLES, 0);
		if (catedit || (ccat!=catlist.idx)) {
			buildlist(form, sfmainlist, sitdb, catlist.idx, listdrawfunc);
			LstDrawList(objectinform(form, sfmainlist));
		}
		return true;
			
	case sfnew:
		preparetoedit(dmMaxRecordIndex, NULL);
		return true;
	}

	return false;
}

/*******************************************************************************
*
*	startmenuev
*
*	Handle menu events.
*/
static Boolean
startmenuev(EventPtr ev,
            FormPtr form)
{
	UInt16 memimp, dstcat;
	UInt8 name[dmCategoryLength];

	switch (ev->data.menu.itemID) {
	case sfmfrommemo:
		memimp=importmemo(form, IMPORTTOSECRETS, &dstcat);
		if (memimp) {	/* switch to category */
			catlist.idx=dstcat;
			buildlist(form, sfmainlist, sitdb, catlist.idx, listdrawfunc);
			CategoryGetName(sitdb, catlist.idx, name);
			CategorySetTriggerLabel(objectinform(form, sfcatpopup), name);
			LstDrawList(objectinform(form, sfmainlist));
		}
		break;
	case sfmtomemo:
		exporttomemo(form, EXPORTFROMSECRETS);
		buildlist(form, sfmainlist, sitdb, catlist.idx, listdrawfunc);
		LstDrawList(objectinform(form, sfmainlist));
		break;
	case sfmtemplates:
		FrmGotoForm(formtemplateeditor);
		break;
	case sfmvisual:
		visual(form);
		rethinkscrollspeed();
		break;
	case sfmsecurity:
		security(form);
		break;
	case sfmchpwd:
		infobox(form, formaboutchangepwd);
		getpassword(PASSDOUBLE);
		break;
	case sfmabout:
		MenuEraseStatus(NULL);
		FrmGotoForm(formabout);
		break;
	default:
		return false;
	}

	return true;
}

/*******************************************************************************
*
*	rethinkscrollspeed
*
*	Calculate new scrollspeed based on users settings.
*/
static void
rethinkscrollspeed(void)
{
	if (prefs.flags&PREFPAGESCROLL) {
		if (prefs.flags&PREFQUICKACCESS)
			scrollspeed=7;
		else
			scrollspeed=10;
	} else
		scrollspeed=1;
}

/*******************************************************************************
*
*	infobox
*
*	Handle infobox.
*/
void
infobox(FormPtr parform,
        UInt16 adf)
{
	FormPtr tfrm;
	
	tfrm=FrmInitForm(adf);
	FrmSetActiveForm(tfrm);

	FrmDoDialog(tfrm);
	if (parform)
		FrmSetActiveForm(parform);

	FrmEraseForm(tfrm);
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
	if (prefs.flags&PREFQUICKACCESS)
		FrmGotoForm(formstart);
	else
		FrmGotoForm(formstartnqa);
}
