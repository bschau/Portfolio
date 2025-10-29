#include "occman.h"

/*
**	OCCListInit
**
**	Initialize the list.
**
**	 -> ol		OCC List.
*/
void
OCCListInit(OCCList *ol)
{	
	ol->head=(OCCNode *)&ol->tail;
	ol->tail=NULL;
	ol->tailPred=(OCCNode *)&ol->head;
	ol->count=0;
}

/*
**	OCCListDestroy
**
**	Deletes all entries of a occ list.
**
**	 -> ol		OCC List.
*/
void
OCCListDestroy(OCCList *ol)
{
	OCCNode *n;
	MemHandle mh;

	for (n=ol->head; n->next; ) {
		mh=n->mh;
		n=n->next;
		MemHandleFree(mh);
	}
}

/*
**	OCCDiscover
**
**	Build a list of all OCCs of a given type.
**
**	 -> type	Type to look for.
**	 -> meCard	Exclude the database on this card ...
**	 -> meId	... and with this id.
**	 -> lh		OCC List header.
*/
void
OCCDiscover(UInt32 type, UInt16 meCard, LocalID meId, OCCList *lh)
{
	Boolean firstSearch=true;
	DmSearchStateType state;
	UInt16 card;
	LocalID dbId;
	Char dbName[dmDBNameLength];
	MemHandle mh;
	OCCNode *mp, *n, *n1;
	Err err;

	for (EVER) {
		if ((err=DmGetNextDatabaseByTypeCreator(firstSearch, &state, type, CRID, false, &card, &dbId))!=errNone)
			break;

		firstSearch=false;
		if ((err=DmDatabaseInfo(card, dbId, dbName, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL))==errNone) {
			if (meCard==card && meId==dbId)
				continue;

			mh=MemHandleNew(sizeof(OCCNode));
			ErrFatalDisplayIf(mh==NULL, "(OCCDiscover) Out of memory.");

			mp=MemHandleLock(mh);
			mp->mh=mh;
			MemSet(mp->name, dmDBNameLength, 0);
			StrNCopy(mp->name, dbName, dmDBNameLength);

			for (n=lh->head; n->next; n=n->next) {
				if (StrCaselessCompare(mp->name, n->name)<0)
					break;
			}

			n1=n->prev;
			n->prev=mp;
			n1->next=mp;
			mp->next=n;
			mp->prev=n1;

			lh->count++;
		}
	}
}

/*
**	OCCDiscoverAll
**
**	Rebuild list with all OCCs.
**
**	 -> lst		OCCList.
*/
void
OCCDiscoverAll(OCCList *lst)
{
	UInt16 meCard;
	LocalID meId;
	Err err;

	err=SysCurAppDatabase(&meCard, &meId);
	ErrFatalDisplayIf(err!=errNone, "(OCCDiscoverAll) Cannot find myself");

	OCCListInit(lst);
	OCCDiscover('occ ', meCard, meId, lst);
	OCCDiscover('appl', meCard, meId, lst);
}

/*
**	OCCGetByName
**
**	Find a given occ by name.
**
**	 -> name	occ to look for.
**	<-  card	Where to store Card no.#
**	<-  id		Where to store Local ID.
**
**	Returns true if occ was found, false otherwise.
*/
Boolean
OCCGetByName(Char *name, UInt16 *card, LocalID *id)
{
	UInt16 numcards=MemNumCards(), cno;

	for (cno=0; cno<numcards; cno++) {
		if ((*id=DmFindDatabase(cno, name))) {
			*card=cno;
			return true;
		}
	}

	return false;
}

/*
**	OCCGetUrl
**
**	Get URL from occ at (card, id).
**
**	 -> card	Card no. of occ.
**	 -> id		Local ID of occ.
**
**	Returns url if successful, NULL otherwise.
*/
MemHandle
OCCGetUrl(UInt16 card, LocalID id)
{
	MemHandle mh=NULL, rmh;
	Char *mp, *rmp;
	UInt16 len, resIdx;
	DmOpenRef db;

	if ((db=DmOpenDatabase(card, id, dmModeReadOnly))) {
		if ((resIdx=DmFindResource(db, 'tSTR', URLID, NULL))!=0xffff) {
			if ((rmh=DmGetResourceIndex(db, resIdx))) {
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
			}
		}

		DmCloseDatabase(db);
	}

	return mh;
}

/*
**	OCCIsStandalone
**
**	Check to see if OCC is a standalone OCC.
**
**	 -> card	OCC on card ...
**	 -> id		... with LocalID.
**	 -> occType	Where to store occ type information.
**
**	Returns true if operation succeeded, false otherwise.
*/
Boolean
OCCIsStandalone(UInt16 card, LocalID id, UInt16 *occType)
{
	DmOpenRef db;
	UInt16 resIdx;

	if ((db=DmOpenDatabase(card, id, dmModeReadOnly))==NULL)
		return false;

	resIdx=DmFindResource(db, 'code', 0, NULL);
	DmCloseDatabase(db);

	*occType=(resIdx==0xffff) ? OCCTypePiggyBack : OCCTypeStandalone;

	return true;
}

/*
**	OCCDelete
**
**	Delete a occ given its index in the list.
**
**	 -> lh		List header.
**	 -> idx		Index of OCC in list.
**
**	Returns true if occ was deleted, false otherwise.
*/
Boolean
OCCDelete(OCCList *lh, Int16 idx)
{
	UInt16 card;
	LocalID id;
	OCCNode *n, *next, *prev;
	Err err;

	for (n=lh->head; n->next && idx; n=n->next)
		idx--;

	if (OCCGetByName(n->name, &card, &id)==false)
		return false;

	if ((err=DmDeleteDatabase(card, id))==errNone) {
		prev=n->prev;
		next=n->next;
		prev->next=next;
		next->prev=prev;

		MemHandleFree(n->mh);
		lh->count--;

		return true;
	}

	return false;
}

/*
**	OCCLaunch
**
**	Launch the given OCC using Clipper.
**
**	 -> card	Card # of OCC.
**	 -> id		LocalID of OCC.
*/
void
OCCLaunch(UInt16 card, LocalID id)
{
	Err err;
	UInt16 cCard;
	LocalID cid;
	DmSearchStateType state;
	MemHandle mh;

	if ((err=DmGetNextDatabaseByTypeCreator(true, &state, sysFileTApplication, sysFileCClipper, true, &cCard, &cid))==errNone) {
		if ((mh=OCCGetUrl(card, id))) {
			MemHandleSetOwner(mh, 0);
			SysUIAppSwitch(cCard, cid, sysAppLaunchCmdGoToURL, MemHandleLock(mh));
		}
	}
}
