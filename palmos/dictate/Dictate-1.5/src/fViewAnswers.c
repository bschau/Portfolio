/*
 * fViewAnswers.c
 */
#include "Include.h"

/* protos */
static void SentenceUpdate(void);

/* globals */
static UInt16 sentenceNo, sentenceCnt;

/*
 * fViewAnswersInit
 */
Boolean
fViewAnswersInit(void)
{
	sentenceNo=0;
	sentenceCnt=DmNumRecords(dbAnswer.db);
	return true;
}

/*
 * fViewAnswersEH
 */
Boolean
fViewAnswersEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		SentenceUpdate();
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cViewAnswersDone:
			FrmGotoForm(fDictationDone);
			return true;

		case cViewAnswersPrev:
			if (sentenceNo>0) {
				sentenceNo--;
				SentenceUpdate();
			}
			return true;

		case cViewAnswersNext:
			if (sentenceNo<(sentenceCnt-1)) {
				sentenceNo++;
				SentenceUpdate();
			}
			return true;
		}
		break;

	case keyDownEvent:
		if (UIFieldScrollBarKeyHandler(ev, cViewAnswersYouField, cViewAnswersYouScrollBar)==false)
			UIFieldScrollBarKeyHandler(ev, cViewAnswersCorrectField, cViewAnswersCorrectScrollBar);

	default: /* FALL-THRU */
		if (UIFieldScrollBarHandler(ev, cViewAnswersYouField, cViewAnswersYouScrollBar)==false)
			UIFieldScrollBarHandler(ev, cViewAnswersCorrectField, cViewAnswersCorrectScrollBar);
		break;
	}

	return false;
}

/*
 * SentenceUpdate
 *
 * Update sentence header.
 */
static void
SentenceUpdate(void)
{
	Char cSentence[SentenceLength+2];
	Char iSentence[SentenceLength+2];
	UInt16 pW, len;
	Char buffer[32];
	Boolean trunc;
	RectangleType r;
	MemHandle mh, rh;
	Char *p;

	*cSentence='\x00';
	if ((pW=XRefIndex(&dbSXref, game.guid, sentenceNo))!=dmMaxRecordIndex) {
		mh=DBGetRecord(&dbSXref, pW);
		if (DmFindRecordByID(dbSSentence.db, ((XRefRecord *)MemHandleLock(mh))->suid, &pW)==errNone) {
			rh=DBGetRecord(&dbSSentence, pW);
			p=MemHandleLock(rh);
			StringStrip(cSentence, p);
			UIFieldSetText(cViewAnswersCorrectField, p);
			MemHandleUnlock(rh);
		}

		MemHandleUnlock(mh);
	}

	mh=DBGetRecord(&dbAnswer, sentenceNo);
	p=MemHandleLock(mh);
	StringStrip(iSentence, p);
	UIFieldSetText(cViewAnswersYouField, p);
	MemHandleUnlock(mh);
	UIFieldUpdateScrollBar(cViewAnswersYouField, cViewAnswersYouScrollBar);

	if (!(*cSentence)) {
		UIFieldSetText(cViewAnswersCorrectField, "--- missing ---");
		UIFieldUpdateScrollBar(cViewAnswersCorrectField, cViewAnswersCorrectScrollBar);
	}

	if (*cSentence && StrCompare(iSentence, cSentence)==0) {
		UIObjectHide(cViewAnswersYouLabel);
		UIObjectHide(cViewAnswersYouField);
		UIObjectHide(cViewAnswersYouScrollBar);
	} else {
		UIObjectShow(cViewAnswersYouLabel);
		UIObjectShow(cViewAnswersYouField);
		UIObjectShow(cViewAnswersYouScrollBar);
	}

	r.topLeft.x=80;
	r.topLeft.y=0;
	r.extent.x=80;
	r.extent.y=12;
	pW=80;

	if (sentenceNo==(sentenceCnt-1))
		StrCopy(buffer, "Last sentence");
	else
		StrPrintF(buffer, "Sentence #%d", sentenceNo+1);

	len=StrLen(buffer);
	FntCharsInWidth(buffer, &pW, &len, &trunc);
	WinEraseRectangle(&r, 0);

	WinDrawChars(buffer, len, 160-pW, 1);

	if (sentenceNo>0)
		UIObjectShow(cViewAnswersPrev);
	else
		UIObjectHide(cViewAnswersPrev);

	if (sentenceNo<(sentenceCnt-1))
		UIObjectShow(cViewAnswersNext);
	else
		UIObjectHide(cViewAnswersNext);
}
