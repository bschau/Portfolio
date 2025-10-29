#include "bmilog.h"

/*
**	FSearch
**
**	Supporting Global Find. This function performs a search of the
**	events on behalf of the OS. Yeah, yeah. Tap find, enter search
**	string and let function do it's work.
**
**	 -> findParams	Arguments from find ...
*/
void
FSearch(FindParamsType *findParams)
{
	UInt16 recordNum=findParams->recordNum, matchPos, sLen, pw;
	Boolean trunc;
	MemHandle mh, bmh;
	RectangleType r;
	Person *rp;
	Char buffer[128];
	DB dbo;
	DmOpenRef db;
	BMI *b;
	Preferences pr;

	if (FindDrawHeader(findParams, "BMI Log")==true)
		return;	/* no more room on screen */

	DBInit(&dbo, DBNAME);
	if (DBOpenDatabase(&dbo, dmModeReadOnly, NULL)==false) {
		/* no database, no search */
		findParams->more=false;
		return;
	}

	PreferencesGet(&pr);
	db=dbo.db;
	for (EVER) {
		/* stop every 16th record if any events pending */
		if ((recordNum&0x000f)==0 && EvtSysEventAvail(true)) {
			findParams->more=true;
			break;
		}

		if ((mh=DmQueryNextInCategory(db, &recordNum, dmAllCategories))==NULL) {
			/* stop if no more records */
			findParams->more=false;
			break;
		}

		rp=MemHandleLock(mh);
		if (FindStrInStr(rp->name, findParams->strToFind, &matchPos)==true) {
			if (FindSaveMatch(findParams, recordNum, matchPos, 0, 0, dbo.card, dbo.id)==true) {
				/* no more room in the find param block */
				MemHandleUnlock(mh);
				break;
			}

			pw=0;
			if (BmiFirst(&dbo, rp->r.id, &pw)==true) {
				if ((bmh=DmQueryRecord(db, pw))!=NULL) {
					b=MemHandleLock(bmh);
					StrPrintF(buffer, "(%u) ", BmiCalculate(pr.bmiTable, b->height, b->weight));
					MemHandleUnlock(bmh);
				}
			}
			StrNCat(buffer, rp->name, sizeof(buffer));

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
