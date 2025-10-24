#include "sit.h"

/* protos */
static UInt16 chrecspwd(DmOpenRef, blowfishkey *, blowfishkey *);

/*******************************************************************************
*
*	getpassword
*
*	Get password from user.
*/
void
getpassword(UInt16 func)
{
	UInt16 gf;

	StrNCopy(formtitle, "Enter Password", FORMTITLELEN);

	switch (prefs.pinidx) {
	case PINSHUFFLE:
		gf=initshuffle(func);
		break;
	case PINPHONEPAD:
		gf=initphonepad(func);
		break;
	case PINCALCULATOR:
		gf=initcalculator(func);
		break;
	case PINTEXTUAL:
		/* FALL-THROUGH */
	default:
		gf=inittextual(func);
		break;
	}

	FrmGotoForm(gf);
}

/*******************************************************************************
*
*	doublepassword
*
*	Check if two passwords are equal.   Annoy user if not.
*/
Int16
doublepassword(UInt8 *p1,
               UInt8 *p2)
{
	Int16 c;

	c=StrNCompare(p1, p2, PASSWORDLEN);
	if (c) {
		FrmCustomAlert(alertpwdmismatch, NULL, NULL, NULL);
		return -1;
	}

	return 0;
}

/*******************************************************************************
*
*	singlepassword
*
*	Check if single password is valid.
*/
Int16
singlepassword(UInt8 *p1)
{
	UInt32 lmd5[4], s;
	UInt16 card;
	LocalID dbid;
	Err err;

	md5(lmd5, p1);
	s=lmd5[0]-md5pwd[0];
	s|=(lmd5[1]-md5pwd[1]);
	s|=(lmd5[2]-md5pwd[2]);
	s|=(lmd5[3]-md5pwd[3]);
	if (s) {	/* error */
		FrmCustomAlert(alertillcred, NULL, NULL, NULL);
		if (kaboom) {
			kaboom--;
			if (!kaboom) {	/* blow up */
				err=DmOpenDatabaseInfo(sitdb, &dbid, NULL, NULL, &card, NULL);
				if (err)	/* fatal, just lock */
					for (EVER);

				DmCloseDatabase(sitdb);
				sitdb=NULL;
				err=DmDeleteDatabase(card, dbid);
				if (err)	/* another fatal, just lock */
					for (EVER);

				/* Bye ... */
				SysReset();
				for (EVER);
			}
		}

		return -1;
	}

	return generatekey(&mainkeyh, p1);
}

/*******************************************************************************
*
*	setpwdindb
*
*	Set password in database.
*/
Int16
setpwdindb(UInt16 silent)
{
	Err err;
	UInt16 card, p;
	LocalID dbid, appid;
	aiblock *ai;

	err=DmOpenDatabaseInfo(sitdb, &dbid, NULL, NULL, &card, NULL);
	if (err) {
		if (!silent)
			FrmCustomAlert(alertnodbi, NULL, NULL, NULL);

		return -1;
	}

	err=DmDatabaseInfo(card, dbid, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &appid, NULL, NULL, NULL);
	if (err) {
		if (!silent)
			FrmCustomAlert(alertnodbi, NULL, NULL, NULL);

		return -1;
	}

	ai=(aiblock *)MemLocalIDToLockedPtr(appid, card);
	p=1;
	DmWrite(ai, OffsetOf(aiblock, pwdgiven), &p, sizeof(UInt16));
	DmWrite(ai, OffsetOf(aiblock, md5), &md5pwd, sizeof(UInt32)*4);
	DmSetDatabaseInfo(card, dbid, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &appid, NULL, NULL, NULL);
	MemPtrUnlock(ai);

	pwdgiven=1;
	return 0;
}

/*******************************************************************************
*
*	changepassword
*
*	Changes password on the database.
*/
void
changepassword(UInt8 *p)
{
	MemHandle newkeyh=NULL;
	UInt32 localmd5[4], s;
	Int16 err;
	UInt16 dead;
	blowfishkey *mainkey, *newkey;
	UInt8 buffer[12];
	EventType ev;

	/* this will be set in database ... */
	md5(localmd5, p);

	if (!nopwd) {	/* normal usage */
		/* is new and old password equal? */
		s=localmd5[0]-md5pwd[0];
		s|=localmd5[1]-md5pwd[1];
		s|=localmd5[2]-md5pwd[2];
		s|=localmd5[3]-md5pwd[3];
		if (!s) {	/* they're equal */
			FrmCustomAlert(alertpwdoldeqnew, NULL, NULL, NULL);
			return;
		}

		/* do we really want to do this? */
		err=FrmCustomAlert(alertlastchance, NULL, NULL, NULL);
		if (err==1)
			return;

		err=generatekey(&newkeyh, p);
		if (err) {	/* bummer! */
			FrmCustomAlert(alertoom, NULL, NULL, NULL);
			return;
		}

		/* gasp, go ahead, change password on all secrets */
		mainkey=MemHandleLock(mainkeyh);
		newkey=MemHandleLock(newkeyh);

		dead=chrecspwd(sitdb, mainkey, newkey);
		if (dead) {
			StrPrintF(buffer, "%i", dead);
			FrmCustomAlert(alertdeadentry, buffer, (dead==1) ? "entry" : "entries", "secrets");
		}

		dead=chrecspwd(tmpldb, mainkey, newkey);
		if (dead) {
			StrPrintF(buffer, "%i", dead);
			FrmCustomAlert(alertdeadentry, buffer, (dead==1) ? "entry" : "entries", "templates");
		}

		MemHandleUnlock(newkeyh);
		MemHandleFree(mainkeyh);
		mainkeyh=newkeyh;
	} else {
		err=generatekey(&mainkeyh, p);
		if (err) {
			FrmCustomAlert(alertoom, NULL, NULL, NULL);
			MemSet(&ev, sizeof(ev), 0);
			ev.eType=appStopEvent;
			EvtAddEventToQueue(&ev);
			return;
		}
	}

	/* fix md5 sum in database */
	md5pwd[0]=localmd5[0];
	md5pwd[1]=localmd5[1];
	md5pwd[2]=localmd5[2];
	md5pwd[3]=localmd5[3];
	setpwdindb(0);
	nopwd=0;
}

/*******************************************************************************
*
*	chrecspwd
*
*	Change password on records in a database.
*/
static UInt16
chrecspwd(DmOpenRef db,
          blowfishkey *oldkey,
          blowfishkey *newkey)
{
	UInt16 recs=DmNumRecords(db), dead=0, lauf=1, rno;
	UInt32 offset, cnt;
	MemHandle rech;
	UInt8 *recp;
	UInt8 buffer[64];
	recdesc rd;

	if (!recs)
		return 0;	/* nothing to do ... */

	working(FrmGetActiveForm(), recs);
	for (rno=0; rno<recs; rno++) {
		StrPrintF(buffer, "Working on %i of %i ...", lauf, recs);
		lauf++;
		updatevisual(buffer);
		
		rech=DmGetRecord(db, rno);
		if (!rech) {
			dead++;
			continue;
		}

		recp=MemHandleLock(rech);
		recorddesc(&rd, recp);

		offset=2;
		for (cnt=0; cnt<(rd.secretlen/8); cnt++) {
			MemMove(buffer, rd.secret, 8);
			blowfishdecrypt(oldkey, buffer);
			blowfishencrypt(newkey, buffer);
			DmWrite(recp, offset, buffer, 8);
			rd.secret+=8;
			offset+=8;
		}

		MemHandleUnlock(rech);
		DmReleaseRecord(db, rno, true);
	}

	stopworking();
	return dead;
}
