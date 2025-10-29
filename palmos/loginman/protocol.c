#include "loginman.h"

/* protos */
static UInt16 findcredentials(DmOpenRef, arguments *);

/*******************************************************************************
*
*	findcredentials
*
*	Find a users given credentials.
*/
static UInt16
findcredentials(DmOpenRef db,
                arguments *args)
{
	MemHandle mh;
	UInt16 idx;

	mh=fakerecord(args->realm);
	idx=findrecord(db, mh);
	MemHandleFree(mh);

	return idx;
}

/*******************************************************************************
*
*	getprotorealm
*
*	Setup protocol and realm info.
*/
Boolean
getprotorealm(arguments *args,
              UInt8 *src)
{
	UInt8 *p;

	p=StrStr(src, "LOGIN:");
	if (p) {
		args->protocol=PLOGIN;
		args->realm=p+6;
		return true;
	}

	p=StrStr(src, "STORE:");
	if (p) {
		args->protocol=PSTORE;
		args->realm=p+6;
		return true;
	}

	p=StrStr(src, "DELETE:");
	if (p) {
		args->protocol=PDELETE;
		args->realm=p+7;
		return true;
	}

	args->protocol=0;
	args->realm=NULL;
	return false;
}

/*******************************************************************************
*
*	protologin
*
*	Login user.
*/
MemHandle
protologin(DmOpenRef db,
           arguments *args)
{
	MemHandle rech, rh, lh, ph, ch;
	UInt16 crdidx, rl, ll, pl, ul, cl;
	realmrecord rr;
	UInt8 *cp;
	Boolean goon;

	crdidx=findcredentials(db, args);
	if (crdidx!=dmMaxRecordIndex) {
		rech=clonerecord(db, crdidx);
		if (rech) {
			qunpack(&rr, MemHandleLock(rech));
			goon=popup(rr.realm, rr.login, rr.password, &rh, &lh, &ph);
			MemHandleFree(rech);
			if (goon==false)
				return NULL;
		} else
			return NULL;
	} else {
		goon=popup(args->realm, NULL, NULL, &rh, &lh, &ph);
		if (goon==false)
			return NULL;
	}

	/* build clipper args ... */
	rl=urlencodehandle(&rh);
	ll=urlencodehandle(&lh);
	pl=urlencodehandle(&ph);
	
	/* concat, free */
	ul=StrLen(args->url);

	cl=ul+3+rl+3+ll+3+pl+1+4;	/* ?R= &L= &P= &M=1 term */
	ch=MemHandleNew(cl);
	ErrFatalDisplayIf(ch==NULL, "(protologin) Out of memory");

	cp=MemHandleLock(ch);
	MemMove(cp, args->url, ul);
	cp+=ul;
	MemMove(cp, "?M=1&R=", 7);
	cp+=7;
	MemMove(cp, MemHandleLock(rh), rl);
	MemHandleFree(rh);
	cp+=rl;

	if (ll) {
		MemMove(cp, "&L=", 3);
		cp+=3;
		MemMove(cp, MemHandleLock(lh), ll);
		MemHandleFree(lh);
		cp+=ll;
	}

	if (pl) {
		MemMove(cp, "&P=", 3);
		cp+=3;
		MemMove(cp, MemHandleLock(ph), pl);
		MemHandleFree(ph);
		cp+=pl;
	}

	*cp='\x00';

	MemHandleUnlock(ch);
	
	return ch;
}

/*******************************************************************************
*
*	protostore
*
*	Store (possible new) realm credentials.
*/
void
protostore(DmOpenRef db,
           arguments *args)
{
	Boolean new=false;
	UInt16 crdidx;
	realmpacked rr;

	crdidx=findcredentials(db, args);
	if (crdidx==dmMaxRecordIndex)
		new=true;

	qpack(&rr, args->realm, args->login, args->password);
	storerecord(db, &rr, crdidx);
	FrmCustomAlert(alertcredentials, args->realm, (new==true) ? "set" : "updated", NULL);
	MemHandleFree(rr.mh);
}

/*******************************************************************************
*
*	protodelete
*
*	Delete realm credentials.
*/
void
protodelete(DmOpenRef db,
            arguments *args)
{
	UInt16 crdidx;

	crdidx=findcredentials(db, args);
	if (crdidx!=dmMaxRecordIndex)
		DmRemoveRecord(db, crdidx);

	FrmCustomAlert(alertcredentials, args->realm, "deleted", NULL);
}
