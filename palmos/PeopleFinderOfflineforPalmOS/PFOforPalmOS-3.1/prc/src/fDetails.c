/*
 * fDetails.c
 */
#include <Helper.h>
#include <HelperServiceClass.h>
#include <BtPrefsPnlTypes.h>
#include "Include.h"

/* protos */
static Boolean EH(EventType *);
static void UpdateForm(void);
static UInt32 SetupName(Char *, Char *, UInt32);
static void GetLocation(UInt16, UInt16);
static MemHandle UncrunchEmail(Char *, UInt32, UInt32);
static void LookupManager(UInt16, UInt8 *, UInt16 *, UInt16 *);
static void SetupPhoneDialing(Char *);
static void Dial(void);
static Boolean DialEH(EventType *);
static void FilterPhone(Char *, Char *);

/* globals */
static const Char phoneChars[]="0123456789,+#*";
static Char nameBuffer[DetailsFieldLength+2];
static Char personBuffer[DetailsFieldLength+2];
static Char phnPrimary[MaxPhoneLength+1];
static Char phnMobile[MaxPhoneLength+1];
static Char tmpPhone[MaxPhoneLength+1];
static Char *pPhnPrimary, *pPhnMobile;
static Boolean runDialog;
static MemHandle recMH;
static UInt32 recOffset;
static UInt8 mgrKey;
static UInt16 mgrRIdx, mgrPIdx;

/*
 * fDetailsRun
 *
 * Show the details dialog.
 *
 *  -> mh		MemHandle with record information.
 *  -> idx		Index into mh.
 */
void
fDetailsRun(MemHandle mh, UInt32 idx)
{
	FormType *saveCurrent=currentForm;
	UInt16 saveID=currentFormID;
	FormActiveStateType frmSave;
	EventType ev;
	Err err;

	recMH=mh;
	recOffset=idx;

	FrmSaveActiveState(&frmSave);
	currentFormID=fDetails;
	currentForm=FrmInitForm(currentFormID);
	ErrFatalDisplayIf(currentForm==NULL, "(fDetailsRun) Cannot initialize form.");
	FrmSetActiveForm(currentForm);
	FrmSetEventHandler(currentForm, (FormEventHandlerType *)EH);

	MemSet(&ev, sizeof(EventType), 0);
	ev.eType=frmOpenEvent;
	ev.data.frmLoad.formID=currentFormID;
	EvtAddEventToQueue(&ev);

	runDialog=true;
	while (runDialog) {
		EvtGetEvent(&ev, evtWaitForever);
		if (ev.eType==appStopEvent) {
			EvtAddEventToQueue(&ev);
			appStopped=true;
			break;
		} else if (ev.eType==frmCloseEvent)
			break;

		if (SysHandleEvent(&ev)==true ||
		    MenuHandleEvent(0, &ev, &err)==true)
			continue;

		FrmDispatchEvent(&ev);
	}

	FrmEraseForm(currentForm);
	FrmDeleteForm(currentForm);
	FrmRestoreActiveState(&frmSave);
	currentForm=saveCurrent;
	currentFormID=saveID;
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		UpdateForm();
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cDetailsLookup:
			if ((recMH=fDetailsGetManager(&dbNameSearch, mgrKey, mgrRIdx))==NULL)
				return true;

			recOffset=mgrPIdx;
			UpdateForm();
			return true;
		case cDetailsDial:
			if (!pPhnPrimary && !pPhnMobile) {
				FrmAlert(aNoPhones);
				return true;
			}

			Dial();
			return true;
		default:
			runDialog=false;
			break;
		}
		break;

	case keyDownEvent:
		UIFieldScrollBarKeyHandler(ev, cDetailsAddress, cDetailsScrollbar);
	default:	/* FALL-THRU */
		UIFieldScrollBarHandler(ev, cDetailsAddress, cDetailsScrollbar);
		break;
	}
		
	return false;
}

/*
 * UpdateForm
 *
 * Populate fields.
 */
static void
UpdateForm(void)
{
	UInt32 sIdx=recOffset, idx=recOffset;
	MemHandle rh;
	UInt16 i;
	UInt8 *p, *s;

	p=MemHandleLock(recMH);
	i=p[idx++]<<8;
	i|=p[idx++];

	LookupManager(i, &mgrKey, &mgrRIdx, &mgrPIdx);
	if (*nameBuffer) {
		s=NSUppercaseWords(nameBuffer);
		UIFieldSetText(cDetailsManager, s);
		MemPtrFree(s);
		UIObjectShow(cDetailsLookup);
	} else {
		UIFieldSetText(cDetailsManager, "-");
		UIObjectHide(cDetailsLookup);
	}

	idx=SetupName(personBuffer, p, idx);
	s=NSUppercaseWords(personBuffer);
	UIFieldSetText(cDetailsName, s);
	MemSet(personBuffer, sizeof(personBuffer), 0);
	StrNCopy(personBuffer, s, DetailsFieldLength);
	MemPtrFree(s);

	MemSet(phnPrimary, sizeof(phnPrimary), 0);
	if ((i=(UInt16)p[idx])) {
		pPhnPrimary=NSUncrunchPhone(phnPrimary, p+idx);
		s=pPhnPrimary;
		idx+=i;
	} else {
		pPhnPrimary=NULL;
		s="-";
	}

	idx++;
	UIFieldSetText(cDetailsPhone, s);

	MemSet(phnMobile, sizeof(phnMobile), 0);
	if ((i=(UInt16)p[idx])) {
		pPhnMobile=NSUncrunchPhone(phnMobile, p+idx);
		s=pPhnMobile;
		idx+=i;
	} else {
		pPhnMobile=NULL;
		s="-";
	}

	idx++;
	UIFieldSetText(cDetailsMobile, s);

	i=p[idx++]<<8;
	i|=p[idx++];
	
	GetLocation(i, cDetailsAddress);

	if ((rh=UncrunchEmail(p, sIdx, idx))) {
		UIFieldSetText(cDetailsEmail, MemHandleLock(rh));
		MemHandleFree(rh);
	} else
		UIFieldSetText(cDetailsEmail, "-");

	MemHandleUnlock(recMH);
	UIFieldUpdateScrollBar(cDetailsAddress, cDetailsScrollbar);
}

/*
 * SetupName
 *
 * Store name in buffer.
 *
 * <-  dst		Where to store name (DetailsFieldLength in size!)
 *  -> p		Src block.
 *  -> idx		Index.
 *
 * returns index position after name.
 */
static UInt32
SetupName(Char *dst, Char *p, UInt32 idx)
{
	Int32 size=DetailsFieldLength;
	UInt16 toMove, cnt;

	cnt=(UInt16)p[idx++];

	toMove=(size<cnt) ? size : cnt;
	MemMove(dst, p+idx, toMove);
	size-=toMove;
	dst+=toMove;
	idx+=cnt;

	if (size>0) {
		*dst++=' ';
		if (size>0) {
			cnt=(UInt16)StrLen(p);
			toMove=(size<cnt) ? size : cnt;
			MemMove(dst, p, toMove);
			dst+=toMove;
		}
	}

	*dst='\x00';
	return idx;
}

/*
 * GetLocation
 *
 * Get location from database.
 *
 *  -> idx		Location index.
 *  -> id		ID of receiving field.
 *
 * Returns MemHandle (if found) or NULL (not found).
 */
static void
GetLocation(UInt16 idx, UInt16 id)
{
	MemHandle mh=NULL, rh;
	UInt32 len;
	Char *p;

	if (dbLocations.isOpen==true) {
		mh=DBGetRecord(&dbLocations, idx);
		len=MemHandleSize(mh);

		rh=MemHandleNew(len+1);
		ErrFatalDisplayIf(rh==NULL, "(GetLocation) Out of memory.");
		p=MemHandleLock(rh);
		MemMove(p, MemHandleLock(mh), len);
		MemHandleFree(mh);
		p[len]=0;

		UIFieldSetText(id, p);
		MemHandleFree(rh);
		return;
	}

	UIFieldSetText(id, "-");
}

/*
 * UncrunchEmail
 *
 * Uncrunch the cryptic email address ...
 *
 *  -> sSrc		Source.
 *  -> sIdx		Source idx.
 *  -> idx		Current index.
 *
 * Returns MemHandle to address (or NULL if no address).
 */
static MemHandle
UncrunchEmail(Char *sSrc, UInt32 sIdx, UInt32 idx)
{
	Boolean reDash=false;
	Char *src=sSrc+idx, *p, *g, *s, *domain;
	UInt32 len;
	MemHandle mh;
	UInt16 mbType, dmType;
	Char c;

	if (!*src)
		return NULL;

	if (*src==32) {
		src++;
		len=StrLen(src);
		mh=MemHandleNew(len+1);
		ErrFatalDisplayIf(mh==NULL, "(UncrunchEmail) Out of memory.");

		p=MemHandleLock(mh);
		MemMove(p, src, len);
		p[len]=0;
		MemHandleUnlock(mh);
		return mh;
	}

	if (*src==16) {
		src++;
		len=StrLen(src);
		mh=MemHandleNew(len+2+6);
		ErrFatalDisplayIf(mh==NULL, "(UncrunchEmail) Out of memory.");

		p=MemHandleLock(mh);
		StrPrintF(p, "%s@hp.com", src);
		MemHandleUnlock(mh);
		return mh;
	}

	dmType=(UInt16)*src++;
	mbType=dmType&15;

	if ((dmType&16))
		domain="hp.com";
	else
		domain=src;

	len=(UInt32)sSrc[sIdx+2];
	MemMove(nameBuffer, sSrc+sIdx+3, len);
	nameBuffer[len]=0;
	
	/* + surName + delimiter + @ + domain + term char */
	len+=StrLen(sSrc)+2+StrLen(domain)+1;

	if (mbType<8) {
		g=nameBuffer;
		s=sSrc;
		switch (mbType) {
		case 2:		/* givenName.surName */
			c='.';
			break;
		case 3:		/* givenName_surName */
			c='_';
			break;
		case 4:		/* given-Name.sur-Name */
			c='.';
			reDash=true;
			break;
		case 5:		/* given-Name_sur-Name */
			c='_';
			reDash=true;
			break;
		case 6:		/* givenNamesurName */
			c=0;
			break;
		default:	/* given-Namesur-name */
			c=0;
			reDash=true;
			break;
		}
	} else {
		g=sSrc;
		s=nameBuffer;
	
		switch (mbType) {
		case 8:		/* surName.givenName */
			c='.';
			break;
		case 9:		/* surName_givenName */
			c='_';
			break;
		case 10:	/* sur-Name.given-Name */
			c='.';
			reDash=true;
			break;
		case 11:	/* sur-Name_given_Name */
			c='_';
			reDash=true;
			break;
		case 12:	/* surNamegivenName */
			c=0;
			break;
		default:	/* sur-Namegiven-Name */
			c=0;
			reDash=true;
			break;
		}
	}

	mh=MemHandleNew(len);
	ErrFatalDisplayIf(mh==NULL, "(UncrunchEmail) Out of memory.");
	p=MemHandleLock(mh);
	if (c)
		StrPrintF(p, "%s%c%s@%s", g, c, s, domain);
	else
		StrPrintF(p, "%s%s@%s", g, s, domain);

	if (reDash) {
		for (s=p; *s; s++) {
			if (*s==' ')
				*s='-';
		}
	}

	MemHandleUnlock(mh);

	return mh;
}

/*
 * LookupManager
 *
 * Lookup manager - store name in nameBuffer.
 *
 *  -> idx		Index of manager.
 * <-  mgrKey		Where to store manager key.
 * <-  mgrRIdx		Where to store manager record idx.
 * <-  mgrPIdx		Where to store manager person idx.
 */
static void
LookupManager(UInt16 idx, UInt8 *mgrKey, UInt16 *mgrRIdx, UInt16 *mgrPIdx)
{
	MemHandle mh;
	UInt8 *mData;
	DB db;

	*nameBuffer=0;

	if (idx==0xffff)
		return;

	if (dbManagers.isOpen==false)
		return; 

	mh=DBGetRecord(&dbManagers, idx);

	mData=MemHandleLock(mh);
	*mgrKey=*mData++;
	*mgrRIdx=(*mData++)<<8;
	*mgrRIdx|=*mData++;
	*mgrPIdx=(*mData++)<<8;
	*mgrPIdx|=*mData;
	MemHandleFree(mh);

	if ((mh=fDetailsGetManager(&db, *mgrKey, *mgrRIdx))!=NULL) {
		mData=MemHandleLock(mh);
		SetupName(nameBuffer, mData, *mgrPIdx+2);
		MemHandleFree(mh);
		DBClose(&db);
	}
}

/*
 * fDetailsGetManager
 *
 * Get manager details.
 *
 *  -> db		Database object.
 *  -> key		Key.
 *  -> idx		Record index.
 *
 * Caller must close database.
 */
MemHandle
fDetailsGetManager(DB *db, UInt8 key, UInt16 idx)
{
	Char dbName[dmDBNameLength];

	if (idx==0xffff)
		return NULL;

	StrPrintF(dbName, "PFOC_pfoc index%c%c", hexChars[(key>>4)&0x0f], hexChars[key&0x0f]);
	DBInit(db, dbName, rootPath);
	if (DBFindDatabase(db)==FDFound) {
		if (DBOpen(db, dmModeReadOnly, true)==true)
			return DBGetRecordSilent(db, idx);
	}

	return NULL;
}

/*
 * SetupPhoneDialing
 *
 * Broadcast helper code for Dialing.
 * 
 *  -> tlf		Telephone number to dial.
 */
static void
SetupPhoneDialing(Char *tlf)
{
	UInt32 version=0;
	SysNotifyParamType param;
	HelperNotifyEventType details;
	HelperNotifyExecuteType execute;

	if (FtrGet(sysFtrCreator, sysFtrNumNotifyMgrVersion, &version)!=errNone || version==0) {
		FrmAlert(aNoNotifications);
		return;
	}

	if (prefs.flags&PFlgBluetooth)
		PMBtToggleOn();

	param.notifyType=sysNotifyHelperEvent;
	param.broadcaster=sysFileCAddress;	//CRID;
	param.notifyDetailsP=&details;
	param.handled=false;

	details.version=kHelperNotifyCurrentVersion;
	details.actionCode=kHelperNotifyActionCodeExecute;
	details.data.executeP=&execute;

	execute.serviceClassID=kHelperServiceClassIDVoiceDial;
	execute.helperAppID=0;
	execute.dataP=tlf;
	execute.displayedName=personBuffer;
	execute.detailsP=0;
	execute.err=errNone;
	SysNotifyBroadcast(&param);
}

/*
 * Dial
 *
 * Show the select number to dial form
 */
static void
Dial(void)
{
	UInt16 nos=0;
	FormSave frm;
	struct {
		Char *p;
	} arr[2]= { { NULL }, { NULL } };
	ListType *lst;

	if (pPhnPrimary) {
		arr[0].p=pPhnPrimary;
		nos++;
	}

	if (pPhnMobile) {
		if (StrCaselessCompare(pPhnPrimary, pPhnMobile)!=0) {
			arr[nos].p=pPhnMobile;
			nos++;
		}
	}

	UIFormPrologue(&frm, fDial, DialEH);
	lst=UIObjectGet(cDialList);
	LstSetListChoices(lst, (Char **)arr, nos);
	LstDrawList(lst);
	LstSetSelection(lst, noListSelection);
	if (nos==1) {
		FieldType *fld=UIObjectGet(cDialNumber);

		FilterPhone(tmpPhone, arr[0].p);
		UIFieldSetText(cDialNumber, tmpPhone);
		FldSetSelection(fld, 0, FldGetTextLength(fld));
	}

	UIObjectFocus(cDialNumber);
	UIFormEpilogue(&frm, NULL, 0);
}

/*
 * DialEH
 */
static Boolean
DialEH(EventType *ev)
{
	Int16 sel;

	switch (ev->eType) {
	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==cDialDial) {
			Char *p=UIFieldGetText(cDialNumber);

			if (p && StrLen(p)>0) {
				FilterPhone(tmpPhone, p);
				SetupPhoneDialing(tmpPhone);
			} else {
				FrmAlert(aNoNumber);
				return true;
			}
		}
		break;

	case lstSelectEvent:
		if ((sel=ev->data.lstSelect.selection)==noListSelection)
			return true;

		FilterPhone(tmpPhone, LstGetSelectionText(UIObjectGet(cDialList), sel));
		UIFieldSetText(cDialNumber, tmpPhone);
		return true;

	default:
		break;
	}
		
	return false;
}

/*
 * FilterPhone
 *
 * Filter the phone number.
 *
 * <-  dst		Where to store filtered number.
 *  -> src		Source number.
 */
static void
FilterPhone(Char *dst, Char *src)
{
	Boolean extension=false;

	while (*src) {
		if (StrChr(phoneChars, *src))
			*dst++=*src;
		else if (extension==false) {
			*dst++='#';
			extension=true;
		}

		src++;
	}

	*dst=0;
}
