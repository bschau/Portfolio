/*
 * fSDictate.c
 */
#include "Include.h"

/* protos */
static void SaveData(void);
static void SentenceUpdate(void);
static void Validate(void);

/* globals */
static UInt16 sentenceCnt;

/*
 * fSDictateInit
 */
Boolean
fSDictateInit(void)
{
	DmOpenRef db=dbAnswer.db;
	Char buffer[GroupNameLength+1];

	sentenceCnt=XRefCount(&dbSXref, game.guid);

	if (!game.round) {
		while (DmNumRecords(db)>0)
			ErrFatalDisplayIf(DmRemoveRecord(db, 0)!=errNone, "(fSDictateInit) Cannot empty database.");
	}

	if (game.sentenceLength) {
		MemHandle mh=MemHandleNew(game.sentenceLength+1);
		Char *p;

		ErrFatalDisplayIf(mh==NULL, "(fSDictateInit) Out of memory.");

		p=MemHandleLock(mh);
		if (PMGetPref(p, game.sentenceLength, PrfGameField)==true) {
			p[game.sentenceLength]='\x00';
			UIFieldSetText(cSDictateField, p);
		}

		MemHandleFree(mh);
	}

	PMGetGroupName(&dbSGroup, game.guid, buffer);
	UIFieldSetText(cSDictateGroup, buffer);

	return true;
}

/*
 * fSDictateEH
 */
Boolean
fSDictateEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		SentenceUpdate();
		IRImport();
		return true;

	case frmSaveEvent:
		if (appStopped)
			SaveData();
		break;

	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==cSDictateSubmit) {
			Validate();
			return true;
		}
		break;

	case menuEvent:
		if (ev->data.menu.itemID==mSDictateAbort) {
			if (FrmAlert(aStudentAbort)==0) {
				PMSetPref(NULL, 0, PrfGameInfo);
				PMSetPref(NULL, 0, PrfGameField);
				FrmGotoForm(fStudent);
			}
			return true;
		}
		break;

	case keyDownEvent:
		UIFieldScrollBarKeyHandler(ev, cSDictateField, cSDictateScrollBar);
	default: /* FALL-THRU */
		UIFieldScrollBarHandler(ev, cSDictateField, cSDictateScrollBar);
		break;
	}

	return false;
}

/*
 * SaveData
 *
 * Save field and game.
 */
static void
SaveData(void)
{
	Char *p=UIFieldGetText(cSDictateField);

	if (p)
		game.sentenceLength=StrLen(p);
	else
		game.sentenceLength=0;

	if (game.score==0 && game.round==0 && game.sentenceLength==0) {
		PMSetPref(NULL, 0, PrfGameInfo);
		PMSetPref(NULL, 0, PrfGameField);
	} else {
		PMSetPref(&game, sizeof(Game), PrfGameInfo);
		if (game.sentenceLength)
			PMSetPref(p, game.sentenceLength, PrfGameField);
		else
			PMSetPref(NULL, 0, PrfGameField);
	}
}

/*
 * SentenceUpdate
 *
 * Update sentence header.
 */
static void
SentenceUpdate(void)
{
	UInt16 pW, len;
	Char buffer[32];
	Boolean trunc;
	RectangleType r;

	r.topLeft.x=80;
	r.topLeft.y=0;
	r.extent.x=80;
	r.extent.y=12;
	pW=80;

	if (game.round==(sentenceCnt-1))
		StrCopy(buffer, "Last sentence");
	else
		StrPrintF(buffer, "Sentence #%d", game.round+1);

	len=StrLen(buffer);
	FntCharsInWidth(buffer, &pW, &len, &trunc);
	WinEraseRectangle(&r, 0);

	WinDrawChars(buffer, len, 160-pW, 1);

	UIFieldFocus(cSDictateField);
}

/*
 * Validate
 *
 * Validate students answer.
 */
static void
Validate(void)
{
	Char *p=UIFieldGetText(cSDictateField);
	UInt16 len, rIdx;
	Char sentence[SentenceLength+2];
	Char orig[SentenceLength+2];
	MemHandle mh;

	len=StrLen(p);
	if (p==NULL || len==0) {
		FrmAlert(aInvalidEntry);
		return;
	}

	MemMove(sentence, p, len);
	sentence[len]='\x00';

	DBSetRecord(&dbAnswer, dmMaxRecordIndex, dmUnfiledCategory, p, len+1, NULL);
	StringStrip(sentence, sentence);

	rIdx=XRefIndex(&dbSXref, game.guid, game.round);
	ErrFatalDisplayIf(rIdx==dmMaxRecordIndex, "(Validate) XRef not found - database damaged?");

	mh=DBGetRecord(&dbSXref, rIdx);
	if (DmFindRecordByID(dbSSentence.db, ((XRefRecord *)MemHandleLock(mh))->suid, &rIdx)!=errNone)
		ErrFatalDisplay("(Validate) Cannot query Sentence database.");

	MemHandleUnlock(mh);

	mh=DBGetRecord(&dbSSentence, rIdx);
	StringStrip(orig, MemHandleLock(mh));
	MemHandleUnlock(mh);

	if (StrCompare(orig, sentence)==0)
		game.score++;

	game.round++;
	game.sentenceLength=0;
	PMSetPref(&game, sizeof(Game), PrfGameInfo);
	PMSetPref(NULL, 0, PrfGameField);

	if (game.round<sentenceCnt) {
		UIFieldClear(cSDictateField);
		UIFieldUpdateScrollBar(cSDictateField, cSDictateScrollBar);
		UIFieldFocus(cSDictateField);
		SentenceUpdate();
	} else
		FrmGotoForm(fWait);
}
