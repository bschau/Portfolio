/*
 * fReader.c
 */
#include <TxtGlue.h>
#include "Include.h"

/* macros */
#define TitleWidth 80

/* protos */
static void Reposition(UInt16);
static UInt16 GetDisplayLines(void);
static void RenderPage(void);
static void UpdatePopup(void);
static void DLLeft(UInt8 *, UInt16, UInt16);
static void DLCenter(UInt8 *, UInt16, UInt16);
static void DLRight(UInt8 *, UInt16, UInt16);
static void DLJustify(UInt8 *, UInt16, UInt16);
static void PrevPage(UInt16);
static void NextPage(UInt16);
static void SaveData(void);
static Boolean BMAdd(void);
static Boolean BMEH(EventType *);
static void YadrFind(void);
static void YadrFindNext(void);
static void GoTo(void);
static Boolean GTEH(EventType *);

/* globals */
static Char formTitle[DocNameLength], txtPopup[16], bmName[BookmarkLength];
static MemHandle floatH, bmStringH;
static UInt32 floatSize, lastCharPos, secondLinePos, historyUID;
static Boolean eopSeen;

/*
 * fReaderAlloc
 *
 * Do first time allocations.
 */
void
fReaderAlloc(void)
{
	floatH=NULL;
	floatSize=0;
	bmStringH=NULL;
}

/*
 * fReaderFree
 *
 * De-alloc reader.
 */
void
fReaderFree(void)
{
	if (bmStringH)
		MemHandleFree(bmStringH);

	if (floatH)
		MemHandleFree(floatH);
}

/*
 * fReaderRun
 *
 * Launch the fReader or fReaderFullScreen form.
 */
void
fReaderRun(void)
{
	if (prefs.flags&PFFullScreen)
		FrmGotoForm(fReaderFullScreen);
	else
		FrmGotoForm(fReader);
}

/*
 * fReaderInit
 */
Boolean
fReaderInit(void)
{
	UInt16 width=TitleWidth, len;
	Boolean fit;

	HistoryAdd(doc->title, &historyUID);
	eopSeen=false;

	if (!(prefs.flags&PFFullScreen)) {
		MemMove(formTitle, doc->title, DocNameLength);

		len=StrLen(formTitle);
		FntCharsInWidth(formTitle, &width, &len, &fit);
		formTitle[len]=0;
		if (!fit) {
			if (len>3) {
				formTitle[--len]='.';
				formTitle[--len]='.';
				formTitle[--len]='.';
			}
		}

		FrmSetTitle(currentForm, formTitle);
	}

	secondLinePos=0;
	lastCharPos=0;

	if (!(prefs.flags&PFFullScreen)) {
		MemHandle mh;

		UpdatePopup();
		mh=BookmarksBuildList(doc->title, cReaderBookmarksList, true, BookmarksListLines, sbBookmarks);
		if (bmStringH)
			MemHandleFree(bmStringH);

		bmStringH=mh;
	}
	return true;
}

/*
 * fReaderEH
 */
Boolean
fReaderEH(EventType *ev)
{
	RectangleType r;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		RenderPage();
		return true;

	case frmCloseEvent:
		SaveData();
		break;

	case popSelectEvent:
		switch (ev->data.popSelect.listID) {
		case cReaderList:
			if (ev->data.popSelect.selection!=noListSelection)
				Reposition(ev->data.popSelect.selection);

			return true;

		case cReaderBookmarksList:
			if (!(ev->data.popSelect.selection)) {
				if (BMAdd()==true) {
					MemHandle mh;

					BookmarksAdd(doc->title, doc->position, bmName);
					mh=BookmarksBuildList(doc->title, cReaderBookmarksList, true, BookmarksListLines, sbBookmarks);
					if (bmStringH)
						MemHandleFree(bmStringH);

					bmStringH=mh;
				}
			} else {
				if (ev->data.popSelect.selection!=noListSelection) {
					doc->position=BookmarksGetPosition(doc->title, ev->data.popSelect.selection-1);
					RenderPage();
				}
			}

			if (!(prefs.flags&PFFullScreen))
				CtlDrawControl(UIObjectGet(cReaderBookmarksPopup));
			return true;
		}
		break;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mReaderDocList:
			FrmGotoForm(fMain);
			return true;

		case mReaderFormatting:
			FrmGotoForm(fFormatting);
			return true;

		case mReaderManage:
			fBookmarksRun(currentFormID);
			return true;

		case mReaderFind:
			YadrFind();
			return true;

		case mReaderFindNext:
			YadrFindNext();
			return true;

		case mReaderAdd:
			if (BMAdd()==true) {
				BookmarksAdd(doc->title, doc->position, bmName);
				if (!(prefs.flags&PFFullScreen)) {
					MemHandle mh;

					mh=BookmarksBuildList(doc->title, cReaderBookmarksList, true, BookmarksListLines, sbBookmarks);
					if (bmStringH)
						MemHandleFree(bmStringH);

					bmStringH=mh;
				}
			}
			return true;

		case mReaderGoTo:
			GoTo();
			RenderPage();
			return true;

		case mReaderTop:
		case mReader10:
		case mReader20:
		case mReader30:
		case mReader40:
		case mReader50:
		case mReader60:
		case mReader70:
		case mReader80:
		case mReader90:
			Reposition(ev->data.menu.itemID-mReaderTop);
			return true;
		}
		break;

	case penDownEvent:
		if (prefs.flags&PFFullScreen) {
			RctSetRectangle(&r, 0, 0, 160, 15);

			if (RctPtInRectangle(ev->screenX, ev->screenY, &r)) {
				EventType ev;

				MemSet(&ev, sizeof(EventType), 0);
				ev.eType=keyDownEvent;
				ev.data.keyDown.chr=vchrMenu;
				EvtAddEventToQueue(&ev);
				return true;
			}
		}

		FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cReaderControl), &r);
		if (RctPtInRectangle(ev->screenX, ev->screenY, &r)) {
			UInt16 c;
			
			if (prefs.controlMode==PCMUpDown)
				c=(ev->screenY-r.topLeft.y)/36;
			else
				c=(ev->screenX-r.topLeft.x)/40;

			switch (c) {
			case 0:
				PrevPage(GetDisplayLines());
				break;
			case 1:
				PrevPage(1);
				break;
			case 2:
				NextPage(1);
				break;
			default:
				NextPage(GetDisplayLines());
				break;
			}
			return true;
		}
		break;

	case lstExitEvent:
		if (!(prefs.flags&PFFullScreen)) {
			if (ev->data.lstExit.listID==cReaderBookmarksList) {
				CtlDrawControl(UIObjectGet(cReaderBookmarksPopup));
				return true;
			}	
		}
		break;

	case yadrFindEvent:
		YadrFind();
		return true;

	case keyDownEvent:
		if (ev->data.keyDown.chr==pageUpChr) {
			PrevPage(GetDisplayLines());
			return true;
		} else if (ev->data.keyDown.chr==pageDownChr) {
			NextPage(GetDisplayLines());
			return true;
		} else if ((ev->data.keyDown.chr&~32)=='N') {
			YadrFindNext();
			return true;
		}
	default: /* FALL-THRU */
		break;
	}

	return false;
}

/*
 * Reposition
 *
 * Reposition top ... 90%
 *
 *  -> pos		Position 0-9.
 */
static void
Reposition(UInt16 pos)
{
	/* if doc->length is large this will overflow. */
	doc->position=pos*doc->length;
	if (doc->position%10)
		doc->position+=10;

	doc->position/=10;
	RenderPage();
}

/*
 * GetDisplayLines
 *
 * Calculate number of lines to display in the current font.
 */
static UInt16
GetDisplayLines(void)
{
	FontID fID=FntSetFont(prefs.font);
	UInt16 l;

	if (prefs.flags&PFFullScreen)
		l=dispH;
	else
		l=dispH-TopY;

	l/=(FntCharHeight()+prefs.lineSpacing);
	FntSetFont(fID);

	return l;
}

/*
 * RenderPage
 *
 * Draw a page of text.
 */
static void
RenderPage()
{
	UInt32 savePos=doc->position, dispL=GetDisplayLines(), l;
	FontID fID=FntSetFont(prefs.font);
	IndexedColorType saveText=WinSetTextColor(prefs.foreColor);
	IndexedColorType saveBack=WinSetBackColor(prefs.backColor);
	UInt16 y, x, pW, cnt, fontH;
	Boolean lineComplete;
	RectangleType r;
	UInt8 *p;

	if (prefs.flags&PFFullScreen)
		y=0;
	else
		y=TopY;

	fontH=FntCharHeight()+prefs.lineSpacing;
	RctSetRectangle(&r, 0, y, dispW, dispH-y);
	WinEraseRectangle(&r, 0);

	for (l=0; l<dispL; l++) {
		if (y+fontH>dispH)
			break;

		if (l==1)
			secondLinePos=doc->position;

		pW=dispW;
		x=0;
		SBReset(sbReader);
		do {
			if (DocGetLine(doc, pW, &cnt, &lineComplete)==false)
				break;

			if (!cnt) {
				doc->position++;
				continue;
			}

			p=doc->page+doc->posInPage;

			SBAppend(sbReader, p, cnt);
			cnt=FntCharsWidth(p, cnt);
			if (cnt>=pW)
				break;

			x+=cnt;
			pW-=cnt;
		} while (!lineComplete);

		p=SBGetData(sbReader, &cnt);
		if (cnt>0) {
			Boolean seenTab=false;
			Char *s=p;
			UInt16 c;
#if DEBUG
			UInt16 i, t;
			Char buffer[51];

			t=(cnt > sizeof(buffer)-1) ? sizeof(buffer)-1 : cnt;
			for (i=0; i<t; i++) {
				if (p[i])
					buffer[i]=p[i];
				else
					break;
			}
			buffer[i]=0;

			DPrint("   '%s'\n", buffer);
#endif

			for (c=0; c<cnt; c++) {
				if (*s=='\t') {
					seenTab=true;
					break;
				}
				s++;
			}

			if (seenTab)
				DLLeft(p, cnt, y);
			else {
				switch (prefs.drawMode) {
				case PDMLeft:
					DLLeft(p, cnt, y);
					break;
				case PDMCenter:
					DLCenter(p, cnt, y);
					break;
				case PDMRight:
					DLRight(p, cnt, y);
					break;
				case PDMJustify:
					DLJustify(p, cnt, y);
					break;
				}
			}
		}

		y+=fontH;
	}

	DPrint("Page Rendering done\n\n");
	WinSetTextColor(saveText);
	WinSetBackColor(saveBack);
	FntSetFont(fID);

	lastCharPos=doc->position;
	doc->position=savePos;

	UpdatePopup();

	if (lastCharPos>=doc->length)
		eopSeen=true;
}

/*
 * UpdatePopup
 *
 * Update the popup (and list) values.
 */
static void
UpdatePopup(void)
{
	UInt16 pct;

	if (!(prefs.flags&PFFullScreen)) {
		if (doc->length)
			pct=(UInt16)((doc->position*100)/doc->length);
		else
			pct=0;

		LstSetSelection(UIObjectGet(cReaderList), (pct/10));

		if (pct)
			StrPrintF(txtPopup, "%u%%", pct);
		else
			StrPrintF(txtPopup, "Top");

		CtlSetLabel(UIObjectGet(cReaderPopup), txtPopup);
	}
}

/*
 * DLLeft
 *
 * Draw a line, left aligned.
 * 
 *  -> src		Source string.
 *  -> len		Length of source.
 *  -> y		Y position.
 */
static void
DLLeft(UInt8 *src, UInt16 len, UInt16 y)
{
	UInt16 cPos=0, x=0, sow, cnt;

	while (cPos<len) {
		if (src[cPos]=='\n' || src[cPos]=='\r')
			break;
		if (src[cPos]=='\t') {
			x+=(fntTabChrWidth-(x%fntTabChrWidth));
			cPos++;
		} else if (src[cPos]==' ') {
			x+=FntCharWidth(' ');
			cPos++;
		} else {
			sow=cPos;
			while (cPos<len) {
				if (src[cPos]<=' ')
					break;

				cPos++;
			}

			if ((cnt=cPos-sow)>0) {
				WinDrawChars(src+sow, cnt, x, y);
				x+=FntCharsWidth(src+sow, cnt);
			}
		}
	}
}

/*
 * DLCenter
 *
 * Draw a line, centered.
 * 
 *  -> src		Source string.
 *  -> len		Length of source.
 *  -> y		Y position.
 */
static void
DLCenter(UInt8 *src, UInt16 len, UInt16 y)
{
	UInt16 cPos;

	/* remove trailing whitespace */
	while (--len>0) {
		if (src[len]>=' ') {
			len++;
			break;
		}
	}

	/* remove leading white space */
	for (cPos=0; cPos<len; cPos++) {
		if (src[cPos]>' ')
			break;
	}

	if (cPos==len)
		return;

	len-=cPos;
	WinDrawChars(src+cPos, len, (dispW-FntCharsWidth(src+cPos, len))/2, y);
}

/*
 * DLRight
 *
 * Draw a line, right aligned.
 * 
 *  -> src		Source string.
 *  -> len		Length of source.
 *  -> y		Y position.
 */
static void
DLRight(UInt8 *src, UInt16 len, UInt16 y)
{
	UInt16 cPos;

	while (--len>0) {
		if (src[len]>' ') {
			len++;
			break;
		}
	}

	/* remove leading white space */
	for (cPos=0; cPos<len; cPos++) {
		if (src[cPos]!=' ')
			break;
	}

	if (cPos==len)
		return;

	len-=cPos;
	WinDrawChars(src+cPos, len, dispW-FntCharsWidth(src+cPos, len), y);
}

/*
 * DLJustify
 *
 * Draw a line, justified.
 * 
 *  -> src		Source string.
 *  -> len		Length of source.
 *  -> y		Y position.
 */
static void
DLJustify(UInt8 *src, UInt16 len, UInt16 y)
{
	UInt16 words=0, strWidth=0, x=0, cPos, tPos, sow, cnt, kern, xtra;

	/* If line ends in \r | \n then just draw it. */
	switch (src[len-1]) {
	case '\r':
	case '\n':
		len--;
		if (len>0)
			WinDrawChars(src, len, 0, y);
		return;
	case ' ':
		/* remove trailing whitespace */
		while (--len>0) {
			if (src[len]>' ') {
				len++;
				break;
			}
		}
		break;
	}

	/* remove leading white space */
	for (cPos=0; cPos<len; cPos++) {
		if (src[cPos]>' ')
			break;
	}

	if (cPos==len)
		return;

	/* count words */
	for (tPos=cPos; tPos<len; ) {
		sow=tPos;
		while (tPos<len) {
			if (src[tPos]<=' ')
				break;

			tPos++;
		}

		if ((cnt=tPos-sow)>0) {
			strWidth+=FntCharsWidth(src+sow, cnt);
			words++;
		}

		while (src[tPos]<=' ' && tPos<len)
			tPos++;
	}

	/* one word? */
	if (words==1) {
		WinDrawChars(src+cPos, len-cPos, 0, y);
		return;
	}

	kern=(dispW-strWidth)/(words-1);
	xtra=(dispW-strWidth)%(words-1);

	/* ok, draw the line ... */
	for (tPos=cPos; tPos<len; ) {
		sow=tPos;
		while (tPos<len) {
			if (src[tPos]<=' ')
				break;

			tPos++;
		}

		if ((cnt=tPos-sow)>0) {
			WinDrawChars(src+sow, cnt, x, y);
			x+=FntCharsWidth(src+sow, cnt)+kern;
			if (xtra) {
				x++;
				xtra--;
			}
		}

		while (src[tPos]<=' ' && tPos<len)
			tPos++;
	}
}

/*
 * PrevPage
 *
 * Go to previous page.
 *
 *  -> linesToScroll	Lines to scroll.
 */
static void
PrevPage(UInt16 linesToScroll)
{
	if (doc->position>0) {
		UInt32 oldPos;
		UInt16 page, lts, charPos;

		DocPagePos(doc, doc->position, &page, &oldPos);

		lts=linesToScroll;
		if (!oldPos) {
			if (doc->pageNo==1) {
				doc->position=0;
				RenderPage();
				return;
			}

			DocGetPage(doc, doc->pageNo-1);

			doc->position--;
			DocPagePos(doc, doc->position, &page, &oldPos);
		} else {
			if (doc->pageNo!=page)
				DocGetPage(doc, page);
		}

		charPos=(UInt16)oldPos;
		FntWordWrapReverseNLines((Char *)doc->page, dispW, &lts, &charPos);
		if (lts==linesToScroll)
			doc->position-=oldPos-charPos;
		else if (charPos==0 && doc->pageNo>1) {
			UInt16 pageSize=doc->pageLengths[doc->pageNo-1];
			UInt32 size=oldPos+1;
			Char *p;

			size+=(UInt32)pageSize;
			if (size>floatSize) {
				if (floatH) {
					MemHandleFree(floatH);
					floatH=NULL;
				}

				floatSize=size;
				floatH=MemHandleNew(floatSize);
				ErrFatalDisplayIf(floatH==NULL, "(PrevPage) Out of memory.");
			}

			p=MemHandleLock(floatH);
			MemMove(p+pageSize, doc->page, oldPos);
			*(p+pageSize+oldPos)=0;
			DocGetPage(doc, doc->pageNo-1);
			MemMove(p, doc->page, pageSize);

			oldPos+=pageSize;
			charPos=oldPos;
			lts=linesToScroll;
			FntWordWrapReverseNLines(p, dispW, &lts, &charPos);
			doc->position-=oldPos-charPos;
			MemHandleUnlock(floatH);
		} else
			doc->position=0;

		RenderPage();
	}
}

/*
 * NextPage
 *
 * Go to next page.
 *
 *  -> linesToScroll		Lines to scroll.
 */
static void
NextPage(UInt16 linesToScroll)
{
	if (lastCharPos<doc->length) {
		if (linesToScroll==1)
			doc->position=secondLinePos;
		else
			doc->position=lastCharPos;

		RenderPage();
	}
}

/*
 * SaveData
 *
 * Save document position/name.
 */
static void
SaveData(void)
{
	UInt32 len=sizeof(PrfSaveDoc), nLen=StrLen(doc->db.name);
	MemHandle mh;
	PrfSaveDoc *s;
	Char *p;

	len+=1+nLen;
	if (doc->db.path)
		len+=1+StrLen(doc->db.path);

	mh=MemHandleNew(len);
	ErrFatalDisplayIf(mh==NULL, "(SaveData) Out of memory");
	s=MemHandleLock(mh);
	MemSet((Char *)s, len, 0);

	s->position=doc->position;
	p=((Char *)s)+sizeof(PrfSaveDoc);
	StrCopy(p, doc->db.name);
	if (doc->db.path) {
		p+=nLen+1;
		StrCopy(p, doc->db.path);
		s->vfs=1;
	}

	PMSetPref((Char *)s, len, PrfDocument);

	MemHandleFree(mh);

	if (eopSeen)
		HistoryMarkAsRead(historyUID);
}

/*
 * BMAdd
 *
 * Show the "Add bookmark" dialog.
 *
 * Returns true if the bookmark should be added, false otherwise.
 */
static Boolean
BMAdd(void)
{
	FormSave frm;

	UIFormPrologue(&frm, fAddBookmark, BMEH);

	UIFieldSelectAll(cAddBookmarkName, "New bookmark", true);

	return UIFormEpilogue(&frm, NULL, cAddBookmarkCancel);
}

/*
 * BMEH
 */
static Boolean
BMEH(EventType *ev)
{
	if (ev->eType==ctlSelectEvent && ev->data.ctlSelect.controlID==cAddBookmarkOK) {
		Char *p=UIFieldGetText(cAddBookmarkName);

		MemSet(bmName, BookmarkLength, 0);
		StrCopy(bmName, p);
	}

	return false;
}

/*
 * YadrFind
 *
 * Handle find and find next.
 */
static void
YadrFind(void)
{
	UInt32 goToPos;

	if (fFindRun(&goToPos, doc)==true) {
		doc->position=goToPos;
		RenderPage();
	}
}

/*
 * YadrFindNext
 *
 * Handle find next.
 */
static void
YadrFindNext(void)
{
	if (*findTerm) {
		UInt32 goToPos;

		if (fFindNext(&goToPos, doc)==true) {
			doc->position=goToPos;
			RenderPage();
		}
	}
}

/*
 * GoTo
 *
 * Show the Go To Bookmark form.
 */
static void
GoTo(void)
{
	FormSave frm;
	MemHandle mh;

	UIFormPrologue(&frm, fGoTo, GTEH);
	
	mh=BookmarksBuildList(doc->title, cGoToList, false, GoToListLines, sbGoTo);
	LstDrawList(UIObjectGet(cGoToList));

	UIFormEpilogue(&frm, NULL, cGoToCancel);
	MemHandleFree(mh);
}

/*
 * GTEH
 */
static Boolean
GTEH(EventType *ev)
{
	if (ev->eType==lstSelectEvent && ev->data.lstSelect.listID==cGoToList) {
		if (ev->data.lstSelect.selection!=noListSelection) {
			doc->position=BookmarksGetPosition(doc->title, ev->data.lstSelect.selection);
			CtlHitControl(UIObjectGet(cGoToCancel));
		}
	}

	return false;
}
