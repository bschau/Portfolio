#include "sit.h"

/* macros */
#define MAXMEMOSIZE 4096

/* globals */
static UInt16 categoryfrom, categoryto;
static sitlist fromlist, tolist;

/* protos */
static Boolean evh(EventPtr);
static UInt16 runexport(Int16, UInt16, UInt16);
static Boolean export(UInt8 *);

/*******************************************************************************
*
*	exporttomemo
*
*	Exports secrets from selected category to Memos 'unfiled' category.
*	This is crippled.  It works - but it is not userfriendly  :-(
*	Bugs guaranteed!
*/
void
exporttomemo(FormPtr parform,
             UInt16 fromtype)
{
	FormPtr tfrm;
	ControlType *ctl;
	UInt16 button, mr, exported;

	memodb=openmemodb();
	if (!memodb)
		return;

	categoryfrom=0;
	categoryto=0;
	if (fromtype==EXPORTFROMSECRETS) {
		tfrm=FrmInitForm(formexportsecrets);
		CategoryGetName(sitdb, dmUnfiledCategory, fromlist.name);
		ctl=FrmGetObjectPtr(tfrm, FrmGetObjectIndex(tfrm, fexcatpopupfrom));
		CategorySetTriggerLabel(ctl, fromlist.name);
	} else
		tfrm=FrmInitForm(formexporttemplates);

	FrmSetEventHandler(tfrm, (FormEventHandlerPtr)evh);
	FrmSetActiveForm(tfrm);

	CategoryGetName(memodb, dmUnfiledCategory, tolist.name);
	ctl=FrmGetObjectPtr(tfrm, FrmGetObjectIndex(tfrm, fexcatpopupto));
	CategorySetTriggerLabel(ctl, tolist.name);

	button=FrmDoDialog(tfrm);

	switch (button) {
	case fexexport:
		mr=DmNumRecordsInCategory(((fromtype==EXPORTFROMSECRETS) ? sitdb : tmpldb), categoryfrom);
		if (mr) {
			working(parform, mr);
			ctl=objectinform(tfrm, fexcheck);
			exported=runexport(CtlGetValue(ctl), fromtype, mr);
			stopworking();
		} else
			FrmCustomAlert(alertemptycategory, NULL, NULL, NULL);
		/* FALL-THRU */
	default:
		break;
	}
	
	FrmEraseForm(tfrm);
	FrmDeleteForm(tfrm);
	FrmSetActiveForm(parform);

	DmCloseDatabase(memodb);
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

	if (ev->eType==ctlSelectEvent) {
		switch (ev->data.ctlSelect.controlID) {
		case fexcatpopupfrom:
			CategorySelect(sitdb, form, fexcatpopupfrom, fexcatlistfrom, true, &categoryfrom, fromlist.name, 0, 0);
			return true;
		case fexcatpopupto:
			CategorySelect(memodb, form, fexcatpopupto, fexcatlistto, false, &categoryto, tolist.name, 0, 0);
			return true;
		}
	}

	return false;
}

/*******************************************************************************
*
*	runexport
*
*	Export to memo - main.
*/
static UInt16
runexport(Int16 delete,
          UInt16 fromtype,
          UInt16 items)
{
	Int16 idx=0, exported=0, lauf=1;
	Err err;
	UInt8 buffer[64];
	MemHandle mh;
	Boolean exp;
	DmOpenRef db;

	if (fromtype==EXPORTFROMSECRETS)
		db=sitdb;
	else
		db=tmpldb;

	err=DmSeekRecordInCategory(db, &idx, 0, dmSeekForward, categoryfrom);
	if (err==errNone) {
		for (EVER) {
			mh=DmQueryNextInCategory(db, &idx, categoryfrom);
			if (mh) {
				StrPrintF(buffer, "Exporting %i of %i %s ...", lauf, items, ((fromtype==EXPORTFROMSECRETS) ? "secrets" : "templates"));
				updatevisual(buffer);
				exp=export(MemHandleLock(mh));
				MemHandleUnlock(mh);

				if (exp==true) {
					exported++;
					if (delete) {
						DmRemoveRecord(db, idx);
						idx=-1;
					}
				}
				idx++;
			} else
				break;
			lauf++;
		}
	}

	StrPrintF(buffer, "%i", exported);
	if (fromtype==EXPORTFROMSECRETS)
		FrmCustomAlert(alertexportinfo, buffer, "secret", ((exported==1) ? " " : "s "));
	else
		FrmCustomAlert(alertexportinfo, buffer, "template", ((exported==1) ? " " : "s "));

	return exported;
}

/*******************************************************************************
*
*	export
*
*	Do the actual exporting.  Clone record, rebuild, store in memodb.
*/
static Boolean
export(UInt8 *src)
{
	UInt16 attr=dmUnfiledCategory|dmRecAttrDirty, idx;
	UInt8 funchar1='\n', funchar2='\x00';
	UInt32 clen, tlen, klen, i;
	MemHandle mh, rech;
	UInt8 *mp, *rp;

	clen=GETOFFSET(src);
	tlen=clen;
	klen=StrLen(src+tlen+3);

	if ((clen+klen+2)>MAXMEMOSIZE)
		clen=((MAXMEMOSIZE-2-klen+7)/8)*8;

	mh=MemHandleNew(clen);
	if (!mh)
		return false;

	mp=MemHandleLock(mh);
	MemMove(mp, src+2, clen);

	decrypt(mp, clen);

	idx=DmNumRecords(memodb);
	rech=DmNewRecord(memodb, &idx, clen+klen+2);	/* '\n' & NUL */
	if (!rech) {
		MemHandleFree(mh);
		return false;
	}

	rp=MemHandleLock(rech);
	i=0;
	DmWrite(rp, i, src+tlen+3, klen);
	i+=klen;
	DmWrite(rp, i, &funchar1, 1);
	i++;
	DmWrite(rp, i, mp, clen);
	i+=clen;
	DmWrite(rp, i, &funchar2, 1);

	MemHandleUnlock(rech);
	MemHandleFree(mh);

	attr|=categoryto;

	DmSetRecordInfo(memodb, idx, &attr, NULL);
	DmReleaseRecord(memodb, idx, true);

	return true;
}
