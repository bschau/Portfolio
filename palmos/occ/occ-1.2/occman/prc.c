#include "occman.h"

/* protos */
static void SetResource(DmOpenRef, DmResType, DmResID, UInt8 *, UInt32);

/* globals */
static Char *occVersion=OCCVERSION;
extern UInt32 code0000Len, code0001Len, data0000Len, pref0000Len;
extern UInt32 rloc0000Len, largeLen, smallLen;
extern UInt8 code0000Arr[], code0001Arr[], data0000Arr[];
extern UInt8 pref0000Arr[], rloc0000Arr[], largeArr[], smallArr[];

/*
**	SetResource
**
**	Allocate and set resource in occ.
**
**	 -> dbH		Database handle.
**	 -> resType	Resource type.
**	 -> resID	Resource id.
**	 -> data	Source.
**	 -> size	Size of resource.
*/
static void
SetResource(DmOpenRef dbH, DmResType resType, DmResID resID, UInt8 *data, UInt32 size)
{
	MemHandle mh=DmNewResource(dbH, resType, resID, size);
	Err err;

	ErrFatalDisplayIf(mh==NULL, "(SetResource) Cannot add resource.");
	err=DmWrite(MemHandleLock(mh), 0, data, size);
	MemHandleUnlock(mh);
	DmReleaseResource(mh);

	ErrFatalDisplayIf(err!=errNone, "(SetResource) Cannot write resource.");
}

/*
**	PrcBuild
**
**	Build and install the OCC.
**
**	 -> name	Name of OCC.
**	 -> url		Url to place in OCC.
**	 -> exe		Is this a standalone or piggyback OCC.
**
**	Returns true if succesful, false otherwise.
*/
Boolean
PrcBuild(Char *name, Char *url, UInt16 exe)
{
	Char dbName[dmDBNameLength];
	UInt32 type, urlLen;
	UInt16 card, attr;
	LocalID id;
	DmOpenRef dbH;
	Err err;
	MemHandle mh;
	Char *p;

	MemSet(dbName, dmDBNameLength, 0);
	StrNCopy(dbName, name, dmDBNameLength);
	if (exe)
		type='appl';
	else
		type='occ ';

	if (DmCreateDatabase(0, dbName, CRID, type, true)!=errNone)
		return false;

	/* hmmm ... if we can't find the database we can't delete it */
	if (OCCGetByName(dbName, &card, &id)==false)
		return false;

	dbH=DmOpenDatabase(card, id, dmModeReadWrite);
	ErrFatalDisplayIf(dbH==NULL, "(PrcBuild) Cannot open newly created database.");

	if (exe) {
		SetResource(dbH, 'code', 0, code0000Arr, code0000Len);
		SetResource(dbH, 'code', 1, code0001Arr, code0001Len);
		SetResource(dbH, 'data', 0, data0000Arr, data0000Len);
		SetResource(dbH, 'pref', 0, pref0000Arr, pref0000Len);
		SetResource(dbH, 'rloc', 0, rloc0000Arr, rloc0000Len);
	}

	SetResource(dbH, 'tAIB', 1000, largeArr, largeLen);
	SetResource(dbH, 'tAIB', 1001, smallArr, smallLen);
	urlLen=StrLen(url);
	if (StrNCaselessCompare(url, "http://", 7)==0)
		SetResource(dbH, 'tSTR', URLID, url, urlLen+1);
	else {
		mh=MemHandleNew(urlLen+8);
		ErrFatalDisplayIf(mh==NULL, "(PrcBuild) Out of memory.");
		p=MemHandleLock(mh);
		MemMove(p, "http://", 7);
		MemMove(p+7, url, urlLen);
		*(p+7+urlLen)='\x00';
		SetResource(dbH, 'tSTR', URLID, p, urlLen+8);
		MemHandleFree(mh);
	}

	SetResource(dbH, 'tVER', 1, occVersion, StrLen(occVersion)+1);
	DmCloseDatabase(dbH);

	err=DmDatabaseInfo(card, id, NULL, &attr, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	if (exe)
		attr|=dmHdrAttrBackup;
	else
		attr|=dmHdrAttrLaunchableData|dmHdrAttrBackup;

	if ((err|=DmSetDatabaseInfo(card, id, NULL, &attr, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL))!=errNone)
		return false;

	return true;
}
