/*
 * Find.c
 */
#include "Include.h"

/*
 * FSearch
 *
 * Supporting Global Find. This function performs a search of the
 * events on behalf of the OS. Yeah, yeah. Tap find, enter search
 * string and let function do it's work.
 *
 *  -> fP		Arguments from find ...
 */
void
FSearch(FindParamsType *fP)
{
	UInt16 recordNum=fP->recordNum, matchPos, sLen, pw;
	MemHandle mh=NULL;
	Boolean trunc;
	RectangleType r;
	Char buffer[8];
	DB dbo;
	TTask *t;
	Char *s;

	if (FindDrawHeader(fP, "@Tracker")==true)
		return;	/* no more room on screen */

	DBInit(&dbo, DBNAME);
	if (DBOpen(&dbo, dmModeReadOnly, false)==false) {
		/* no database, no search */
		fP->more=false;
		return;
	}

	while (true) {
		/* stop every 16th record if any events pending */
		if ((recordNum&0x000f)==0 && EvtSysEventAvail(true)) {
			fP->more=true;
			break;
		}

		if (mh)
			MemHandleUnlock(mh);
	
		if ((mh=DmQueryNextInCategory(dbo.db, &recordNum, dmAllCategories))==NULL) {
			/* stop if no more records */
			fP->more=false;
			break;
		}

		t=MemHandleLock(mh);
		s=(UInt8 *)t+sizeof(TTask);
		if (*s) {
			if (FindStrInStr(s, fP->strToFind, &matchPos)==true) {
				if (FindSaveMatch(fP, recordNum, matchPos, 0, 0, dbo.card, dbo.id)==true) /* no more room in the find param block */
					break;

				FindGetLineBounds(fP, &r);
				WinEraseRectangle(&r, 0);

				DTDateFormatShort(buffer, t->date);
				sLen=StrLen(buffer);
				pw=DATEFIELDWIDTH;
				FntCharsInWidth(buffer, &pw, &sLen, &trunc);
				WinDrawChars(buffer, sLen, r.topLeft.x+DATEFIELDWIDTH-pw, r.topLeft.y);

				sLen=StrLen(s);
				pw=r.extent.x-DATEFIELDWIDTH-SPACING;
				FntCharsInWidth(s, &pw, &sLen, &trunc);
				WinDrawChars(s, sLen, r.topLeft.x+DATEFIELDWIDTH+SPACING, r.topLeft.y);

				fP->lineNumber++;
			}
			recordNum++;
		}
	}

	if (mh)
		MemHandleUnlock(mh);

	DBClose(&dbo);
}
