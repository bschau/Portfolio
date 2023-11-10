/*
 * fSaveScore.c
 */
#include <DLServer.h>
#include "Include.h"

/* macros */
#define MemoSize 4096
#define RealMemoSize (MemoSize-1)

/* protos */
static Boolean EH(EventType *);
static void SaveScore(Char *);
static void ResetMemo(void);
static void SetMemo(void);
static void ExportScore(void);
static void PackAndAdd(UInt16, Char *, Char *, Char *);

/* globals */
static UInt16 memoSize, packSize;
static MemHandle packH;
static Char *pack, *memoPtr, *memo;

/*
 * fSaveScoreRun
 *
 * Save the current score to the database.
 *
 * Returns true if score was saved, false otherwise.
 */
Boolean
fSaveScoreRun(void)
{
	FormSave frm;
	Boolean ret;

	DBClear(&dbMemo);
	if (DBOpen(&dbMemo, dmModeReadWrite, true)==false)
		return false;

	UIFormPrologue(&frm, fSaveScore, EH);

	if (prefs.userName[0]) {
		FieldType *fld=UIObjectGet(cSaveScoreName);

		UIFieldSetText(cSaveScoreName, prefs.userName);
		FldSetSelection(fld, 0, FldGetTextLength(fld));
	}
	UIFieldFocus(cSaveScoreName);

	if (prefs.exchangeFlags&ExchgSaveScore)
		CtlSetValue(UIObjectGet(cSaveScoreCheckbox), 1);

	catTo=prefs.catExchangeIdx;
	UIPopupSet(&dbMemo, catTo, catToName, cSaveScorePopup);

	ret=UIFormEpilogue(&frm, NULL, cSaveScoreCancel);
	DBClose(&dbMemo);
	return ret;
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	Char *p;

	if (ev->eType==ctlSelectEvent) {
		switch (ev->data.ctlSelect.controlID) {
		case cSaveScorePopup:
			CategorySelect(dbMemo.db, currentForm, cSaveScorePopup, cSaveScoreList, false, &catTo, catToName, 0, categoryHideEditCategory);
			return true;

		case cSaveScoreSave:
			if ((CtlGetValue(UIObjectGet(cSaveScoreCheckbox))))
				prefs.exchangeFlags|=ExchgSaveScore;
			else
				prefs.exchangeFlags&=~ExchgSaveScore;

			prefs.catExchangeIdx=catTo;
			SaveScore(UIFieldGetText(cSaveScoreName));

			if (prefs.exchangeFlags&ExchgSaveScore) {
				MemHandle memoH=MemHandleNew(MemoSize);
				ErrFatalDisplayIf(memoH==NULL, "(fSaveScoreRunEH) Device is full.");
				memoPtr=MemHandleLock(memoH);
				ExportScore();
				MemHandleFree(memoH);
				p=" and exported to Memo Pad";
			} else
				p="";

			FrmCustomAlert(aScoreSaved, p, NULL, NULL);

		default:	/* FALL-THRU */
			break;
		}
	}

	return false;
}

/*
 * SaveScore
 *
 * Save score information to the scores database.
 *
 *  -> p		User name (or NULL).
 */
static void
SaveScore(Char *p)
{
	Boolean found=false;
	UInt16 rIdx=0;
	DmOpenRef db;
	MemHandle rh;
	Score *r;
	Score s;

	db=dbScore.db;
	if (p) {
		MemSet(prefs.userName, sizeof(prefs.userName), 0);
		StrNCopy(prefs.userName, p, HotSyncNameLength);
	} else {
		if (StrLen(prefs.userName)<1)
			DlkGetSyncInfo(NULL, NULL, NULL, prefs.userName, NULL, NULL);
		p=prefs.userName;
	}

	MemSet(&s, sizeof(s), 0);
	s.score=game.score;
	s.round=game.round;
	s.year=game.gameEnded.year+1904;
	s.month=game.gameEnded.month;
	s.day=game.gameEnded.day;
	StrNCopy(s.name, prefs.userName, HotSyncNameLength);
	PMGetGroupName(&dbSGroup, game.guid, s.group);

	for (EVER) {
		if ((rh=DmQueryNextInCategory(db, &rIdx, dmAllCategories))==NULL)
			break;

		r=MemHandleLock(rh);
		if (StrCaselessCompare(r->name, s.name)==0) {
			MemHandleUnlock(rh);
			found=true;
			break;
		} else {
			MemHandleUnlock(rh);
			rIdx++;
		}
	}

	if (found==false)
		rIdx=dmMaxRecordIndex;

	DBSetRecord(&dbScore, rIdx, dmUnfiledCategory, &s, sizeof(s), SFScore);
}

/*
 * ResetMemo
 *
 * Reset the memo information.
 */
static void
ResetMemo(void)
{
	memoSize=RealMemoSize;
	memo=memoPtr;
	MemSet(memo, RealMemoSize, 0);
}

/*
 * SetMemo
 *
 * Commit a memo to the memo pad.
 */
static void
SetMemo(void)
{
	UInt16 len=RealMemoSize-memoSize, rIdx=dmMaxRecordIndex, attrs;
	DmOpenRef db=dbMemo.db;
	MemHandle rh;
	void *p;

	rh=DmNewRecord(db, &rIdx, len+1);
	ErrFatalDisplayIf(rh==NULL, "(SetMemo) Device is full.");

	p=MemHandleLock(rh);
	DmWrite(p, 0, memoPtr, len+1);
	MemHandleUnlock(rh);

	DmRecordInfo(db, rIdx, &attrs, NULL, NULL);
	attrs&=~dmRecAttrCategoryMask;
	attrs|=catTo;
	DmSetRecordInfo(db, rIdx, &attrs, NULL);
	DmReleaseRecord(db, rIdx, true);
}

/*
 * AddToMemo
 *
 * Add text to memo - flush if necessary.
 *
 *  -> src		Source text to add.
 */
static void
AddToMemo(Char *src)
{
	UInt16 len=StrLen(src);

	if (len>(memoSize-1)) {
		SetMemo();
		ResetMemo();
	}

	MemMove(memo, src, len);
	memo+=len;
	*memo++='\n';
	memoSize-=(len+1);
}

/*
 * ExportScore
 *
 * Export score info to Memo pad.
 */
static void
ExportScore(void)
{
	Char buffer[GroupNameLength+128];
	UInt16 pct, rIdx;
	MemHandle mh, rh, sh, sentencesH;
	Char *you, *sBuffer;

	ResetMemo();
	if (game.round)
		pct=(game.score*100)/game.round;
	else
		pct=0;

	StrPrintF(buffer, "%s, ", prefs.userName);
	DateToAscii(game.gameEnded.month, game.gameEnded.day, game.gameEnded.year+1904, PrefGetPreference(prefDateFormat), buffer+StrLen(buffer));
	StrPrintF(buffer+StrLen(buffer), "\nScore: %u%% (%u of %u)\n", pct, game.score, game.round);
	StrCat(buffer, "Group: ");
	PMGetGroupName(&dbSGroup, game.guid, buffer+StrLen(buffer));
	StrCat(buffer, "\n");

	memo=memoPtr;
	AddToMemo(buffer);

	packH=NULL,
	packSize=0;

	sentencesH=MemHandleNew(2*(SentenceLength+1));
	ErrFatalDisplayIf(sentencesH==NULL, "(ExportScore) Out of memory.");
	sBuffer=MemHandleLock(sentencesH);

	for (pct=0; pct<game.round; pct++) {
		mh=DBGetRecord(&dbAnswer, pct);
		you=MemHandleLock(mh);

		if ((rIdx=XRefIndex(&dbSXref, game.guid, pct))!=dmMaxRecordIndex) {
			rh=DBGetRecord(&dbSXref, rIdx);
			if (DmFindRecordByID(dbSSentence.db, ((XRefRecord *)MemHandleLock(rh))->suid, &rIdx)==errNone) {
				sh=DBGetRecord(&dbSSentence, rIdx);
				PackAndAdd(pct, you, MemHandleLock(sh), sBuffer);
				MemHandleUnlock(sh);
			} else
				PackAndAdd(pct, you, NULL, sBuffer);

			MemHandleUnlock(rh);
		} else
			PackAndAdd(pct, you, NULL, sBuffer);

		MemHandleUnlock(mh);
	}

	MemHandleFree(sentencesH);

	if (packH)
		MemHandleFree(packH);

	if (*memoPtr)
		SetMemo();
}

/*
 * PackAndAdd
 *
 * Pack two sentences and header into chunk - add the chunk to memo.
 *
 *  -> sNo		Sentence number (zero index based).
 *  -> student		Student sentence.
 *  -> correct		Correct answer (or NULL if unknown).
 *  -> sBuffer		Sentences buffer.
 */
static void
PackAndAdd(UInt16 sNo, Char *student, Char *correct, Char *sBuffer)
{
	UInt16 len=100+StrLen(student);
	Char *c;

	sNo++;
	if (correct==NULL) {
		if (len>packSize) {
			if (packH)
				MemHandleFree(packH);

			packH=MemHandleNew(len);
			ErrFatalDisplayIf(packH==NULL, "(PackAndAdd) Out of memory.");
			pack=MemHandleLock(packH);
		}

		StrPrintF(pack, "Sentence %u - Incomplete:\n%s\n\n", sNo, student);
	} else {
		len+=StrLen(correct);
		if (len>packSize) {
			if (packH)
				MemHandleFree(packH);

			packH=MemHandleNew(len);
			ErrFatalDisplayIf(packH==NULL, "(PackAndAdd) Out of memory.");
			pack=MemHandleLock(packH);
		}

		StringStrip(sBuffer, student);
		c=sBuffer+StrLen(sBuffer)+1;
		StringStrip(c, correct);
		if (StrCompare(sBuffer, c)==0)
			StrPrintF(pack, "Sentence %u - Correct:\nA: %s\n", sNo, correct);
		else
			StrPrintF(pack, "Sentence %u - Incorrect:\nS: %s\nA: %s\n", sNo, student, correct);
	}

	AddToMemo(pack);
}
