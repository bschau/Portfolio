#include <PalmOS.h>
#include "resources.h"

/* structs */
typedef struct {
	UInt32 flags;
	UInt16 catidx;
	UInt16 pinidx;
	UInt16 alaidx;
	UInt16 kaboomidx;
	UInt16 pinstars;
} sitpref;	/* v2r11 */
#define PINTEXTUAL 3

/*******************************************************************************
*
*	PilotMain
*
*	Main for pilot applications.	This is the only place with UpperLower
*	case mix in function names.    It's ugly :)
*/
UInt32
PilotMain(UInt16 cmd,
          void *cmdPBP,
          UInt16 launchFlags)
{
	UInt16 prflen=sizeof(sitpref);
	Int16 retprf;
	sitpref prefs;

	switch (cmd) {
  	case sysAppLaunchCmdNormalLaunch:
		retprf=PrefGetAppPreferences('SeSt', 0, &prefs, &prflen, true);
		if (retprf==noPreferenceFound || prflen!=sizeof(sitpref))
			FrmAlert(aNoPreferences);
		else {
			prefs.pinidx=PINTEXTUAL;
			PrefSetAppPreferences('SeSt', 0, APPVER, &prefs, sizeof(sitpref), true);
			FrmAlert(aPreferencesReset);
		}
	default:	/* FALL-THRU */
		break;
	}

	return 0;
}
