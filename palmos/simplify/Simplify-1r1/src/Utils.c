/*
 * Utils.c
 */
#include "Include.h"

/*
 * UtilsRomCheck
 *
 * Returns true if the Rom Version is equal to or better than the required
 * Rom Version. False is returned otherwise.
 */
Boolean
UtilsRomCheck(void)
{
	UInt32 version;

	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &version);

	if (version<sysMakeROMVersion(3, 0, 0, sysROMStageRelease, 0)) {
		FrmAlert(aBadRom);
		return false;
	}

	return true;
}

/*
 * UtilsGetPref
 *
 * Get preference #.
 *
 *  -> p	Destination.
 *  -> len	Length of destination.
 *  -> id	Preference id.
 *
 * Returns true if preferences was gotten, false otherwise.
 */
Boolean
UtilsGetPref(void *p, UInt16 len, UInt16 id)
{	
	UInt16 l=len;
	Int16 retPrf;

	retPrf=PrefGetAppPreferences((UInt32)CRID, id, p, &l, true);
	if (retPrf==noPreferenceFound || len!=l)
		return false;

	return true;
}

/*
 * UtilsSetPref
 *
 * Set preference #.
 *
 *  -> p	Source.
 *  -> len	Length of source.
 *  -> id	Preferences id.
 */
void
UtilsSetPref(void *p, UInt16 len, UInt16 id)
{
	PrefSetAppPreferences((UInt32)CRID, id, APPVER, p, len, true);
}
