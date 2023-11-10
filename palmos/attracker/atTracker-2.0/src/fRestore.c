/*
 * fRestore.c
 */
#include "Include.h"

/* protos */
static void Restore(void);

/* globals */
static UInt16 gCardNo;
static LocalID gLocalID;
static Boolean archiveSet;
static Char archiveName[dmDBNameLength];

/*
 * fRestoreInit
 */
Boolean
fRestoreInit(void)
{
	archiveSet=false;

	StrCopy(archiveName, "Not Set");
	CtlSetLabel(UIObjectGet(cRestoreName), archiveName);
	return true;
}

/*
 * fRestoreEH
 */
Boolean
fRestoreEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cRestoreName:
			fManagerRun("Select Archive", &gCardNo, &gLocalID);
			ErrFatalDisplayIf(DmDatabaseInfo(gCardNo, gLocalID, archiveName, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)!=errNone, "(RestoreFormEventHandler) Cannot get database name.");
			CtlSetLabel(UIObjectGet(cRestoreName), archiveName+5);
			archiveSet=true;
			return true;

		case cRestoreOK:
			if (archiveSet==false) {
				FrmAlert(aArchiveNotSet);
				return true;
			}

			if (FrmAlert(aRestoreOK)==0)
				Restore();
			else
				return true;

		case cRestoreCancel:	/* FALL-THRU */
			FrmGotoForm(fMain);
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * Restore
 *
 * Restore tasks to the 'unfiled' category.
 */
static void
Restore(void)
{
	DmOpenRef db=dbData.db, aDb;
	UInt16 restoreCnt=0, aRecIdx=0, attrs=dmRecAttrDirty|dmUnfiledCategory, recIdx;
	MemHandle mh=NULL, aMh;
	UInt32 mhSize;

	aDb=DmOpenDatabase(gCardNo, gLocalID, dmModeReadOnly);
	ErrFatalDisplayIf(aDb==NULL, "(Restore) Cannot open selected archive.");

	for (EVER) {
		if (DmSeekRecordInCategory(aDb, &aRecIdx, 0, dmSeekForward, dmAllCategories)!=errNone)
			break;

		aMh=DmQueryRecord(aDb, aRecIdx);
		ErrFatalDisplayIf(aMh==NULL, "(Restore) Cannot retrieve record.");
		mhSize=MemHandleSize(aMh);

		recIdx=dmMaxRecordIndex;
		mh=DmNewRecord(db, &recIdx, mhSize);
		ErrFatalDisplayIf(mh==NULL, "(Restore) Device is full.");

		DmWrite(MemHandleLock(mh), 0, MemHandleLock(aMh), mhSize);
		MemHandleUnlock(aMh);
		MemHandleUnlock(mh);

		DmSetRecordInfo(db, recIdx, &attrs, NULL);

		aRecIdx++;
		restoreCnt++;
	}

	if (restoreCnt)
		DmQuickSort(db, TaskCompare, 0);

	DmCloseDatabase(aDb);

	StrPrintF(selector1, "%u", restoreCnt);
	FrmCustomAlert(aRestoreResult, selector1, (restoreCnt==1) ? " " : "s ", NULL);
}
