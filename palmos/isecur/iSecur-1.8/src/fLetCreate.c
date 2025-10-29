/*
 * Let.c
 */
#include "Include.h"

/* protos */
_UTL(static Boolean EH(EventType *));
_UTL(static void CreateSlet(UInt32));
_UTL(static void SetResource(DmOpenRef, DmResType, DmResID, UInt8 *, UInt32));

/* globals */
extern Char name[dmDBNameLength];
extern DB dbData;

/*
 * fLetCreateRun
 *
 * Create iSecurLet pointing to this secret.
 *
 *  -> rIdx		Record Index.
 */
void
fLetCreateRun(UInt16 rIdx)
{
	FormSave frm;
	MemHandle mh;
	RecordHdr *h;
	UInt32 id;

	if (DmRecordInfo(dbData.db, rIdx, NULL, &id, NULL)!=errNone) {
		FrmAlert(aNoRecByID);
		return;
	}

	UIFormPrologue(&frm, fLet, EH);

	mh=DBGetRecord(&dbData, rIdx);
	h=MemHandleLock(mh);
	if (h->flags&RHHasTitle) {
		UInt8 *p=((UInt8 *)h)+sizeof(RecordHdr);

		StrNCopy(name, p, 31);
	} else
		StrCopy(name, "Untitled");

	MemHandleUnlock(mh);

	UIFieldSelectAll(cLetFldName, name, true);

	if (UIFormEpilogue(&frm, NULL, cLetCancel)==false)
		return;

	CreateSlet(id);
}

/*
 * EH
 */
Boolean
EH(EventType *ev)
{
	if ((ev->eType==ctlSelectEvent) && (ev->data.ctlSelect.controlID==cLetOK)) {
		Char *p=UIFieldGetText(cLetFldName);

		if ((p) && (StrLen(p)>0))
			StrNCopy(name, p, 31);
		else
			StrCopy(name, "Untitled");
	}
	return false;
}

/*
 * CreateSlet
 *
 * Create the iSecurLet.
 *
 *  -> sID		Secret ID.
 */
static void
CreateSlet(UInt32 sID)
{
	UInt16 attrs=0;
	MemHandle mh;
	DB db, psys;

	DBInit(&db, name);
	switch (DBFindDatabase(&db)) {
	case FDFound:
		FrmAlert(aSletAlreadyExist);
		return;
	case FDError:
		FrmAlert(aCannotCreateSlet);
		return;
	default:
		break;
	}

	ErrFatalDisplayIf(DmCreateDatabase(0, name, CRID, (UInt32)'slet', true)!=errNone, "(CreateSlet) Cannot create database");

	if (DBOpen(&db, dmModeReadWrite, true)==false) {
		DmDeleteDatabase(db.card, db.id);
		return;
	}

	mh=DmGetResource(bitmapRsc, SLetLargeIcon);
	ErrFatalDisplayIf(mh==NULL, "(CreateSlet) Cannot load large icon");
	SetResource(db.db, 'tAIB', 1000, MemHandleLock(mh), MemHandleSize(mh));
	MemHandleUnlock(mh);
	DmReleaseResource(mh);

	mh=DmGetResource(bitmapRsc, SLetSmallIcon);
	ErrFatalDisplayIf(mh==NULL, "(CreateSlet) Cannot load small icon");
	SetResource(db.db, 'tAIB', 1001, MemHandleLock(mh), MemHandleSize(mh));
	MemHandleUnlock(mh);
	DmReleaseResource(mh);

	SetResource(db.db, 'tVER', 1, "1.0", 4);
	
	DBInit(&psys, "psysLaunchDB");
	if (DBOpen(&psys, dmModeReadOnly, false)==true) {
		if (CategoryFind(psys.db, "iSecurLets")!=dmAllCategories)
			SetResource(db.db, 'taic', 1000, "iSecurLets", 11);

		DBClose(&psys);
	}
	
	SetResource(db.db, 'tint', iSecurLetResID, (UInt8 *)&sID, sizeof(UInt32));
	DmDatabaseInfo(db.card, db.id, NULL, &attrs, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	attrs|=dmHdrAttrLaunchableData;

	DmSetDatabaseInfo(db.card, db.id, NULL, &attrs, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	DBClose(&db);

	FrmAlert(aSletCreated);
}

/*
 * SetResource
 *
 * Set resource in prc file.
 *
 *  -> db		DmOpenRef.
 *  -> type		DmResType.
 *  -> id		DmResID.
 *  -> src		Source data.
 *  -> srcLen		Source length.
 */
static void
SetResource(DmOpenRef db, DmResType type, DmResID id, UInt8 *src, UInt32 srcLen)
{
	MemHandle mh=DmNewResource(db, type, id, srcLen);
	Err err;

	ErrFatalDisplayIf(mh==NULL, "(SetResource) Cannot add resource");
	err=DmWrite(MemHandleLock(mh), 0, src, srcLen);
	MemHandleUnlock(mh);
	DmReleaseResource(mh);

	ErrFatalDisplayIf(err!=errNone, "(SetResource) Cannot write resource");
}
