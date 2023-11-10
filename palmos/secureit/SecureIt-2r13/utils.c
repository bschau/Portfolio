/*******************************************************************************
*
*	utils
*
*	Utilities ...
*/
#include "sit.h"

/* structs */
typedef struct {
	UInt16 type;
	UInt16 index;
	UInt16 size;
} SavePref;

/* macros */
#define MAXDYNAMICLISTLEN 256	/* larger than the largest ... */

/*******************************************************************************
*
*	objectinform
*
*	Returns pointer to the object in the given form.
*/
MemPtr
objectinform(FormPtr form,
             UInt16 id)
{
	UInt16 index;

	index=FrmGetObjectIndex(form, id);
	return FrmGetObjectPtr(form, index);
}

/*******************************************************************************
*
*	generatekey
*
*	Generate a new blowfishkey given a password.   Allocates necessary
*	space to hold the key.
*/
Int16
generatekey(MemHandle *h,
            UInt8 *p)
{
	if (!*h) {
		*h=MemHandleNew(sizeof(blowfishkey));
		if (!*h)
			return -1;
	}

	blowfishmakekey(MemHandleLock(*h), p, StrLen(p));
	MemHandleUnlock(*h);

	return 0;
}

/*******************************************************************************
*
*	setfieldtext
*
*	Set text in field.
*/
void
setfieldtext(MemHandle th,
             UInt8 *src,
             UInt16 len,
             UInt16 dec,
             UInt16 fid,
             FormPtr form)
{
	FieldType *fld;
	MemHandle mh;
	UInt8 *tp;

	tp=MemHandleLock(th);
	MemMove(tp, src, len);
	tp[len]='\x00';

	if (dec)
		decrypt(tp, len);

	MemHandleUnlock(th);

	fld=objectinform(form, fid);
	mh=FldGetTextHandle(fld);
	FldSetTextHandle(fld, th);

	if (mh)
		MemHandleFree(mh);
}

/*******************************************************************************
*
*	buildlist
*
*	Initializes list object.
*/
void
buildlist(FormPtr form,
          UInt16 obj,
          DmOpenRef db,
          UInt16 cat,
          void (*ldf)(Int16, RectangleType *, Char **))
{
	ListPtr lp;
	UInt16 items;

	items=DmNumRecordsInCategory(db, cat);
	lp=objectinform(form, obj);
	LstSetListChoices(lp, NULL, items);
	LstSetSelection(lp, -1);
	LstSetDrawFunction(lp, ldf);
}

/*******************************************************************************
*
*	dynamiclistdraw
*
*	Draws one entry of the dynamic list.
*/
void
dynamiclistdraw(DmOpenRef db,
                UInt16 item,
                RectangleType *rect)
{
	MemHandle mh;
	UInt16 slen, pw;
	UInt8 *k;
	Boolean trunc;

	mh=DmQueryRecord(db, item);
	if (mh) {
		k=MemHandleLock(mh);
		slen=GETOFFSET(k);
		k+=(slen+3);

		slen=StrLen(k);

		pw=rect->extent.x;
		FntCharsInWidth(k, &pw, &slen, &trunc);
		WinDrawChars(k, slen, rect->topLeft.x, rect->topLeft.y);
		MemHandleUnlock(mh);
	}
}

/*******************************************************************************
*
*	populate2fieldsmemhandle
*
*	Populate 2 fields with title and decrypted data from MemHandle.
*/
Boolean
populate2fieldsmemhandle(FormPtr form,
                         MemHandle mh,
                         UInt16 fld1,
                         UInt16 fld2)
{
	MemHandle f1h, f2h;
	UInt8 *mp;
	recdesc rd;

	mp=MemHandleLock(mh);
	recorddesc(&rd, mp);

	f1h=MemHandleNew(rd.titlelen+1);
	f2h=MemHandleNew(rd.secretlen+1);
	if (!f1h || !f2h) {
		MemHandleUnlock(mh);

		if (f1h)
			MemHandleFree(f1h);
		if (f2h)
			MemHandleFree(f2h);

		FrmCustomAlert(alertoom, NULL, NULL, NULL);
		return false;
	}

	setfieldtext(f1h, rd.title, rd.titlelen, 0, fld1, form);
	setfieldtext(f2h, rd.secret, rd.secretlen, 1, fld2, form);

	MemHandleUnlock(mh);

	return true;
}

/*******************************************************************************
*
*	populate2fields
*
*	Populate 2 fields with title and decrypted data.
*/
Boolean
populate2fields(FormPtr form,
                DmOpenRef db,
                UInt16 idx,
                UInt16 fld1,
                UInt16 fld2)
{
	MemHandle mh;

	mh=DmQueryRecord(db, idx);
	if (!mh) {
		FrmCustomAlert(alertqueryfailed, NULL, NULL, NULL);
		return false;
	}

	return populate2fieldsmemhandle(form, mh, fld1, fld2);
}

/*******************************************************************************
*
*	refocus
*
*	Shift focus between two fields.
*/
Boolean
refocus(EventPtr ev,
        FormPtr form,
        UInt16 fld1,
        UInt16 fld2)
{
	Int16 focusidx;

	if (ev->data.keyDown.chr==prevFieldChr || ev->data.keyDown.chr==nextFieldChr) {
		focusidx=FrmGetFocus(form);
		if (focusidx!=noFocus) {
			focusidx=FrmGetObjectId(form, focusidx);
			if (focusidx==fld1 || focusidx==fld2) {
				if (focusidx==fld1)
					FrmSetFocus(form, FrmGetObjectIndex(form, fld2));
				else
					FrmSetFocus(form, FrmGetObjectIndex(form, fld1));
				return true;
			}
		}
	}

	return false;
}

/*******************************************************************************
*
*	fieldblank
*
*	Test to see if field is blank.
*/
Int16
fieldblank(UInt8 *ft)
{
	UInt32 cnt;

	if (!ft)
		return -1;

	for (cnt=0; cnt<StrLen(ft); cnt++) {
		if (*ft==' ' || *ft=='\t' || *ft=='\n' || *ft=='\r')
			ft++;
		else
			return 0;
	}

	return -1;
}

/*******************************************************************************
*
*	openmemodb
*
*	Open Memo database.
*/
DmOpenRef
openmemodb(void)
{
	LocalID dbid;
	DmOpenRef db;

	dbid=DmFindDatabase(0, "MemoDB");
	if (!dbid) {
		FrmCustomAlert(alertnomemodb, NULL, NULL, NULL);
		return NULL;
	}

	db=DmOpenDatabase(0, dbid, dmModeReadWrite);
	if (!db) {
		FrmCustomAlert(alertnomemodb, NULL, NULL, NULL);
		return NULL;
	}

	return db;
}

/*******************************************************************************
*
* 	deletedialog
*
*	Popup delete dialog.
*/
Int16
deletedialog(FormPtr pfrm)
{
	FormPtr tfrm;
	UInt16 button;
	ControlPtr chkbox;

	if (prefs.flags&PREFCONFIRMDELETE) {
		tfrm=FrmInitForm(formdeleteentry);
		FrmSetActiveForm(tfrm);

		chkbox=objectinform(tfrm, defcheckbox);
		CtlSetValue(chkbox, true);

		button=FrmDoDialog(tfrm);
		if (CtlGetValue(chkbox)==false)
			prefs.flags&=~PREFCONFIRMDELETE;

		FrmSetActiveForm(pfrm);
		FrmEraseForm(tfrm);
		FrmDeleteForm(tfrm);

		if (button==defno)
			return 0;
	}

	return 1;
}

/*******************************************************************************
*
*	updatetoggle
*
*	Update starflag toggle graphics.
*/
void
updatetoggle(UInt16 starflag,
             UInt16 x,
             UInt16 y)
{
	MemHandle bh;

	bh=DmGetResource(bitmapRsc, (starflag) ? pincodeswitchon : pincodeswitchoff);
	if (bh) {
		WinDrawBitmap(MemHandleLock(bh), x, y);
		MemHandleUnlock(bh);
		DmReleaseResource(bh);
	}
}

/*******************************************************************************
*
*	decrypt
*
*	Decrypt stream.
*/
void
decrypt(UInt8 *stream,
        UInt32 streamlen)
{
	blowfishkey *k=MemHandleLock(mainkeyh);
	UInt32 cnt;

	for (cnt=0; cnt<(streamlen/8); cnt++) {
		blowfishdecrypt(k, stream);
		stream+=8;
	}

	MemHandleUnlock(mainkeyh);
}

/*******************************************************************************
*
*	saveentry
*
*	Save key/txt in preferences (called by frmSaveEvents).
*
*	 -> type	Type (Secret/Template).
*	 -> idx		Record index.
*	 -> key		Key.
*	 -> txt		Body text.
*/
Boolean
saveentry(UInt16 type, UInt16 idx, UInt8 *key, UInt8 *txt)
{
	Int16 kf, tf;
	MemHandle mh;
	SavePref s;
	UInt32 recsize;

	kf=fieldblank(key);
	tf=fieldblank(txt);
	if (kf && tf)
		return true;

	if (kf)
		key="";
	if (tf)
		txt="";

	s.type=type;
	s.index=idx;
	if ((mh=formrecord(false, &recsize, key, txt))==NULL)
		return false;

	s.size=(UInt16)recsize;
	PrefSetAppPreferences(CREATORID, PrfSaveType, APPVER, &s, sizeof(SavePref), true);
	PrefSetAppPreferences(CREATORID, PrfSaveData, APPVER, MemHandleLock(mh), s.size, true);
	MemHandleFree(mh);

	return true;
}

/*******************************************************************************
*
*	loadentry
*
*	Loads key/txt pair (if present), then jumps to edit form. If a
*	key/txt pair is not found, jump to start form.
*/
void
loadentry(void)
{
	UInt16 prflen=sizeof(SavePref);
	Int16 retprf;
	SavePref s;
	MemHandle mh;

	retprf=PrefGetAppPreferences(CREATORID, PrfSaveType, &s, &prflen, true);
	if (retprf==noPreferenceFound || prflen!=sizeof(SavePref)) {
		PrefSetAppPreferences(CREATORID, PrfSaveType, APPVER, NULL, 0, true);
		PrefSetAppPreferences(CREATORID, PrfSaveData, APPVER, NULL, 0, true);
		gotostart();
		return;
	}

	PrefSetAppPreferences(CREATORID, PrfSaveType, APPVER, NULL, 0, true);

	/* type preferences ok .. */
	if ((mh=MemHandleNew(s.size))==NULL) {
		gotostart();
		return;
	}

	prflen=s.size;
	retprf=PrefGetAppPreferences(CREATORID, PrfSaveData, MemHandleLock(mh), &prflen, true);
	MemHandleUnlock(mh);

	PrefSetAppPreferences(CREATORID, PrfSaveData, APPVER, NULL, 0, true);
	if (retprf==noPreferenceFound || prflen!=s.size) {
		MemHandleFree(mh);
		gotostart();
		return;
	}

	/* ok, we're here .. now init form .. */
	switch (s.type) {
	case TypeSecret:
		preparetoedit(s.index, mh);
		break;
	case TypeTemplate:
		edittemplate(s.index, mh);
		break;
	default:
		MemHandleFree(mh);
		gotostart();
		break;
	}
}
