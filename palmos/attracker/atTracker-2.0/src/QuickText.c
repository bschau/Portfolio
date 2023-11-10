/*
 *	QuickText.c
 *
 *	Code to drive the quicktext "popup"/"list".
 *
 *	Work as Category.  Drop down list + "Manage"@bottom
 *	"Manage" will popup a window where one can add, edit and delete QT's.
 */
#include "Include.h"

/* macros */
#define QTEntries 16

/* structs */
typedef struct {
	Char e[QTEntries][QTEntryLen];
} QuickTextPrefs;

/* protos */
static UInt16 TextCount(void);
static void ListInit(UInt16, UInt16, Boolean);
static Boolean EH(EventType *);
static void EditQuickText(Int16);
static void Edit(Int16);
static Boolean EditEH(EventType *);
static void EditHook(void);
static Int16 SortFunc(void *, void *, Int32);

/* globals */
static Char *editText="Edit ...";
static Int16 quickTextIdx=noListSelection, quickTextEdit;
static MemHandle quickTextH;
static QuickTextPrefs *quickText;
static Char *qtArr[QTEntries+2];
static UInt16 quickTextCnt;

/*
 * QuickTextGet
 *
 * Get quick text from preferences.
 */
void
QuickTextGet(void)
{
	UInt16 len=sizeof(QuickTextPrefs);
	Int16 retPrf;

	quickTextH=MemHandleNew(sizeof(QuickTextPrefs));
	ErrFatalDisplayIf(quickTextH==NULL, "(QuickTextGet) Out of memory.");

	quickText=MemHandleLock(quickTextH);
	retPrf=PrefGetAppPreferences((UInt32)CRID, PrfQuickText, quickText, &len, true);
	if (retPrf==noPreferenceFound || len!=sizeof(QuickTextPrefs)) {
		MemSet(quickText, sizeof(QuickTextPrefs), 0);
		StrCopy(quickText->e[0], "Kngl Mgmt");
		StrCopy(quickText->e[1], "Misc");
		StrCopy(quickText->e[2], "Presales");
		StrCopy(quickText->e[3], "Vacation");
	}
}

/*
 * QuickTextPut
 *
 * Save quick text to preferences.
 */
void
QuickTextPut(void)
{
	PrefSetAppPreferences((UInt32)CRID, PrfQuickText, APPVER, quickText, sizeof(QuickTextPrefs), true);
	MemHandleFree(quickTextH);
}

/*
 * TextCount
 *
 * Return no. of quick texts.
 */
static UInt16
TextCount(void)
{
	UInt16 i;

	for (i=0; i<QTEntries; i++) {
		if (!quickText->e[i][0])
			break;
	}

	return i;
}

/*
 * ListInit
 *
 * Real list init.
 *
 * -> obj		Object ID.
 * -> length		Max lines in list.
 * -> eFlag		Add 'Edit ...'.
 */
static void
ListInit(UInt16 obj, UInt16 length, Boolean eFlag)
{
	ListType *lst=UIObjectGet(obj);
	UInt16 i;

	quickTextCnt=TextCount();

	for (i=0; i<quickTextCnt; i++)
		qtArr[i]=&quickText->e[i][0];

	if (eFlag==true) {
		qtArr[i++]=editText;
		quickTextCnt++;
	}
	qtArr[i]=NULL;

	LstSetListChoices(lst, &qtArr[0], quickTextCnt);
	if (eFlag==true) {
		if (quickTextCnt<length)
			LstSetHeight(lst, quickTextCnt);
		else
			LstSetHeight(lst, length);
	} else
		LstSetHeight(lst, length);
}

/*
 * QuickTextListInit
 *
 * Initialize quick text list.
 *
 *  -> id		Object id.
 */
void
QuickTextListInit(UInt16 id)
{
	ListInit(id, QuickTextListLines, true);
}

/*
 * QuickTextGetCount
 *
 * Return count of quicktexts.
 */
UInt16
QuickTextGetCount(void)
{
	return quickTextCnt;
}

/*
 * QuickTextManager
 *
 * Handle manager dialog.
 */
void
QuickTextManager(void)
{
	ListType *lst;
	FormSave frm;

	UIFormPrologue(&frm, fQuickTextManager, EH);
	ListInit(cQuickTextManagerList, QuickTextManagerLines, false);

	lst=UIObjectGet(cQuickTextManagerList);
	LstSetSelection(lst, quickTextIdx);
	LstDrawList(lst);

	UIFormEpilogue(&frm, NULL, 0);
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	ListType *lst=UIObjectGet(cQuickTextManagerList);
	Int16 idx;

	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cQuickTextManagerOK:
			quickTextIdx=LstGetSelection(lst);
			break;
		case cQuickTextManagerNew:
			if (QuickTextGetCount()==QTEntries)
				FrmAlert(aQuickTextFull);
			else
				EditQuickText(-1);

			return true;
		case cQuickTextManagerEdit:
			if ((idx=LstGetSelection(lst))==noListSelection)
				FrmAlert(aQuickTextManagerNoneSelected);
			else 
				EditQuickText(idx);

			return true;
		default:
			break;
		}
	default:	/* FALL-THRU */
		break;
	}
	return false;
}

/*
 * EditQuickText
 *
 * Wrapper for Edit.
 */
static void
EditQuickText(Int16 idx)
{
	Edit(idx);
	ListInit(cQuickTextManagerList, QuickTextManagerLines, false);
}

/*
 * Edit
 *
 * Handle Edit Quick Text form.
 *
 *   -> idx		Quick Text index (or -1)
 */
static void
Edit(Int16 idx)
{
	FormSave frm;

	quickTextEdit=idx;
	UIFormPrologue(&frm, fQuickTextManagerEdit, EditEH);
	if (idx!=-1) {
		UIFieldSetText(cQuickTextManagerEditName, quickText->e[idx]);
		UIObjectShow(cQuickTextManagerEditDelete);
	}
	UIFieldFocus(cQuickTextManagerEditName);
	UIFormEpilogue(&frm, EditHook, cQuickTextManagerEditCancel);
}

/*
 * EditEH
 */
static Boolean
EditEH(EventType *ev)
{
	UInt16 i;
	Char *s;

	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cQuickTextManagerEditOK:
			if ((s=UIFieldGetText(cQuickTextManagerEditName))==NULL)
				return true;
	
			if (StrLen(s)==0)
				return true;

			for (i=0; i<QTEntries; i++) {
				if (StrCompare(quickText->e[i], s)==0)
					return false;
			}
			
			if (quickTextEdit==-1) {
				StrCopy(&quickText->e[QuickTextGetCount()][0], s);
				quickTextCnt++;
			} else 
				StrCopy(&quickText->e[quickTextEdit][0], s);

			if (quickTextCnt>1)
				SysQSort(quickText, quickTextCnt, QTEntryLen, SortFunc, 0);
			return false;

		case cQuickTextManagerEditDelete:
			if (FrmAlert(aQuickTextDelete)==0) {
				quickText->e[quickTextEdit][0]='\xff';
				if (quickTextCnt>1)
					SysQSort(quickText, quickTextCnt, QTEntryLen, SortFunc, 0);

				for (i=0; i<QTEntries; i++) {
					if (quickText->e[i][0]=='\xff')
						quickText->e[i][0]='\x00';
				}
			}
			return false;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * EditHook
 */
static void
EditHook(void)
{
	ListInit(cQuickTextManagerList, QuickTextManagerLines, false);
	LstSetSelection(UIObjectGet(cQuickTextManagerList), noListSelection);
}

/*
 * SortFunc
 */	
static Int16
SortFunc(void *p1, void *p2, Int32 unused)
{
	return StrCompare((Char *)p1, (Char *)p2);
}

/*
 * QuickTextIdxToText
 *
 * Return text pointer given index.
 *
 *  -> idx		Index.
 *
 * Returns text pointer or NULL (if index is invalid).
 */
Char *
QuickTextIdxToText(UInt16 idx)
{
	if (!quickText->e[idx][0])
		return NULL;

	return &quickText->e[idx][0];
}
