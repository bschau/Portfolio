/*
 * fCreateDatabase.c
 */
#include "Include.h"

/* macros */
#ifdef DEBUG
#define Rounds 1
#else
#define Rounds 8
#endif

/* protos */
_UTL(static void UpdateCounter(void));
_UTL(static void CreateDatabase(void));
_UTL(static void LoadDefaults(void));

/* globals */
extern UInt16 sCnt, rounds, lastX, lastY;
extern UInt16 libRefNum;
extern MemHandle aesData, sha256Data;
extern UInt8 gKey[AESKeyLength];
extern DB dbData;
extern Prefs prefs;
extern Char catName1[dmCategoryLength];
extern Char gPwd[];

/*
 * fCreateDatabaseInit
 */
Boolean
fCreateDatabaseInit(void)
{
	UInt16 idx;

	sCnt=0;
	rounds=AESKeyLength*Rounds;

	Random(TimGetSeconds());
	for (idx=0; idx<AESKeyLength; idx++)
		gKey[idx]=(UInt8)Random(0);

	return true;
}

/*
 * fCreateDatabaseEH
 */
Boolean
fCreateDatabaseEH(EventType *ev)
{
	EntropyAdd(pool, ev);

	switch (ev->eType) {
	case frmOpenEvent:
	case frmUpdateEvent:
		FrmDrawForm(currentForm);
		UpdateCounter();
		return true;

	case penMoveEvent:
		if ((lastX!=ev->screenX) || (lastY!=ev->screenY)) {
			gKey[sCnt]^=(ev->screenX^ev->screenY);
			if (sCnt==(AESKeyLength-1))
				sCnt=0;
			else
				sCnt++;

			rounds--;
			if (rounds==0) {
				fPasswordRun(PwdDialogConfirm, NULL, 0, 1, false, DBVERSION);
				if (appStopped) {
					break;
				}

//				fPasswordRunMullify(pwd, pwd);
				DPrint("Password is '%s'\n", gPwd);
				CreateDatabase();
				if (FrmAlert(aSetNewHint)==0)
					fPasswordHintRun();

				FrmEraseForm(currentForm);
				FrmGotoForm(fMain);
				return true;
			} else 
				UpdateCounter();
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * UpdateCounter
 *
 * Draw the counter.
 */
static void
UpdateCounter(void)
{
	FontID fID=FntSetFont(boldFont);
	RectangleType r;
	Char buffer[12];

	RctSetRectangle(&r, 80, 92, 20, 12);
	WinEraseRectangle(&r, 0);

	StrPrintF(buffer, "%u", rounds);
	WinDrawChars(buffer, StrLen(buffer), r.topLeft.x, r.topLeft.y);
	FntSetFont(fID);
}

/*
 * CreateDatabase
 *
 * Create the database and store the key and SHA256 sum of password.
 */
static void
CreateDatabase(void)
{
	DBInit(&dbData, DBNameData);

	ErrFatalDisplayIf(DmCreateDatabase(0, dbData.name, (UInt32)CRID, (UInt32)'DATA', false)!=errNone, "(CreateDatabase) Cannot create database");

	if (DBOpen(&dbData, dmModeReadWrite, true)==true) {
		MemHandle mh;

		if ((mh=DmNewHandle(dbData.db, sizeof(AppInfoBlock)))) {
			UInt16 attrs=dmHdrAttrBackup, version=DBVERSION;
			LocalID appId;

			if (prefs.flags&PFlgNoBeam)
				attrs|=dmHdrAttrCopyPrevention;

			appId=MemHandleToLocalID(mh);
			if (DmSetDatabaseInfo(dbData.card, dbData.id, NULL, &attrs, &version, NULL, NULL, NULL, NULL, &appId, NULL, NULL, NULL)==errNone) {

				SetPassword(MemHandleLock(mh), gPwd, 1);
				MemHandleUnlock(mh);
				return;
			}
		}

		DBClose(&dbData);
	}

	DmDeleteDatabase(dbData.card, dbData.id);
	ErrFatalDisplay("(CreateDatabase) Error while creating database");
	SysReset();
}

/*
 * LoadDefaults
 *
 * Load default templates.
 */
static void
LoadDefaults(void)
{
	MemHandle mh=MemHandleNew(514);
	UInt8 *title, *secret;
	UInt16 idx;

	ErrFatalDisplayIf(mh==NULL, "(LoadDefaults) Out of memory");
	title=MemHandleLock(mh);
	secret=title+257;

#if DEBUG
	if (DmNumRecordsInCategory(dbData.db, dmAllCategories)>0) {
		title[1]=0;
		secret[1]=0;
		for (idx='a'; idx<='z'; idx++) {
			*title=(UInt8)idx;
			*secret=(UInt8)idx;
			DBSetRecord(&dbData, dmMaxRecordIndex, dmUnfiledCategory, RHHasTitle|RHHasSecret, noIconSelected, title, secret, DBSort);
		}
	}
#endif

	MemSet(title, 514, 0);
	MemSet(catName1, sizeof(catName1), 0);
	SysStringByIndex(strDefEntryCard, 0, catName1, sizeof(catName1)-1);
	SysStringByIndex(strDefEntryCard, 1, title, 256);
	SysStringByIndex(strDefEntryCard, 2, secret, 256);
	idx=CategoryFind(dbData.db, catName1);
	DBSetRecord(&dbData, dmMaxRecordIndex, idx, RHTemplate|RHHasTitle|RHHasSecret, noIconSelected, title, secret, DBSort);

	MemSet(title, 514, 0);
	MemSet(catName1, sizeof(catName1), 0);
	SysStringByIndex(strDefEntryPassword, 0, catName1, sizeof(catName1)-1);
	SysStringByIndex(strDefEntryPassword, 1, title, 256);
	SysStringByIndex(strDefEntryPassword, 2, secret, 256);
	idx=CategoryFind(dbData.db, catName1);
	DBSetRecord(&dbData, dmMaxRecordIndex, idx, RHTemplate|RHHasTitle|RHHasSecret, noIconSelected, title, secret, DBSort);

	MemHandleFree(mh);
}

/**
 * Set password and optionally initialize database.
 *
 * @param mh AppInfoBlock Handle.
 * @param pwd New password.
 * @param init Set to 1 to initialize database.
 */
void
SetPassword(AppInfoBlock *a, Char *pwd, Int16 init)
{
	Char buffer[PasswordLength];

	DPrint("SetPassword entered\n");

	if (init) {
		DPrint("Needs to init\n");
		DmSet(a, 0, sizeof(AppInfoBlock), 0);
		DPrint("DmSet OK\n");
		CategoryInitialize((AppInfoType *)a, defCategories);
		DPrint("CategoryInitialize OK\n");
	}

	SHA256Init();
	DPrint("SHA256Init OK\n");
	SHA256Transform(pwd, PasswordLength);
	DPrint("SHA256Transform OK\n");
	SHA256Final();
	DPrint("SHA256Final OK\n");

	DmWrite(a, OffsetOf(AppInfoBlock, digest), MemHandleLock(sha256Data), SHA256DigestLength);
	DPrint("DmWrite OK\n");
	MemHandleUnlock(sha256Data);

	AESCreateKeys(pwd);
	DPrint("AESCreateKeys OK\n");

	MemMove(buffer, gKey, AESKeyLength);
	AESEncrypt(buffer, buffer);
	AESEncrypt(buffer+16, buffer+16);
	DPrint("AESEncrypt OK\n");

	DmWrite(a, OffsetOf(AppInfoBlock, key), buffer, AESKeyLength);
	DPrint("DmWrite OK\n");

	AESCreateKeys(gKey);
	DPrint("AESCreateKey OK\n");

	if (init)
		LoadDefaults();
}
