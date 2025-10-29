/*
 * iSecurLib.c
 */
#include <SystemMgr.h>
#include "Include.h"

/* macros */
#define LibraryVersion 1

/* protos */
static void Initialize(iSecurLibGlobals *, SysLibTblEntryPtr);

/*
 * start
 */
Err
LibMain(UInt16 refNum, SysLibTblEntryPtr e)
{
	extern void *jmptable();

	e->dispatchTblP=(void *)jmptable;
	e->globalsP=NULL;
	return errNone;
}

/*
 * LockGlobals
 *
 * Lock the Library globals.
 *
 *  -> refNum		Library ref num.
 *
 * Returns pointer to locked globals or NULL.
 */
iSecurLibGlobals *
LockGlobals(UInt16 refNum)
{
	iSecurLibGlobals *g=NULL;
	SysLibTblEntryPtr sysLib=NULL;
	MemHandle mh=NULL;

	ErrFatalDisplayIf(refNum==sysInvalidRefNum, "(LockGlobals) Invalid refNum");
	sysLib=SysLibTblEntry(refNum);
	ErrFatalDisplayIf(sysLib==NULL, "(LockGlobals) Invalid refNum");

	if ((mh=sysLib->globalsP))
		g=MemHandleLock(mh);

	return g;
}

/*
 * Open
 */
Err
iSecurLibOpen(UInt16 refNum, UInt16 version)
{
	iSecurLibGlobals *g=NULL;

	ErrFatalDisplayIf(refNum==sysInvalidRefNum, "(iSecurLibOpen) Invalid refNum");

	if ((g=LockGlobals(refNum))==NULL) {
		SysLibTblEntryPtr sysLib;
		MemHandle mh;

		sysLib=SysLibTblEntry(refNum);
		ErrFatalDisplayIf(sysLib==NULL, "(iSecurLibOpen) Invalid refNum");
		ErrFatalDisplayIf(sysLib->globalsP, "(iSecurLibOpen) Library globals already exist?!?!");
		mh=MemHandleNew(sizeof(iSecurLibGlobals));
		ErrFatalDisplayIf(mh==NULL, "(iSecurLibOpen) Out of memory");

		sysLib->globalsP=mh;
		g=LockGlobals(refNum);
		ErrFatalDisplayIf(g==NULL, "(iSecurLibOpen) Cannot lock library globals");
		MemPtrSetOwner(g, 0);
		MemSet(g, sizeof(iSecurLibGlobals), 0);

		g->refCount=1;
		Initialize(g, sysLib);
	} else
		g->refCount++;

	MemPtrUnlock(g);
	return errNone;
}

/*
 * Initialize
 *
 * Initialize library.
 *
 *  -> g		Globals.
 *  -> sysLib		Sys library ptr.
 */
static void
Initialize(iSecurLibGlobals *g, SysLibTblEntryPtr sysLib)
{
	LocalID id=0;
	UInt16 idx;
	DmOpenRef db;

	for (idx=0; idx<MemNumCards(); idx++) {
		if ((id=DmFindDatabase(idx, iSecurLibName))!=0)
			break;
	}

	ErrFatalDisplayIf(id==0, "(Initialize) Cannot find myself");
	
	db=DmOpenDatabase(idx, id, dmModeReadOnly);
	ErrFatalDisplayIf(db==NULL, "(Initialize) Cannot open myself");

	for (idx=0; idx<10; idx++) {
		UInt16 resIdx;

		resIdx=DmFindResource(db, (UInt32)'AESx', idx+1000, NULL);
		ErrFatalDisplayIf(resIdx==dmMaxRecordIndex, "(Initialize) Cannot find resource");

		if ((g->mhs[idx]=DmGetResourceIndex(db, resIdx))==NULL) {
			sysLib->globalsP=NULL;
			MemPtrFree(g);
			DmCloseDatabase(db);
			ErrFatalDisplay("Cannot lock table");
			SysReset();
		}
	}

	DmCloseDatabase(db);
}

/*
 * Close
 *
 * Return 0 if library was closed succesfully and useCount is to believed.
 */
Err
iSecurLibClose(UInt16 refNum, UInt16 *useCount)
{
	iSecurLibGlobals *g=NULL;

	ErrFatalDisplayIf(refNum==sysInvalidRefNum, "(iSecurLibClose) Invalid refNum");

	if (useCount==NULL || ((g=LockGlobals(refNum))==NULL))
		return -1;

	g->refCount--;
	ErrFatalDisplayIf(g->refCount<0, "(iSecurLibClose) Library under lock");

	*useCount=g->refCount;
	MemPtrUnlock(g);

	if (*useCount<=0) {
		SysLibTblEntryPtr sysLib=SysLibTblEntry(refNum);
		MemHandle mh;

		ErrFatalDisplayIf(sysLib==NULL, "(iSecurLibClose) Invalid refNum");

		mh=sysLib->globalsP;
		sysLib->globalsP=NULL;

		if (mh) {
			iSecurLibGlobals *g=MemHandleLock(mh);
			UInt16 idx;

			for (idx=0; idx<10; idx++)
				DmReleaseResource(g->mhs[idx]);
		
			MemHandleFree(mh);
		}
	}

	return 0;
}

/*
 * Sleep
 */
Err
iSecurLibSleep(UInt16 refNum)
{
	return errNone;
}

/*
 * Wake
 */
Err
iSecurLibWake(UInt16 refNum)
{
	return errNone;
}

/*
 * iSecurLibVersion
 *
 * Return the version of the iSecurLib library.
 *
 *  -> refNum		Refnum.
 * <-  version		Where to store version.
 */
void
iSecurLibVersion(UInt16 refNum, UInt32 *version)
{
	ErrFatalDisplayIf(version==NULL, "(iSecurLibVersion) null argument");

	*version=LibraryVersion;
}
