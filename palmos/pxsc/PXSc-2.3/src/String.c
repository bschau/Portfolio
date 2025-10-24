/*
 * String.c
 */
#include "Include.h"

/* globals */
static struct {
	UInt16 id;
	MemHandle mh;
	Char *txt;
} strings[]={
	{ Str01, NULL, NULL },
	{ Str02, NULL, NULL },
	{ Str03, NULL, NULL },
	{ Str04, NULL, NULL },
	{ Str05, NULL, NULL },
	{ Str06, NULL, NULL },
	{ Str07, NULL, NULL },
	{ Str08, NULL, NULL },
	{ Str09, NULL, NULL },
	{ Str10, NULL, NULL },
	{ Str11, NULL, NULL },
	{ Str12, NULL, NULL },
	{ Str13, NULL, NULL },
	{ Str14, NULL, NULL },
	{ Str15, NULL, NULL },
	{ Str16, NULL, NULL },
	{ 0 },
};

/*
 * StringGet
 *
 * Lookup a string.
 *
 *  -> strNo	String number.
 */
Char *
StringGet(UInt16 strNo)
{
	MemHandle mh;
	UInt16 idx;

	for (idx=0; strings[idx].id; idx++) {
		if (strings[idx].id==strNo) {
			if (strings[idx].mh==NULL) {
				mh=DmGetResource(strRsc, strings[idx].id);
				ErrFatalDisplayIf(mh==NULL, "(StringGet) Cannot lock resource.");
				strings[idx].mh=mh;
				strings[idx].txt=MemHandleLock(mh);
			}

			return strings[idx].txt;
		}
	}

	ErrFatalDisplay("(StringGet) String resource not found.");
	return NULL;	// shaddup, gcc!
}

/*
 * StringFree
 *
 * Free all locked strings.
 */
void
StringFree(void)
{
	UInt16 idx;
	MemHandle mh;

	for (idx=0; strings[idx].id; idx++) {
		if ((mh=strings[idx].mh)!=NULL) {
			MemHandleUnlock(mh);
			DmReleaseResource(mh);
		}
	}
}
