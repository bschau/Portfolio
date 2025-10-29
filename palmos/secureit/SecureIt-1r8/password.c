#include "sit.h"

/* protos */
static Int16 setpwdindb(void);

/*******************************************************************************
*
*	getpassword
*
*	Get password from user.
*
*	Input:
*		func - function (Single or Double).
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
*
*	Input:
*		p1 - password 1.
*		p2 - password 2.
*	Output:
*		s: 0 (match).
*		f: !0 (no match).
*/
Int16
doublepassword(Char *p1,
               Char *p2)
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
*
*	Input:
*		p1 - password 1.
*/
Int16
singlepassword(Char *p1)
{
	unsigned long lmd5[4], s;
	UInt16 card;
	LocalID dbid;
	Err err;
	blowfishkey *k;

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

	k=MemHandleLock(mainkeyh);
	blowfishmakekey(k, p1, StrLen(p1));
	MemHandleUnlock(mainkeyh);

	return 0;
}

/*******************************************************************************
*
*	changedbpwd
*
*	Changes database password.   This is a biggie.
*
*	Input:
*		pwd - new password.
*	Output:
*		s: 0.
*		f: !0.
*/
void
changedbpwd(Char *pwd)
{
	MemHandle oldmainkeyh=mainkeyh, wkeyh;
	Int16 err;
	UInt16 recs, rno, dead, p;
	unsigned long lmd5pwd[4], s;
	MemHandle rh;
	record *rp, r;
	unsigned char bs[BSSIZE];
	blowfishkey *wkeyp;

	md5(&lmd5pwd[0], pwd);
	s=lmd5pwd[0]-md5pwd[0];
	s|=lmd5pwd[1]-md5pwd[1];
	s|=lmd5pwd[2]-md5pwd[2];
	s|=lmd5pwd[3]-md5pwd[3];
	if (!s) {	/* old = new */
		FrmCustomAlert(alertpwdoldeqnew, NULL, NULL, NULL);
		return;
	}

	if (pwdgiven) {
		p=FrmCustomAlert(alertlastchance, NULL, NULL, NULL);
		if (p==1)
			return;
	}

	wkeyh=MemHandleNew(sizeof(blowfishkey));
	if (!wkeyh) {
		FrmCustomAlert(alertoom, NULL, NULL, NULL);
		return;
	}
	
	md5pwd[0]=lmd5pwd[0];
	md5pwd[1]=lmd5pwd[1];
	md5pwd[2]=lmd5pwd[2];
	md5pwd[3]=lmd5pwd[3];
	err=setpwdindb();
	if (err) {
		MemHandleFree(wkeyh);
		return;
	}

	wkeyp=MemHandleLock(wkeyh);
	blowfishmakekey(wkeyp, pwd, StrLen(pwd));
	MemHandleUnlock(wkeyh);
	MemSet(pwd, PASSWORDLEN, 0);

	dead=0;
	recs=DmNumRecords(sitdb);
	for (rno=0; rno<recs; rno++) {
		rh=DmGetRecord(sitdb, rno);
		if (!rh) {
			dead++;
			continue;
		}

		rp=MemHandleLock(rh);
		MemMove(bs, rp, BSSIZE);

		mainkeyh=oldmainkeyh;
		recunpack(&r, bs);
		mainkeyh=wkeyh;
		recpack(bs, r.key, r.txt);
		
		DmWrite(rp, 0, bs, BSSIZE);
		MemHandleUnlock(rh);
		DmReleaseRecord(sitdb, rno, true);
	}

	mainkeyh=wkeyh;
	MemHandleFree(oldmainkeyh);

	if (dead) {
		StrPrintF(bs, "%d", dead);
		FrmCustomAlert(alertdeadentry, bs, NULL, NULL);
	}
}

/*******************************************************************************
*
*	setpwdindb
*
*	Set password in database.
*
*	Output:
*		s: 0.
*		f: !0.
*/
static Int16
setpwdindb(void)
{
	Err err;
	UInt16 card, p;
	LocalID dbid, appid;
	aiblock *ai;

	err=DmOpenDatabaseInfo(sitdb, &dbid, NULL, NULL, &card, NULL);
	if (err) {
		FrmCustomAlert(alertnodbi, NULL, NULL, NULL);
		return -1;
	}

	err=DmDatabaseInfo(card, dbid, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &appid, NULL, NULL, NULL);
	if (err) {
		FrmCustomAlert(alertnodbi, NULL, NULL, NULL);
		return -1;
	}

	ai=(aiblock *)MemLocalIDToLockedPtr(appid, card);
	p=1;
	DmWrite(ai, OffsetOf(aiblock, pwdgiven), &p, sizeof(UInt16));
	DmWrite(ai, OffsetOf(aiblock, md5), &md5pwd, sizeof(unsigned long)*4);
	DmSetDatabaseInfo(card, dbid, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &appid, NULL, NULL, NULL);
	MemPtrUnlock(ai);

	pwdgiven=1;
	return 0;
}
