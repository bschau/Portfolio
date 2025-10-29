/*
**	datetime.c
**
**	Various date/time utility functions.
*/
#include "include.h"

/* macros */
enum { dYMDSlash=1, dYMDDot, dYMDDash, dMDYSlash, dMDYDot, dMDYDash, dDMYSlash, dDMYDot, dDMYDash };
enum { tHMColon=1, tHMDot, tHMComma };

/* globals */
static struct {
	char *fmt;
	int token;
} dateFormatStrings[]={
	{ "ymdslash", dYMDSlash },
	{ "ymddot", dYMDDot },
	{ "ymddash", dYMDDash },
	{ "mdyslash", dMDYSlash },
	{ "mdydot", dMDYDot },
	{ "mdydash", dMDYDash },
	{ "dmyslash", dDMYSlash },
	{ "dmydot", dDMYDot },
	{ "dmydash", dDMYDash },
	{ NULL, 0 },
};
static struct {
	char *fmt;
	int token;
} timeFormatStrings[]={
	{ "hmcolon", tHMColon },
	{ "hmdot", tHMDot},
	{ "hmcomma", tHMComma },
	{ NULL, 0 },
};
static int dateFormat=dYMDDash, timeFormat=tHMColon;
static char dateBuffer[32], timeBuffer[16];

/*
**	DTSetDateFormat
**
**	Set dateFormat based on input string.
**
**	 -> fmt		Format string.
*/
void
DTSetDateFormat(char *fmt)
{
	int idx;

	for (idx=0; dateFormatStrings[idx].fmt; idx++) {
		if (strcasecmp(dateFormatStrings[idx].fmt, fmt)==0) {
			dateFormat=dateFormatStrings[idx].token;
			return;
		}
	}

	fprintf(stderr, "(DTSetDateFormat) Unknown date format string\n");
	exit(1);
}

/*
**	DTSetTimeFormat
**
**	Set timeFormat based on input string.
**
**	 -> fmt		Format string.
*/
void
DTSetTimeFormat(char *fmt)
{
	int idx;

	for (idx=0; timeFormatStrings[idx].fmt; idx++) {
		if (strcasecmp(timeFormatStrings[idx].fmt, fmt)==0) {
			timeFormat=timeFormatStrings[idx].token;
			return;
		}
	}

	fprintf(stderr, "(DTSetTimeFormat) Unknown time format string\n");
	exit(1);
}

/*
**	DTDateFormat
**
**	Format a date to a string based on preferences.
**
**	 -> date	Date (Palm OS).
**
**	Returns pointer to string.
*/
char *
DTDateFormat(unsigned short date)
{
	int y, m, d;

	y=((date>>9)&127)+1904;	/* Magic Palm OS year :-) */
	m=(date>>5)&15;
	d=date&31;

	switch (dateFormat) {
	/* European */
	case dYMDSlash:
		sprintf(dateBuffer, "%04i/%02i/%02i", y, m, d);
		break;
	case dYMDDot:
		sprintf(dateBuffer, "%04i.%02i.%02i", y, m, d);
		break;
	case dYMDDash:
		sprintf(dateBuffer, "%04i-%02i-%02i", y, m, d);
		break;

	/* American */
	case dMDYSlash:
		sprintf(dateBuffer, "%02i/%02i/%04i", m, d, y);
		break;
	case dMDYDot:
		sprintf(dateBuffer, "%02i.%02i.%04i", m, d, y);
		break;
	case dMDYDash:
		sprintf(dateBuffer, "%02i-%02i-%04i", m, d, y);
		break;

	/* Danish */
	case dDMYSlash:
		sprintf(dateBuffer, "%02i/%02i/%04i", d, m, y);
		break;
	case dDMYDot:
		sprintf(dateBuffer, "%02i.%02i.%04i", d, m, y);
		break;
	case dDMYDash:
		sprintf(dateBuffer, "%02i-%02i-%04i", d, m, y);
		break;

	/* Error */
	default:
		fprintf(stderr, "(DTDateFormat) Unknown date format\n");
		exit(1);
	}

	return dateBuffer;
}

/*
**	DTTimeFormat
**
**	Format a time to a string based on preferences.
**
**	 -> time	Time ((h<<8)|m)
**
**	Returns pointer to string.
*/
char *
DTTimeFormat(unsigned short time)
{
	int h, m;

	h=(time>>8)&255;
	m=time&255;

	switch (timeFormat) {
	case tHMColon:
		sprintf(timeBuffer, "%02i:%02i", h, m);
		break;
	case tHMDot:
		sprintf(timeBuffer, "%02i.%02i", h, m);
		break;
	case tHMComma:
		sprintf(timeBuffer, "%02i,%02i", h, m);
		break;
	default:
		fprintf(stderr, "(DTTimeFormat) Unknown time format\n");
		exit(1);
	}

	return timeBuffer;
}
