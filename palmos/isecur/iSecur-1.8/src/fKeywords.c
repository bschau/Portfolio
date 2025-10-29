/*
 * fKeywords.c
 */
#include <DLServer.h>
#include "Include.h"

/* macros */
#define MaxKeywords 30
#define KeywordDescLen 30
#define PageSize 5

/* structs */
typedef struct {
	UInt16 len;
	Char keyword[KeywordDescLen+1];
	Char value[KeywordLength+1];
} Keyword;

/* protos */
_FRM(static Boolean KeywordExists(Char *, UInt16, UInt16 *));
_FRM(static void CountKeywords(Char *));
_FRM(static void ExtractKeywords(Char *));
_FRM(static Boolean ExpandMacro(Char *, UInt16, Char *, UInt16));
_FRM(static Boolean InsertKeyword(Char *, UInt16, Char *, UInt16));
_FRM(static Boolean EH(EventType *));
_FRM(static void Redraw(void));
_FRM(static void UpdateArrows(void));
_FRM(static void SaveFields(void));
_FRM(static void Scroll(Int16));
_FRM(static void PageScrollFocus(Int16));
_FRM(static void KeyDownHandler(EventType *));
_FRM(static void SetupSystemKeywords(void));
_FRM(static void Keywordize(Char *, MemHandle *, UInt16));
_FRM(static void Copy(MemHandle, UInt16, UInt16 *, Char *, UInt16));
_FRM(static void GeneratePassword(Char *, Char *));

/* globals */
extern Char *tArrUp, *tArrDown, *tArrUpDis, *tArrDownDis;
static Keyword *keywords;
static UInt16 fKeywordsBand[]={ cKeywordsF1, cKeywordsF2, cKeywordsF3, cKeywordsF4, cKeywordsF5,  0 };
extern MemHandle keywordsH;
extern UInt16 keywordsCnt, keywordsIdx;
extern Int16 current;
extern Boolean keywordsDone, useKeywords;
extern Char bufDate[longDateStrLength+1], bufTime[timeStringLength+1];
extern Char gPwd[];

/*
 * KeywordExists
 *
 * Do a keyword lookup.
 *
 *  -> src		Source keyword.
 *  -> len		Keyword length.
 * <-  idx		Index found.
 *
 * Returns true if keyword was found, false otherwise.
 */
static Boolean
KeywordExists(Char *src, UInt16 len, UInt16 *idx)
{
	UInt16 i;

	for (i=0; i<keywordsCnt; i++) {
		if (keywords[i].len==len) {
			if (StrCaselessCompare(keywords[i].keyword, src)==0) {
				if (idx)
					*idx=i;

				return true;
			}
		}
	}

	return false;
}

/*
 * CountKeywords
 *
 * Count number of keywords in this string.
 *
 *  -> str		Source string.
 */
static void
CountKeywords(Char *str)
{
	Boolean inBrackets=false;

	if (str) {
		for (; *str; str++) {
			if (*str=='[') {
				if (*(str+1)!='[' && *(str+1)!=']' && *(str+1)!='=')
					inBrackets=true;
			} else if (*str==']') {
				if (inBrackets==true) {
					keywordsCnt++;
					inBrackets=false;
				}
			}
		}
	}
}

/*
 * ExtractKeywords
 *
 * Extract and store the keywords from the string.
 *
 *  -> str		Source string.
 */
static void
ExtractKeywords(Char *str)
{
	Boolean inBrackets=false;
	UInt16 len=0;
	Char *s=NULL;

	if (!str)
		return;

	for (; *str; str++) {
		if (*str=='[') {
			if (*(str+1)!='[' && *(str+1)!=']') {
				if (inBrackets==false) {
					len=0;
					s=str+1;
					inBrackets=true;
				}
			}
		} else if (*str==']') {
			if (inBrackets) {
				if (InsertKeyword(s, len, NULL, 0)==false)
					break;

				inBrackets=false;
			}
		} else if (inBrackets) {
			if (*str=='=') {
				if (*(str+1)=='@') {
					UInt16 dLen=0;
					Char *p;

					for (p=str+2; *p && (*p!=']'); p++)
						dLen++;

					ExpandMacro(s, len, str+2, dLen);

					str=p+1;
					inBrackets=false;
				} else {
					UInt16 dLen=0;
					Char *p;

					for (p=str+1; *p && (*p!=']'); p++)
						dLen++;

					if (InsertKeyword(s, len, str+1, dLen)==false)
						break;

					str=p+1;
					inBrackets=false;
				}
			} else
				len++;
		}
	}
}

/*
 * ExpandMacro
 *
 * Expand a macro.
 *
 *  -> s		s.
 *  -> len		s length.
 *  -> mStrt		Macro name start.
 *  -> mLen		Macro name length.
 *
 * Returns true if there is room for one more keyword, false otherwise.
 */
static Boolean
ExpandMacro(Char *s, UInt16 len, Char *mStrt, UInt16 mLen)
{
	Char buffer[100];
	DateTimeType dt;

	TimSecondsToDateTime(TimGetSeconds(), &dt);

	*buffer=0;

	if (StrNCaselessCompare(mStrt, "time", 4)==0)
		TimeToAscii(dt.hour, dt.minute, PrefGetPreference(prefTimeFormat), buffer);
	else if (StrNCaselessCompare(mStrt, "name", 4)==0) {
		if (DlkGetSyncInfo(NULL, NULL, NULL, buffer, NULL, NULL)!=errNone)
			StrCopy(buffer, "Eddie the Psyched PDA");
	} else if (StrNCaselessCompare(mStrt, "advice", 6)==0)
		StrCopy(buffer, "Don't get lost!");
	else if (StrNCaselessCompare(mStrt, "longdate", 8)==0)
		DateToAscii(dt.month, dt.day, dt.year, PrefGetPreference(prefLongDateFormat), buffer);
	else if (StrNCaselessCompare(mStrt, "password", 8)==0)
		GeneratePassword(mStrt+8, buffer);
	else if (StrNCaselessCompare(mStrt, "shortdate", 9)==0)
		DateToAscii(dt.month, dt.day, dt.year, PrefGetPreference(prefDateFormat), buffer);

	if (*buffer)
		return InsertKeyword(s, len, buffer, StrLen(buffer));
	else
		return InsertKeyword(s, len, "** Invalid Macro **", 19);

	return true;
}

/*
 * InsertKeyword
 *
 * Insert a new keyword.
 *
 *  -> src		Source keyword.
 *  -> sLen		Length of keyword.
 *  -> value		Value.
 *  -> vLen		Length of value.
 *
 * Returns true if there is room for one more keyword, false otherwise.
 */
static Boolean
InsertKeyword(Char *src, UInt16 sLen, Char *value, UInt16 vLen)
{
	if (KeywordExists(src, sLen, NULL)==false) {
		if (sLen>KeywordDescLen)
			sLen=KeywordDescLen;

		keywords[keywordsIdx].len=sLen;
		MemMove(keywords[keywordsIdx].keyword, src, sLen);
		if (value && vLen>0) {
			if (vLen>KeywordLength)
				vLen=KeywordLength;

			MemMove(keywords[keywordsIdx].value, value, vLen);
			keywords[keywordsIdx].value[vLen]=0;
		} else
			MemSet(keywords[keywordsIdx].value, KeywordLength+1, 0);

		keywordsIdx++;
		if (keywordsIdx>=MaxKeywords)
			return false;
	}

	return true;
}

/*
 * fKeywordsInit
 *
 * Initialize keywords.
 */
void
fKeywordsInit(void)
{
	keywordsH=NULL;
}

/*
 * fKeywordsOpen
 *
 * Parse a template and set up keywords.
 *
 *  -> title		Title (or NULL).
 *  -> secret		Secret (or NULL).
 *
 * Returns true if one or more keywords was found, false otherwise.
 */
Boolean
fKeywordsOpen(Char *title, Char *secret)
{
	fKeywordsClose();
	if (!title && !secret)
		return false;

	keywordsCnt=0;
	CountKeywords(title);
	CountKeywords(secret);

	if (!keywordsCnt)
		return false;

	if (keywordsCnt>MaxKeywords)
		keywordsCnt=MaxKeywords;

	keywordsH=MemHandleNew(MaxKeywords*sizeof(Keyword));
	ErrFatalDisplayIf(keywordsH==NULL, "(fKeywordsOpen) Out of memory");
	keywords=MemHandleLock(keywordsH);

	MemSet(keywords, MaxKeywords*sizeof(Keyword), 0);

	keywordsIdx=0;
	ExtractKeywords(title);
	ExtractKeywords(secret);
	return true;
}

/*
 * fKeywordsClose
 *
 * Close the keywords.
 */
void
fKeywordsClose(void)
{
	if (keywordsH) {
		MemSet(MemHandleLock(keywordsH), MemHandleSize(keywordsH), 0);
		MemHandleFree(keywordsH);
		keywordsH=NULL;
	}
}

/*
 * fKeywordsRun
 *
 * Show the Keywords screen.
 * 
 *  -> title		Title.
 *  -> secret		Secret.
 * <-  dstTH		Destination title handle.
 * <-  dstSH		Destination secret handle.
 *
 * Returns true if keywords should be used, false otherwise.
 */
Boolean
fKeywordsRun(Char *title, Char *secret, MemHandle *dstTH, MemHandle *dstSH)
{
	FormType *saveCurrent=currentForm;
	UInt16 saveID=currentFormID;
	FormActiveStateType frmSave;
	EventType ev;
	Err err;

	*dstTH=NULL;
	*dstSH=NULL;
	current=0;

	FrmSaveActiveState(&frmSave);

	currentFormID=fKeywords;
	currentForm=FrmInitForm(currentFormID);
	ErrFatalDisplayIf(currentForm==NULL, "(fKeywordsRun) Cannot initialize form.");
	FrmSetActiveForm(currentForm);
	FrmSetEventHandler(currentForm, (FormEventHandlerType *)EH);

	MemSet(&ev, sizeof(EventType), 0);
	ev.eType=frmOpenEvent;
	ev.data.frmLoad.formID=currentFormID;
	EvtAddEventToQueue(&ev);

	keywordsDone=false;
	useKeywords=false;
	while (!keywordsDone) {
		EvtGetEvent(&ev, evtWaitForever);
		if (ev.eType==appStopEvent) {
			EvtAddEventToQueue(&ev);
			appStopped=true;
			break;
		} else if (ev.eType==frmCloseEvent)
			break;

		if (PMHardPower(&ev)==true)
			continue;

		if (SysHandleEvent(&ev)==true ||
		    MenuHandleEvent(0, &ev, &err)==true)
			continue;

		FrmDispatchEvent(&ev);
	}

	/* merge keywords & template here */
	SaveFields();
	FrmEraseForm(currentForm);
	FrmDeleteForm(currentForm);
	FrmRestoreActiveState(&frmSave);
	currentForm=saveCurrent;
	currentFormID=saveID;

	if (useKeywords) {
		SetupSystemKeywords();
		Keywordize(title, dstTH, TitleLength);
		Keywordize(secret, dstSH, SecretLength);
	}

	return useKeywords;
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	EntropyAdd(pool, ev);

	switch (ev->eType) {
	case frmOpenEvent:
		UIFieldFocus(cKeywordsF1);
	case frmUpdateEvent:
		FrmDrawForm(currentForm);
		Redraw();
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cKeywordsOK:
			useKeywords=true;
			keywordsDone=true;
			EvtWakeup();
			return true;
		case cKeywordsCancel:
			keywordsDone=true;
			EvtWakeup();
			return true;
		case cKeywordsClearAll:
			if (FrmAlert(aClearAll)==0) {
				UInt16 idx;

				for (idx=0; idx<keywordsCnt; idx++)
					MemSet(keywords[idx].value, KeywordLength+1, 0);

				current=0;
				Redraw();
				UIFieldFocus(cKeywordsF1);
			}
			return true;
		case cKeywordsUp:
			Scroll(-(PageSize-1));
			PageScrollFocus(-1);
			return true;
		case cKeywordsDown:
			Scroll(PageSize-1);
			PageScrollFocus(1);
			return true;
		case cKeywordsGenerate:
			fPwdGenRequester();
			SaveFields();
			return true;
		}

		if ((ev->data.ctlSelect.controlID>=cKeywordsS1) && (ev->data.ctlSelect.controlID<=cKeywordsS5)) {
			UInt16 sIdx=ev->data.ctlSelect.controlID-cKeywordsS1;

			MemSet(keywords[sIdx+current].value, KeywordLength+1, 0);
			sIdx+=cKeywordsF1;
			UIFieldClear(sIdx);
			UIFieldFocus(sIdx);
			return true;
		}
		break;

	case frmSaveEvent:
		UIFieldClear(cKeywordsF1);
		UIFieldClear(cKeywordsF2);
		UIFieldClear(cKeywordsF3);
		UIFieldClear(cKeywordsF4);
		UIFieldClear(cKeywordsF5);
		break;

	case keyDownEvent:
		KeyDownHandler(ev);
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * Redraw
 *
 * Redraw page - usually when scrolling ...
 */
static void
Redraw(void)
{
	FontID fID=FntSetFont(boldFont);
	UInt16 pos=current, i, pW, len, spc;
	RectangleType r, a;
	Boolean fitAll;

	RctSetRectangle(&r, 4, 14, 140, FntCharHeight());
	spc=(2*r.extent.y)+3;

	for (i=0; i<PageSize; i++) {
		WinEraseRectangle(&r, 0);
		FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, i+cKeywordsS1), &a);

		if (pos<keywordsCnt) {
			FntSetFont(boldFont);
			pW=136;
			len=keywords[current+i].len;
			FntCharsInWidth(keywords[current+i].keyword, &pW, &len, &fitAll);
			WinDrawChars(keywords[current+i].keyword, len, r.topLeft.x, r.topLeft.y);
			WinDrawChars(":", 1, r.topLeft.x+pW, r.topLeft.y);
			FntSetFont(fID);

			if (keywords[current+i].value[0])
				UIFieldSetText(i+cKeywordsF1, keywords[current+i].value);
			else
				UIFieldSetText(i+cKeywordsF1, "");

			UIObjectShow(i+cKeywordsF1);
			UIObjectShow(i+cKeywordsS1);
		} else {
			UIObjectHide(i+cKeywordsF1);
			UIObjectHide(i+cKeywordsS1);
		}

		pos++;
		r.topLeft.y+=spc;
	}

	FntSetFont(fID);
	UpdateArrows();
}

/*
 * UpdateArrows
 *
 * Update state of arrows.
 */
static void
UpdateArrows(void)
{
	ControlType *ctl;

	ctl=UIObjectGet(cKeywordsUp);
	if (current<1) {
		CtlSetEnabled(ctl, 0);
		CtlSetLabel(ctl, tArrUpDis);
	} else {
		CtlSetEnabled(ctl, 1);
		CtlSetLabel(ctl, tArrUp);
	}

	ctl=UIObjectGet(cKeywordsDown);
	if ((current+PageSize)>(keywordsCnt-1)) {
		CtlSetEnabled(ctl, 0);
		CtlSetLabel(ctl, tArrDownDis);
	} else {
		CtlSetEnabled(ctl, 1);
		CtlSetLabel(ctl, tArrDown);
	}
}

/*
 * SaveFields
 *
 * Save the contents of the fields to the keyword store.
 */
static void
SaveFields(void)
{
	UInt16 pos=current, idx, len;
	Char *p;

	for (idx=0; idx<PageSize; idx++) {
		keywords[pos].value[0]=0;
		if ((p=UIFieldGetText(cKeywordsF1+idx))!=NULL) {
			if ((len=StrLen(p))>0) {
				MemMove(keywords[pos].value, p, len);
				keywords[pos].value[len]=0;
			}
		}

		pos++;
	}
}

/*
 * Scroll
 *
 * Scroll the page up/down.
 *
 *  -> amount		Amount to scroll
 */
static void
Scroll(Int16 amount)
{
	SaveFields();

	current+=amount;

	if ((current+PageSize)>(keywordsCnt-1))
		current=keywordsCnt-PageSize;

	if (current<0)
		current=0;

	Redraw();
}

/*
 * PageScrollFocus
 *
 * Focus field when a page has been scrolled.
 *
 *  -> dir		Direction.
 */
static void
PageScrollFocus(Int16 dir)
{
	if (!current)
		UIFieldFocus(cKeywordsF1);
	else if ((current+PageSize)==keywordsCnt)
		UIFieldFocus(cKeywordsF5);
	else if (dir<0)
		UIFieldFocus(cKeywordsF4);
	else if (dir>0)
		UIFieldFocus(cKeywordsF2);
}

/*
 * KeyDownHandler
 *
 * Handle key down events on form.
 */
static void
KeyDownHandler(EventType *ev)
{
	WChar chr=ev->data.keyDown.chr;
	Int16 focusIdx;
	UInt16 id;

	if (keywordsCnt<=PageSize) {
		UIFieldRefocus(fKeywordsBand, ev->data.keyDown.chr);
		return;
	}

	if (chr==pageUpChr) {
		Scroll(-(PageSize-1));
		PageScrollFocus(-1);
		return;
	} else if (chr==pageDownChr) {
		Scroll(PageSize-1);
		PageScrollFocus(1);
		return;
	}

	if ((focusIdx=FrmGetFocus(currentForm))==noFocus)
		return;

	id=FrmGetObjectId(currentForm, focusIdx);
	if (id==cKeywordsF1 && chr==prevFieldChr) {
		if (current>0)
			Scroll(-1);

		UIFieldFocus(id);
		return;
	} else if (id==cKeywordsF5 && chr==nextFieldChr) {
		if (current<keywordsCnt)
			Scroll(1);

		UIFieldFocus(id);
		return;
	}

	UIFieldRefocus(fKeywordsBand, chr);
}

/*
 * SetupSystemKeywords
 *
 * Setup system keywords.
 */
static void
SetupSystemKeywords(void)
{
	DateTimeType dt;

	TimSecondsToDateTime(TimGetSeconds(), &dt);
	DateToAscii(dt.month, dt.day, dt.year, PrefGetPreference(prefLongDateFormat), bufDate);
	TimeToAscii(dt.hour, dt.minute, PrefGetPreference(prefTimeFormat), bufTime);
}

/*
 * Keywordize
 *
 * Search and replace keywords in string.
 *
 *  -> src		Source string.
 *  <- dstH		Destination handle.
 *  -> maxLen		Max length.
 */
static void
Keywordize(Char *src, MemHandle *dstH, UInt16 maxLen)
{
	UInt16 cCnt=0;
	MemHandle mh;
	Char *p;

	if (!src)
		return;

	mh=MemHandleNew(maxLen+1);
	ErrFatalDisplayIf(mh==NULL, "(Keywordize) Out of memory");
	MemSet(MemHandleLock(mh), maxLen+1, 0);
	MemHandleUnlock(mh);
	maxLen--;
	*dstH=mh;

	for (p=src; *p; p++) {
		if (*p=='[') {
			if (*(p+1)=='[' || *(p+1)==']')
				continue;

			Copy(mh, maxLen, &cCnt, src, p-src);

			for (src=p+1; *src; src++) {
				if (*src==']' || *src=='=') {
					UInt16 len=src-p-1, i;

					for (i=0; i<keywordsCnt; i++) {
						if (keywords[i].len==len) {
							if (StrNCaselessCompare(keywords[i].keyword, p+1, len)==0) {
								Copy(mh, maxLen, &cCnt, keywords[i].value, StrLen(keywords[i].value));
								break;
							}
						}
					}

					if (*src=='=') {
						for (; *src && *src!=']'; src++);
						if (*src==']')
							src++;
					} else
						src++;

					p=src;
					break;
				}
			}

			if (!*src) {
				Copy(mh, maxLen, &cCnt, p, src-p);
				return;
			}
		}
	}

	if (src<p)
		Copy(mh, maxLen, &cCnt, src, p-src);
}

/*
 * Copy
 *
 * Copy to memhandle.
 *
 *  -> mh		MemHandle.
 *  -> maxLen		Max.
 * <-> cCnt		# of characters in MemHandle.
 *  -> src		Source data.
 *  -> len		Length of source data.
 */
static void
Copy(MemHandle mh, UInt16 maxLen, UInt16 *cCnt, Char *src, UInt16 len)
{
	if ((*cCnt+len>maxLen) || (!len))
		return;

	MemMove(MemHandleLock(mh)+(*cCnt), src, len);
	MemHandleUnlock(mh);
	*cCnt+=len;
}

/**
 * Generate a password.
 *
 * @param src Source string.
 * @param dst Where to store password.
 */
static void
GeneratePassword(Char *src, Char *dst)
{
	UInt16 length=0, flags=0;

	for (; *src!=']' && TxtCharIsSpace(*src); src++);
	for (; *src!=']'; ) {
		if (TxtCharIsDigit(*src)) {
			length=0;
			while (TxtCharIsDigit(*src)) {
				length*=10;
				length+=*src-'0';
				src++;
			}
			for (; *src!=']' && TxtCharIsSpace(*src); src++);
		} else if (TxtCharIsAlpha(*src)) {
			while (TxtCharIsAlpha(*src)) {
				switch (*src) {
				case 'U':
				case 'u':
					flags|=PGUpper;
					break;
				case 'L':
				case 'l':
					flags|=PGLower;
					break;
				case 'D':
				case 'd':
					flags|=PGDigits;
					break;
				case 'S':
				case 's':
					flags|=PGSpecials;
				default:	/* FALL-THRU */
					break;
				}
				src++;
			}
			for (; *src!=']' && TxtCharIsSpace(*src); src++);
		} else
			src++;
	}

	if (!length)
		length=8;
	else if (length>99)
		length=99;

	if (!flags)
		flags=PGLower|PGUpper|PGDigits;

	fPwdGenGenerate(length, flags);
	StrCopy(dst, gPwd);
}
