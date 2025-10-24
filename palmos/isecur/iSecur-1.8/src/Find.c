/*
 * Find.c
 */
#include "Include.h"

/* globals */
extern UInt16 libRefNum;
extern MemHandle aesData;

/**
 * Supporting Global Find. This function performs a search of the
 * events on behalf of the OS. Yeah, yeah. Tap find, enter search
 * string and lets function do it's work.
 *
 * @param fP Arguments from find ...
 * @param inApp Zero if find was launched outside app, !0 if from within.
 */
void
FindSearch(FindParamsPtr fP)
{
	UInt16 recordNum=fP->recordNum, matchPos, sLen, pw;
	UInt32 inApp=0;
	MemHandle mh;
	Boolean fitAll, useThis;
	RectangleType r;
	DB dbo;
	Char *s;
	RecordHdr *h;
	UInt8 *p;

	if (FindDrawHeader(fP, "iSecur")==true)
		return;	/* no more room on screen */

	DBInit(&dbo, DBNameData);
	if (DBOpen(&dbo, dmModeReadOnly, false)==false) {
		/* no database, no search */
		fP->more=false;
		return;
	}

	FtrGet((UInt32)CRID, iSecurFtrHasGlobals, &inApp);

	while (true) {
		/* stop every 16th record if any events pending */
		if ((recordNum&0x000f)==0 && EvtSysEventAvail(true)) {
			fP->more=true;
			break;
		}

		if ((mh=DmQueryNextInCategory(dbo.db, &recordNum, dmAllCategories))==NULL) {
			/* stop if no more records */
			fP->more=false;
			break;
		}

		h=MemHandleLock(mh);
		p=((UInt8 *)h)+sizeof(RecordHdr);
		useThis=false;
		if (h->flags&RHHasTitle) {
			s=p;
			if (FindStrInStr(p, fP->strToFind, &matchPos)==true)
				useThis=true;
		} else
			s="???";

		if ((useThis==false) && (inApp) && (h->flags&RHHasSecret)) {
			UInt32 hLen=MemHandleSize(mh)-(p-(UInt8 *)h);
			UInt8 *e;
			MemHandle rh;

			pw=StrLen(p)+1;
			if (pw&1)
				pw++;

			p+=pw;
			hLen-=pw;

			rh=MemHandleNew(hLen);
			ErrFatalDisplayIf(rh==NULL, "(FindSearch) Out of memory");
			e=MemHandleLock(rh);

			MemMove(e, p, hLen);

			for (pw=0; pw<hLen; pw+=32) {
				AESDecrypt(&e[pw], &e[pw]);
				AESDecrypt(&e[pw+16], &e[pw+16]);
			}

			if (FindStrInStr(e, fP->strToFind, &matchPos)==true)
				useThis=true;

			MemHandleFree(rh);
		}

		if (useThis) {
			FontID fID=stdFont;

			if (FindSaveMatch(fP, recordNum, matchPos, 0, 0, dbo.card, dbo.id)==true) {
				MemHandleUnlock(mh);
				break;
			}
	
			FindGetLineBounds(fP, &r);
			WinEraseRectangle(&r, 0);

			sLen=StrLen(s);
			pw=r.extent.x;
			if (h->flags&RHTemplate)
				fID=FntSetFont(boldFont);

			FntCharsInWidth(s, &pw, &sLen, &fitAll);
			WinDrawChars(s, sLen, r.topLeft.x, r.topLeft.y);
			if (h->flags&RHTemplate)
				FntSetFont(fID);

			fP->lineNumber++;
		}
		recordNum++;
		MemHandleUnlock(mh);
	}

	DBClose(&dbo);
}
