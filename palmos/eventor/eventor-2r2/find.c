#include "eventor.h"

/*
**	FSearch
**
**	Supporting Global Find. This function performs a search of the
**	events on behalf of the OS. Yeah, yeah. Tap find, enter search
**	string and lets function do it's work.
**
**	 -> findParams	Arguments from find ...
*/
void
FSearch(FindParamsPtr findParams)
{
	UInt16 recordNum=findParams->recordNum, matchPos, sLen, pw, dF;
	Char dS;
	Boolean trunc;
	MemHandle mh;
	RectangleType r;
	Char buffer[128];
	DB dbo;
	TEvent *e;

	if (FindDrawHeader(findParams, "Eventor")==true)
		return;	/* no more room on screen */

	DBInit(&dbo, DBNAME);
	if (DBOpen(&dbo, dmModeReadOnly, NULL)==false) {
		/* no database, no search */
		findParams->more=false;
		return;
	}

	UtilsDateFormat(&dF, &dS);
	while (true) {
		/* stop every 16th record if any events pending */
		if ((recordNum&0x000f)==0 && EvtSysEventAvail(true)) {
			findParams->more=true;
			break;
		}

		if ((mh=DmQueryNextInCategory(dbo.db, &recordNum, dmAllCategories))==NULL) {
			/* stop if no more records */
			findParams->more=false;
			break;
		}

		e=MemHandleLock(mh);
		if (FindStrInStr(e->event, findParams->strToFind, &matchPos)==true) {
			if (FindSaveMatch(findParams, recordNum, matchPos, 0, 0, dbo.card, dbo.id)==true) {
				/* no more room in the find param block */
				MemHandleUnlock(mh);
				break;
			}

			UtilsGetShortDate(buffer, e->date, dF, dS);
			StrCat(buffer, " ");
			StrCat(buffer, e->event);

			FindGetLineBounds(findParams, &r);
			WinEraseRectangle(&r, 0);
			
			sLen=StrLen(buffer);
			pw=r.extent.x;
			FntCharsInWidth(buffer, &pw, &sLen, &trunc);
			WinDrawChars(buffer, sLen, r.topLeft.x, r.topLeft.y);
			findParams->lineNumber++;
		}
		MemHandleUnlock(mh);
		recordNum++;
	}
	DBClose(&dbo);
}
