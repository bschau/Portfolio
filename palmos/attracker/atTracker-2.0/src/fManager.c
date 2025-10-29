/*
 * fManager.c
 */
#include "Include.h"

/* protos */
static Boolean FindDB(Char *, LocalID *, UInt16 *);
static void UpdateDBList(void);
static void UpdateTable(Boolean);
static void DrawTable(void *, Int16, Int16, RectangleType *);
static Boolean EH(EventType *);
static void NewDB(void);
static Boolean NewDBEH(EventType *);
static void Edit(UInt16);
static Boolean EditEH(EventType *);

/* globals */
static Boolean gNeedSel;
static MemHandle dbIDs;
static UInt16 dbCount, gCardNo, gIdx;
static LocalID gLocalID;
static Table tblMgr;

/*
 * FindDB
 *
 * Locate a database given it's name - searches all cards.
 *
 *  -> n		Name.
 * <-  lId		Local ID.
 * <-  card		Card.
 *
 * Returns true (successful) or false (not found).
 */
static Boolean
FindDB(Char *n, LocalID *lId, UInt16 *card)
{
	UInt16 cards=MemNumCards(), idx;

	for (idx=0; idx<cards; idx++) {
		if ((*lId=DmFindDatabase(idx, n))) {
			*card=idx;
			return true;
		}
	}

	return false;
}

/*
 * UpdateDBList
 *
 * Update database list.
 */
static void
UpdateDBList(void)
{
	if (dbIDs) {
		MemHandleFree(dbIDs);
		dbIDs=NULL;
	}

	dbCount=0;
	SysCreateDataBaseList(DBARCHIVE, CRID, &dbCount, &dbIDs, true);
}

/*
 * UpdateTable
 *
 * Update table values.
 *
 *  -> redraw		Redraw table.
 */
static void
UpdateTable(Boolean redraw)
{
	UpdateDBList();
	tblMgr.records=dbCount;
	UITableUpdateValues(&tblMgr, redraw);
}

/*
 * DrawTable
 */
static void
DrawTable(void *tbl, Int16 row, Int16 column, RectangleType *r)
{
	Int32 recCnt=tblMgr.top+row;
	SysDBListItemType *dbList;
	UInt16 pw, sLen;
	Char *p;
	Boolean trunc;
	UInt32 numR;
	Char buffer[16];

	if (!dbIDs)
		return;

	if (dbCount && recCnt<dbCount) {
		dbList=MemHandleLock(dbIDs);

		TblSetRowData(tbl, row, recCnt);
		WinEraseRectangle(r, 0);

		pw=ARCHIVENAMEWIDTH;
		r->extent.x=pw;
		p=dbList[recCnt].name+5;
		sLen=StrLen(p);
		FntCharsInWidth(p, &pw, &sLen, &trunc);
		WinDrawChars(p, sLen, r->topLeft.x, r->topLeft.y);

		r->topLeft.x+=ARCHIVENAMEWIDTH+SPACING;
		pw=TASKSWIDTH;
		r->extent.x=pw;

		if (DmDatabaseSize(dbList[recCnt].cardNo, dbList[recCnt].dbID, &numR, NULL, NULL)==errNone)
			StrPrintF(buffer, "%lu", numR);
		else {
			*buffer='-';
			buffer[1]='\x00';
		}
		
		sLen=StrLen(buffer);
		FntCharsInWidth(buffer, &pw, &sLen, &trunc);
		WinDrawChars(buffer, sLen, r->topLeft.x+TASKSWIDTH-pw, r->topLeft.y);

		MemHandleUnlock(dbIDs);
	}
}

/*
 * fManagerRun
 *
 * Handle manager dialog.
 *
 *  -> t		Title.
 * <-  cNo		Card no (or NULL).
 * <-  dbId		LocalID (or NULL).
 *
 * Both cNo and dbId must be set if dbInfo needs to be returned.
 */
void
fManagerRun(Char *t, UInt16 *cNo, LocalID *dbId)
{
	TableStyle style[]={ {0, customTableItem }, { -1 } };
	FormSave frm;
	SysDBListItemType *dbList;

	dbIDs=NULL;
	dbCount=0;
	if (cNo==NULL && dbId==NULL)
		gNeedSel=false;
	else
		gNeedSel=true;

	UIFormPrologue(&frm, fManager, EH);
	FrmSetTitle(currentForm, t);
	UITableInit(NULL, &tblMgr, cManagerTable, cManagerScrollBar, DrawTable, style);
	UpdateTable(true);
	UIFormEpilogue(&frm, NULL, 0);

	if (gNeedSel==true) {
		dbList=MemHandleLock(dbIDs);
		*cNo=dbList[gIdx].cardNo;
		*dbId=dbList[gIdx].dbID;
	}

	if (dbIDs)
		MemHandleFree(dbIDs);
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	Int16 row, col;

	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cManagerOK:
			if (gNeedSel==true) {
				if (TblGetSelection(tblMgr.tbl, &row, &col)==false) {
					FrmAlert(aSelectArchive);
					return true;
				}
				gIdx=tblMgr.top+row;
			}
			break;
		case cManagerNew:
			NewDB();
			UpdateTable(false);
			return true;
		case cManagerEdit:
			if (TblGetSelection(tblMgr.tbl, &row, &col)==true) {
				Edit(tblMgr.top+row);
				UpdateTable(false);
			} else
				FrmAlert(aNeedsArchive);
			return true;
		default:
			break;
		}
		break;
	default:	/* FALL-THRU */
		UITableEvents(&tblMgr, ev);
		break;
	}
	return false;
}

/*
 * NewDB
 *
 * Prompt and possible create new database.
 */
static void
NewDB(void)
{
	FormSave frm;

	UIFormPrologue(&frm, fManagerNewDB, NewDBEH);
	UIFieldFocus(cManagerNewDBName);
	UIFormEpilogue(&frm, NULL, cManagerNewDBCancel);
}

/*
 * NewDBEH
 */
static Boolean
NewDBEH(EventType *ev)
{
	UInt16 attrs=dmHdrAttrBackup, dbVer=DBVERSION, card;
	Char *s;
	Char dbName[ArchiveNameLength+6];
	LocalID id;
	Err err;
	DmOpenRef db;
	MemHandle h;
	AppInfoBlock *appInfo;
	LocalID appID;

	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cManagerNewDBOK:
			if ((s=UIFieldGetText(cManagerNewDBName))==NULL)
				return true;

			if (StrLen(s)==0)
				return true;

			StrPrintF(dbName, "TTTX_%s", s);

			err=DmCreateDatabase(0, dbName, CRID, DBARCHIVE, false);
			if (err==dmErrAlreadyExists) {
				FrmAlert(aArchiveExists);
				return true;
			}

			if (err!=errNone) {
				FrmAlert(aArchiveUncreateable);
				return true;
			}

			if (FindDB(dbName, &id, &card)==false)
				ErrFatalDisplay("(NewDBHandler) Cannot find newly created archive?!?!");

			db=DmOpenDatabase(card, id, dmModeReadWrite);
			ErrFatalDisplayIf(db==NULL, "(NewDBHandler) Cannot open newly created archive.");
			h=DmNewHandle(db, sizeof(AppInfoBlock));
			ErrFatalDisplayIf(h==NULL, "(NewDBHandler) Cannot allocate AppInfoBlock.");
			appID=MemHandleToLocalID(h);
			DmSetDatabaseInfo(card, id, NULL, &attrs, &dbVer, NULL, NULL, NULL, NULL, &appID, NULL, NULL, NULL);
			appInfo=(AppInfoBlock *)MemHandleLock(h);
			DmSet(appInfo, 0, sizeof(AppInfoBlock), 0);
			CategoryInitialize((AppInfoType *)appInfo, defaultCategories);
			MemHandleUnlock(h);
			DmCloseDatabase(db);

			return false;
		default:
			break;
		}
	default:	/* FALL-THRU */
		break;
	}
	return false;
}

/*
 * Edit
 *
 * Handle edit dialog.
 *
 *  -> dbIdx		Index in dbIDs.
 */
static void
Edit(UInt16 dbIdx)
{
	FormSave frm;
	SysDBListItemType *dbList;

	gIdx=dbIdx;
	UIFormPrologue(&frm, fManagerEditDB, EditEH);
	dbList=MemHandleLock(dbIDs);
	gCardNo=dbList[dbIdx].cardNo;
	gLocalID=dbList[dbIdx].dbID;
	UIFieldSetText(cManagerEditDBName, dbList[dbIdx].name+5);
	MemHandleUnlock(dbIDs);
	UIFieldFocus(cManagerEditDBName);
	UIFormEpilogue(&frm, NULL, cManagerEditDBCancel);
}

/*
 * EditEH
 */
static Boolean
EditEH(EventType *ev)
{
	UInt16 d1, d2;
	Char *s;
	Char dbName[ArchiveNameLength+6];
	Err err;
	SysDBListItemType *dbList;
	Int16 cV;

	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cManagerEditDBOK:
			if ((s=UIFieldGetText(cManagerEditDBName))==NULL)
				return true;

			if (StrLen(s)==0)
				return true;

			StrPrintF(dbName, "TTTX_%s", s);

			dbList=MemHandleLock(dbIDs);
			cV=TxtGlueCaselessCompare(dbList[gIdx].name, StrLen(dbList[gIdx].name), &d1, dbName, StrLen(dbName), &d2);
			MemHandleUnlock(dbIDs);

			if (!cV)
				return false;

			err=DmSetDatabaseInfo(gCardNo, gLocalID, dbName, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
			if (err==dmErrAlreadyExists) {
				FrmAlert(aArchiveExists);
				return true;
			}

			if (err!=errNone) {
				FrmAlert(aArchiveUncreateable);
				return true;
			}
			return false;

		case cManagerEditDBDelete:
			if (FrmAlert(aDeleteArchive))
				return true;

			if (DmDeleteDatabase(gCardNo, gLocalID)==errNone) {
				if (dbIDs) {
					MemHandleFree(dbIDs);
					dbIDs=NULL;
				}
				return false;
			}

			FrmAlert(aCannotDeleteArchive);
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}
	return false;
}
