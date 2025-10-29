/*
**	utils.c
**
**	Various non gui utilities.
*/
#include "eventor.h"

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
	UInt16 date=0;

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
	*month=(date>>5)&0x0f;
	*day=date&0x1f;
}

/*
**	UtilsDateFormat
**
**	Get users preferred date settings.
**
**	 -> f		Where to store format.
**	 -> s		Where to store separator.
*/
void
UtilsDateFormat(UInt16 *f, Char *s)
{
	switch (PrefGetPreference(prefDateFormat)) {
	case dfMDYWithSlashes:
	case dfYMDWithSlashes:
		*f=DateFormatMD;
		*s='/';
		break;
	case dfDMYWithDots:
		*f=DateFormatDM;
		*s='.';
		break;
	case dfDMYWithDashes:
		*f=DateFormatDM;
		*s='-';
		break;
	case dfYMDWithDots:
		*f=DateFormatMD;
		*s='.';
		break;
	case dfYMDWithDashes:
		*f=DateFormatMD;
		*s='-';
		break;
	case dfDMYWithSlashes:
	default:	/* FALL-THRU */
		*f=DateFormatDM;
		*s='/';
		break;
	}
}

/*
**	UtilsFormatDate
**
**	Format date depending on users preference.
**
**	 -> dst		Where to store formatted date.
**	 -> year	Year.
**	 -> month	Month.
**	 -> day		Day.
*/
void
UtilsFormatDate(Char *dst, Int16 year, Int16 month, Int16 day)
{
	switch (PrefGetPreference(prefDateFormat)) {
	case dfMDYWithSlashes:
		StrPrintF(dst, "%d-%d-%d", month, day, year);
		break;
	case dfDMYWithSlashes:
		StrPrintF(dst, "%d/%d/%d", day, month, year);
		break;
	case dfDMYWithDots:
		StrPrintF(dst, "%d.%d.%d", day, month, year);
		break;
	case dfDMYWithDashes:
		StrPrintF(dst, "%d-%d-%d", day, month, year);
		break;
	case dfYMDWithDots:
		StrPrintF(dst, "%d.%d.%d", year, month, day);
		break;
	case dfYMDWithDashes:
		StrPrintF(dst, "%d-%d-%d", year, month, day);
		break;
	case dfMDYWithDashes:
		StrPrintF(dst, "%d-%d-%d", month, day, year);
		break;
	case dfYMDWithSlashes:
	default:	/* Fall-Thru */
		StrPrintF(dst, "%d/%d/%d", year, month, day);
		break;
	}

	*(dst+10)='\x00';
}

/*
**	UtilsGetShortDate
**
**	Format the short form of the date to dst.
**
**	 -> dst		Where to store string.
**	 -> date	Date.
**	 -> f		Date format.
**	 -> s		Date separator.
*/
void
UtilsGetShortDate(Char *dst, UInt16 date, UInt16 f, Char s)
{
	Int16 year, month, day;
	
	UtilsUnpackDate(date, &year, &month, &day);
	if (f==DateFormatMD)
		StrPrintF(dst, "%d%c%d", month, s, day);
	else
		StrPrintF(dst, "%d%c%d", day, s, month);
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
	UInt32 strLen=StrLen(src);
	MemHandle mh;
	UInt8 *start, *p, *s;
	Boolean spaceOk;

	if (src==NULL || strLen==0)
		return NULL;

	strLen++;
	mh=MemHandleNew(strLen);
	ErrFatalDisplayIf(mh==NULL, "(UtilsTidyString) Out of memory.");

	start=MemHandleLock(mh);
	MemMove(start, src, strLen);

	/* run baby, run! */
	s=start;
	for (p=s; *p && TxtGlueCharIsSpace(*p); p++);

	if (*p) {
		spaceOk=true;
		for (; *p; p++) {
			if (TxtGlueCharIsSpace(*p)) {
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
	for (s--; s>=start && !TxtGlueCharIsGraph(*s); s--)
		*s='\x00';

	MemHandleUnlock(mh);
	return mh;
}

/*
**	UtilsSetNotification
**
**	Set/reset notification broadcast.
**
**	 -> reg		Register notifcation if true, unregister if false.
**	 -> event	Notification event.
*/
void
UtilsSetNotification(Boolean reg, UInt32 event)
{
	UInt32 notifyMgr=0;
	LocalID lid;
	UInt16 card;

	FtrGet(sysFtrCreator, sysFtrNumNotifyMgrVersion, &notifyMgr);
	if (notifyMgr) {
		if (SysCurAppDatabase(&card, &lid)==errNone) {
			if (reg==true)
				SysNotifyRegister(card, lid, event, NULL, sysNotifyNormalPriority, 0);
			else
				SysNotifyUnregister(card, lid, event, sysNotifyNormalPriority);
		}
	}
}

/*
**	UtilsGetRomVersion
**
**	Retrieves Rom Version.
**
**	 -> dst		Where to store Rom Version (or NULL if doesn't care).
**	 -> required	Required Rom Version.
**
**	Returns true if current Rom Version is equal or better to required
**	Rom Version. False is returned otherwise.
*/
Boolean
UtilsGetRomVersion(UInt32 *dst, UInt32 required)
{
	UInt32 romVersion;

	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);

	if (dst)
		*dst=romVersion;

	return ((romVersion<required) ? false : true);
}

/*
**	UtilsLockString
**
**	Lock a string resource given it's id.
*/
MemHandle
UtilsLockString(UInt16 id)
{
	MemHandle mh;

	mh=DmGetResource(strRsc, id);
	ErrFatalDisplayIf(mh==NULL, "(UtilsLockString) Cannot locate string.");

	return mh;
}

/*
**	UtilsUnlockString
**
**	Unlock a string gotten from UtilsLockString.
**
**	 -> mh		MemHandle.
*/
void
UtilsUnlockString(MemHandle mh)
{
	MemHandleUnlock(mh);
	DmReleaseResource(mh);
}
