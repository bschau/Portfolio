/*
 * fPwdGen.c
 */
#include "Include.h"

/* protos */
_PWD(static Boolean EH(EventType *));
_PWD(static void Generate(Boolean));

/* globals */
static Char specialChars[]="!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
extern UInt8 *dstPwd;
extern Boolean bPwdGen;
extern State state;
extern Char gPwd[];

/*
 * fPwdGen
 *
 * Show the password generator.
 *
 * Return true if password is to be used, false otherwise.
 */
/*
 * <- pwd		Where to store password.
 *
Boolean
fPwdGenRun(Char *pwd)
*/
Boolean
fPwdGenRun(void)
{
	UInt16 mask=1, idx;
	FormSave frm;
	ControlType *ctl;

	MemSet(gPwd, PasswordLength+2, 0);
	bPwdGen=false;

	UIFormPrologue(&frm, fPwdGen, EH);

	for (idx=cPwdGenL6; idx<=cPwdGenL20; idx++)
		CtlSetValue(UIObjectGet(idx), 0);

	CtlSetValue(UIObjectGet(cPwdGenL6+state.pwdGenLength), 1);

	for (idx=cPwdGenLower; idx<=cPwdGenSpecials; idx++) {
		ctl=UIObjectGet(idx);
		if (state.pwdGenContains&mask)
			CtlSetValue(ctl, 1);
		else
			CtlSetValue(ctl, 0);

		mask<<=1;
	}

//	Generate(passwd, false);
	Generate(false);
	UIFieldSetText(cPwdGenPassword, gPwd);

	if (UIFormEpilogue(&frm, NULL, cPwdGenCancel)==false)
		return false;

	return bPwdGen;
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	UInt16 idx;
	Char *p;

	EntropyAdd(pool, ev);

	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cPwdGenUse:
			state.pwdGenLength=0;
			for (idx=cPwdGenL6; idx<=cPwdGenL20; idx++) {
				if (CtlGetValue(UIObjectGet(idx)))
					state.pwdGenLength=idx-cPwdGenL6;
			}

			state.pwdGenContains=0;
			if (CtlGetValue(UIObjectGet(cPwdGenLower)))
				state.pwdGenContains|=1;

			if (CtlGetValue(UIObjectGet(cPwdGenUpper)))
				state.pwdGenContains|=2;

			if (CtlGetValue(UIObjectGet(cPwdGenDigits)))
				state.pwdGenContains|=4;

			if (CtlGetValue(UIObjectGet(cPwdGenSpecials)))
				state.pwdGenContains|=8;

			if (((p=UIFieldGetText(cPwdGenPassword))) && (StrLen(p)>0)) {
				bPwdGen=true;
				StrCopy(gPwd, p);
			} else {
				FrmAlert(aPwdGenNone);
				return true;
			}
			break;

		case cPwdGenGenerate:
			{
				Generate(true);
				UIFieldSetText(cPwdGenPassword, gPwd);
			}
			return true;
		}
		break;

	case frmSaveEvent:
		UIFieldClear(cPwdGenPassword);
	default:	/* FALL-THRU */
		break;
	}
	
	return false;
}

/*
 * Generate
 *
 * Generate a password.
 *
 *  -> verbose		Show alert if error?
 */
/*
 * <-  pwd		Where to store password.
static void
Generate(Char *pwd, Boolean verbose)
*/
static void
Generate(Boolean verbose)
{
	UInt16 length=6, flags=0;

	if (CtlGetValue(UIObjectGet(cPwdGenL8)))
		length=8;
	if (CtlGetValue(UIObjectGet(cPwdGenL10)))
		length=10;
	if (CtlGetValue(UIObjectGet(cPwdGenL12)))
		length=12;
	if (CtlGetValue(UIObjectGet(cPwdGenL20)))
		length=20;

	if (CtlGetValue(UIObjectGet(cPwdGenLower)))
		flags|=PGLower;

	if (CtlGetValue(UIObjectGet(cPwdGenUpper)))
		flags|=PGUpper;

	if (CtlGetValue(UIObjectGet(cPwdGenDigits)))
		flags|=PGDigits;

	if (CtlGetValue(UIObjectGet(cPwdGenSpecials)))
		flags|=PGSpecials;

	if (!flags) {
		*gPwd=0;
		if (verbose)
			FrmAlert(aPwdGenError);
	} else
		fPwdGenGenerate(length, flags);
}

/**
 * Generate a password with the given attributes.
 *
 * @param length Length of password.
 * @param flags Attributes.
 */
/*
 * @param dst Where to store password.
void
fPwdGenGenerate(Char *dst, UInt16 length, UInt16 flags)
*/
void
fPwdGenGenerate(UInt16 length, UInt16 flags)
{
	UInt16 sIdx=0, l=length, a, b;
	Char *p=gPwd;
	UInt16 randIdx[4];
	UInt8 c;

	MemSet(gPwd, PasswordLength+2, 0);
	if (flags&PGLower) {
		randIdx[sIdx++]=PGLower;
		*p++=(EntropyGet(pool)%26)+'a';
		l--;
	}

	if ((l) && (flags&PGUpper)) {
		randIdx[sIdx++]=PGUpper;
		*p++=(EntropyGet(pool)%26)+'A';
		l--;
	}

	if ((l) && (flags&PGDigits)) {
		randIdx[sIdx++]=PGDigits;
		*p++=(EntropyGet(pool)%10)+'0';
		l--;
	}

	if ((l) && (flags&PGSpecials)) {
		randIdx[sIdx++]=PGSpecials;
		*p++=specialChars[(EntropyGet(pool)%StrLen(specialChars))];
		l--;
	}

	while (l>0) {
		c=EntropyGet(pool);
		switch (randIdx[EntropyGet(pool)%sIdx]) {
		case PGLower:
			*p++=(c%26)+'a';
			break;
		case PGUpper:
			*p++=(c%26)+'A';
			break;
		case PGDigits:
			*p++=(c%10)+'0';
			break;
		default:
			*p++=specialChars[c%StrLen(specialChars)];
			break;
		}

		l--;
	}

	*p=0;

	Random(((((UInt32)EntropyGet(pool))<<24)|(((UInt32)EntropyGet(pool))<<16)|(((UInt32)EntropyGet(pool))<<8)|((UInt32)EntropyGet(pool)))^TimGetSeconds());

	for (l=0; l<length; l++) {
		a=Random(0)%length;
		b=Random(0)%length;
		c=gPwd[a];
		gPwd[a]=gPwd[b];
		gPwd[b]=c;
	}
}

/**
 * Bring up the password generator and insert result into secret.
 */
void
fPwdGenRequester(void)
{
	UInt16 id;

	if (((id=FrmGetFocus(currentForm))==noFocus) || (FrmGetObjectType(currentForm, id)!=frmFieldObj)) {
		FrmAlert(aNoFocus);
		return;
	}

	MemSet(gPwd, PasswordLength+2, 0);
	if (fPwdGenRun()==true) {
		id=FrmGetObjectId(currentForm, id);
		FldInsert(UIObjectGet(id), gPwd, StrLen(gPwd));
	}
}
