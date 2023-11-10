#include "occman.h"

/* protos */
static Boolean ViewFormEventHandler(EventPtr ev);

/* globals */
static Int16 occIndex;
static Boolean didChange;

/*
**	ViewOCC
**
**	View the properties for this OCC.
**
**	 -> item	Item no.# in occlist.
**
**	Returns true if item was deleted, false otherwise.
*/
Boolean
ViewOCC(Int16 item)
{
	FormPtr oFrm=currentForm, vFrm;
	OCCNode *n;
	UInt16 card, type;
	LocalID id;
	MemHandle urlH;
	Boolean ret;
	Char *typeText;

	occIndex=item;
	didChange=false;
	for (n=occList.head; n->next && item; n=n->next)
		item--;

	if ((ret=OCCGetByName(n->name, &card, &id))==false) {
		FrmCustomAlert(aNoSuchOCC, n->name, NULL, NULL);
		return false;
	}

	if ((urlH=OCCGetUrl(card, id))==NULL) {
		FrmCustomAlert(aNoUrl, n->name, NULL, NULL);
		return false;
	}

	vFrm=FrmInitForm(fView);
	ErrFatalDisplayIf(vFrm==NULL, "(ViewOCC) Cannot initialize View form.");
	currentForm=vFrm;

	FrmSetActiveForm(currentForm);
	FrmSetEventHandler(currentForm, (FormEventHandlerPtr)ViewFormEventHandler);
	FrmDrawForm(currentForm);

	UISetFieldText(fViewUrl, MemHandleLock(urlH));
	MemHandleFree(urlH);

	UIUpdateScrollbar(fViewScrollbar, fViewUrl);

	if ((OCCIsStandalone(card, id, &type))==true)
		typeText=(type==OCCTypeStandalone) ? "Yes" : "No";
	else
		typeText="Not known";

	UISetFieldText(fViewStandalone, typeText);
	FrmDoDialog(currentForm);

	FrmEraseForm(vFrm);
	FrmDeleteForm(vFrm);
	currentForm=oFrm;
	FrmSetActiveForm(currentForm);

	return didChange;
}

/*
**	ViewFormEventHandler
**
**	Event handler for View form.
**
**	 -> ev		Event to handle.
**
**	Returns true if event is handled, false otherwise.
*/
static Boolean
ViewFormEventHandler(EventPtr ev)
{
	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case fViewDelete:
			if ((FrmCustomAlert(aDelete, NULL, NULL, NULL))==1)
				return true;

			didChange=OCCDelete(&occList, occIndex);
			return false;
		case fViewEdit:
			didChange=EditOCC(occIndex);
			return false;
		}
		break;

	default:
		if (UIScrollbarKeyHandler(ev, fViewScrollbar, fViewUrl)==true)
			return true;

		UIScrollbarHandler(ev, fViewScrollbar, fViewUrl);
		/* do not return true here as it disables sclRepeat */
		break;
	}
	return false;
}
