#include <PalmOS.h>
#include "otresources.h"

/* protos */
static MemHandle GetUrl(void);

/*
**	PilotMain
**
**	Main for pilot applications.
*/
UInt32
PilotMain(UInt16 cmd,
          void *cmdPBP,
          UInt16 launchFlags)
{
	Err err;
	DmSearchStateType state;
	UInt16 card;
	LocalID dbid;
	MemHandle mh;

	if (cmd==sysAppLaunchCmdNormalLaunch) {
		if ((err=DmGetNextDatabaseByTypeCreator(true, &state, sysFileTApplication, sysFileCClipper, true, &card, &dbid))==errNone) {
			if ((mh=GetUrl())) {
				if ((err=SysUIAppSwitch(card, dbid, sysAppLaunchCmdGoToURL, MemHandleLock(mh)))!=errNone)
					MemHandleFree(mh);
			}
		}
	}

	return 0;
}

/*
**	GetUrl
**
**	Return systemized url.
*/
static MemHandle
GetUrl(void)
{
	MemHandle mh=NULL, rmh;
	Char *mp, *rmp;
	UInt16 len;

	if ((rmh=DmGetResource('tSTR', URLID))==NULL)
		return NULL;
	
	rmp=MemHandleLock(rmh);
	len=StrLen(rmp);
	if ((mh=MemHandleNew(len+1))) {
		mp=MemHandleLock(mh);
		MemPtrSetOwner(mp, 0);
		MemMove(mp, rmp, len);
		mp[len]='\x00';
		MemHandleUnlock(mh);
	}

	MemHandleUnlock(rmh);
	DmReleaseResource(rmh);
	return mh;
}
