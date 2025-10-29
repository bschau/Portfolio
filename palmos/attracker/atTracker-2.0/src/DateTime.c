/*
 * DateTime.c
 */
#include "Include.h"

/*
 * DTToday
 *
 * Return todays date.
 */
UInt16
DTToday(void)
{
	UInt32 secs=TimGetSeconds();
	DateTimeType dt;

	TimSecondsToDateTime(secs, &dt);

	return DTDatePack(dt.year, dt.month, dt.day);
}

/*
 * DTDatePack
 *
 * Pack year/month/day into a UInt16.
 *
 *  -> year		Year.
 *  -> month		Month.
 *  -> day		Not surprisingly, this is the day!
 *
 * Returns the packed date.
 */
UInt16
DTDatePack(Int16 year, Int16 month, Int16 day)
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
 * DTDateUnpack
 *
 * Extract year/month/day from a UInt16.
 *
 *  -> date		Source date.
 *  -> year		Where to store year.
 *  -> month		Where to store month.
 *  -> day		Where to store day.
 */
void
DTDateUnpack(UInt16 date, Int16 *year, Int16 *month, Int16 *day)
{
	*year=(date>>9)&0x7f;
	*year+=MAGICYEAR;
	*month=(date>>5)&0x0f;
	*day=date&0x1f;
}

/*
 * DTDateFormatLong
 *
 * Format date depending on users preference.
 *
 *  -> dst		Where to store formatted date.
 *  -> date		Date.
 */
void
DTDateFormatLong(Char *dst, UInt16 date)
{
	UInt16 year, month, day;

	DTDateUnpack(date, &year, &month, &day);
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
}

/*
 * UtilsDateFormatShort
 *
 * Format date depending on users preference.
 *
 *  -> dst		Where to store formatted date.
 *  -> date		Date.
 */
void
DTDateFormatShort(Char *dst, UInt16 date)
{
	UInt16 year, month, day;

	DTDateUnpack(date, &year, &month, &day);
	switch (PrefGetPreference(prefDateFormat)) {
	case dfDMYWithSlashes:
		StrPrintF(dst, "%d/%d", day, month);
		break;
	case dfDMYWithDots:
		StrPrintF(dst, "%d.%d", day, month);
		break;
	case dfDMYWithDashes:
		StrPrintF(dst, "%d-%d", day, month);
		break;
	case dfYMDWithDots:
		StrPrintF(dst, "%d.%d", month, day);
		break;
	case dfYMDWithDashes:
	case dfMDYWithDashes:
	case dfMDYWithSlashes:
		StrPrintF(dst, "%d-%d", month, day);
		break;
	case dfYMDWithSlashes:
	default:	/* Fall-Thru */
		StrPrintF(dst, "%d/%d", month, day);
		break;
	}
}

/*
 * UtilsTimePack
 *
 * Combine hours minutes to a UInt16.
 *
 *  -> hours		Hours.
 *  -> Minutes		Minutes.
 *
 * Returns the packed time.
 */
UInt16
DTTimePack(UInt8 hours, UInt8 minutes)
{
	return (UInt16)((((UInt16)hours)<<8)|((UInt16)minutes));
}

/*
 * UtilsTimeUnpack
 *
 * Extract hours minutes from a UInt16.
 *
 *  -> time		Source time.
 * <-  hours		Where to store hours.
 * <-  minutes		Where to store minutes.
 */
void
DTTimeUnpack(UInt16 time, UInt8 *hours, UInt8 *minutes)
{
	*hours=(UInt8)((time>>8)&0xff);
	*minutes=(UInt8)(time&0xff);
}

/*
 * DTTimeDelimiter
 *
 * Retrieve users preferred time delimiter setting.
 */
Char
DTTimeDelimiter(void)
{
	switch (PrefGetPreference(prefTimeFormat)) {
	case tfDot:
	case tfDotAMPM:
	case tfDot24h:
		return '.';
	case tfComma24h:
		return ',';
	case tfColon:
	case tfColonAMPM:
	case tfColon24h:
	case tfHoursAMPM:
	case tfHours24h:
	default:
		break;
	}

	return ':';
}

/*
 * UtilsTimeFormat
 *
 * Format time depending on users time settings.
 *
 *  -> dst		Where to store string.
 *  -> dur		Time.
 */
void
DTTimeFormat(Char *dst, UInt16 dur)
{
	Char delim=DTTimeDelimiter();
	UInt8 h, m;

	DTTimeUnpack(dur, &h, &m);

	StrPrintF(dst, "%i%c%02i", (UInt16)h, delim, (UInt16)m);
}

/*
 * DTSelectorSetDate
 *
 * Set label of a date selector (or Not Set if value is 0).
 *
 *  -> id		Selector id.
 * <-  dst		Label.
 *  -> v		Value.
 */
void
DTSelectorSetDate(UInt16 id, Char *dst, UInt32 v)
{
	if (v)
		DTDateFormatLong(dst, v);
	else
		StrCopy(dst, "Not Set");

	CtlSetLabel(UIObjectGet(id), dst);
}

/*
 * DTSelectorSetTime
 *
 * Set label of a time selector (or Not Set if value is 0).
 *
 *  -> id		Selector id.
 * <-  dst		Label.
 *  -> v		Value.
 */
void
DTSelectorSetTime(UInt16 id, Char *dst, UInt32 v)
{
	if (v)
		DTTimeFormat(dst, v);
	else
		StrCopy(dst, "Not Set");

	CtlSetLabel(UIObjectGet(id), dst);
}
