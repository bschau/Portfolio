#include "eventor.h"

/* globals */
static FormPtr pFrm;
static Int16 wHour, wMinute;
static Char alarmText[timeStringLength];

/* protos */
static void ShowAlarm(void);
static Boolean EventHandler(EventPtr ev);

/*
**	ShowAlarm
**
**	Set new alarm text.
*/
static void
ShowAlarm(void)
{

	TimeToAscii((UInt8)wHour, (UInt8)wMinute, (TimeSelectorNeedsAMPM()==true) ? tfColonAMPM : tfColon24h, alarmText);
	CtlSetLabel(FrmGetObjectPtr(pFrm, FrmGetObjectIndex(pFrm, fPrefsAlarmTime)), alarmText);
}

/*
**	PreferencesDialog
**
**	Handle preferences dialog.
*/
void
PreferencesDialog(void)
{
	FormType *oldForm=currentForm;
	ControlType *snChk=NULL, *saChk;
	Boolean notify;

	wHour=prefs.alarmHour;
	wMinute=prefs.alarmMinute;

	pFrm=FrmInitForm(fPrefs);
	ErrFatalDisplayIf(pFrm==NULL, "(Preferences) Cannot initialize Prefs form.");
	FrmSetActiveForm(pFrm);
	currentForm=pFrm;
	FrmSetEventHandler(pFrm, (FormEventHandlerPtr)EventHandler);
	FrmDrawForm(pFrm);
	if ((notify=UtilsGetRomVersion(NULL, sysMakeROMVersion(4, 0, 0, sysROMStageRelease, 0)))==true) {
		snChk=UIFormGetObject(fPrefsShowNotifications);
		CtlSetValue(snChk, (prefs.flags.showNotifications==1) ? 1 : 0);
		UIShowObject(fPrefsShowNotifications);
	}

	saChk=UIFormGetObject(fPrefsSoundAlarm);
	CtlSetValue(saChk, (prefs.flags.soundAlarm==1) ? 1 : 0);
	ShowAlarm();
	if (FrmDoDialog(pFrm)==fPrefsOK) {
		prefs.alarmHour=wHour;
		prefs.alarmMinute=wMinute;

		if (notify==true) {
			prefs.flags.showNotifications=CtlGetValue(snChk) ? 1 : 0;
			UtilsSetNotification(prefs.flags.showNotifications ? true : false, sysNotifyLateWakeupEvent);
			UtilsSetNotification(prefs.flags.showNotifications ? true : false, eventorLateWakeup);
		}
		prefs.flags.soundAlarm=CtlGetValue(saChk) ? 1 : 0;
	}
	FrmSetActiveForm(oldForm);
	currentForm=oldForm;
	FrmEraseForm(pFrm);
	FrmDeleteForm(pFrm);
}

/*
**      EventHandler
**
**      Event handler for the alarm form.
**
**       -> ev          Event.
**
**      Returns true if event is handled, false otherwise.
*/
static Boolean
EventHandler(EventPtr ev)
{
	MemHandle mh;

        switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case fPrefsAlarmTime:
			mh=UtilsLockString(strSetNewAlarm);
			if (TimeSelector(&wHour, &wMinute, MemHandleLock(mh))==true)
				ShowAlarm();

			UtilsUnlockString(mh);
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}
	return false;
}

/*
**	PreferencesGet
**
**	Retrieves preferences for Eventor.
**
**	 -> dst		Where to store preferences.
**
**	Returns true if preferences was found, false otherwise.
*/
Boolean
PreferencesGet(Preferences *dst)
{
	UInt16 len=sizeof(Preferences);
	Int16 retPrf;

	retPrf=PrefGetAppPreferences((UInt32)CRID, 0, dst, &len, true);
	if (retPrf==noPreferenceFound || len!=sizeof(Preferences))
		return false;

	return true;
}
