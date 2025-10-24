/*
 * fMain.c
 */
#include "Include.h"
#include "ToDoDB.h"

/* protos */
static void SaveData(void);
static void Sort(void);
static Int16 TasksSortClassic(void *, void *, Int16, SortRecordInfoType *, SortRecordInfoType *, MemHandle);
static Int16 TasksSortNew(void *, void *, Int16, SortRecordInfoType *, SortRecordInfoType *, MemHandle);
static Int16 DoSort(Char *, Char *, Int16);
static Char *GetDesc(ToDoDBRecordType *);

/* globals */
static Char *empty="";

/*
 * fMainInit
 */
Boolean
fMainInit(void)
{
	if (state.sortOrder==SSOAscending)
		CtlSetValue(UIObjectGet(cMainAsc), 1);
	else
		CtlSetValue(UIObjectGet(cMainDesc), 1);

	if (prefs.flags&PFlgLaunchTasks)
		CtlSetValue(UIObjectGet(cMainChkLaunch), 1);

	if (PMHasNotifications()) {
		UIObjectShow(cMainChkHotKey);
		UIObjectShow(cMainFldHotKey);

		if (prefs.flags&PFlgHotKey)
			CtlSetValue(UIObjectGet(cMainChkHotKey), 1);

		if (prefs.hotKey) {
			Char c[2];

			c[0]=(Char)prefs.hotKey;
			c[1]=0;
			UIFieldSetText(cMainFldHotKey, c);
			UIFieldFocus(cMainFldHotKey);
		}
	}

	return true;
}

/*
 * fMainEH
 */
Boolean
fMainEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case frmSaveEvent:
		if (appStopped)
			SaveData();
		break;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cMainSort:
			{
				MemHandle appls=NULL;
				UInt16 cnt=0;

				SaveData();

				Sort();

				if (prefs.flags&PFlgLaunchTasks) {
					if (SysCreateDataBaseList((UInt32)'appl', (UInt32)'PTod', &cnt, &appls, false)==true) {
						if (cnt>0) {
							SysDBListItemType *sys=MemHandleLock(appls);

							SysUIAppSwitch(sys->cardNo, sys->dbID, sysAppLaunchCmdNormalLaunch, NULL);
						}	
					}

					if (appls)
						MemHandleFree(appls);
				}
			}
			return true;
		default:
			break;
		}
		break;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mMainAbout:
			fAboutRun();
			return true;
		}
		break;

	default:
		break;
	}

	return false;
}

/**
 * Save data on form.
 */
static void
SaveData(void)
{
	prefs.flags=0;
	if (CtlGetValue(UIObjectGet(cMainChkLaunch)))
		prefs.flags|=PFlgLaunchTasks;

	prefs.hotKey=0;
	if (PMHasNotifications()) {
		if (CtlGetValue(UIObjectGet(cMainChkHotKey))) {
			Char *p=UIFieldGetText(cMainFldHotKey);

			if (p && *p) {
				StrToLower(p, p);
				prefs.hotKey=(WChar)*p;
				prefs.flags|=PFlgHotKey;
			}
		}
	}

	if (CtlGetValue(UIObjectGet(cMainAsc)))
		state.sortOrder=SSOAscending;
	else
		state.sortOrder=SSODescending;
}

/**
 * Sort the Tasks database.
 */
static void
Sort(void)
{
	UInt16 cnt, version;
	MemHandle dbList;
	SysDBListItemType *sys;
	DmOpenRef db;

	if ((SysCreateDataBaseList((UInt32)'DATA', (UInt32)'PTod', &cnt, &dbList, false)==false) || (cnt==0)) {
		FrmAlert(aNoTasksDB);
		return;
	}

	sys=MemHandleLock(dbList);
	if (DmDatabaseInfo(sys->cardNo, sys->dbID, NULL, NULL, &version, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)!=errNone) {
		MemHandleFree(dbList);
		FrmAlert(aNoDBInfo);
		return;
	}

	db=DmOpenDatabase(sys->cardNo, sys->dbID, dmModeReadWrite);
	MemHandleFree(dbList);

	if (db==NULL) {
		FrmAlert(aCannotOpenDB);
		return;
	}

	DPrint("sortOrder: %u\n", state.sortOrder);
	switch (version) {
	case 0:
		DmQuickSort(db, TasksSortClassic, (Int16)state.sortOrder);
		break;
	case 1:
		DmQuickSort(db, TasksSortNew, (Int16)state.sortOrder);
		break;
	default:
		FrmAlert(aVersionError);
		break;
	}

	DmCloseDatabase(db);
}

/**
 * TasksSortClassic
 */
static Int16
TasksSortClassic(void *r1, void *r2, Int16 order, SortRecordInfoType *u1, SortRecordInfoType *u2, MemHandle u3)
{
	return DoSort(((Char *)r1)+3, ((Char *)r2)+3, order);
}

/**
 * TasksSortNew
 */
static Int16
TasksSortNew(void *r1, void *r2, Int16 order, SortRecordInfoType *u1, SortRecordInfoType *u2, MemHandle u3)
{
	return DoSort(GetDesc((ToDoDBRecordType *)r1), GetDesc((ToDoDBRecordType *)r2), order);
}

/**
 * Perform actual sort
 */
static Int16
DoSort(Char *c1, Char *c2, Int16 order)
{
	if (order==SSOAscending)
		return StrCaselessCompare(c1, c2);

	return StrCaselessCompare(c2, c1);
}

/**
 * Parse out the description string.
 *
 * @param rec Record.
 * @return description string.
 */
static Char *
GetDesc(ToDoDBRecordType *rec)
{
	Char *str=((Char *)rec)+(3*sizeof(UInt16));

	if (!rec->dataFlags.description)
		return empty;

	if (rec->dataFlags.dueDate)
		str+=sizeof(DateType);

	if (rec->dataFlags.completionDate)
		str+=sizeof(DateType);

	if (rec->dataFlags.alarm)
		str+=sizeof(TimeType)+sizeof(UInt16);

	if (rec->dataFlags.repeat)
		str+=sizeof(DateType)+sizeof(RepeatInfoType);

	return str;
}
