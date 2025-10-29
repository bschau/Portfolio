/*
 * fTDictate.c
 */
#include "Include.h"

/* protos */
static void SentenceUpdate(void);

/* globals */
static UInt16 sentenceNo, sentenceCnt;

/*
 * fTDictateInit
 */
Boolean
fTDictateInit(void)
{
	Char buffer[GroupNameLength+1];

	sentenceNo=0;
	sentenceCnt=XRefCount(&dbTXref, groupUid);

	PMGetGroupName(&dbTGroup, groupUid, buffer);
	UIFieldSetText(cTDictateGroup, buffer);

	return true;
}

/*
 * fTDictateEH
 */
Boolean
fTDictateEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		SentenceUpdate();
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cTDictateStop:
			if (FrmAlert(aStopDictation)==0)
				FrmGotoForm(fGroup);

			return true;

		case cTDictatePrev:
			if (sentenceNo>0) {
				sentenceNo--;
				SentenceUpdate();
			}
			return true;

		case cTDictateNext:
			if (sentenceNo<(sentenceCnt-1)) {
				sentenceNo++;
				SentenceUpdate();
			} else 
				FrmGotoForm(fGroup);

			return true;
		}
		break;

	case keyDownEvent:
		UIFieldScrollBarKeyHandler(ev, cTDictateField, cTDictateScrollBar);
	default: /* FALL-THRU */
		UIFieldScrollBarHandler(ev, cTDictateField, cTDictateScrollBar);
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
	UInt16 pW, len;
	Char buffer[32];
	Boolean trunc;
	RectangleType r;
	MemHandle mh;

	if ((pW=XRefIndex(&dbTXref, groupUid, sentenceNo))==dmMaxRecordIndex)
		return;

	mh=DBGetRecord(&dbTXref, pW);
	if (DmFindRecordByID(dbTSentence.db, ((XRefRecord *)MemHandleLock(mh))->suid, &pW)!=errNone) {
		MemHandleUnlock(mh);
		return;
	}
	MemHandleUnlock(mh);

	mh=DBGetRecord(&dbTSentence, pW);
	UIFieldSetText(cTDictateField, MemHandleLock(mh));
	MemHandleUnlock(mh);
	UIFieldUpdateScrollBar(cTDictateField, cTDictateScrollBar);

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
		UIObjectShow(cTDictatePrev);
	else
		UIObjectHide(cTDictatePrev);
}
