/*
 * fFind.c
 */
#include "Include.h"

/* protos */
static Boolean FindNext(UInt32 *, Document *, Boolean *);
static Boolean EH(EventType *);

/* globals */
static MemHandle findH;
static UInt32 endPos, curPos, pip;

/*
 * fFindAlloc
 *
 * Initialize the Find engine.
 */
void
fFindAlloc(void)
{
	findH=NULL;
	findTerm=NULL;
	fFindReset();
}

/*
 * fFindClose
 *
 * Close the Find engine.
 */
void
fFindClose(void)
{
	if (findH)
		MemHandleFree(findH);
}

/*
 * fFindReset
 *
 * Reset "Find Next" marker.
 */
void
fFindReset(void)
{
	endPos=0;
}

/*
 * fFindRun
 *
 * Show and run the Find dialog.
 *
 * <-  goToPos		Where a match was found.
 *  -> d		Document.
 *
 * Returns true if a match was found (and goToPos is updated), false otherwise.
 */
Boolean
fFindRun(UInt32 *goToPos, Document *d)
{
	FormSave frm;

	UIFormPrologue(&frm, fFind, EH);
	if (findH)
		UIFieldSelectAll(cFindTxtFind, findTerm, true);
	else
		UIFieldFocus(cFindTxtFind);

	if (prefs.flags&PFFromTop)
		CtlSetValue(UIObjectGet(cFindChkFromTop), 1);

	if (UIFormEpilogue(&frm, NULL, cFindCancel)==false) {
		if (findTerm)
			*findTerm=0;

		return false;
	}

	fFindReset();
	if (StrLen(findTerm)==0)
		return false;

	if (prefs.flags&PFFromTop)
		curPos=0;
	else
		curPos=d->position;

	return fFindNext(goToPos, d);
}

/*
 * fFindNext
 *
 * Find "next" match.
 *
 * <-  goToPos		Where a match was found.
 *  -> d		Document.
 *
 * Returns true if a match was found (and goToPos is updated), false otherwise.
 */
Boolean
fFindNext(UInt32 *goToPos, Document *d)
{
	Boolean showAlert=true, ret;

	if (((ret=FindNext(goToPos, d, &showAlert))==false) && (showAlert))
		FrmAlert(aFindNotFound);

	return ret;
}

/*
 * FindNext
 *
 * Handle find next.
 *
 * <-  goToPos		Where a match was found.
 *  -> d		Document.
 * <-  showAlert	Show aFindNotFound alert?
 *
 * Returns true if a match was found (and goToPos is updated), false otherwise.
 */
static Boolean
FindNext(UInt32 *goToPos, Document *d, Boolean *showAlert)
{
	UInt32 savePos=d->position;
	UInt16 stLen, stIdx, pNo;

	if ((stLen=(UInt16)StrLen(findTerm))==0) {
		fFindReset();
		return false;
	}

	if (endPos)
		curPos+=endPos;

	if (curPos>=d->length)
		return false;

	DocPagePos(d, curPos, &pNo, &pip);
	DocGetPage(d, pNo);

	for (stIdx=0; stIdx<stLen; ) {
		if (curPos>=d->length) {
			d->position=savePos;
			DocPagePos(d, d->position, &pNo, &d->posInPage);
			DocGetPage(d, pNo);
			return false;
		}

		while (findTerm[stIdx]==' ') {
			if (!d->page[pip]) {
				DocPagePos(d, curPos, &pNo, &pip);
				DocGetPage(d, pNo);
				pip=0;
			}

			if (d->page[pip]<=' ') {
				pip++;
				curPos++;
			} else
				stIdx++;
		}

		if (!d->page[pip]) {
			DocPagePos(d, curPos, &pNo, &pip);
			DocGetPage(d, pNo);
			pip=0;
		}

		if ((findTerm[stIdx]&~32)==(d->page[pip]&~32)) {
			if (!stIdx)
				*goToPos=curPos;

			stIdx++;
			if (!findTerm[stIdx])
				break;
		} else
			stIdx=0;

		pip++;
		curPos++;
	}

	return true;
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	if ((ev->eType==ctlSelectEvent) && (ev->data.ctlSelect.controlID==cFindOK)) {
		Char *p=UIFieldGetText(cFindTxtFind);
		UInt16 dIdx=0, sIdx, len;
		Boolean isSpace=true;

		if (!p || StrLen(p)==0)
			return false;

		if (CtlGetValue(UIObjectGet(cFindChkFromTop)))
			prefs.flags|=PFFromTop;
		else
			prefs.flags&=~PFFromTop;

		if (findH==NULL) {
			findH=MemHandleNew(FindTermLength+1);
			ErrFatalDisplayIf(findH==NULL, "(fFindEH) Out of memory");
			findTerm=MemHandleLock(findH);
		}

		len=StrLen(p);
		for (sIdx=0; sIdx<len; sIdx++, p++) {
			if (*p<=' ') {
				if (isSpace)
					continue;
				else {
					findTerm[dIdx++]=' ';
					isSpace=true;
				}
			} else {
				findTerm[dIdx++]=*p;
				isSpace=false;
			}
		}

		if (findTerm[dIdx-1]==' ')
			dIdx--;

		findTerm[dIdx]=0;
	}

	return false;
}
