#define MAIN
#include "p0ls.h"

/* protos */
static UInt16 getromversion(void);
static Boolean startapp(void);
static Int16 dyngetmem(UInt32);
static void stopapp(void);
static void eventloop(void);

/*******************************************************************************
 *
 *	getromversion
 *
 *	Get ROM version.  Implements minimum logic.
 *
 *	Output:
 *		ROMNONE, ROMV2, ROMV3.
 */
static UInt16
getromversion(void)
{
	UInt32 rv;

	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &rv);
	if (rv>=0x03003000)
		return ROMV3;

	if (rv>=0x02003000)
		return ROMV2;

	return ROMNONE;
}

/*******************************************************************************
*
*	startapp
*
*	Start application - opens database etc. etc.
*
*	Output:
*		s: 0.
*		f: !0.
*/
static Boolean
startapp(void)
{
	UInt16 prflen=sizeof(p0lspref), dbattr=dmHdrAttrBackup, dbver=1, card;
	Int16 retprf;
	LocalID dbid;
	Err err;

	retprf=PrefGetAppPreferences((UInt32)CREATORID, 0, &prefs, &prflen, true);
	if (retprf==noPreferenceFound || prflen!=sizeof(p0lspref)) {
		prefs.depth=1;
		prefs.angle=60;
	}

	userinput=1;

	retprf=dyngetmem((UInt32)256*(UInt32)1024);
	if (retprf)
		return -1;

	dbid=DmFindDatabase(0, DATABASENAME);
	if (!dbid) {
		err=DmCreateDatabase(0, DATABASENAME, (UInt32)CREATORID, (UInt32)DATABASETYPE, false);
		if (err)
			return err;

		dbid=DmFindDatabase(0, DATABASENAME);
		gdb=DmOpenDatabase(0, dbid, dmModeReadWrite);
		if (!gdb)
			return DmGetLastErr();

		err=DmOpenDatabaseInfo(gdb, &dbid, NULL, NULL, &card, NULL);
		if (err)
			return -1;

		/* NOTE: These must follow in sorted (ascending) order! */
		dbloader("bush 1", 6, 25, "F", "F[+F]F[-F]F", NULL, NULL, NULL, NULL, NULL, NULL);
		dbloader("bush 2", 6, 25, "0", NULL, NULL, "0F1[+0][-0]", "1[-FFF][+FFF]F1", NULL, NULL, NULL);
		dbloader("bush 3", 4, 22, "F", "FF+[+F-F-F]-[-F+F+F]", NULL, NULL, NULL, NULL, NULL, NULL);
		dbloader("christmas tree", 8, 18, "01FFF", NULL, NULL, "[+++2][---2]30", "[-FFF][+FFF]F", "+4[-2]1", "31", "-2[+4]1");
		dbloader("classic sierpinsky gasket", 6, 60, "F0F--FF--FF", "FF", NULL, "--F0F++F0F++F0F--", NULL, NULL, NULL, NULL);
		dbloader("dragon", 13, 90, "0", NULL, NULL, "0+1F+", "-F0-1", NULL, NULL, NULL);
		dbloader("hilbert curve", 6, 90, "0", NULL, NULL, "-1F+0F0+F1-", "+0F-1F1-F0+", NULL, NULL, NULL);
		dbloader("peano curve", 4, 90, "0", NULL, NULL, "0F1F0+F+1F0F1-F-0F1F0", "1F0F1-F-0F1F0+F+1F0F1", NULL, NULL, NULL);
		dbloader("quadratic koch island", 4, 90, "F+F+F+F", "F+F-F-FF+F+F-F", NULL, NULL, NULL, NULL, NULL, NULL);
		dbloader("sierpinsky square", 4, 90, "F+F+F+F", "FF+F+F+F+FF", NULL, NULL, NULL, NULL, NULL, NULL);
		dbloader("von koch snowflake curve", 6, 60, "F", "F-F++F-F", NULL, NULL, NULL, NULL, NULL, NULL);

		DmSetDatabaseInfo(card, dbid, NULL, &dbattr, &dbver, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		prefs.depth=1;
		prefs.angle=60;
	} else {
		gdb=DmOpenDatabase(0, dbid, dmModeReadWrite);
		if (!gdb)
			return DmGetLastErr();
	}

	clearvars();
	return 0;
}

/*******************************************************************************
*
*	dyngetmem
*
*	Get dynamic sized string memory.
*
*	Input:
*		bl - bufferlen (requested).
*	Output:
*		s: 0.
*		f: !0.
*/
static Int16
dyngetmem(UInt32 bl)
{
	UInt32 oldmemlen, steplen;

	bufferlen=bl;
	oldmemlen=bufferlen;
	while (!buf1h) {
		buf1h=MemHandleNew(bufferlen);
		if (!buf1h) {
			bufferlen>>=1;
			if (bufferlen<8)
				break;

			steplen=oldmemlen-bufferlen;
			while (steplen>0) {
				buf1h=MemHandleNew(bufferlen+steplen);
				if (buf1h) {
					bufferlen+=steplen;
					return 0;
				}

				steplen>>=1;
			}
		}
	}

	return -1;
}

/*******************************************************************************
*
*	stopapp
*
*	Stop application - closes databases etc. etc.
*/
static void
stopapp(void)
{
	if (gdb) {
		DmCloseDatabase(gdb);
		gdb=NULL;
	}

	if (buf1h)
		MemHandleFree(buf1h);

	PrefSetAppPreferences((UInt32)CREATORID, 0, APPVER, &prefs, sizeof(p0lspref), true);

	FrmSaveAllForms();
	FrmCloseAllForms();
}

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
	Boolean err;

	switch (cmd) {
	case sysAppLaunchCmdNormalLaunch:
		romv=getromversion();
		if (!romv)
			FrmCustomAlert(alertrom20, NULL, NULL, NULL);
		else {
			err=startapp();
			if (!err)
				eventloop();
			else
				FrmCustomAlert(3456, "going", "going", "gone");

			stopapp();
		}
		break;

	default:
		break;
	}

	return 0;
}

/*******************************************************************************
*
*	eventloop
*
*	Main dispatch routine.
*/
static void
eventloop(void)
{
	UInt16 step=1, formid, err;
	Int16 terr;
	FormPtr form;
	Boolean handled;
	EventType ev;

	FrmGotoForm(formstart);
	for (EVER) {
		if (userinput)
			EvtGetEvent(&ev, evtWaitForever);
		else
			EvtGetEvent(&ev, 1);
			
		if (ev.eType==appStopEvent)
			break;

		if (ev.eType==nilEvent) {
			if (userinput)
				continue;

			updatevisual(step);
			if (step<=prefs.depth) {
				terr=turtlestep();
				if (terr) {
					turtledone();
					FrmGotoForm(formstart);
				}
			} else {
				if (step==prefs.depth+1) {
					terr=turtlestack();
					if (terr) {
						turtledone();
						FrmGotoForm(formstart);
					}
				} else {	
					terr=turtlecurvesize();
					if (terr) {
						turtledone();
						FrmGotoForm(formstart);
					} else
						FrmGotoForm(formdisplay);
				}
			}
			step++;
		}

		handled=SysHandleEvent(&ev);
		if (handled)
			continue;

		handled=MenuHandleEvent(NULL, &ev, &err);
		if (handled)
			continue;

		switch (ev.eType) {
		case frmLoadEvent:
			formid=ev.data.frmLoad.formID;
			form=FrmInitForm(formid);
			FrmSetActiveForm(form);
			switch (formid) {
				case formstart:
					startinit(form, formid);
					break;
				case formprodrules:
					prodrulesinit(form, formid);
					break;
				case formworking:
					step=1;
					working(form);
					break;
				case formdisplay:
					displayinit(form, formid);
					break;
				case formio:
					ioinit(form, formid);
					break;
			}
			break;

		default:
			FrmDispatchEvent(&ev);
			break;
		}
	}
}
