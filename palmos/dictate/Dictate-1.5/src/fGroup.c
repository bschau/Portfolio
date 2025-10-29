/*
 * fGroup.c
 */
#include "Include.h"

/* protos */
static void SetupList(Boolean);
static void DrawList(Int16, RectangleType *, Char **);
static void AddSentence(void);
static Boolean AddSentenceEH(EventType *);

/* globals */
static UInt16 addSentenceTopSave=0, group;
static Char groupName[GroupNameLength+1];

/*
 * DrawList
 */
static void
DrawList(Int16 itemNum, RectangleType *r, Char **unused)
{
	MemHandle mh, rh;
	XRefRecord *xR;
	UInt16 rIdx, pW, len;
	Boolean trunc;
	Char *p;

	WinEraseRectangle(r, 0);

	if ((rIdx=XRefIndex(&dbTXref, groupUid, itemNum))==dmMaxRecordIndex)
		return;

	mh=DBGetRecord(&dbTXref, rIdx);
	xR=MemHandleLock(mh);
	if (DmFindRecordByID(dbTSentence.db, xR->suid, &rIdx)==errNone) {
		rh=DBGetRecord(&dbTSentence, rIdx);
		p=MemHandleLock(rh);

		pW=r->extent.x;
		len=StrLen(p);
		FntCharsInWidth(p, &pW, &len, &trunc);
		WinDrawChars(p, len, r->topLeft.x, r->topLeft.y);
		MemHandleUnlock(rh);
	}
	MemHandleUnlock(mh);
}

/*
 * fGroupOpen
 *
 * Open group page.
 *
 *  -> gno		Group no. #.
 */
void
fGroupOpen(UInt16 gno)
{
	group=gno;
	DmRecordInfo(dbTGroup.db, group, NULL, &groupUid, NULL);
	FrmGotoForm(fGroup);
}

/*
 * SetupList
 *
 * Setup and possibly draw list.
 *
 *  -> redraw		Redraw list (true = yes, false = no).
 */
static void
SetupList(Boolean redraw)
{
	ListType *lst=UIObjectGet(cGroupList);

	LstSetDrawFunction(lst, DrawList);
	LstSetListChoices(lst, NULL, XRefCount(&dbTXref, groupUid));

	if (redraw) {
		UInt16 pW=40, len;
		RectangleType r;
		Char buffer[32];
		Boolean trunc;

		LstDrawList(lst);

		r.extent.x=40;
		r.extent.y=FntCharHeight();
		r.topLeft.x=160-r.extent.x;
		r.topLeft.y=160-r.extent.y;
		WinEraseRectangle(&r, 0);

		StrPrintF(buffer, "#%d", LstGetNumberOfItems(lst));

		len=StrLen(buffer);
		FntCharsInWidth(buffer, &pW, &len, &trunc);
		WinDrawChars(buffer, len, 160-pW, r.topLeft.y);
	}
}

/*
 * fGroupEH
 */
Boolean
fGroupEH(EventType *ev)
{
	ListType *lst=UIObjectGet(cGroupList);
	UInt16 cnt, len;
	RectangleType r;
	Boolean trunc;
	Int16 idx;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		LstSetSelection(lst, noListSelection);
		PMGetGroupName(&dbTGroup, groupUid, groupName);
		cnt=120;

		r.extent.x=cnt;
		r.extent.y=FntCharHeight();
		r.topLeft.x=160-r.extent.x;
		r.topLeft.y=0;
		WinEraseRectangle(&r, 0);

		len=StrLen(groupName);
		FntCharsInWidth(groupName, &cnt, &len, &trunc);
		WinDrawChars(groupName, len, 160-cnt, r.topLeft.y);
	case frmUpdateEvent:
		SetupList(true);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cGroupAdd:
			AddSentence();
			SetupList(true);
			LstSetSelection(lst, noListSelection);
			cnt=LstGetNumberOfItems(lst);
			if (cnt>GroupLines)
				LstScrollList(lst, winDown, cnt);

			return true;

		case cGroupRemove:
			if ((idx=LstGetSelection(lst))!=noListSelection) {
				LstSetSelection(lst, noListSelection);
				XRefDelete(&dbTXref, groupUid, idx);
				SetupList(true);
			}
			return true;

		case cGroupUp:
			if ((idx=LstGetSelection(lst))!=noListSelection) {
				if (idx>0) {
					XRefSwap(&dbTXref, groupUid, idx, idx-1);
					SetupList(true);
					LstSetSelection(lst, idx-1);
				}
			}
			return true;

		case cGroupDown:
			if ((idx=LstGetSelection(lst))!=noListSelection) {
				if (idx<(LstGetNumberOfItems(lst)-1)) {
					XRefSwap(&dbTXref, groupUid, idx, idx+1);
					SetupList(true);
					LstSetSelection(lst, idx+1);
				}
			}
			return true;

		case cGroupDictate:
			if (XRefCount(&dbTXref, groupUid)>0)
				FrmGotoForm(fTDictate);
			else
				FrmAlert(aNoSentences);

			return true;

		case cGroupBeam:
			if (XRefCount(&dbTXref, groupUid)>0)
				Beam(groupName, &dbTXref, &dbTSentence);
			else
				FrmAlert(aNoSentences);

			return true;	

		case cGroupGroups:
			FrmGotoForm(fTeacher);
			return true;
		}
		break;

	case menuEvent:
		if (ev->data.menu.itemID==mGroupStudent) {
			if (XRefCount(&dbTXref, groupUid)>0) {
				Char buffer[16];
				UInt16 cnt;

				cnt=ExchangeDBToStudent(groupUid);
				StrPrintF(buffer, "%d", cnt);
				FrmCustomAlert(aSetAsStudentDB, buffer, (cnt==1) ? " " : "s ", NULL);
			} else
				FrmAlert(aNoSentences);

			return true;
		}

	default: // FALL-THRU
		break;
	}

	return false;
}

/*
 * AddSentence
 *
 * Show the Add Sentence form.
 */
static void
AddSentence(void)
{
	FormSave frm;

	UIFormPrologue(&frm, fAddSentence, AddSentenceEH);
	UITableInit(&dbTSentence, &tblSentence, cAddSentenceTable, cAddSentenceScrollBar, fSentenceDrawTable, tsCustom1);
	tblSentence.top=addSentenceTopSave;
	tblSentence.records=DmNumRecordsInCategory(dbTSentence.db, prefs.catSentenceIdx);
	UITableUpdateValues(&tblSentence, true);

	UIPopupSet(&dbTSentence, prefs.catSentenceIdx, catSentenceName, cAddSentencePopup);

	UIFormEpilogue(&frm, NULL, cAddSentenceCancel);
}

/*
 * AddSentenceEH
 */
static Boolean
AddSentenceEH(EventType *ev)
{
	XRefRecord xRef;
	UInt16 oldCat, rIdx;

	switch (ev->eType) {
	case tblSelectEvent:
		TblUnhighlightSelection(tblSentence.tbl);

		xRef.guid=groupUid;
		DmRecordInfo(dbTSentence.db, (UInt16)TblGetRowData(tblSentence.tbl, ev->data.tblSelect.row), NULL, &xRef.suid, NULL);

		if (XRefExists(&dbTXref, &xRef)==true) {
			FrmAlert(aXRefExists);
			return true;
		}

		XRefInsert(&dbTXref, &xRef);

		addSentenceTopSave=tblSentence.top;
		CtlHitControl(UIObjectGet(cAddSentenceCancel));
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cAddSentencePopup:
			oldCat=prefs.catSentenceIdx;

			CategorySelect(dbTSentence.db, currentForm, cAddSentencePopup, cAddSentenceList, true, &prefs.catSentenceIdx, catSentenceName, 1, 0);
			if (prefs.catSentenceIdx!=oldCat) {
				tblSentence.records=DmNumRecordsInCategory(dbTSentence.db, prefs.catSentenceIdx);
				UITableUpdateValues(&tblSentence, true);
			}
			return true;

		case cAddSentenceAll:
			if (DmNumRecordsInCategory(dbTSentence.db, prefs.catSentenceIdx)==0) {
				FrmAlert(aNoAllSentences);
				return true;
			}

			xRef.guid=groupUid;
			rIdx=0;
			while (DmQueryNextInCategory(dbTSentence.db, &rIdx, prefs.catSentenceIdx)!=NULL) {
				DmRecordInfo(dbTSentence.db, rIdx, NULL, &xRef.suid, NULL);

				if (XRefExists(&dbTXref, &xRef)==false)
					XRefInsert(&dbTXref, &xRef);

				rIdx++;
			}

			CtlHitControl(UIObjectGet(cAddSentenceCancel));
			return true;
		}
		break;

	default:
		UITableEvents(&tblSentence, ev);
		break;
	}

	return false;
}
