/*
 * fKeywords.c
 */
#include "Include.h"

/* macros */
#define PageSize 5

/* protos */
static void KeyDownHandler(EventType *);
static void PageScrollFocus(Int16);
static void UpdateTitle(void);
static void UpdateArrows(void);
static Boolean UpDown(EventType *);
static Boolean ScissorHandler(EventType *);
static void SaveFields(void);
static void Scroll(Int32);
static void Redraw(void);
static void SaveData(void);

/* globals */
static UInt16 focusBand[]={ cKeywordF1, cKeywordF2, cKeywordF3, cKeywordF4, cKeywordF5, 0 };
static Int16 current, keywordCnt;

/*
 * fKeywordsRun
 */
void
fKeywordsRun(UInt16 rIdx)
{
	recordIdx=rIdx;
	StoryGet(recordIdx);
	if ((keywordCnt=StoryGetCount())>0)
		FrmGotoForm(fKeywords);
	else
		fStoryRun(recordIdx);
}

/*
 * fKeywordsInit
 */
Boolean
fKeywordsInit(void)
{
	current=0;
	if (keywordCnt>0)
		UIFieldFocus(cKeywordF1);

	return true;
}

/*
 * fKeywordsEH
 */
Boolean
fKeywordsEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		UpdateTitle();
		Redraw();
		return true;

	case frmSaveEvent:
		SaveData();
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cKeywordsView:
			SaveData();
			if (StoryContainsBlanks()==true) {
				if ((FrmAlert(aMissingKeyword)))
					return true;
			}

			fStoryRun(recordIdx);
			return true;

		case cKeywordsTemplates:
			SaveData();
			FrmGotoForm(fMain);
			return true;
		}
		break;

	case menuEvent:
		if (ev->data.menu.itemID==mKeywordsClear) {
			if (FrmAlert(aClearKeywords)==0) {
				StoryClearKeywords();
				Redraw();
			}

			return true;
		} else if (ev->data.menu.itemID==mKeywordsHelp) {
			fHelpRun("Keyword Help", strKeywordHelpText);
			return true;
		}
		break;

	case penDownEvent:
		if (UpDown(ev)==true)
			return true;

		if (ScissorHandler(ev)==true) {
			Redraw();
			return true;
		}
		break;

	case keyDownEvent:
		KeyDownHandler(ev);
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * KeyDownHandler
 *
 * Handle key down events on form.
 */
static void
KeyDownHandler(EventType *ev)
{
	Int16 focusIdx;
	UInt16 id;

	if (keywordCnt<=PageSize) {
		UIFieldRefocus(focusBand, ev->data.keyDown.chr);
		return;
	}

	if (ev->data.keyDown.chr==pageUpChr) {
		Scroll(-(PageSize-1));
		PageScrollFocus(-1);
		return;
	} else if (ev->data.keyDown.chr==pageDownChr) {
		Scroll(PageSize-1);
		PageScrollFocus(1);
		return;
	}

	if ((focusIdx=FrmGetFocus(currentForm))==noFocus)
		return;

	id=FrmGetObjectId(currentForm, focusIdx);
	if (id==cKeywordF1 && ev->data.keyDown.chr==prevFieldChr) {
		if (current>0)
			Scroll(-1);

		UIFieldFocus(id);
		return;
	} else if (id==cKeywordF5 && ev->data.keyDown.chr==nextFieldChr) {
		if (current<keywordCnt)
			Scroll(1);

		UIFieldFocus(id);
		return;
	}

	UIFieldRefocus(focusBand, ev->data.keyDown.chr);
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
	if (current==0)
		UIFieldFocus(cKeywordF1);
	else if ((current+PageSize)==keywordCnt)
		UIFieldFocus(cKeywordF5);
	else if (dir<0)
		UIFieldFocus(cKeywordF4);
	else if (dir>0)
		UIFieldFocus(cKeywordF2);
}

/*
 * UpdateTitle
 *
 * Write Story Title in upper right corner.
 */
static void
UpdateTitle(void)
{
	RectangleType r;
	MemHandle mh;
	UInt16 len;
	Char *s;

	r.topLeft.x=60;
	r.topLeft.y=1;
	r.extent.x=100;
	r.extent.y=FntCharHeight();
	WinEraseRectangle(&r, 0);

	mh=DBGetRecord(&dbTemplate, recordIdx);
	s=MemHandleLock(mh);
	len=StringGetExact(&s);
	if (len>0) {
		UInt16 pW=100;
		Boolean trunc;

		FntCharsInWidth(s, &pW, &len, &trunc);
		WinDrawChars(s, len, 160-pW, r.topLeft.y);
	}
	MemHandleUnlock(mh);
}

/*
 * UpdateArrows
 *
 * Update the up/down arrows.
 */
static void
UpdateArrows(void)
{
	FontID f=FntSetFont(symbol7Font);
	RectangleType r;
	Char c[2];

	c[1]='\x00';
	FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cKeywordsUp), &r);
	if (current<1)
		*c=3;
	else
		*c=1;

	WinEraseRectangle(&r, 0);
	WinDrawChars(c, 1, r.topLeft.x, r.topLeft.y);

	FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cKeywordsDown), &r);
	if ((current+PageSize)>(keywordCnt-1))
		*c=4;
	else
		*c=2;

	WinEraseRectangle(&r, 0);
	WinDrawChars(c, 1, r.topLeft.x, r.topLeft.y);
	FntSetFont(f);
}

/*
 * UpDown
 *
 * Scroll up/down on gadget tap.
 *
 *  -> ev		Event.
 *
 * Returns true/false.
 */
static Boolean
UpDown(EventType *ev)
{
	RectangleType r;

	FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cKeywordsUp), &r);
	if (RctPtInRectangle(ev->screenX, ev->screenY, &r)) {
		Scroll(-(PageSize-1));
		PageScrollFocus(-1);
		return true;
	}

	FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cKeywordsDown), &r);
	if (RctPtInRectangle(ev->screenX, ev->screenY, &r)) {
		Scroll(PageSize-1);
		PageScrollFocus(1);
		return true;
	}

	return false;
}

/*
 * ScissorHandler
 *
 * Handle potential tap on the scissors.
 *
 *  -> ev		Event.
 *
 * Returns true if a scissor was pressed, false otherwise.
 */
static Boolean
ScissorHandler(EventType *ev)
{
	UInt16 max=(keywordCnt<PageSize) ? keywordCnt : PageSize, idx;
	RectangleType r;

	for (idx=0; idx<max; idx++) {
		FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, idx+cKeywordS1), &r);
		if (RctPtInRectangle(ev->screenX, ev->screenY, &r)) {
			Boolean wasInBounds=true, nowInBounds, penDown;
			Int16 x, y;

			WinInvertRectangle(&r, 0);
			do {
				PenGetPoint(&x, &y, &penDown);
				nowInBounds=RctPtInRectangle(x, y, &r);
				if (nowInBounds!=wasInBounds) {
					WinInvertRectangle(&r, 0);
					wasInBounds=nowInBounds;
				}
			} while (penDown);

			if (wasInBounds)
				WinInvertRectangle(&r, 0);

			if (wasInBounds==false)
				break;

			StorySetValue(idx+current, "");
			idx+=cKeywordF1;
			UIFieldFocus(idx);
			return true;
		}
	}

	return false;
}

/*
 * SaveFields
 *
 * Save text in fields.
 */
static void
SaveFields(void)
{
	UInt16 idx, pos;

	pos=current;
	for (idx=0; idx<PageSize; idx++) {
		StorySetValue(pos, UIFieldGetText(cKeywordF1+idx));
		pos++;
	}
}

/*
 * Scroll
 *
 * Scroll the page up/down.
 *
 *  -> amount		Amount to scroll.
 */
static void
Scroll(Int32 amount)
{
	SaveFields();

	current+=amount;
	if ((current+PageSize)>(keywordCnt-1))
		current=keywordCnt-PageSize;

	if (current<0)
		current=0;

	Redraw();
}

/*
 * Redraw
 *
 * Redraw page - this usually happens when scrolling.
 */
static void
Redraw(void)
{
	FontID fId=FntSetFont(boldFont);
	RectangleType r, a;
	UInt16 i, pos, pW, len, spc;
	Char *s;
	Boolean trunc;
	MemHandle mh;
	BitmapType *scissor;

	r.topLeft.x=0;
	r.topLeft.y=18;
	r.extent.x=160;
	r.extent.y=FntCharHeight();
	spc=(2*r.extent.y)+3;
	pos=current;

	mh=DmGetResource(bitmapRsc, bmpScissor);
	ErrFatalDisplayIf(mh==NULL, "(Redraw) Cannot find Scissor bitmap.");
	scissor=MemHandleLock(mh);

	for (i=0; i<PageSize; i++) {
		WinEraseRectangle(&r, 0);
		FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, i+cKeywordS1), &a);
		if (pos<keywordCnt) {
			if ((s=StoryGetKeyword(current+i))==NULL) {
				s="--- missing ---";
				len=StrLen(s);
			} else
				len=StoryGetKeywordLength(current+i);

			pW=150;
			FntCharsInWidth(s, &pW, &len, &trunc);
			WinDrawChars(s, len, r.topLeft.x, r.topLeft.y);
			WinDrawChars(":", 1, r.topLeft.x+pW, r.topLeft.y);
			if ((s=StoryGetValue(pos)) && (StrLen(s)>0))
				UIFieldSetText(i+cKeywordF1, s);
			else
				UIFieldSetText(i+cKeywordF1, "");

			UIObjectShow(i+cKeywordF1);
			UIObjectShow(i+cKeywordS1);
			WinDrawBitmap(scissor, a.topLeft.x, a.topLeft.y);
		} else {
			UIObjectHide(i+cKeywordF1);
			UIObjectHide(i+cKeywordS1);
			WinEraseRectangle(&a, 0);
		}

		pos++;
		r.topLeft.y+=spc;
	}
	FntSetFont(fId);

	MemHandleUnlock(mh);
	DmReleaseResource(mh);

	UpdateArrows();
}

/*
 * SaveData
 *
 * Save data to preferences block.
 */
static void
SaveData(void)
{
	UInt32 uid;

	SaveFields();

	if (StorySaveKeywords()==true) {
		DmRecordInfo(dbTemplate.db, recordIdx, NULL, &uid, NULL);
		PMSetPref(&uid, sizeof(UInt32), PrfKeywords);
	} else
		PMSetPref(NULL, 0, PrfKeywords);
}
