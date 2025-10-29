/*
 * Notifications.c
 */
#include "Include.h"

/*
 * NotificationsExists
 *
 * Check if OS supports notifications.
 *
 * Return true if OS supports notifications, false otherwise.
 */
Boolean
NotificationsExists(void)
{
	UInt32 v;

	if (FtrGet(sysFtrCreator, sysFtrNumNotifyMgrVersion, &v)!=errNone)
		return false;

	return (v ? true : false);
}

/*
 * NotificationsSetup
 *
 * Setup/clear notifcations if required.
 */
void
NotificationsSetup(void)
{
	Prefs p;

	if (NotificationsExists()==true) {
		UInt16 cardNo;
		LocalID dbID;

		if (SysCurAppDatabase(&cardNo, &dbID)==errNone) {
			PMGetPref(&p, sizeof(Prefs), PrfApplication);

			if (p.flags&PFlgHotSync)
				SysNotifyRegister(cardNo, dbID, sysNotifySyncFinishEvent, NULL, sysNotifyNormalPriority, NULL);
			else
				SysNotifyUnregister(cardNo, dbID, sysNotifySyncFinishEvent, sysNotifyNormalPriority);

			if (p.flags&PFlgLaunching)
				SysNotifyRegister(cardNo, dbID, sysNotifyAppLaunchingEvent, NULL, sysNotifyNormalPriority, NULL);
			else
				SysNotifyUnregister(cardNo, dbID, sysNotifyAppLaunchingEvent, sysNotifyNormalPriority);

			if (p.flags&PFlgQuitting)
				SysNotifyRegister(cardNo, dbID, sysNotifyAppQuittingEvent, NULL, sysNotifyNormalPriority, NULL);
			else
				SysNotifyUnregister(cardNo, dbID, sysNotifyAppQuittingEvent, sysNotifyNormalPriority);

			if (p.flags&PFlgWakeup)
				SysNotifyRegister(cardNo, dbID, sysNotifyLateWakeupEvent, NULL, sysNotifyNormalPriority, NULL);
			else
				SysNotifyUnregister(cardNo, dbID, sysNotifyLateWakeupEvent, sysNotifyNormalPriority);
		}
	}
}
