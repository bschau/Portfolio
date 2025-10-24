#include "sit.h"

/* globals */
static UInt16 eeidx;
static MemHandle eemh;
static sitlist entrylist;

/* protos */
static Boolean evh(EventPtr);
static Boolean selev(EventPtr, FormPtr);

/*******************************************************************************
*
*	preparetoedit
*
*	Set up edit/new dialog.
*/
void
preparetoedit(UInt16 idx, MemHandle mh)
{
	eeidx=idx;
	eemh=mh;

	if (idx==dmMaxRecordIndex)
		StrNCopy(formtitle, "New Secret", FORMTITLELEN);
	else
		StrNCopy(formtitle, "Edit Secret", FORMTITLELEN);

	FrmGotoForm(formeditentry);
}

/*******************************************************************************
*
*	editentryinit
*
*	Initializes the editentry form.
*/
void
editentryinit(FormPtr form)
{
	ControlType *ctl;
	UInt16 recattr;
	Boolean err;

	FrmSetEventHandler(form, (FormEventHandlerPtr)evh);
	FrmSetTitle(form, formtitle);
	if (eemh) {
		err=populate2fieldsmemhandle(form, eemh, eeftitlef, eefsecretf);
		MemHandleFree(eemh);
		if (err==false) {
			gotostart();
			return;
		}
	}

	if (eeidx==dmMaxRecordIndex) {
		if (catlist.idx==dmAllCategories)
			entrylist.idx=dmUnfiledCategory;
		else
			entrylist.idx=catlist.idx&dmRecAttrCategoryMask;
	} else {
		if (eemh==NULL) {
			if (populate2fields(form, sitdb, eeidx, eeftitlef, eefsecretf)==false) {
				gotostart();
				return;
			}
		}

		DmRecordInfo(sitdb, eeidx, &recattr, NULL, NULL);
		entrylist.idx=recattr&dmRecAttrCategoryMask;
	}

	CategoryGetName(sitdb, entrylist.idx, entrylist.name);
	ctl=FrmGetObjectPtr(form, FrmGetObjectIndex(form, eefcatpopup));
	CategorySetTriggerLabel(ctl, entrylist.name);

	updatescrollbar(form, eeftitlescrb, eeftitlef);
	updatescrollbar(form, eefsecretscrb, eefsecretf);

	FrmSetFocus(form, FrmGetObjectIndex(form, eeftitlef));
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
	Boolean handled=false;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(form);
		return true;
	case frmSaveEvent:
		saveentry(TypeSecret, eeidx, FldGetTextPtr(objectinform(form, eeftitlef)), FldGetTextPtr(objectinform(form, eefsecretf)));
		return true;
	case ctlSelectEvent:
		return selev(ev, form);
	case keyDownEvent:
		handled=scrollbarkeyhandler(ev, eeftitlescrb, eeftitlef);
		if (handled==true)
			break;
		handled=scrollbarkeyhandler(ev, eefsecretscrb, eefsecretf);
		if (handled==true)
			break;

		return refocus(ev, form, eeftitlef, eefsecretf);
	default:
		handled=scrollbarhandler(ev, eeftitlescrb, eeftitlef);
		if (handled==false)
			scrollbarhandler(ev, eefsecretscrb, eefsecretf);

		handled=false;	/* to make sclRepeat work */
		break;
	}

	return handled;
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
	UInt16 ccat;
	UInt8 *key, *txt;
	Int16 kf, tf;

	switch (ev->data.ctlSelect.controlID) {
	case eefcatpopup:
		ccat=entrylist.idx;
		CategorySelect(sitdb, form, eefcatpopup, eefcatlist, false, &entrylist.idx, entrylist.name, CATNONEDITABLES, 0);
		return true;
	case eefok:
		key=FldGetTextPtr(objectinform(form, eeftitlef));
		kf=fieldblank(key);
		txt=FldGetTextPtr(objectinform(form, eefsecretf));
		tf=fieldblank(txt);
		if (kf || tf) {
			FrmCustomAlert(alertwrongentries, NULL, NULL, NULL);
			return true;
		}

		catlist.idx=entrylist.idx;
		if (storerecord(sitdb, key, txt, catlist.idx, eeidx)==true)
			gotostart();
		else
			FrmCustomAlert(alertoom, NULL, NULL, NULL);

		return true;

	case eeftemplate:
		selecttemplate(form, eefsecretf);
		return true;

	case eefcancel:
		gotostart();
		return true;
	}

	return false;
}
