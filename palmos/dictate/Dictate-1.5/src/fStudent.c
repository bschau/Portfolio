/*
 * fStudent.c
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

	if (DmSeekRecordInCategory(dbSGroup.db, &recIdx, tblMain.top+row, dmSeekForward, dmAllCategories)!=errNone)
		return;

	mh=DBGetRecord(&dbSGroup, recIdx);
	p=MemHandleLock(mh);

	TblSetRowData(tbl, row, (UInt32)recIdx);
	WinEraseRectangle(r, 0);

	if (!column) {
		FontID fId=FntSetFont(symbolFont);
		UInt32 guid;

		DmRecordInfo(dbSGroup.db, recIdx, NULL, &guid, NULL);
		WinDrawChar(XRefCount(&dbSXref, guid)>0 ? 12 : 11, r->topLeft.x+1, r->topLeft.y);
		FntSetFont(fId);
	} else {
		pW=138;
		len=StrLen(p);
		FntCharsInWidth(p, &pW, &len, &trunc);
		WinDrawChars(p, len, r->topLeft.x, r->topLeft.y);
	}
	
	MemHandleUnlock(mh);
}

/*
 * fStudentInit
 */
Boolean
fStudentInit(void)
{
	TableStyle style[]={ { 0, customTableItem }, { 1, customTableItem }, { -1 } };

	UITableInit(&dbSGroup, &tblMain, cStudentTable, cStudentScrollBar, DrawTable, style);
	tblMain.top=returnTop;
	UITableUpdateValues(&tblMain, false);
	return true;
}

/*
 * fStudentEH
 */
Boolean
fStudentEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		IRImport();
		return true;

	case tblSelectEvent:
		returnTop=tblMain.top;
		recordIdx=(UInt16)TblGetRowData(tblMain.tbl, ev->data.tblSelect.row);
		TblUnhighlightSelection(tblMain.tbl);
		DmRecordInfo(dbSGroup.db, recordIdx, NULL, &groupUid, NULL);
		if (ev->data.tblSelect.column) {
			if (XRefCount(&dbSXref, groupUid)>0) {
				if (game.round) {
					if (FrmAlert(aNewDictation))
						return true;
				}

				MemSet(&game, sizeof(Game), 0);
				game.guid=groupUid;
				game.saved=false;
				FrmGotoForm(fSDictate);
			} else
				FrmAlert(aNoSentencesGroup);
		} else
			EditGroup(); 

		return true;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mStudentLastScore:
			if (PMGetPref(&game, sizeof(Game), PrfGameInfo)==true)
				FrmGotoForm(fDictationDone);
			else
				FrmAlert(aNoLastScore);

			return true;

		case mStudentTeacher:
			prefs.function=PFuncTeacher;
			PMRun();
			return true;
		case mStudentAbout:
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
 * Edit a group.
 *
 * Note. recordIdx must be setup beforehand.
 */
static void
EditGroup(void)
{
	FormSave frm;
	MemHandle mh;

	UIFormPrologue(&frm, fEditGroup, EditGroupEH);

	mh=DBGetRecord(&dbSGroup, recordIdx);
	UIFieldSetText(cEditGroupField, MemHandleLock(mh));
	MemHandleUnlock(mh);
	UIObjectShow(cEditGroupDelete);
	UIObjectShow(cEditGroupBeam);

	UIFieldFocus(cEditGroupField);
	if (UIFormEpilogue(&frm, NULL, cEditGroupCancel)==true) {
		tblMain.records=DmNumRecordsInCategory(dbSGroup.db, dmAllCategories);
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
				DBSetRecord(&dbSGroup, recordIdx, dmUnfiledCategory, p, StrLen(p)+1, SFString);
				MemPtrFree(p);
			}
			break;
		case cEditGroupDelete:	
			if (FrmAlert(aGroupDelete)==0) {
				ExchangeDelete(groupUid, &dbSXref, &dbSSentence);
				DmDeleteRecord(dbSGroup.db, recordIdx);
			} else
				return true;
			break;
		case cEditGroupBeam:
			if (XRefCount(&dbSXref, groupUid)<1) {
				FrmAlert(aNoSentences);
				return true;
			} else {
				MemHandle mh=DBGetRecord(&dbSGroup, recordIdx);
				Beam(MemHandleLock(mh), &dbSXref, &dbSSentence);
				MemHandleUnlock(mh);
			}
		default:	/* FALL-THRU */
			break;
		}
	}

	return false;
}

/*
 * fStudentIR
 *
 * Update display in case or IR receive.
 */
void
fStudentIR(void)
{
	if (FrmGetActiveFormID()==fStudent) {
		tblMain.records=DmNumRecordsInCategory(dbSGroup.db, dmAllCategories);
		tblMain.top=0;
		returnTop=0;
		UITableUpdateValues(&tblMain, true);
		FrmUpdateForm(fStudent, frmRedrawUpdateCode);
	}
}
