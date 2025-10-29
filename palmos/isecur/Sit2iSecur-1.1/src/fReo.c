/*
 * fReo.c
 */
#include "Include.h"

/* macros */
#define RectExtentX 150
enum { SLoginSIT=0, SLoginISecur, SMerge, SCopySecrets, SCopyTemplates, SSort, SError };
/* The following steps are defined:
 *
 * 1.	Login to Secure It
 * 2.	Login to iSecur
 * 3.	Merge categories
 * 4.	Copy Secrets
 * 5.	Copy Templates
 * 6.	Sort database
 */

/* protos */
static Boolean EH(EventType *);
static void SetHeader(Char *);
static void SetText(Char *);
static void UpdateStatus(void);
static void LoginSIT(Char *);
static void LoginISecur(Char *);
static void Merge(void);
static Int16 AddCategory(Char *);
static void CopySecrets(void);
static void CopyTemplates(void);

/* globals */
static Char *pwdDummy="1234567890abcdefghijklmnopqrstuvwxyz";
static Char header[100], text[100];
static UInt16 step, oldStep, hdrLen, hdrWidth, txtLen;
static UInt32 loopCounter;
static DB dbSit, dbISecur;
static MemHandle blowfishKeyH;
static UInt8 gKey[AESKeyLength];
static Int16 categories[dmRecNumCategories];

/*
 * fReoClose
 *
 * Close all ...
 */
void
fReoClose(void)
{
	DBClose(&dbISecur);
	DBClose(&dbSit);
	if (blowfishKeyH)
		MemHandleFree(blowfishKeyH);
}

/*
 * fReoRun
 *
 * Run the reorganization.
 *
 *  -> pwdSit		Password to Secure It.
 *  -> pwdISecur	Password to iSecur.
 */
void
fReoRun(Char *pwdSit, Char *pwdISecur)
{
	FormType *saveCurrent=currentForm;
	UInt16 saveID=currentFormID, wait=1, err;
	Boolean runLoop=true;
	FormActiveStateType frmSave;
	EventType ev;
	RectangleType r;

	step=SLoginSIT;
	oldStep=step;
	txtLen=0;
	hdrLen=0;
	blowfishKeyH=NULL;

	FrmSaveActiveState(&frmSave);

	currentFormID=fReo;
	currentForm=FrmInitForm(currentFormID);
	ErrFatalDisplayIf(currentForm==NULL, "(fReoRun) Cannot initialize form.");
	FrmSetActiveForm(currentForm);
	FrmSetEventHandler(currentForm, (FormEventHandlerType *)EH);

	RctSetRectangle(&r, 0, 15, 160, 145);
	WinEraseRectangle(&r, 0);

	MemSet(&ev, sizeof(EventType), 0);
	ev.eType=frmOpenEvent;
	ev.data.frmLoad.formID=currentFormID;
	EvtAddEventToQueue(&ev);
	
	while (runLoop) {
		if (!(loopCounter&15)) {
			EvtGetEvent(&ev, wait);
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

		loopCounter++;
		if (step!=SError)
			oldStep=step;

		switch (step) {
		case SLoginSIT:
			LoginSIT(pwdSit);
			break;
		case SLoginISecur:
			LoginISecur(pwdISecur);
			break;
		case SMerge:
			Merge();
			break;
		case SCopySecrets:
			CopySecrets();
			break;
		case SCopyTemplates:
			CopyTemplates();
			break;
		case SSort:
			SetText("Sorting database");
			DmQuickSort(dbISecur.db, DBSort, 0);
			FrmGotoForm(fDone);
			runLoop=false;
			break;
		case SError:
			wait=evtWaitForever;
			break;
		}
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
		UpdateStatus();
		return true;

	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * SetHeader
 */
static void
SetHeader(Char *h)
{
	FontID fID=FntSetFont(boldFont);

	StrNCopy(header, h, sizeof(header));
	hdrLen=(UInt16)StrLen(header);
	hdrWidth=FntCharsWidth(header, hdrLen);
	FntSetFont(fID);
	UpdateStatus();
}

/*
 * SetText
 */
static void
SetText(Char *t)
{
	StrNCopy(text, t, sizeof(text));
	txtLen=(UInt16)StrLen(text);
	UpdateStatus();
}

/*
 * UpdateStatus
 */
static void
UpdateStatus(void)
{
	RectangleType r;

	RctSetRectangle(&r, 0, 40, 160, 12);
	WinEraseRectangle(&r, 0);

	if (hdrLen) {
		FontID fID=FntSetFont(boldFont);
		WinDrawChars(header, hdrLen, (160-hdrWidth)/2, r.topLeft.y);
		FntSetFont(fID);
	}

	RctSetRectangle(&r, (160-RectExtentX)/2, 60, RectExtentX, 16);
	WinDrawRectangleFrame(rectangleFrame, &r);

	r.extent.x=(RectExtentX/SError)*oldStep;
	WinInvertRectangle(&r, 0);

	RctSetRectangle(&r, 0, 80, 160, 80);
	WinEraseRectangle(&r, 0);

	if (txtLen) {
		UInt16 tLen=txtLen, y=r.topLeft.y, pW, cnt;
		Char *p=text;
		Boolean emitted=false;

		while (tLen>0) {
			pW=r.extent.x;
			cnt=FntWordWrap(p, pW);
			tLen-=cnt;
			if (!emitted && !tLen) {
				pW=FntCharsWidth(p, cnt);
				WinDrawChars(p, cnt, ((r.extent.x-pW)/2)+r.topLeft.x, y);
			} else {
				WinDrawChars(p, cnt, 0, y);
				emitted=true;
			}

			y+=12;
			if (y>148)
				break;

			p+=(UInt32)cnt;
		}
	}
}

/*
 * LoginSIT
 *
 * Login to Secure It.
 *
 *  -> pwd		Password.
 */
static void
LoginSIT(Char *pwd)
{
	SitAppInfoBlock *ai;
	LocalID appID;
	UInt32 md5[4];
	Int16 ret;

	SetHeader("Logging on to Secure It");

	SetText("Locating database");

	DBInit(&dbSit, "Secure It Secrets");
	if (DBOpen(&dbSit, dmModeReadOnly, false)==false) {
		SetText("Failed to locate the Secure It Secrets database. You don't need to run this program on this device.");
		step=SError;
		return;
	}

	SetText("Verifying password");

	if (DmDatabaseInfo(dbSit.card, dbSit.id, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &appID, NULL, NULL, NULL)!=errNone) {
		SetText("Failed to find password");
		step=SError;
		return;
	}
	
	MD5Sum(md5, pwd);
	ai=(SitAppInfoBlock *)MemLocalIDToLockedPtr(appID, dbSit.card);
	ret=MemCmp(ai->md5, md5, 4*sizeof(UInt32));
	MemPtrUnlock(ai);

	if (ret) {
		SetText("Login error");
		step=SError;
		return;
	}

	SetText("Generating Blowfish key");
	if ((blowfishKeyH=MemHandleNew(sizeof(BlowfishKey)))==NULL) {
		SetText("Out of memory while generating Blowfish key");
		step=SError;
		return;
	}

	BlowfishMakeKey(MemHandleLock(blowfishKeyH), pwd, StrLen(pwd));
	MemHandleUnlock(blowfishKeyH);

	step=SLoginISecur;
}

/*
 * LoginISecur
 *
 * Login to iSecur.
 *
 *  -> pwd		Password.
 */
static void
LoginISecur(Char *pwd)
{
	UInt16 pIdx=0, dIdx=0, cnt, version;
	ISecurAppInfoBlock *ai;
	Char passwd[PasswordLength];
	LocalID appID;
	SHA256 ctx;
	Int16 ret;

	SetHeader("Logging on to iSecur");

	SetText("Locating database");

	DBInit(&dbISecur, "iSec_iSecur Data");
	if (DBOpen(&dbISecur, dmModeReadWrite, false)==false) {
		SetText("Failed to locate the iSec_iSecur Data database. Please install iSecur and create an empty database.");
		step=SError;
		return;
	}

	SetText("Verifying password");

	if (DmDatabaseInfo(dbISecur.card, dbISecur.id, NULL, NULL, &version, NULL, NULL, NULL, NULL, &appID, NULL, NULL, NULL)!=errNone) {
		SetText("Failed to find password");
		step=SError;
		return;
	}

	switch (version) {
	case 1:
		for (cnt=0; cnt<PasswordLength; cnt++) {
			passwd[dIdx++]=pwd[pIdx++];
			if (!pwd[pIdx])
				pIdx=0;
		}
		break;
	case 2:
		for (cnt=0; cnt<PasswordLength; cnt++) {
			if (!pwd[pIdx]) {
				pIdx=0;
				pwd=pwdDummy;
			}
			passwd[dIdx++]=pwd[pIdx++];
		}
		break;
	default:
		SetText("Unsupported database version");
		step=SError;
		return;
	}
	
	SHA256Init(&ctx);
	SHA256Transform(&ctx, passwd, PasswordLength);
	SHA256Final(&ctx);

	ai=(ISecurAppInfoBlock *)MemLocalIDToLockedPtr(appID, dbSit.card);
	ret=MemCmp(ai->digest, ctx.digest, 32);
	MemPtrUnlock(ai);

	if (ret) {
		SetText("Login error");
		step=SError;
		return;
	}

	SetText("Creating iSecur encryption key");
	AESCreateKeys(&aes, passwd);
	MemMove(gKey, ai->key, AESKeyLength);
	AESDecrypt(&aes, gKey, gKey);
	AESDecrypt(&aes, gKey+16, gKey+16);
	AESCreateKeys(&aes, gKey);
	MemSet(gKey, AESKeyLength, 0);

	step=SMerge;
}

/*
 * Merge
 *
 * Merge categories.
 */
static void
Merge(void)
{
	Char catName[dmCategoryLength+2];
	UInt16 cIdx;
	Int16 category;

	SetHeader("Merging Categories");
	SetText("Initializing Category index");
	for (cIdx=0; cIdx<dmRecNumCategories; cIdx++)
		categories[cIdx]=-1;

	for (cIdx=0; cIdx<dmRecNumCategories; cIdx++) {
		CategoryGetName(dbSit.db, cIdx, catName);
		if (*catName) {
			if ((category=AddCategory(catName))!=-1)
				categories[cIdx]=category;
		}
	}

#if DEBUG
	for (cIdx=0; cIdx<dmRecNumCategories; cIdx++) {
		CategoryGetName(dbSit.db, cIdx, catName);
		DPrint("Category (%u): %s\n", cIdx, catName);
	}
#endif

	step=SCopySecrets;
}

/*
 * AddCategory
 *
 * Add a category to the iSecur database (if possible).
 *
 *  -> catName		Category name.
 *
 * Returns >=0 if succeeded, -1 if error.
 */
static Int16
AddCategory(Char *catName)
{
	Int16 firstFree=-1;
	ISecurAppInfoBlock *ai;
	LocalID appID;
	UInt16 cnt;
	Char cName[dmCategoryLength+2], buffer[32+dmCategoryLength];

	StrPrintF(buffer, "Adding %s", catName);
	SetText(buffer);
	if (DmDatabaseInfo(dbISecur.card, dbISecur.id, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &appID, NULL, NULL, NULL)!=errNone) {
		SetText("Failed to find Categories");
		return -1;
	}

	ai=(ISecurAppInfoBlock *)MemLocalIDToLockedPtr(appID, dbSit.card);
	for (cnt=0; cnt<dmRecNumCategories; cnt++) {
		CategoryGetName(dbISecur.db, cnt, cName);
		if (*cName) {
			if (StrCaselessCompare(cName, catName)==0) {
				MemPtrUnlock(ai);
				return (Int16)cnt;
			}
		} else {
			if (firstFree==-1)
				firstFree=(Int16)cnt;
		}
	}

	MemPtrUnlock(ai);
	if (firstFree==-1)
		return -1;

	CategorySetName(dbISecur.db, firstFree, catName);

	return (Int16)firstFree;
}

/*
 * CopySecrets
 *
 * Copy the secrets across.
 */
static void
CopySecrets(void)
{
	UInt16 rIdx=0, offset, sLen, cnt, attrs;
	Int16 category;
	MemHandle mh, th, sh;
	Char *text, *title;
	UInt8 *sit, *t, *b;
	BlowfishKey *bKey;
	EventType ev;

	SetHeader("Copying Secrets");
	if ((th=MemHandleNew(300))==NULL) {
		SetText("Out of memory");
		step=SError;
		return;
	}

	bKey=MemHandleLock(blowfishKeyH);
	text=MemHandleLock(th);
	StrCopy(text, "Copying: ");
	title=text+StrLen(text);

	while ((mh=DmQueryNextInCategory(dbSit.db, &rIdx, dmAllCategories))!=NULL) {
		EvtGetEvent(&ev, 1);

		if (DmRecordInfo(dbSit.db, rIdx, &attrs, NULL, NULL)==errNone) {
			if ((category=categories[attrs&dmRecAttrCategoryMask])==-1)
				category=0;
		} else
			category=0;

		if ((sh=MemHandleNew(MemHandleSize(mh)))!=NULL) {
			sit=MemHandleLock(sh);
			MemMove(sit, MemHandleLock(mh), MemHandleSize(mh));
			MemHandleUnlock(mh);

			offset=((*sit<<8)|(*(sit+1)));

			t=sit+3+((UInt32)offset);
			sLen=StrLen(t);
			MemMove(title, t, sLen+1);

			b=sit+2;
			for (cnt=0; cnt<(offset/8); cnt++) {
				BlowfishDecrypt(bKey, b);
				b+=8;
			}

			SetText(text);
			DBSetRecord(&dbISecur, dmMaxRecordIndex, category, 0, noIconSelected, title, sit+2, NULL);
			MemHandleFree(sh);
		} else {
			SetText("Out of memory");
			MemHandleFree(th);
			step=SError;
			return;
		}
		rIdx++;
	}


	MemHandleFree(th);
	step=SCopyTemplates;
}

/*
 * CopyTemplates
 *
 * Copy the templates across.
 */
static void
CopyTemplates(void)
{
	UInt16 rIdx=0, offset, sLen, cnt;
	Int16 category;
	MemHandle mh, th, sh;
	Char *text, *title;
	UInt8 *sit, *t, *b;
	BlowfishKey *bKey;
	EventType ev;
	DB db;

	SetHeader("Copying Templates");
	DBInit(&db, "Secure It Templates");
	switch (DBFindDatabase(&db)) {
	case FDFound:
		break;
	case FDNotFound:
		step=SSort;
		return;
	default:
		SetText("Error while searching databases");
		step=SError;
		return;
	}

	if (DBOpen(&db, dmModeReadOnly, false)==false) {
		SetText("Failed to open the Secure It Templates database");
		step=SError;
		return;
	}

	if ((th=MemHandleNew(300))==NULL) {
		DBClose(&db);
		SetText("Out of memory");
		step=SError;
		return;
	}

	SetText("Adding Templates Category - if possible ...");
	if ((category=CategoryFind(dbISecur.db, "Templates"))==dmAllCategories) {
		if ((category=AddCategory("Templates"))==-1)
			category=dmUnfiledCategory;
	}

	bKey=MemHandleLock(blowfishKeyH);
	text=MemHandleLock(th);
	StrCopy(text, "Copying: ");
	title=text+StrLen(text);

	while ((mh=DmQueryNextInCategory(db.db, &rIdx, dmAllCategories))!=NULL) {
		EvtGetEvent(&ev, 1);

		if ((sh=MemHandleNew(MemHandleSize(mh)))!=NULL) {
			sit=MemHandleLock(sh);
			MemMove(sit, MemHandleLock(mh), MemHandleSize(mh));
			MemHandleUnlock(mh);

			offset=((*sit<<8)|(*(sit+1)));

			t=sit+3+((UInt32)offset);
			sLen=StrLen(t);
			MemMove(title, t, sLen+1);

			b=sit+2;
			for (cnt=0; cnt<(offset/8); cnt++) {
				BlowfishDecrypt(bKey, b);
				b+=8;
			}

			SetText(text);
			DBSetRecord(&dbISecur, dmMaxRecordIndex, category, RHTemplate, noIconSelected, title, sit+2, NULL);
			MemHandleFree(sh);
		} else {
			SetText("Out of memory");
			DBClose(&db);
			MemHandleFree(th);
			step=SError;
			return;
		}
		rIdx++;
	}

	DBClose(&db);
	MemHandleFree(th);
	step=SSort;
}
