#include "p0ls.h"

/* globals */
static Int16 iolistnum=-1;
static struct {
	Char rule;
	Char *dst;
} rulearr[]={
	{ 'A', axiom }, { 'F', ruleuf }, { 'f', ruleuf }, { '0', rule0 },
	{ '1', rule1 }, { '2', rule2}, { '3', rule3 }, { '4', rule4 },
	{ '5', rule5 }, { '6', rule6}, { '7', rule7 }, { '8', rule8 },
	{ '9', rule9 }, { ' ', NULL }
};
static Char *titload="Load 0L System", *titsave="Save 0L System";
static Char *butload="Load", *butsave="Save";
static UInt16 ioitems;

/* protos */
static Int16 iosaverecord(void);
static Int16 ioloadrecord(Char *);
static Int16 iodelrecord(Char *);
static int ircompare(unsigned char *, unsigned char *, Int16, SortRecordInfoPtr, SortRecordInfoPtr, MemHandle);
static void buildlist(FormPtr);
static Boolean evh(EventPtr);
static Boolean selev(EventPtr, FormPtr);
static void getnthid(Char *, Int16);
static void listdrawfunc(Int16, RectangleType *, Char **);

/*******************************************************************************
*
*	iopack
*
*	Pack all rules etc. to structure.
*
*	Input:
*		key - keyname.
*		depth - depth.
*		angle - angle.
*/
void
iopack(Char *key,
       Int16 depth,
       Int16 angle)
{
	Char *rp=grec, *r;
	UInt32 len=StrLen(key), idx;
	unsigned char oct;

	oct=(depth>>8)&0xff;
	*rp++=oct;
	oct=depth&0xff;
	*rp++=oct;

	oct=(angle>>8)&0xff;
	*rp++=oct;
	oct=angle&0xff;
	*rp++=oct;

	*rp++=len;
	MemMove(rp, key, len);
	rp+=len;

	for (idx=0; ; idx++) {
		r=rulearr[idx].dst;
		if (!r)
			break;

		if (*r) {
			len=StrLen(r);
			*rp++=rulearr[idx].rule;
			*rp++=len;
			MemMove(rp, r, len);
			rp+=len;
		}
	}

	*rp='\x00';
}

/*******************************************************************************
*
*	iounpack
*
*	Unpack from structure to rules.
*
*	Input:
*		src - source.
*/
void
iounpack(Char *src)
{
	Char *rp=src, *r, c;
	UInt32 len, idx;
	UInt16 v;

	clearvars();

	/* get depth & angle */
	v=(*rp++)<<8;
	v|=*rp++;
	prefs.depth=v;

	v=(*rp++)<<8;
	v|=*rp++;
	prefs.angle=v;

	/* skip key */
	len=*rp++;
	rp+=len;

	while (*rp) {
		c=*rp++;
		len=*rp++;
		r=rp;
		rp+=len;
		for (idx=0; ; idx++) {
			if (rulearr[idx].rule==' ')
				break;

			if (rulearr[idx].rule==c) {
				MemMove(rulearr[idx].dst, r, len);
				break;
			}
		}
	}
}

/*******************************************************************************
*
*	iosaverecord
*
*	Save record.   Prompts for overwrite.
*
*	Output:
*		0 - success, goto start.
*		1 - don't want to overwrite.
*		2 - error.
*/
static Int16
iosaverecord(void)
{
	unsigned char *kptr=grec+INTOFF;
	Int16 t;
	UInt16 recindex;
	MemHandle rec;
	Char key[KEYSIZE+2];

	t=*kptr++;
	MemMove(key, kptr, t);
	*(key+t)='\x00';

	recindex=DmFindSortPosition(gdb, grec, 0, (DmComparF *)ircompare, 0);
	if (recindex) {
		rec=DmGetRecord(gdb, recindex-1);
		kptr=MemHandleLock(rec);
			
		t=ircompare(grec, kptr, 0, NULL, NULL, NULL);
		MemHandleUnlock(rec);
		if (!t) {
			t=FrmCustomAlert(alertoverwrite, key, NULL, NULL);
			if (!t) {
				rec=DmResizeRecord(gdb, recindex-1, StrLen(grec+INTOFF)+INTOFF);
				if (rec) {
					kptr=MemHandleLock(rec);
					DmWrite(kptr, 0, grec, StrLen(grec+INTOFF)+INTOFF+1);
					MemHandleUnlock(rec);
					DmReleaseRecord(gdb, recindex-1, true);
					return 0;
				} else
					FrmCustomAlert(alertsavefailed, key, NULL, NULL);
			}
			DmReleaseRecord(gdb, recindex-1, false);
			return 1;

		}

		DmReleaseRecord(gdb, recindex-1, false);
	}

	rec=DmNewRecord(gdb, &recindex, StrLen(grec+INTOFF)+INTOFF+1);
	if (rec) {
		kptr=MemHandleLock(rec);
		DmWrite(kptr, 0, grec, StrLen(grec+INTOFF)+INTOFF+1);
		MemHandleUnlock(rec);
		DmReleaseRecord(gdb, recindex, true);
		return 0;
	}

	return 2;
}

/*******************************************************************************
*
*	ioloadrecord
*
*	Load record by key.
*
*	Input:
*		key - key.
*	Output:
*		s: 0.
*		f: !0.
*/
static Int16
ioloadrecord(Char *key)
{
	unsigned char *kptr=grec;
	Int16 t=StrLen(key), err=false;
	UInt16 recindex;
	MemHandle rec;

	MemSet(kptr, KEYSIZE+2+INTOFF, 0);
	kptr+=INTOFF;
	*kptr++=t;
	MemMove(kptr, key, t);

	recindex=DmFindSortPosition(gdb, grec, 0, (DmComparF *)ircompare, 0);
	if (recindex) {
		rec=DmGetRecord(gdb, recindex-1);
		kptr=MemHandleLock(rec);
			
		t=ircompare(grec, kptr, 0, NULL, NULL, NULL);
		if (!t) {
			iounpack(kptr);
			err=true;
		}

		MemHandleUnlock(rec);
		DmReleaseRecord(gdb, recindex-1, false);
	}

	return err;
}

/*******************************************************************************
*
*	iodelrecord
*
*	Delete record by key.
*
*	Input:
*		key - key.
*	Output:
*		s: 0.
*		f: !0.
*/
static Int16
iodelrecord(Char *key)
{
	unsigned char *kptr=grec;
	Int16 t=StrLen(key), err=false;
	UInt16 recindex;
	MemHandle rec;

	MemSet(kptr, KEYSIZE+2+INTOFF, 0);
	kptr+=INTOFF;
	*kptr++=t;
	MemMove(kptr, key, t);

	recindex=DmFindSortPosition(gdb, grec, 0, (DmComparF *)ircompare, 0);
	if (recindex) {
		rec=DmGetRecord(gdb, recindex-1);
		kptr=MemHandleLock(rec);
		t=ircompare(grec, kptr, 0, NULL, NULL, NULL);
		MemHandleUnlock(rec);
		if (!t) {
			DmRemoveRecord(gdb, recindex-1);
			err=true;
		}
	}

	return err;
}

/*******************************************************************************
*
*	ioinsert
*
*	Inserts record in database.
*
*	Output:
*		true/false.
*/
Int16
ioinsert(void)
{
	UInt16 recindex;
	MemHandle rec;
	unsigned char *kptr;
	Int16 t;

	recindex=DmFindSortPosition(gdb, grec, 0, (DmComparF *)ircompare, 0);
	if (recindex) {
		rec=DmGetRecord(gdb, recindex-1);
		kptr=MemHandleLock(rec);
			
		t=ircompare(grec, kptr, 0, NULL, NULL, NULL);
		if (!t) {	/* overwrite */
			DmWrite(kptr, 0, grec, StrLen(grec+INTOFF)+INTOFF+1);
			MemHandleUnlock(rec);
			DmReleaseRecord(gdb, recindex-1, true);
			return true;
		}
		MemHandleUnlock(rec);
		DmReleaseRecord(gdb, recindex-1, false);
	}

	rec=DmNewRecord(gdb, &recindex, StrLen(grec+INTOFF)+INTOFF+1);
	if (rec) {
		kptr=MemHandleLock(rec);
		DmWrite(kptr, 0, grec, StrLen(grec+INTOFF)+INTOFF+1);
		MemHandleUnlock(rec);
		DmReleaseRecord(gdb, recindex, true);
		return true;
	}

	return false;
}

/*******************************************************************************
*
*	ircompare
*
*	Compare function for DmFindSortPosition.
*
*	See reference manual for arguments.
*/
static int
ircompare(unsigned char *r1,
          unsigned char *r2,
          Int16 unused0,
          SortRecordInfoPtr unused1,
          SortRecordInfoPtr unused2,
          MemHandle unused3)
{
	Char k1[KEYSIZE+2], k2[KEYSIZE+2];
	UInt32 r1len, r2len;

	r1+=INTOFF;
	r2+=INTOFF;

	r1len=*r1++;
	r2len=*r2++;

	MemMove(k1, r1, r1len);
	MemMove(k2, r2, r2len);
	k1[r1len]='\x00';
	k2[r2len]='\x00';

	return StrCompare(k1, k2);
}

/*******************************************************************************
*
*	ioinit
*
*	Initializes the ioform.
*
*	Input:
*		form - form.
*		id - form id.
*/
void
ioinit(FormPtr form,
       UInt16 formid)
{
	buildlist(form);
	FrmSetEventHandler(form, (FormEventHandlerPtr)evh);
}

/*******************************************************************************
*
*	buildlist
*
*	Build 0L System list.
*
*	Input:
*		form - form.
*/
static void
buildlist(FormPtr form)
{
	ListPtr list;
	Int16 tn;
	Char key[KEYSIZE+2];

	ioitems=DmNumRecords(gdb);
	list=(ListPtr)objectinform(form, iolist);
	LstSetListChoices(list, NULL, ioitems);
	LstSetSelection(list, 0);
	LstSetDrawFunction(list, listdrawfunc);
	LstSetSelection(list, iolistnum);
	if (iolistnum!=-1) {
		tn=iolistnum-8;	/* -8 = visible lines before this line */
		if (tn<0)
			tn=0;

		LstSetTopItem(list, tn);
		getnthid(key, iolistnum);
	} else
		*key='\x00';

	setfieldtext(form, iofield, key);
}

/*******************************************************************************
*
*	evh
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
evh(EventPtr ev)
{
	FormPtr form;
	ControlType *obj;
	FieldType *fld;
	Char key[KEYSIZE+2];

	form=FrmGetActiveForm();
	switch (ev->eType) {
	case frmOpenEvent:
		obj=objectinform(form, ioaction);
		if (ioform==LOADFORM) {
			FrmSetTitle(form, titload);
			CtlSetLabel(obj, butload);
		} else {
			FrmSetTitle(form, titsave);
			CtlSetLabel(obj, butsave);
		}

		FrmDrawForm(form);
		return true;

	case ctlSelectEvent:
		return selev(ev, form);

	case lstSelectEvent:
		if (ev->data.lstSelect.listID==iolist) {
			iolistnum=ev->data.lstSelect.selection;
			getnthid(key, iolistnum);
			setfieldtext(form, iofield, key);
			fld=objectinform(form, iofield);
			FldDrawField(fld);
		}

		return true;

	case menuEvent:
		return globalmenu(ev, form);

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
	Char key[KEYSIZE+2], wkey[KEYSIZE+2];
	Int16 but;
	ListType *list;
	FieldType *fld;

	switch (ev->data.ctlSelect.controlID) {
	case iocancel:
		FrmGotoForm(formstart);
		return true;
	case iodelete:
		getfieldtext(form, iofield, wkey, KEYSIZE);
		if (*wkey) {
			normalizekey(key, wkey);
			but=FrmCustomAlert(alertdelete, key, NULL, NULL);
			if (!but) {
				but=iodelrecord(key);
				if (but==true) {
					iolistnum=-1;
					buildlist(form);
					list=objectinform(form, iolist);
					LstDrawList(list);
					fld=objectinform(form, iofield);
					FldDrawField(fld);
				} else
					FrmCustomAlert(alertdeletefailed, key, NULL, NULL);
			}
		} else
			FrmCustomAlert(alertnodelete, NULL, NULL, NULL);

		return true;
	case ioaction:
		getfieldtext(form, iofield, wkey, KEYSIZE);
		if (ioform==LOADFORM) {
			if (*wkey) {
				normalizekey(key, wkey);
				but=ioloadrecord(key);
				if (but==true)
					FrmGotoForm(formstart);
				else
					FrmCustomAlert(alertloadfailed, key, NULL, NULL);
			} else
				FrmCustomAlert(alertnoload, NULL, NULL, NULL);
		} else {
			if (*wkey) {
				normalizekey(key, wkey);
				iopack(key, prefs.depth, prefs.angle);
				but=iosaverecord();
				switch (but) {
				case 0:
					FrmGotoForm(formstart);
					/* FALL-THROUGH */
				case 1:
					break;
				case 2:
					FrmCustomAlert(alertsavefailed, key, NULL, NULL);
					FrmGotoForm(formstart);
					break;
				}
			} else
				FrmCustomAlert(alertnosave, NULL, NULL, NULL);
		}
		return true;
	}

	return false;
}

/*******************************************************************************
*
*	getnthid
*
*	Get N'th record key.
*
*	Input:
*		dst - where to store key.
*		idx - index.
*/
static void
getnthid(Char *dst,
         Int16 idx)
{
	MemHandle rec;
	Char *k;
	UInt16 len;
	
	rec=DmQueryRecord(gdb, idx);
	if (rec) {
		k=MemHandleLock(rec);
		k+=INTOFF;
		len=*k++;
		MemSet(dst, KEYSIZE+1, 0);
		MemMove(dst, k, len);
		MemHandleUnlock(rec);
	} else
		*dst='\00';
}
		
/*******************************************************************************
*
*	listdrawfunc
*
*	Draw io list.
*/
static void
listdrawfunc(Int16 itemnum,
             RectangleType *rect,
             Char **data)
{
	UInt16 slen, pw;
	Boolean trunc;
	Char key[KEYSIZE+2];

	if (itemnum<ioitems) {
		getnthid(key, itemnum);
		slen=StrLen(key);
		pw=rect->extent.x;

		FntCharsInWidth(key, &pw, &slen, &trunc);
		WinDrawChars(key, slen, rect->topLeft.x, rect->topLeft.y);
	}
}
