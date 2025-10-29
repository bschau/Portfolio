#include "sit.h"

/* globals */
static int oldeditflag;

/* protos */
static Boolean evh(EventPtr);
static Boolean selev(EventPtr, FormPtr);

/*******************************************************************************
*
*	editentryinit
*
*	Initializes the editentry form.
*
*	Input:
*		form - form.
*/
void
editentryinit(FormPtr form)
{
	int err=0;
	ControlType *ctl;

	FrmSetEventHandler(form, (FormEventHandlerPtr)evh);
	FrmSetTitle(form, formtitle);
	oldeditflag=editflag;
	if (editflag) {
		editflag=0;
		err=setfieldtext(form, eefdescf, trec.key);
		err|=setfieldtext(form, eefcontf, trec.txt);
		if (err) {
			gotostart();
			return;
		}
	}

	newlist(&entrylist, catlist.idx);
	ctl=FrmGetObjectPtr(form, FrmGetObjectIndex(form, eefcatpopup));
	CategorySetTriggerLabel(ctl, entrylist.name);

	FrmSetFocus(form, FrmGetObjectIndex(form, eefdescf));
}

/*******************************************************************************
*
*	evh
*
*	Event handler.
*
*	Input:
*		ev - event.
*	Output:
*		handled: true.
*		not handled: false.
*/
static Boolean
evh(EventPtr ev)
{
	FormPtr form;

	form=FrmGetActiveForm();

	switch (ev->eType) {
		case frmOpenEvent:
			FrmDrawForm(form);
			return true;
		case ctlSelectEvent:
			return selev(ev, form);
		case menuEvent:
			return handleeditmenu(ev, form);
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
*
*	Input:
*		ev - event.
*		form - form.
*	Output:
*		handled: true.
*		not handled: false.
*/
static Boolean
selev(EventPtr ev,
      FormPtr form)
{
	Boolean handled=false;
	UInt16 ccat;
	Char *key, *txt;
	char bs[BSSIZE];

	switch (ev->data.ctlSelect.controlID) {
		case eefcatpopup:
			ccat=entrylist.idx;
			CategorySelect(sitdb, form, eefcatpopup, eefcatlist, false, &entrylist.idx, entrylist.name, CATNONEDITABLES, 0);
			handled=true;
			break;
		case eefok:
			handled=true;
			key=FldGetTextPtr((FieldType *)findobject(eefdescf));
			if (key) {
				txt=FldGetTextPtr((FieldType *)findobject(eefcontf));
				if (txt) {
					recpack(bs, key, txt);
					if (oldeditflag && editindex!=noListSelection) {
						DmRemoveRecord(sitdb, editindex);
						editindex=noListSelection;
					}
					catlist.idx=entrylist.idx;
					ccat=insertrecord(bs, entrylist.idx);
					if (ccat==true)
						gotostart();
					else
						FrmCustomAlert(alertoom, NULL, NULL, NULL);

					return true;
				}
			}

			FrmCustomAlert(alertwrongentries, NULL, NULL, NULL);
			break;

		case eefcancel:
			gotostart();
			return true;
	}

	return handled;
}
