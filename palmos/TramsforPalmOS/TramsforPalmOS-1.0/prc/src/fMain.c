/*
 * fMain.c
 */
#include "Include.h"

/* macros */
#define NoteWidth 12

/* protos */
static void DrawList(Int16, RectangleType *, Char **);
static void NewTER(void);
static Boolean NewTEREH(EventType *);
static Boolean Edit(Int16);
static Boolean EditEH(EventType *);

/* globals */
static Boolean editReturn;
static UInt16 newlyCreated;
static Char curTerName[dmDBNameLength];

/*
 * DrawList
 */
static void
DrawList(Int16 itemNum, RectangleType *rct, Char **unused)
{
	if ((terCnt>0) && (terList!=NULL) && (itemNum<terCnt)) {
		UInt32 recs=0;
		FontID fID=stdFont;
		UInt16 len, pw;
		Boolean fitAll;
		Char *p;

		DmDatabaseSize(terList[itemNum].cardNo, terList[itemNum].dbID, &recs, NULL, NULL);

		WinEraseRectangle(rct, 0);
		if (!recs)
			fID=FntSetFont(boldFont);

		if (terList[itemNum].name[5]==' ') 
			WinDrawChars("\x95", 1, rct->topLeft.x, rct->topLeft.y);

		p=terList[itemNum].name+6;

		len=StrLen(p);
		pw=rct->extent.x-(rct->topLeft.x+6)-2;
		FntCharsInWidth(p, &pw, &len, &fitAll);

		WinDrawChars(p, len, rct->topLeft.x+6, rct->topLeft.y);

		if (!recs)
			FntSetFont(fID);
	}
}

/*
 * fMainInit
 */
Boolean
fMainInit(void)
{
	TLBuildList();

	UIListInit(cMainList, DrawList, terCnt, noListSelection);
	return true;
}

/*
 * fMainEH
 */
Boolean
fMainEH(EventType *ev)
{
	Int16 ter;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cMainNew:
			NewTER();
			return true;
		case cMainEdit:
			if ((ter=LstGetSelection(UIObjectGet(cMainList)))==noListSelection)
				FrmAlert(aNoTERSelected);
			else {
				if (Edit(ter)==true) {
					UIListInit(cMainList, DrawList, terCnt, noListSelection);
					LstDrawList(UIObjectGet(cMainList));
				}
			}
			return true;
		case cMainExpenses:
			if ((ter=LstGetSelection(UIObjectGet(cMainList)))==noListSelection)
				FrmAlert(aNoTERSelected);
			else
				fExpensesRun(ter);

			return true;
		}
		break;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mMainAbout:
			fAboutRun();
			return true;
		case mMainPreferences:
			FrmGotoForm(fPreferences);
			return true;
		}
		break;

	case keyDownEvent:
		return UIListKeyHandler(ev, cMainList);

	default:
		break;
	}

	return false;
}

/*
 * NewTER
 *
 * Create a new TER.
 */
static void
NewTER(void)
{
	FormSave frm;

	UIFormPrologue(&frm, fNewTER, NewTEREH);
	UIObjectFocus(cNewTERName);

	if (UIFormEpilogue(&frm, NULL, cNewTERCancel)==true) {
		ListType *lst=UIObjectGet(cMainList);

		UIListInit(cMainList, DrawList, terCnt, LstGetSelection(lst));

		if (newlyCreated!=dmMaxRecordIndex) {
			LstMakeItemVisible(lst, newlyCreated);
			LstDrawList(lst);
		}
	}
}

/*
 * NewTEREH
 */
static Boolean
NewTEREH(EventType *ev)
{
	Char *p;

	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cNewTEROK:
			if ((p=UIFieldGetText(cNewTERName))!=NULL) {
				Char dbName[dmDBNameLength];

				StrPrintF(dbName, "Trms_ %s", p);

				if (TLHasTer(dbName)==true) {
					FrmAlert(aRenameTERPresent);
					return true;
				}

				DBCreateDatabase(dbName, 0);
				TLBuildList();

				newlyCreated=TLGetIndex(dbName);
			}
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
 * Edit the TER properties.
 */
static Boolean
Edit(Int16 terIdx)
{
	FormSave frm;

	editReturn=false;
	newlyCreated=terIdx;
	StrCopy(curTerName, terList[terIdx].name);
	UIFormPrologue(&frm, fEdit, EditEH);

	UIFieldSetText(cEditName, terList[terIdx].name+6);
	UIObjectFocus(cEditName);

	CtlSetValue(UIObjectGet(cEditLocked), (terList[terIdx].name[5]==' ') ? 0 : 1);

	UIFormEpilogue(&frm, NULL, 0);
	return editReturn;
}

/*
 * EditEH
 */
static Boolean
EditEH(EventType *ev)
{
	Char *p;

	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cEditOK:
			if (((p=UIFieldGetText(cEditName))!=NULL) && (StrLen(p)>0)) {
				Char dbName[dmDBNameLength];

				StrPrintF(dbName, "Trms_%s%s", ((CtlGetValue(UIObjectGet(cEditLocked))) ? "L" : " "), p);

				if (StrCaselessCompare(dbName+6, curTerName+6)!=0) {
					if (TLHasTer(dbName)==true) {
						FrmAlert(aRenameTERPresent);
						return true;
					}
				}

				DmSetDatabaseInfo(terList[newlyCreated].cardNo, terList[newlyCreated].dbID, dbName, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

				StrCopy(terList[newlyCreated].name, dbName);
				TLSort();
				editReturn=true;
			}
			break;

		case cEditDelete:
			if (FrmAlert(aDeleteTER)>0)
				return true;

			ErrFatalDisplayIf(DmDeleteDatabase(terList[newlyCreated].cardNo, terList[newlyCreated].dbID)!=errNone, "(EditEH) Cannot delete TER database.");

			TLBuildList();
			editReturn=true;
			break;
		}
	default:
		break;
	}

	return false;
}
