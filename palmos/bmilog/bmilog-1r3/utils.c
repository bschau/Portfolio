/*
**	utils.c
**
**	Various non gui utilities.
*/
#include "bmilog.h"

/*
**	UtilsRomOK
**
**	Check to see if we are on a proper rom version.
**
**	Returns true if rom version is ok, false otherwise.
*/
Boolean
UtilsRomOK(void)
{
	UInt32 romVersion;

	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
	if (romVersion<sysMakeROMVersion(3,0,0,sysROMStageRelease,0)) {
		FrmAlert(aBadRom);
		return false;
	}

	return true;
}

/*
**	UtilsTidyString
**
**	Tidy string. That is, remove any leading and trailing spaces. Convert
**	tabs and other funny characters to spaces. Pack whitespaces.
**
**	 -> src		Source string.
**
**	Returns MemHandle to new string.
*/
MemHandle
UtilsTidyString(Char *src)
{
	UInt32 strLen;
	MemHandle mh;
	UInt8 *start, *p, *s;
	Boolean spaceOk;

	if (src==NULL)
		return NULL;

	strLen=StrLen(src)+1;
	mh=MemHandleNew(strLen);
	ErrFatalDisplayIf(mh==NULL, "(UtilsTidyString) Out of memory.");

	start=MemHandleLock(mh);
	MemMove(start, src, strLen);

	/* run baby, run! */
	s=start;
	for (p=s; *p && *p<=' '; p++);

	if (*p) {
		spaceOk=true;
		for (; *p; p++) {
			if (*p<=' ') {
				if (spaceOk) {
					*s++=' ';
					spaceOk=false;
				}
			} else {
				*s++=*p;
				spaceOk=true;
			}
		}
	}

	*s='\x00';
	for (s--; s>=start && *s<' '; s--)
		*s='\x00';

	MemHandleUnlock(mh);
	return mh;
}

/*
**	UtilsPackDate
**
**	Pack year/month/day into a UInt16.
**
**	 -> year	Year.
**	 -> month	Month.
**	 -> day		Not surprisingly, this is the day!
**
**	Returns the packed date.
*/
UInt16
UtilsPackDate(Int16 year, Int16 month, Int16 day)
{
	UInt16 date;

	if (year<MAGICYEAR)
		year=0;
	else
		year-=MAGICYEAR;

	date=((UInt16)year)<<9;
	date|=((UInt16)month)<<5;
	date|=(UInt16)day;

	return date;
}

/*
**	UtilsUnpackDate
**
**	Extract year/month/day from a UInt16.
**
**	 -> date	Source date.
**	 -> year	Where to store year.
**	 -> month	Where to store month.
**	 -> day		Where to store day.
*/
void
UtilsUnpackDate(UInt16 date, Int16 *year, Int16 *month, Int16 *day)
{
	*year=(date>>9)&0x7f;
	*year+=MAGICYEAR;
	*month=(date>>5)&0x0f;
	*day=date&0x1f;
}

/*
**	UtilsGetHistoryForm
**
**	 -> recIdx	Person to get data from.
**
**	Returns form ID of the history form to use (based on db format).
*/
UInt16
UtilsGetHistoryForm(UInt16 recIdx)
{
	MemHandle mh=DmQueryRecord(dbData.db, recIdx);
	Person *p;
	UInt16 f;

	ErrFatalDisplayIf(mh==NULL, "(UtilsGetHistoryForm) Cannot query record.");
	p=MemHandleLock(mh);
	f=p->format;
	MemHandleUnlock(mh);

	return (f==FormatKgCm) ? fHistoryKgCm : fHistoryLbsInch;
}

/*
**	UtilsCmtoFeetInch
**
**	Convert cm to feet/inch.
**
**	 -> cm		Cm.
**	<-  f		Where to store feet.
**	<-  i		Where to store inch.
*/
void
UtilsCmToFeetInch(UInt32 cm, UInt32 *f, UInt32 *i)
{
	UInt32 r;

	cm*=100;
	r=cm/254;
	if (r*254!=cm)
		r++;

	*f=(r/12);
	*i=(r%12);
}
