/*
 * fArchive.c
 */
#include "Include.h"

/* protos */
static void Archive(void);

/* globals */
static UInt16 focusBand[]={ cArchiveDays, cArchiveString, 0 };
static UInt16 before, category, gCardNo;
static LocalID gLocalID;
static Boolean archiveSet;
static Char archiveName[dmDBNameLength];

/*
 * fArchiveInit
 */
Boolean
fArchiveInit(void)
{
	before=0;
	archiveSet=false;
	DTSelectorSetDate(cArchiveSelector, selector1, before);

	category=prefs.categoryIdx;

	UIPopupSet(&dbData, category, categoryName, cArchivePopup);

	StrCopy(archiveName, "Not Set");
	CtlSetLabel(UIObjectGet(cArchiveName), archiveName);
	CtlSetValue(UIObjectGet(cArchiveMove), 1);
	CtlSetValue(UIObjectGet(cArchiveCopy), 0);

	UIFieldFocus(cArchiveDays);
	return true;
}

/*
 * fArchiveEH
 */
Boolean
fArchiveEH(EventType *ev)
{
	UInt16 y, m, d;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cArchivePopup:
			CategorySelect(dbData.db, currentForm, cArchivePopup, cArchiveList, true, &category, categoryName, CATNONEDITABLES, categoryHideEditCategory);
			return true;

		case cArchiveSelector:
			if (before)
				DTDateUnpack(before, &y, &m, &d);
			else
				DTDateUnpack(DTToday(), &y, &m, &d);

			if (SelectDay(selectDayByDay, &m, &d, &y, "Select Before Date")==true) {
				before=DTDatePack(y, m, d);
				DTSelectorSetDate(cArchiveSelector, selector1, before);
			}
			return true;

		case cArchiveName:
			fManagerRun("Select Archive", &gCardNo, &gLocalID);
			ErrFatalDisplayIf(DmDatabaseInfo(gCardNo, gLocalID, archiveName, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)!=errNone, "(ArchiveFormEventHandler) Cannot get database name.");
			CtlSetLabel(UIObjectGet(cArchiveName), archiveName+5);
			archiveSet=true;
			return true;

		case cArchiveOK:
			if (archiveSet==false) {
				FrmAlert(aArchiveNotSet);
				return true;
			}

			if (FrmAlert(aArchiveOK)==0) {
				Archive();
			} else
				return true;

		case cArchiveCancel:	/* FALL-THRU */
			FrmGotoForm(fMain);
			return true;
		}
		break;

	case keyDownEvent:
		UIFieldRefocus(focusBand, ev->data.keyDown.chr);
	default:
		break;
	}

	return false;
}

/*
 * Archive
 *
 * Perform archiving.
 */
static void
Archive(void)
{
	Boolean olderFlag=false, searchFlag=false, shouldArchive;
	DmOpenRef db=dbData.db, aDb;
	Boolean move=true;
	Int32 older=0;
	UInt16 archiveCnt=0, recIdx=0, olderDate=DTToday(), aRecIdx, attrs;
	MemHandle mh=NULL, aMh;
	void *bmh=NULL;
	Char *txt;
	TTask *t;
	UInt32 mhSize;
	Char catName[dmCategoryLength];

	aDb=DmOpenDatabase(gCardNo, gLocalID, dmModeReadWrite);
	ErrFatalDisplayIf(aDb==NULL, "(Archive) Cannot open selected archive.");

	if (CtlGetValue(UIObjectGet(cArchiveCopy)))
		move=false;

	if ((txt=UIFieldGetText(cArchiveDays))) {
		if (StrLen(txt)>0) {
			older=StrAToI(txt);
			DateAdjust((DateType *)&olderDate, -older);
			olderFlag=true;
		}
	}

	if ((txt=UIFieldGetText(cArchiveString))) {
		if (StrLen(txt)>0) {
			bmh=BMHOpen(txt);
			ErrFatalDisplayIf(bmh==NULL, "(Archive) Out of memory.");
			searchFlag=true;
		}
	}

	for (EVER) {
		if (DmSeekRecordInCategory(db, &recIdx, 0, dmSeekForward, category)!=errNone)
			break;

		mh=DBGetRecord(recIdx);
		t=MemHandleLock(mh);

		shouldArchive=false;
		if (before) {
			if (t->date<before)
				shouldArchive=true;
		}

		if (shouldArchive==false) {
			if (olderFlag==true) {
				if (t->date<olderDate)
					shouldArchive=true;
			}

			if (shouldArchive==false && searchFlag==true) {
				if (BMHSearch(bmh, (UInt8 *)t+sizeof(TTask))>-1)
					shouldArchive=true;
			}
		}

		MemHandleUnlock(mh);

		if (shouldArchive==false) {
			if (before==0 && olderFlag==false && searchFlag==false)
				shouldArchive=true;
		}

		if (shouldArchive==true) {
			mhSize=MemHandleSize(mh);
			aRecIdx=dmMaxRecordIndex;
			aMh=DmNewRecord(aDb, &aRecIdx, mhSize);
			ErrFatalDisplayIf(aMh==NULL, "(Archive) Device is full.");

			DmWrite(MemHandleLock(aMh), 0, MemHandleLock(mh), mhSize);
			MemHandleUnlock(mh);
			MemHandleUnlock(aMh);

			DmRecordInfo(db, recIdx, &attrs, NULL, NULL);
			attrs|=dmRecAttrDirty;
			DmSetRecordInfo(aDb, aRecIdx, &attrs, NULL);

			if (move==true)
				DmDeleteRecord(db, recIdx);
			else
				recIdx++;

			archiveCnt++;
		} else
			recIdx++;
	}

	if (archiveCnt) {
		DmQuickSort(aDb, TaskCompare, 0);

		/* move categories as well .. */
		for (attrs=0; attrs<dmRecNumCategories; attrs++) {
			CategoryGetName(db, attrs, catName);
			CategorySetName(aDb, attrs, catName);
		}
	}

	BMHClose(bmh);
	DmCloseDatabase(aDb);

	StrPrintF(selector1, "%u", archiveCnt);
	FrmCustomAlert(aArchiveResult, selector1, (archiveCnt==1) ? " " : "s ", (move==true) ? "moved" : "copied");
}
