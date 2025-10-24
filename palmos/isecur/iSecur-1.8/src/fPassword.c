/*
 * fPassword.c
 */
#include "Include.h"

/* protos */
_PWD(static Boolean EH(EventType *));
_PWD(static void SetFont(void));
_PWD(static void SetupString(UInt16));
_PWD(static void DrawTitle(void));
_PWD(static Boolean OnRetentionList(Char *));
_PWD(static void GenPwd(void));
_PWD(static void UpgradePwd(Char *));
_PWD(static UInt16 PopupKeyboard(UInt16));

/* globals */
static UInt16 fPasswordBand[]={ cPasswordRPwd, cPasswordRConfirm, 0 };
static RGBColorType colorRed={ 0, 255, 0, 0 };
static UInt8 v2ExtraChars[]="1234567890abcdefghijklmnopqrstuvwxyz";
extern UInt16 dialogType, maxAttempts, errY, titleIdx;
extern UInt8 gKey[PasswordLength];
extern Boolean runLoop, pwdValid;
extern Int32 timeOut, deleteStringDelay;
extern MemHandle aesData, sha256Data;
extern Prefs prefs;
extern UInt16 libRefNum, kbdRefNum, srcVersion;
extern Char loginDigest[SHA256DigestLength];
extern UI *ui;
extern DB dbData;
extern AppInfoBlock *appInfo;
extern Char gPwd[];

/*
 * fPasswordRun
 *
 * Show the "Enter Password" screen.
 *
 *  -> type		What kind of dialog is needed?
 *  -> ai		AppInfo block
 *  -> max		Max attempts (0 = unlimited, only for PwdDialogNormal).
 *  -> tIdx		Title index string (0xffff to not have a title).
 *  -> cancel		Show Cancel button (only for PwdDialogConfirm).
 *  -> dbVersion	Database version.
 *
 * Returns true if password is valid, false otherwise. False is also returned
 * if appStopped=true.
 */
/*
 * <-  pwdDst		Password destination.
Boolean
fPasswordRun(UInt16 type, Char *pwdDst, AppInfoBlock *ai, UInt16 max, UInt16 tIdx, Boolean cancel, UInt16 dbVersion)
*/
Boolean
fPasswordRun(UInt16 type, AppInfoBlock *ai, UInt16 max, UInt16 tIdx, Boolean cancel, UInt16 dbVersion)
{
	FormType *saveCurrent=currentForm;
	UInt16 saveID=currentFormID;
	FormActiveStateType frmSave;
	EventType ev;
	UInt16 err;

	fIconClose();

	MemSet(gPwd, PasswordLength+2, 0);

	dialogType=type;
	//dstPwd=pwdDst;
	appInfo=ai;
	maxAttempts=max;
	titleIdx=tIdx;
	pwdValid=false;
	srcVersion=dbVersion;
	FrmSaveActiveState(&frmSave);

	currentFormID=fPassword;
	currentForm=FrmInitForm(currentFormID);
	ErrFatalDisplayIf(currentForm==NULL, "(fPasswordRun) Cannot initialize form.");
	FrmSetActiveForm(currentForm);
	FrmSetEventHandler(currentForm, (FormEventHandlerType *)EH);

	if (dialogType==PwdDialogConfirm) {
		UIObjectShow(cPasswordRPwd);
		UIObjectShow(cPasswordRPwdLabel);
		UIObjectShow(cPasswordRConfirm);
		UIObjectShow(cPasswordRConfirmLabel);
		UIFieldFocus(cPasswordRPwd);
		UIObjectShow(cPasswordGenerate);
		if (kbdRefNum!=sysInvalidRefNum) {
			UIObjectShow(cPasswordRPwdKbd);
			UIObjectShow(cPasswordRConfirmKbd);
		}
		errY=126;
	} else {
		UIObjectShow(cPasswordPwd);
		UIObjectShow(cPasswordPwdLabel);
		UIObjectShow(cPasswordHint);
		UIFieldFocus(cPasswordPwd);
		if (kbdRefNum!=sysInvalidRefNum)
			UIObjectShow(cPasswordPwdKbd);

		errY=120;
	}

	if (cancel)
		UIObjectShow(cPasswordCancel);

	SetFont();

	CtlSetValue(UIObjectGet(cPasswordPwdDot+(prefs.flags&PFlgPwdShowText)), 1);

	MemSet(&ev, sizeof(EventType), 0);
	ev.eType=frmOpenEvent;
	ev.data.frmLoad.formID=currentFormID;
	EvtAddEventToQueue(&ev);
	
	runLoop=true;
	timeOut=evtWaitForever;
	deleteStringDelay=-1;
	while (runLoop) {
		EvtGetEvent(&ev, timeOut);
		if (ev.eType==appStopEvent) {
			EvtAddEventToQueue(&ev);
			appStopped=true;
			break;
		} else if (ev.eType==frmCloseEvent)
			break;

		if (PMHardPower(&ev)==true)
			continue;

		if (deleteStringDelay==0) {
			RectangleType r;

			timeOut=evtWaitForever;
			RctSetRectangle(&r, 0, errY, 160, 12);
			WinEraseRectangle(&r, 0);
		} else if (deleteStringDelay>0)
			deleteStringDelay--;

		if (SysHandleEvent(&ev)==true ||
		    MenuHandleEvent(0, &ev, &err)==true)
			continue;

		FrmDispatchEvent(&ev);
	}

	UIFieldClear(cPasswordPwd);
	UIFieldClear(cPasswordRPwd);
	UIFieldClear(cPasswordRConfirm);

	FrmEraseForm(currentForm);
	FrmDeleteForm(currentForm);
	FrmRestoreActiveState(&frmSave);
	currentForm=saveCurrent;
	currentFormID=saveID;

	fIconOpen();
	return pwdValid;
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	MemHandle mh;
	ControlType *ctl;

	EntropyAdd(pool, ev);

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		if ((dialogType==PwdDialogNormal) && (prefs.flags&PFlgAutoPopupKbd))
			CtlHitControl(UIObjectGet(cPasswordPwdKbd));

	case frmUpdateEvent:
		mh=DmGetResource(iconType, 1000);
		ErrFatalDisplayIf(mh==NULL, "(EH) Logo bitmap not found");
		WinDrawBitmap(MemHandleLock(mh), 64, dialogType==PwdDialogConfirm ? 1 : 20);
		MemHandleUnlock(mh);
		DmReleaseResource(mh);
		DrawTitle();
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cPasswordOK:
			if (dialogType==PwdDialogConfirm) {
				Char *p1=UIFieldGetText(cPasswordRPwd), *p2=UIFieldGetText(cPasswordRConfirm);

				if (p1 && (StrLen(p1)>0) && p2 && (StrLen(p2)>0) && (StrCompare(p1, p2)==0)) {
					if (OnRetentionList(p1)==false) {
//						fPasswordRunMullify(dstPwd, p1);
						fPasswordRunMullify(p1);
						pwdValid=true;
						runLoop=false;
					} else
						SetupString(3);
				} else
					SetupString(0);
			} else {
				Char *p=UIFieldGetText(cPasswordPwd);

				if (p && (StrLen(p)>0)) {
//					fPasswordRunMullify(dstPwd, p);
					fPasswordRunMullify(p);
					SHA256Init();
//					SHA256Transform(dstPwd, PasswordLength);
					SHA256Transform(gPwd, PasswordLength);
					SHA256Final();

					if (MemCmp(appInfo->digest, MemHandleLock(sha256Data), SHA256DigestLength)==0) {
						MemHandleUnlock(sha256Data);
						if (srcVersion==1) {
							UpgradePwd(p);
//							fPasswordRunMullify(dstPwd, p);
							fPasswordRunMullify(p);
						} else
							MemMove(loginDigest, appInfo->digest, 32);
						pwdValid=true;
						runLoop=false;
					} else {
						MemHandleUnlock(sha256Data);
						UIFieldClear(cPasswordPwd);
						UIFieldFocus(cPasswordPwd);
						SetupString(1);
						if (maxAttempts) {
							maxAttempts--;
							if (maxAttempts==0)
								runLoop=false;
						}
					}
				} else
					SetupString(2);
			}
			return true;

		case cPasswordCancel:
// 			*dstPwd=0;
			*gPwd=0;
			pwdValid=false;
			runLoop=false;
			return true;

		case cPasswordPwdDot:
			prefs.flags&=~PFlgPwdShowText;
			SetFont();
			return true;

		case cPasswordPwdTxt:
			prefs.flags|=PFlgPwdShowText;
			SetFont();
			return true;

		case cPasswordGenerate:
			GenPwd();
			return true;

		case cPasswordHint:
			if ((mh=AIBExtensionGet(AIBPasswordHint))) {
				Char *p=MemHandleLock(mh);

				if (*p) {
					FrmCustomAlert(aPasswordHint, p, NULL, NULL);
					MemHandleFree(mh);
					return true;
				}
				MemHandleFree(mh);
			}

			FrmAlert(aNoPasswordHint);
			return true;

		case cPasswordRPwdKbd:
			PopupKeyboard(cPasswordRPwd);
			return true;

		case cPasswordRConfirmKbd:
			PopupKeyboard(cPasswordRConfirm);
			return true;

		case cPasswordPwdKbd:
			if (PopupKeyboard(cPasswordPwd)==kbfCloseOK) 
				CtlHitControl(UIObjectGet(cPasswordOK));
			return true;
		}
		break;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mPasswordGenerate:
			if (dialogType==PwdDialogConfirm)
				GenPwd();
			return true;
		case mPasswordMask:
			ctl=UIObjectGet(cPasswordPwdDot);
			CtlHitControl(ctl);
			CtlSetValue(ctl, 1);
			CtlSetValue(UIObjectGet(cPasswordPwdTxt), 0);
			return true;
		case mPasswordUnmask:
			ctl=UIObjectGet(cPasswordPwdTxt);
			CtlHitControl(ctl);
			CtlSetValue(ctl, 1);
			CtlSetValue(UIObjectGet(cPasswordPwdDot), 0);
			return true;
		}
		break;
		
	case keyDownEvent:
		if (dialogType==PwdDialogConfirm) {
			if (UIFieldRefocus(fPasswordBand, ev->data.keyDown.chr)==false) {
				if (ev->data.keyDown.chr==chrLineFeed) {
					ev->data.keyDown.chr=nextFieldChr;
					return UIFieldRefocus(fPasswordBand, ev->data.keyDown.chr);
				}
			}
		} else {
			if (ev->data.keyDown.chr==chrLineFeed) {
				CtlHitControl(UIObjectGet(cPasswordOK));
				return true;
			}
		}

	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * SetFont
 *
 * Set font in fields.
 */
static void
SetFont(void)
{
	if (prefs.flags&PFlgPwdShowText) {
		FldSetFont(UIObjectGet(cPasswordPwd), stdFont);
		FldSetFont(UIObjectGet(cPasswordRPwd), stdFont);
		FldSetFont(UIObjectGet(cPasswordRConfirm), stdFont);
	} else {
		FldSetFont(UIObjectGet(cPasswordPwd), dotFont);
		FldSetFont(UIObjectGet(cPasswordRPwd), dotFont);
		FldSetFont(UIObjectGet(cPasswordRConfirm), dotFont);
	}
}

/*
 * SetupString
 *
 * Display string and fire off timer.
 *
 *  -> idx		Index of string to display.
 */
static void
SetupString(UInt16 idx)
{
	IndexedColorType ict=0;
	Char str[64];
	RectangleType r;
	UInt16 len, w;

	SysStringByIndex(strPasswordDialog, idx, str, sizeof(str));

	RctSetRectangle(&r, 0, errY, 160, 12);
	WinEraseRectangle(&r, 0);
	len=StrLen(str);
	w=FntCharsWidth(str, len);
	deleteStringDelay=3*SysTicksPerSecond();
	timeOut=1;
	if (ui->colorEnabled)
		ict=WinSetTextColor(WinRGBToIndex(&colorRed));

	WinDrawChars(str, len, (160-w)/2, r.topLeft.y);
	if (ui->colorEnabled)
		WinSetTextColor(ict);
}

/*
 * DrawTitle
 *
 * Draw the title string.
 */
static void
DrawTitle(void)
{
	if (titleIdx!=0xffff) {
		FontID fID=FntSetFont(largeFont);
		Char str[64];
		RectangleType r;
		UInt16 len, w;

		SysStringByIndex(strPasswordDialogTitle, titleIdx, str, sizeof(str));

		RctSetRectangle(&r, 0, dialogType==PwdDialogConfirm ? 28 : 52, 160, 12);
		WinEraseRectangle(&r, 0);
		len=StrLen(str);
		w=FntCharsWidth(str, len);
		WinDrawChars(str, len, (160-w)/2, r.topLeft.y);
		FntSetFont(fID);
	}
}

/*
 * OnRetentionList
 *
 * Check if password is on retention list.
 *
 *  -> passwd		Password.
 *
 * Returns true if this password is on the retention list.
 */
static Boolean
OnRetentionList(Char *passwd)
{
	UInt32 dLen=sizeof(UInt16)+(32*prefs.pwdRetention);
	UInt16 pIdx=0, dIdx=0, cnt;
	UInt8 pwd[PasswordLength];
	MemHandle rListH;
	UInt16 *rListCnt;
	UInt8 *rList, *d, *digest;

	if (prefs.pwdRetention==0)
		return false;

	rListH=MemHandleNew(dLen);
	ErrFatalDisplayIf(rListH==NULL, "(OnRetentionList) Out of memory");
	rListCnt=MemHandleLock(rListH);
	rList=((UInt8 *)rListCnt)+sizeof(UInt16);

	if (PMGetPref(rListCnt, dLen, PrfRetentionList, false)==false) {
		MemSet(rListCnt, dLen, 0);
		MemMove(rList, loginDigest, 32);
		*rListCnt=1;
	}

	/* FIXME: this should go through Mullify!!!! */
	for (cnt=0; cnt<PasswordLength; cnt++) {
		pwd[dIdx++]=passwd[pIdx++];
		if (!passwd[pIdx])
			pIdx=0;
	}

	SHA256Init();
	SHA256Transform(pwd, PasswordLength);
	SHA256Final();

	MemSet(pwd, sizeof(pwd), 0);

	d=rList;
	digest=MemHandleLock(sha256Data);
	for (cnt=0; cnt<*rListCnt; cnt++) {
		if (MemCmp(d, digest, SHA256DigestLength)==0) {
			MemSet(MemHandleLock(rListH), MemHandleSize(rListH), 0);
			MemHandleFree(rListH);
			MemHandleUnlock(sha256Data);
			return true;
		}
		d+=SHA256DigestLength;
	}

	if (*rListCnt<prefs.pwdRetention) {
		MemMove(rList+(*rListCnt*SHA256DigestLength), digest, SHA256DigestLength);
		(*rListCnt)++;
	} else {
		d=rList;
		for (cnt=1; cnt<prefs.pwdRetention; cnt++) {
			MemMove(d, d+SHA256DigestLength, SHA256DigestLength);
			d+=SHA256DigestLength;
		}

		MemMove(rList+((prefs.pwdRetention-1)*SHA256DigestLength), digest, SHA256DigestLength);
	}
	MemHandleUnlock(sha256Data);

	PMSetPref(rListCnt, dLen, PrfRetentionList, false);
	MemSet(MemHandleLock(rListH), MemHandleSize(rListH), 0);
	MemHandleFree(rListH);
	return false;
}

/**
 * Generate password.
 */
static void
GenPwd(void)
{
//	Char pwd[GeneratePasswordLength+2];

//	MemSet(pwd, sizeof(pwd), 0);
//	if (fPwdGenRun(pwd)==true) {
	if (fPwdGenRun()==true) {
		UIFieldSetText(cPasswordRPwd, gPwd);
		UIFieldSetText(cPasswordRConfirm, gPwd);
		UIFieldFocus(cPasswordRPwd);
//		MemSet(pwd, sizeof(pwd), 0);
	}
}

/**
 * Expand password depending on version of database.
 *
 * @param srcPwd Password to mullify.
 * @note dstPwd must be PasswordLength or larger.
 */
/*
 * @param dstPwd Where to store mullified password.
void
fPasswordRunMullify(UInt8 *dstPwd, UInt8 *srcPwd)
*/
void
fPasswordRunMullify(UInt8 *srcPwd)
{
	UInt32 dIdx=0, pIdx=0;
	UInt16 cnt;

	switch (srcVersion) {
	case 1:		/* iSecur up to 1.2 - had a bug where 'a', 'aa' and so
			   on would repeat.  If you set your password to say
			   'aaaaa' you could still login using 'a'. */
		for (cnt=0; cnt<PasswordLength; cnt++) {
			gPwd[dIdx++]=srcPwd[pIdx++];
			if (!srcPwd[pIdx])
				pIdx=0;
		}
		break;
	case 2:		/* iSecur-1.3 and better, fix the above but still do
			   some stuff ... */
		for (cnt=0; cnt<PasswordLength; cnt++) {
			if (!srcPwd[pIdx]) {
				pIdx=0;
				srcPwd=v2ExtraChars;
			}
			gPwd[dIdx++]=srcPwd[pIdx++];
		}
		break;
	default:
		ErrFatalDisplay("(MullifyPwd) Invalid/unsupported database version");
	}
}

/**
 * Upgrade the password in the database.
 *
 * @param pwd Password.
 */
static void
UpgradePwd(Char *pwd)
{

	/* Extract global key */
//	fPasswordRunMullify(p, pwd);
	fPasswordRunMullify(pwd);
	AESCreateKeys(gPwd);
	MemMove(gKey, appInfo->key, AESKeyLength);
	AESDecrypt(gKey, gKey);
	AESDecrypt(gKey+16, gKey+16);

	srcVersion=DBVERSION;
	ErrFatalDisplayIf(DmSetDatabaseInfo(dbData.card, dbData.id, NULL, NULL, &srcVersion, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)!=errNone, "(UpgradePwd) Cannot upgrade database version");

	fPasswordRunMullify(pwd);
	SetPassword(appInfo, gPwd, 0);
//	MemSet(p, PasswordLength, 0);
}

/**
 * Popup and handle keyboard.
 *
 * @param dstFldId ID of field to recieve password.
 * @return close flag.
 */
static UInt16
PopupKeyboard(UInt16 dstFldId)
{
	FieldType *fld=UIObjectGet(dstFldId);
	FormActiveStateType frmSave;
	MemHandle mh, sh;
	UInt16 closeF;

	FrmSaveActiveState(&frmSave);

	KeyboardLibRun(kbdRefNum, &mh, &closeF);
	FrmRestoreActiveState(&frmSave);

	if (mh) {
		sh=FldGetTextHandle(fld);
		FldSetTextHandle(fld, mh);
		FldDrawField(fld);
		if (sh)
			MemHandleFree(sh);
	}

	UIFieldFocus(dstFldId);

	return closeF;
}
