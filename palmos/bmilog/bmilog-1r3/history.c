/*
**	history.c
**
**	Code to drive the History form.
*/
#include "bmilog.h"

/* macros */
#define MINPAGESIZE 7
#define CDATEW dateStringLength
#define CHEIGHTW 6
#define CWEIGHTW 4
#define CBMIW 4
#define CSPACING 2
#define HEADERLENGTH (CDATEW+CHEIGHTW+CWEIGHTW+CBMIW+(3*CSPACING))

/* protos */
static void DrawTable(void *, Int16, Int16, RectangleType *);
static void SetDateTrigger(void);
static void HistoryEdit(void);
static Boolean HistoryEditEventHandler(EventType *);
static Boolean KgCmData(UInt16 *, UInt16 *);
static Boolean LbsInchData(UInt16 *, UInt16 *);
static Boolean HistoryAdd(UInt16, UInt16, UInt16);
static void HistoryPrint(void);

/* globals */
static UInt16 focusBandKgCm[]={ fHistoryEditWeight, fHistoryEditHeight1, fHistoryEditWeight, 0 };
static UInt16 focusBandLbsInch[]={ fHistoryEditWeight, fHistoryEditHeight1, fHistoryEditHeight2, fHistoryEditWeight, 0 };
static Table tblHistory;
static UInt16 tableFormat;
static Char dateLabel[dateStringLength];
static Int16 year, month, day;
static Person currentPerson;

/*
**	DrawTable
**
**	Draw main table.
*/
static void
DrawTable(void *tbl, Int16 row, Int16 column, RectangleType *r)
{
	UInt16 item=0;
	MemHandle mh;
	BMI *b;
	Char dBuf[dateStringLength];
	Int16 y, m, d;

	if (BmiFirst(&dbData, personId, &item)==false)
		return;

	if (DmSeekRecordInCategory(dbData.db, &item, tblHistory.top+row, dmSeekForward, dmAllCategories)!=errNone)
		return;

	if ((mh=DmQueryRecord(dbData.db, item))==NULL)
		return;

	TblSetRowData(tblHistory.tbl, row, (UInt32)item);

	b=MemHandleLock(mh);
	switch (column) {
	case 0:		// date
		WinEraseRectangle(r, 0);
		UtilsUnpackDate(b->date, &y, &m, &d);
		DateToAscii(m, d, y, PrefGetPreference(prefDateFormat), dBuf);
		UITextDrawR(dBuf, r);
		break;
	case 1:		// height
		if (tableFormat==FormatKgCm)
			StrPrintF(dBuf, "%u", b->height);
		else
			StrPrintF(dBuf, "%u\"%u'", b->height/12, b->height%12);

		UITextDrawR(dBuf, r);
		break;
	case 2:		// weight
		StrPrintF(dBuf, "%u", b->weight);
		UITextDrawR(dBuf, r);
		break;
	case 3:		// BMI
		StrPrintF(dBuf, "%u", BmiCalculate(tableFormat, b->height, b->weight));
		UITextDrawR(dBuf, r);
		break;
	case 4:		// Smiley
		BmiDrawSmiley(BmiCalculate(tableFormat, b->height, b->weight), r);
		break;
	}
	MemHandleUnlock(mh);
}

/*
**	SetDateTrigger
**
**	Set date in buffer and update trigger.
*/
static void
SetDateTrigger(void)
{
	DateToAscii(month, day, year, PrefGetPreference(prefDateFormat), dateLabel);
	CtlSetLabel(UIFormGetObject(fHistoryEditDate), dateLabel);
}

/*
**	HistoryInit
**
**	Initialize the main form.
**
**	Returns true if successful, false otherwise.
*/
Boolean
HistoryInit(void)
{
	TableStyle styles[]={ { 0, customTableItem, true }, { 1, customTableItem, true }, { 2, customTableItem, true }, { 3, customTableItem, true }, { 4, customTableItem, true }, { -1 } };
	MemHandle mh;

	mh=DmQueryRecord(dbData.db, recordIdx);
	ErrFatalDisplayIf(mh==NULL, "(HistoryInit) Cannot query record.");

	UITableInit(NULL, &tblHistory, fHistoryTable, fHistoryScrollBar, DrawTable, styles);

	MemMove(&currentPerson, MemHandleLock(mh), sizeof(Person));
	MemHandleUnlock(mh);

	personId=currentPerson.r.id;
	tableFormat=currentPerson.format;
	UIFieldSetText(fHistoryName, currentPerson.name);
	tblHistory.records=BmiCount(&dbData, personId);
	UITableUpdateValues(&tblHistory);

	if (PalmPrintOpen()==true)
		UIShowObject(fHistoryPrint);

	return true;
}

/*
**	HistoryEventHandler
**
**	Event handler for History form.
**
**	 -> ev		Event.
**
**	Returns true if event is handled, false otherwise.
*/
Boolean
HistoryEventHandler(EventPtr ev)
{
	EventType stopEvent;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case fHistoryDone:
			if (findReturn==true) {
				MemSet(&stopEvent, sizeof(EventType), 0);
				stopEvent.eType=appStopEvent;
				EvtAddEventToQueue(&stopEvent);
			} else
				FrmGotoForm(fMain);
			return true;
		case fHistoryNew:
			HistoryEdit();
			UITableChanged(&tblHistory, BmiCount(&dbData, personId));
			return true;
		case fHistoryPrint:
			HistoryPrint();
			return true;
		}
		break;

	case tblSelectEvent:
		DBDeleteRecord(&dbData, (UInt16)TblGetRowData(tblHistory.tbl, ev->data.tblSelect.row), aDeleteBMIEntry, NULL);
		UITableChanged(&tblHistory, BmiCount(&dbData, personId));
		return true;

	default:
		UITableEvents(&tblHistory, ev);
		break;
	}

	return false;
}

/*
**	HistoryEdit
**
**	Show the Add New BMI entry form.
*/
static void
HistoryEdit(void)
{
	UInt16 item=0;
	Char buffer[32];
	DateTimeType dt;
	FormSave frm;
	MemHandle mh;
	BMI *b;

	TimSecondsToDateTime(TimGetSeconds(), &dt);
	year=dt.year;
	month=dt.month;	
	day=dt.day;

	UIFormPrologue(&frm, (tableFormat==FormatKgCm) ? fHistoryEditKgCm : fHistoryEditLbsInch, HistoryEditEventHandler);

	SetDateTrigger();
	if (BmiFirst(&dbData, personId, &item)==true) {
		if ((mh=DmQueryRecord(dbData.db, item))!=NULL) {
			b=MemHandleLock(mh);
			if (tableFormat==FormatKgCm) {
				StrPrintF(buffer, "%u", b->height);
				UIFieldSetText(fHistoryEditHeight1, buffer);
			} else {
				StrPrintF(buffer, "%u", b->height/12);
				UIFieldSetText(fHistoryEditHeight1, buffer);
				StrPrintF(buffer, "%u", b->height%12);
				UIFieldSetText(fHistoryEditHeight2, buffer);
			}

			StrPrintF(buffer, "%u", b->weight);
			UIFieldSetText(fHistoryEditWeight, buffer);
			MemHandleUnlock(mh);
		}
	}

	UIFieldFocus(fHistoryEditHeight1);
	UIFormEpilogue(&frm, NULL, fHistoryEditCancel);
}

/*
**	HistoryEditEventHandler
*/
static Boolean
HistoryEditEventHandler(EventType *ev)
{
	Char *title;
	Boolean valid;
	UInt16 height, weight;

	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case fHistoryEditDate:
			title=MemHandleLock(resStrSelectDate);
			if (SelectDay(selectDayByDay, &month, &day, &year, title)==true)
				SetDateTrigger();

			MemHandleUnlock(resStrSelectDate);
			return true;

		case fHistoryEditOK:
			if (tableFormat==FormatKgCm)
				valid=KgCmData(&height, &weight);
			else
				valid=LbsInchData(&height, &weight);

			if (valid==false)
				return true;

			return (HistoryAdd(UtilsPackDate(year, month, day), height, weight)==true) ? false : true;
		}
		break;

	case keyDownEvent:
		return UIRefocus(tableFormat==FormatKgCm ? focusBandKgCm : focusBandLbsInch, ev->data.keyDown.chr);

	default:
		break;
	}

	return false;
}

/*
**	KgCmData
**
**	Fetch and validate kg/cm.
**
**	<-  h		Where to store height.
**	<-  w		Where to store weight.
**
**	Returns true if data is trustworthy, false otherwise.
*/
static Boolean
KgCmData(UInt16 *h, UInt16 *w)
{
	Char *text;

	if ((text=UIFieldGetText(fHistoryEditHeight1))==NULL) {
		FrmAlert(aKgCmHeightInvalid);
		return false;
	}

	if ((*h=(UInt16)StrAToI(text))<1) {
		FrmAlert(aKgCmHeightInvalid);
		return false;
	}

	if ((text=UIFieldGetText(fHistoryEditWeight))==NULL) {
		FrmAlert(aWeightInvalid);
		return false;
	}

	if ((*w=(UInt16)StrAToI(text))<1) {
		FrmAlert(aWeightInvalid);
		return false;
	}

	return true;
}

/*
**	LbsInchData
**
**	Fetch and validate lbs/inch.
**
**	<-  h		Where to store height.
**	<-  w		Where to store weight.
**
**	Returns true if data is trustworthy, false otherwise.
*/
static Boolean
LbsInchData(UInt16 *h, UInt16 *w)
{
	Char *text;
	UInt16 feet, inch;

	if ((text=UIFieldGetText(fHistoryEditHeight1))==NULL) {
		FrmAlert(aFeetHeightInvalid);
		return false;
	}

	if ((feet=(UInt16)StrAToI(text))<1) {
		FrmAlert(aFeetHeightInvalid);
		return false;
	}

	if ((text=UIFieldGetText(fHistoryEditHeight2))==NULL) {
		FrmAlert(aInchHeightInvalid);
		return false;
	}

	if ((inch=(UInt16)StrAToI(text))>11) {
		FrmAlert(aInchHeightInvalid);
		return false;
	}

	*h=((feet*12)+inch);

	if ((text=UIFieldGetText(fHistoryEditWeight))==NULL) {
		FrmAlert(aWeightInvalid);
		return false;
	}

	if ((*w=(UInt16)StrAToI(text))<1) {
		FrmAlert(aWeightInvalid);
		return false;
	}

	return true;
}

/*
**	HistoryAdd
**
**	Adds new BMI to db.
**
**	 -> date	BMI date.
**	 -> height	BMI height.
**	 -> weight	BMI weight.
**
**	Returns true if BMI was added, false otherwise.
*/
static Boolean
HistoryAdd(UInt16 date, UInt16 height, UInt16 weight)
{
	UInt16 recIdx;
	MemHandle mh;
	BMI *b;

	if ((recIdx=BmiFindDate(personId, date))!=dmMaxRecordIndex) {
		if (FrmAlert(aBMIExists)==1) 
			return false;
	}

	mh=MemHandleNew(sizeof(BMI));
	ErrFatalDisplayIf(mh==NULL, "(HistoryAdd) Out of memory.");

	b=MemHandleLock(mh);
	MemSet(b, sizeof(BMI), 0);
	b->r.type=RBmi;
	b->r.id=personId;
	b->date=date;
	b->height=height;
	b->weight=weight;

	DBSetRecord(&dbData, mh, recIdx);
	return true;
}

/*
**	HistoryPrint
**
**	Send history to Palm Print.
*/
static void
HistoryPrint(void)
{
	UInt16 saveItem=0, item, bmi, offSet;
	UInt32 l, recCnt, pageSize;
	MemHandle pmh, mh;
	Char *page, *p, *rsn;
	Char dBuf[dateStringLength];
	Int16 y, m, d;
	BMI *b;
	Char *feet, *inch;

	if (BmiFirst(&dbData, personId, &saveItem)==false) {
		FrmAlert(aNothingToPrint);
		return;
	}

	recCnt=BmiCount(&dbData, personId);

	PalmPrintLineModeStart();
	rsn=MemHandleLock(resStrName);
	pageSize=2+StrLen(rsn)+2+StrLen(currentPerson.name);
	if (pageSize<HEADERLENGTH)
		pageSize=HEADERLENGTH;

	pageSize+=2;
	pmh=MemHandleNew(pageSize+2);
	ErrFatalDisplayIf(pmh==NULL, "(HistoryPrint) Out of memory.");
	page=MemHandleLock(pmh);

	// build header
	p=page;
	rsn=MemHandleLock(resStrName);
	MemMove(p, rsn, StrLen(rsn));
	p+=StrLen(rsn);
	MemHandleUnlock(resStrName);
	*p++=':';
	*p++=' ';
	MemMove(p, currentPerson.name, StrLen(currentPerson.name));
	p+=StrLen(currentPerson.name);
	*p++='\n';
	*p='\x00';
	PalmPrintLineModePrint(page);

	page[0]='\n';
	page[1]='\x00';
	PalmPrintLineModePrint(page);

	// table header
	p=page;
	MemSet(p, pageSize, ' ');
	rsn=MemHandleLock(resStrDate);
	offSet=CDATEW-StrLen(rsn);
	MemMove(p+offSet, rsn, StrLen(rsn));
	MemHandleUnlock(resStrDate);
	p+=CDATEW+CSPACING;
	if (currentPerson.format==FormatKgCm) {
		rsn=MemHandleLock(resStrCm);
		offSet=CHEIGHTW-StrLen(rsn);
		MemMove(p+offSet, rsn, StrLen(rsn));
		MemHandleUnlock(resStrCm);
		p+=CHEIGHTW+CSPACING;

		rsn=MemHandleLock(resStrKg);
		offSet=CWEIGHTW-StrLen(rsn);
		MemMove(p+offSet, rsn, StrLen(rsn));
		MemHandleUnlock(resStrKg);
	} else {
		rsn=MemHandleLock(resStrFeetInch);
		offSet=CHEIGHTW-StrLen(rsn);
		MemMove(p+offSet, rsn, StrLen(rsn));
		MemHandleUnlock(resStrFeetInch);
		p+=CHEIGHTW+CSPACING;

		rsn=MemHandleLock(resStrLbs);
		offSet=CWEIGHTW-StrLen(rsn);
		MemMove(p+offSet, rsn, StrLen(rsn));
		MemHandleUnlock(resStrLbs);
	}
	p+=CWEIGHTW+CSPACING;
	rsn=MemHandleLock(resStrBMI);
	offSet=CBMIW-StrLen(rsn);
	MemMove(p+offSet, rsn, StrLen(rsn));
	MemHandleUnlock(resStrBMI);
	p+=CBMIW;
	*p++='\n';
	*p='\x00';
	PalmPrintLineModePrint(page);

	feet=MemHandleLock(resStrFeet);
	inch=MemHandleLock(resStrInch);
	for (l=0; l<recCnt; l++) {
		item=saveItem;
		if (DmSeekRecordInCategory(dbData.db, &item, l, dmSeekForward, dmAllCategories)!=errNone)
			break;

		if ((mh=DmQueryRecord(dbData.db, item))==NULL)
			break;

		p=page;
		MemSet(p, pageSize, ' ');
		b=MemHandleLock(mh);

		UtilsUnpackDate(b->date, &y, &m, &d);
		DateToAscii(m, d, y, PrefGetPreference(prefDateFormat), dBuf);
		offSet=CDATEW-StrLen(dBuf);
		MemMove(p+offSet, dBuf, StrLen(dBuf));
		p+=CDATEW+CSPACING;

		if (currentPerson.format==FormatKgCm)
			StrPrintF(dBuf, "%u", b->height);
		else
			StrPrintF(dBuf, "%u%s%u%s", b->height/12, feet, b->height%12, inch);
			
		offSet=CHEIGHTW-StrLen(dBuf);
		MemMove(p+offSet, dBuf, StrLen(dBuf));
		p+=CHEIGHTW+CSPACING;

		StrPrintF(dBuf, "%u", b->weight);
		offSet=CWEIGHTW-StrLen(dBuf);
		MemMove(p+offSet, dBuf, StrLen(dBuf));
		p+=CWEIGHTW+CSPACING;

		bmi=BmiCalculate(currentPerson.format, b->height, b->weight);
		StrPrintF(dBuf, "%u", bmi);
		offSet=CBMIW-StrLen(dBuf);
		MemMove(p+offSet, dBuf, StrLen(dBuf));
		p+=CBMIW;
		*p++='\n';
		*p='\x00';
		PalmPrintLineModePrint(page);
		MemHandleUnlock(mh);
	}

	PalmPrintLineModeStop();
	MemHandleUnlock(resStrInch);
	MemHandleUnlock(resStrFeet);
	MemHandleFree(pmh);
}
