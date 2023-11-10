#include "sit.h"

/* globals */
static Int16 listsel=noListSelection, scrollspeed;
static UInt16 eeidx, selected;
static MemHandle eemh;

/* protos */
static void listdrawfunc(Int16, RectangleType *, Char **);
static void rethinkscrollspeed(void);
static Boolean evh(EventPtr);
static Boolean selev(EventPtr, FormPtr);
static Boolean teevh(EventPtr);
static Boolean teselev(EventPtr, FormPtr);
static Boolean stevh(EventPtr);
static void settemplate(FormPtr, UInt16);

/*******************************************************************************
*
*	listdrawfunc
*
*	Updates/redraws templates list.
*/
static void
listdrawfunc(Int16 itemnum,
             RectangleType *rect,
             Char **data)
{
	dynamiclistdraw(tmpldb, itemnum, rect);
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
	if (prefs.flags&PREFPAGESCROLL)
		scrollspeed=9;
	else
		scrollspeed=1;
}

/*******************************************************************************
*
*	templateeditorinit
*
*	Template editor and selector.  Modelled on 'Edit Categories ...'
*/
void
templateeditorinit(FormPtr form)
{
	buildlist(form, telist, tmpldb, 0, listdrawfunc);
	FrmSetEventHandler(form, (FormEventHandlerPtr)evh);
	rethinkscrollspeed();
}

/*******************************************************************************
*
*	evh
*
*	Event handler.
*/
static Boolean
evh(EventPtr ev)
{
	FormPtr form=FrmGetActiveForm();
	ListType *lp;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(form);
		lp=objectinform(form, telist);
		LstSetSelection(lp, listsel);
		return true;

	case lstSelectEvent:
		lp=objectinform(form, telist);
		listsel=LstGetSelection(lp);
		if (listsel!=noListSelection)
			edittemplate(listsel, NULL);
		return true;

	case keyDownEvent:
		switch (ev->data.keyDown.chr) {
		case vchrPageUp:
			lp=objectinform(form, telist);
			LstScrollList(lp, winUp, scrollspeed);
			return true;
		case vchrPageDown:
			lp=objectinform(form, telist);
			LstScrollList(lp, winDown, scrollspeed);
			return true;
		default:
			break;
		}
		break;

	case ctlSelectEvent:
		return selev(ev, form);

	default:
		break;
	}

	return false;
}

/*******************************************************************************
*
*	selev
*
*	Handle select events.
*/
static Boolean
selev(EventPtr ev,
      FormPtr form)
{
	switch (ev->data.ctlSelect.controlID) {
	case teok:
		gotostart();
		return true;
	case tenew:
		edittemplate(dmMaxRecordIndex, NULL);
		return true;
	case teimport:
		importmemo(form, IMPORTTOTEMPLATES, NULL);
		buildlist(form, telist, tmpldb, 0, listdrawfunc);
		LstDrawList(objectinform(form, telist));
		return true;
	case teexport:
		exporttomemo(form, EXPORTFROMTEMPLATES);
		buildlist(form, telist, tmpldb, 0, listdrawfunc);
		LstDrawList(objectinform(form, telist));
		return true;
	}

	return false;
}

/*******************************************************************************
*
*	edittemplate
*
*	Edit or create template.
*/
void
edittemplate(UInt16 idx, MemHandle mh)
{
	eeidx=idx;
	eemh=mh;

	if (eeidx==dmMaxRecordIndex)
		StrNCopy(formtitle, "New Template", FORMTITLELEN);
	else
		StrNCopy(formtitle, "Edit Template", FORMTITLELEN);

	FrmGotoForm(formedittemplate);
}

/*******************************************************************************
*
*	edittemplateinit
*
*	Initializes the edit template form.
*/
void
edittemplateinit(FormPtr form)
{
	Boolean err;

	FrmSetEventHandler(form, (FormEventHandlerPtr)teevh);
	FrmSetTitle(form, formtitle);
	if (eemh) {
		err=populate2fieldsmemhandle(form, eemh, teetitlef, teetemplatef);
		MemHandleFree(eemh);
		if (err==false) {
			FrmGotoForm(formtemplateeditor);
			return;
		}
	}

	if (eeidx!=dmMaxRecordIndex) {
		if (eemh==NULL) {
			if ((err=populate2fields(form, tmpldb, eeidx, teetitlef, teetemplatef))==false) {
				FrmGotoForm(formtemplateeditor);
				return;
			}
		}

		FrmShowObject(form, FrmGetObjectIndex(form, teedelete));
	}

	updatescrollbar(form, teetitlescrb, teetitlef);
	updatescrollbar(form, teetemplatescrb, teetemplatef);
	FrmSetFocus(form, FrmGetObjectIndex(form, teetitlef));
}

/*******************************************************************************
*
*	teevh
*
*	Template Edit Event handler.
*/
static Boolean
teevh(EventPtr ev)
{
	FormPtr form=FrmGetActiveForm();
	Boolean handled;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(form);
		return true;
	case frmSaveEvent:
		saveentry(TypeTemplate, eeidx, FldGetTextPtr(objectinform(form, teetitlef)), FldGetTextPtr(objectinform(form, teetemplatef)));
		return true;
	case ctlSelectEvent:
		return teselev(ev, form);
	case keyDownEvent:
		handled=scrollbarkeyhandler(ev, teetitlescrb, teetitlef);
		if (handled==true)
			return true;

		handled=scrollbarkeyhandler(ev, teetemplatescrb, teetemplatef);
		if (handled==true)
			return true;
		
		return refocus(ev, form, teetitlef, teetemplatef);
	default:
		handled=scrollbarhandler(ev, teetitlescrb, teetitlef);
		if (handled==false)
			scrollbarhandler(ev, teetemplatescrb, teetemplatef);

		handled=false;	/* sclRepeat */
		break;
	}

	return handled;
}

/*******************************************************************************
*
*	teselev
*
*	Handle Template Edit select events.
*/
static Boolean
teselev(EventPtr ev,
        FormPtr form)
{
	UInt8 *titp, *tmplp;
	Int16 t1, t2;

	switch (ev->data.ctlSelect.controlID) {
	case teeok:
		titp=FldGetTextPtr(objectinform(form, teetitlef));
		t1=fieldblank(titp);
		tmplp=FldGetTextPtr(objectinform(form, teetemplatef));
		t2=fieldblank(tmplp);
		if (t1 || t2) {
			FrmCustomAlert(alertwrongentries, NULL, NULL, NULL);
			return true;
		}

		if (storerecord(tmpldb, titp, tmplp, 0, eeidx)==true)
			FrmGotoForm(formtemplateeditor);
		else
			FrmCustomAlert(alertoom, NULL, NULL, NULL);

		return true;
	case teecancel:
		FrmGotoForm(formtemplateeditor);
		return true;
	case teedelete:
		if ((deletedialog(form)))
			DmRemoveRecord(tmpldb, eeidx);

		FrmGotoForm(formtemplateeditor);
		return true;
	}

	return false;
}

/*******************************************************************************
*
*	selecttemplate
*
*	Show template dialog.  Let user choose ...
*/
void
selecttemplate(FormPtr parform,
               UInt16 dstfld)
{
	FormPtr form=FrmInitForm(formselecttemplate);
	UInt16 ever=1, button;

	FrmSetEventHandler(form, (FormEventHandlerPtr)stevh);
	for (; ever; ) {
		FrmSetActiveForm(form);
		buildlist(form, stlist, tmpldb, 0, listdrawfunc);

		selected=noListSelection;
		button=FrmDoDialog(form);
		if (button==stok) {
			if (selected==noListSelection)
				FrmCustomAlert(alertnoneselected, NULL, NULL, NULL);
			else {
				settemplate(parform, dstfld);
				ever=0;
			}
		} else
			ever=0;
	}

	FrmEraseForm(form);
	FrmDeleteForm(form);
	FrmSetActiveForm(parform);
	FrmDrawForm(parform);
}

/*******************************************************************************
*
*	stevh
*
*	Select Template Event handler.
*/
static Boolean
stevh(EventPtr ev)
{
	switch (ev->eType) {
	case lstSelectEvent:
		selected=LstGetSelection(objectinform(FrmGetActiveForm(), stlist));
		return true;
	default:
		break;
	}

	return false;
}

/*******************************************************************************
*
*	settemplate
*
*	Copy template data to field.
*/
static void
settemplate(FormPtr form,
            UInt16 fid)
{
	MemHandle rec, td;
	UInt8 *dst;
	FieldType *f;
	recdesc rd;

	rec=DmQueryRecord(tmpldb, selected);
	if (!rec) {
		FrmCustomAlert(alertqueryfailed, NULL, NULL, NULL);
		return;
	}

	/* fast forward */
	recorddesc(&rd, MemHandleLock(rec));

	td=MemHandleNew(rd.secretlen+1);
	if (!td) {
		FrmCustomAlert(alertoom, NULL, NULL, NULL);
		MemHandleUnlock(rec);
		return;
	}

	dst=MemHandleLock(td);

	MemMove(dst, rd.secret, rd.secretlen+1);
	MemHandleUnlock(rec);

	decrypt(dst, rd.secretlen);
	
	MemHandleUnlock(td);

	f=objectinform(form, fid);
	rec=FldGetTextHandle(f);
	FldSetTextHandle(f, td);

	if (rec)
		MemHandleFree(rec);
}
