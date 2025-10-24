/*
 * fTeacher.c
 */
#include "Include.h"

/* protos */
static void DrawTable(void *, Int16, Int16, RectangleType *);
static void EditGroup(void);
static Boolean EditGroupEH(EventType *);

/* globals */
static Table tblMain;
static Int16 returnTop;
static UInt16 recordIdx;

/*
 * DrawTable
 *
 * Draw the main table.
 *
 *  -> tbl		Table.
 *  -> row		Current row.
 *  -> column		Current column.
 *  -> r		Rectangle.
 */
static void
DrawTable(void *tbl, Int16 row, Int16 column, RectangleType *r)
{
	UInt16 recIdx=0, pW, len;
	MemHandle mh;
	Char *p;
	Boolean trunc;
	UInt32 guid;

	if (DmSeekRecordInCategory(dbTGroup.db, &recIdx, tblMain.top+row, dmSeekForward, dmAllCategories)!=errNone)
		return;

	mh=DBGetRecord(&dbTGroup, recIdx);
	p=MemHandleLock(mh);

	TblSetRowData(tbl, row, (UInt32)recIdx);
	WinEraseRectangle(r, 0);

	DmRecordInfo(dbTGroup.db, recIdx, NULL, &guid, NULL);

	if (!column) {
		FontID fId=FntSetFont(symbolFont);

		WinDrawChar(XRefCount(&dbTXref, guid)>0 ? 12 : 11, r->topLeft.x+1, r->topLeft.y);
		FntSetFont(fId);
	} else {
		Char buffer[16];

		pW=118;
		len=StrLen(p);
		FntCharsInWidth(p, &pW, &len, &trunc);
		WinDrawChars(p, len, r->topLeft.x, r->topLeft.y);

		pW=20;
		StrPrintF(buffer, "%d", XRefCount(&dbTXref, guid));
		len=StrLen(buffer);
		FntCharsInWidth(buffer, &pW, &len, &trunc);
		WinDrawChars(buffer, len, r->topLeft.x+138-pW, r->topLeft.y);
	}

	MemHandleUnlock(mh);
}

/*
 * fTeacherInit
 */
Boolean
fTeacherInit(void)
{
	TableStyle style[]={ { 0, customTableItem }, { 1, customTableItem }, { -1 } };

	UITableInit(&dbTGroup, &tblMain, cTeacherTable, cTeacherScrollBar, DrawTable, style);
	tblMain.top=returnTop;
	UITableUpdateValues(&tblMain, false);
	return true;
}

/*
 * fTeacherEH
 */
Boolean
fTeacherEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==cTeacherNew) {
			recordIdx=dmMaxRecordIndex;
			EditGroup();
			return true;
		} else if (ev->data.ctlSelect.controlID==cTeacherSentences) {
			FrmGotoForm(fSentence);
			return true;
		}
		break;

	case tblSelectEvent:
		returnTop=tblMain.top;
		recordIdx=(UInt16)TblGetRowData(tblMain.tbl, ev->data.tblSelect.row);
		if (!ev->data.tblSelect.column) {
			DmRecordInfo(dbTGroup.db, recordIdx, NULL, &groupUid, NULL);
			EditGroup(); 
		} else
			fGroupOpen(recordIdx);

		return true;

	case menuEvent:
		if (ev->data.menu.itemID==mTeacherStudent) {
			prefs.function=PFuncStudent;
			PMRun();
			return true;
		} else if (ev->data.menu.itemID==mTeacherAbout) {
			fAboutRun();
			return true;
		}
		break;
	default:
		UITableEvents(&tblMain, ev);
		break;
	}

	return false;
}

/*
 * EditGroup
 *
 * Edit/Add a group.
 *
 * Note. recordIdx must be setup beforehand.
 */
static void
EditGroup(void)
{
	FormSave frm;

	UIFormPrologue(&frm, fEditGroup, EditGroupEH);
	if (recordIdx!=dmMaxRecordIndex) {
		MemHandle mh;

		mh=DBGetRecord(&dbTGroup, recordIdx);
		UIFieldSetText(cEditGroupField, MemHandleLock(mh));
		MemHandleUnlock(mh);
		UIObjectShow(cEditGroupDelete);
		UIObjectShow(cEditGroupBeam);
	}

	UIFieldFocus(cEditGroupField);
	if (UIFormEpilogue(&frm, NULL, cEditGroupCancel)==true) {
		tblMain.records=DmNumRecordsInCategory(dbTGroup.db, dmAllCategories);
		UITableUpdateValues(&tblMain, true);
	}
}

/*
 * EditGroupEH
 */
static Boolean
EditGroupEH(EventType *ev)
{
	if (ev->eType==ctlSelectEvent) {
		Char buffer[GroupNameLength+1];
		Char *p;
		UInt16 len;

		switch (ev->data.ctlSelect.controlID) {
		case cEditGroupOK:
			p=UIFieldGetText(cEditGroupField);
			len=StrLen(p);
			MemMove(buffer, p, len);
			buffer[len]='\x00';
			if ((p=StringPurify(buffer))) {
				DBSetRecord(&dbTGroup, recordIdx, dmUnfiledCategory, p, StrLen(p)+1, SFString);
				MemPtrFree(p);
			}
			break;
		case cEditGroupDelete:
			if (FrmAlert(aGroupDelete)==0) {
				XRefDeleteAllGuid(&dbTXref, groupUid);
				DmDeleteRecord(dbTGroup.db, recordIdx);
			} else
				return true;
			break;
		case cEditGroupBeam:
			if (XRefCount(&dbTXref, groupUid)<1) {
				FrmAlert(aNoSentences);
				return true;
			} else {
				MemHandle mh=DBGetRecord(&dbTGroup, recordIdx);
				Beam(MemHandleLock(mh), &dbTXref, &dbTSentence);
				MemHandleUnlock(mh);
			}
		default:	/* FALL-THRU */
			break;
		}
	}

	return false;
}
