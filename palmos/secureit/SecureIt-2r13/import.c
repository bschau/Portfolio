#include "sit.h"

/* globals */
static UInt16 categoryfrom, categoryto;
static sitlist fromlist, tolist;

/* protos */
static Boolean evh(EventPtr);
static UInt16 runimport(Int16, UInt16, UInt16);
static Boolean import(UInt8 *, UInt16, DmOpenRef, UInt16, UInt8 *);

/*******************************************************************************
*
*	importmemo
*
*	Import entries from 'Unfiled' category in Memo database.
*/
UInt16
importmemo(FormPtr parform,
           UInt16 dstdb,
           UInt16 *dstcat)
{
	UInt16 imported=0;
	FormPtr tfrm;
	ControlPtr chk;
	UInt16 mr, button;
	ControlType *ctl;

	categoryfrom=0;
	categoryto=0;
	memodb=openmemodb();
	if (!memodb)
		return 0;

	if (dstdb==IMPORTTOSECRETS) {
		tfrm=FrmInitForm(formimportsecrets);
		CategoryGetName(sitdb, dmUnfiledCategory, tolist.name);
		ctl=FrmGetObjectPtr(tfrm, FrmGetObjectIndex(tfrm, fimcatpopupto));
		CategorySetTriggerLabel(ctl, tolist.name);
	} else
		tfrm=FrmInitForm(formimporttemplates);

	FrmSetEventHandler(tfrm, (FormEventHandlerPtr)evh);
	FrmSetActiveForm(tfrm);

	CategoryGetName(memodb, dmUnfiledCategory, fromlist.name);
	ctl=FrmGetObjectPtr(tfrm, FrmGetObjectIndex(tfrm, fimcatpopupfrom));
	CategorySetTriggerLabel(ctl, fromlist.name);

	button=FrmDoDialog(tfrm);

	switch (button) {
	case fimimport:
		mr=DmNumRecordsInCategory(memodb, categoryfrom);
		if (mr) {
			working(parform, mr);
			chk=objectinform(tfrm, fimcheck);
			imported=runimport(CtlGetValue(chk), dstdb, mr);
			stopworking();
		} else
			FrmCustomAlert(alertnomemos, NULL, NULL, NULL);
		/* FALL-THROUGH */
	default:
		break;
	}
	
	FrmEraseForm(tfrm);
	FrmDeleteForm(tfrm);
	FrmSetActiveForm(parform);

	DmCloseDatabase(memodb);

	if (dstcat)
		*dstcat=categoryto;

	return imported;
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
		case fimcatpopupfrom:
			CategorySelect(memodb, form, fimcatpopupfrom, fimcatlistfrom, true, &categoryfrom, fromlist.name, 0, 0);
			return true;
		case fimcatpopupto:
			CategorySelect(sitdb, form, fimcatpopupto, fimcatlistto, false, &categoryto, tolist.name, CATNONEDITABLES, 0);
			return true;
		}
	}

	return false;
}

/*******************************************************************************
*
*	runimport
*
*	Main import engine.
*	Optionally deletes from Memo Pad.
*/
static UInt16
runimport(Int16 delflag,
          UInt16 dstdb,
          UInt16 toimport)
{
	Int16 imported=0, memoidx=0, lauf=1;
	Boolean imp;
	MemHandle mh;
	UInt8 *mp;
	UInt8 buffer[64];
	Err err;

	err=DmSeekRecordInCategory(memodb, &memoidx, 0, dmSeekForward, categoryfrom);
	if (err==errNone) {
		for (EVER) {
			mh=DmQueryNextInCategory(memodb, &memoidx, categoryfrom);
			if (mh) {
				StrPrintF(buffer, "Importing %i of %i memos ...", lauf, toimport);
				updatevisual(buffer);
				mp=MemHandleLock(mh);
				if (dstdb==IMPORTTOSECRETS)
					imp=import(mp, categoryto, sitdb, MAXTITLELEN, "Untitled secret");
				else
					imp=import(mp, 0, tmpldb, MAXTEMPLATETITLELEN, "Untitled template");
				MemHandleUnlock(mh);

				if (imp==true) {
					imported++;
					/* delete? */
					if (delflag) {
						DmRemoveRecord(memodb, memoidx);
						memoidx=-1;
					}
				}
				memoidx++;
			} else
				break;

			lauf++;
		}
	}

	StrPrintF(buffer, "%i", imported);
	if (dstdb==IMPORTTOSECRETS)
		FrmCustomAlert(alertimportinfo, buffer, "secret", ((imported==1) ? " " : "s "));
	else
		FrmCustomAlert(alertimportinfo, buffer, "template", ((imported==1) ? " " : "s "));

	return imported;
}

/*******************************************************************************
*
*	import
*
*	Copy a memo from Memo pad into the selected database.
*/
static Boolean
import(UInt8 *mp,
       UInt16 category,
       DmOpenRef db,
       UInt16 maxlen,
       UInt8 *unknown)
{
	UInt32 firstlinecnt=0, mhsize=StrLen(mp);
	MemHandle sh=NULL;
	UInt8 *key, *txt, *sp, *t;
	Boolean ret;
	UInt16 l1, l2;

	if (mhsize>MAXSECRETLEN)
		return false;

	/* ok, copy memo */
	sh=MemHandleNew(mhsize+1);
	if (!sh)
		return false;

	sp=MemHandleLock(sh);
	MemMove(sp, mp, mhsize);
	*(sp+mhsize)='\x00';

	/* skip whitespaces */
	key=sp;
	while (*key && (*key==' ' || *key=='\t' || *key=='\n' || *key=='\r'))
		key++;

	if (!*key) {
		MemHandleFree(sh);
		return false;	/* blank line - don't want it :) */
	}

	txt=key;
	while (*txt && *txt!='\n') {
		txt++;
		firstlinecnt++;
	}

	/* end of key found - let's see if this is a single line */
	t=txt;
	while (*t && (*t==' ' || *t=='\t' || *t=='\n' || *t=='\r'))
		t++;

	if (!*t) { 	/* single line, possible with trailing garbage */
		txt=key;
		key=unknown;
	} else
		*txt++='\x00';	/* terminate key */

	l1=StrLen(key);
	l2=StrLen(txt);
	ret=storerecord(db, key, txt, category, dmMaxRecordIndex);
	MemHandleFree(sh);
	return ret;
}	
