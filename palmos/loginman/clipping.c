#include "loginman.h"

/* protos */
static Boolean clippercheck(UInt16 *, LocalID *);
static UInt8 *getvalue(UInt8 *, UInt8 *);
static Boolean parseargs(arguments *, UInt8 *, UInt16);

/*******************************************************************************
*
*	clippercheck
*
*	Is this device capable of doing wireless communication?
*/
static Boolean
clippercheck(UInt16 *card,
             LocalID *dbid)
{
	Err err;
	DmSearchStateType state;

	err=DmGetNextDatabaseByTypeCreator(true, &state, sysFileTApplication, sysFileCClipper, true, card, dbid);
	if (err==errNone)
		return true;

	FrmCustomAlert(alertclipper, NULL, NULL, NULL);
	return false;
}

/*******************************************************************************
*
*	getvalue
*
*	Return value to specified key.
*/
static UInt8 *
getvalue(UInt8 *src,
         UInt8 *key)
{
	UInt16 klen=StrLen(key);
	UInt8 *p;

	p=StrStr(src, key);
	if (p)
		return p+klen;

	return NULL;
}

/*******************************************************************************
*
*	parseargs
*
*	Parses argument list - does necessary urldecoding.
*/
static Boolean
parseargs(arguments *args,
          UInt8 *argstr,
          UInt16 arglen)
{
	Boolean err;
	UInt8 *src;

	/* find start of arguments */
	src=StrStr(argstr, "?");
	if (!src)
		return false;

	/* get protocol (realm), login, password and url */
	err=getprotorealm(args, src);
	if (err==false)
		return false;

	args->login=getvalue(src, "L=");
	args->password=getvalue(src, "P=");
	args->url=getvalue(src, "U=");
	
	/* nuke delimiters */
	for (; *src; src++) {
		if (*src=='&')
			*src='\x00';
	}

	/* decode the rest */
	urldecode(args->realm);
	urldecode(args->login);
	urldecode(args->password);
	urldecode(args->url);
	
	if (args->realm) {
		if (StrLen(args->realm)>0)
			return true;
	}

	FrmCustomAlert(alertnorealm, NULL, NULL, NULL);
	return false;
}

/*******************************************************************************
*
*	popup
*
*	Popup dialog if needed.
*/
Boolean
popup(UInt8 *realm,
      UInt8 *login,
      UInt8 *password,
      MemHandle *rh,
      MemHandle *lh,
      MemHandle *ph)
{
	Boolean goon=true;
	FormPtr form;
	UInt16 button;

	if (!login || !password) {
		form=FrmInitForm(formcredentials);
		FrmSetActiveForm(form);

		setfieldhandle(form, cfrealm, realm);
		setfieldhandle(form, cflogin, login);
		setfieldhandle(form, cfpassword, password);

		button=FrmDoDialog(form);
		switch (button) {
		case cfok:
			break;
		case cfcancel:
			/* FALL-THRU */
		default:
			goon=false;
			break;
		}

		*rh=clonestring(FldGetTextPtr(getformobject(form, cfrealm)));
		*lh=clonestring(FldGetTextPtr(getformobject(form, cflogin)));
		*ph=clonestring(FldGetTextPtr(getformobject(form, cfpassword)));

		FrmEraseForm(form);
		FrmDeleteForm(form);
	} else {
		*rh=clonestring(realm);
		*lh=clonestring(login);
		*ph=clonestring(password);
	}

	return goon;
}

/*******************************************************************************
*
*	clipping
*
*	Main for webclipping functions.
*/
void
clipping(DmOpenRef db,
         UInt8 *argstr,
         Boolean palmcall)
{
	struct {
		UInt16 function;
		Boolean flag;
		UInt8 *name;
	} funcmethod[]={
		{ PLOGIN, false, "LOGIN" },
		{ PSTORE, true, "STORE" },
		{ PDELETE, true, "DELETE" },
		{ 0, false },
	};
	UInt16 arglen=StrLen(argstr), idx, card;
	LocalID dbid;
	MemHandle amh, reply;
	arguments args;
	Boolean goon;
	UInt8 *ap;

	goon=clippercheck(&card, &dbid);
	if (goon==false)
		return;

	reply=NULL;
	amh=clonestring(argstr);
	
	goon=parseargs(&args, MemHandleLock(amh), arglen);
	if (goon) {
		goon=false;
		for (idx=0; funcmethod[idx].function; idx++) {
			if (funcmethod[idx].function==args.protocol) {
				if (palmcall==funcmethod[idx].flag)
					goon=true;
				else
					FrmCustomAlert(alertmethod, funcmethod[idx].name, (funcmethod[idx].flag==true) ? "palmcall" : "palm", NULL);
				break;
			}
		}
	}

	if (goon) {
		if (args.protocol==PLOGIN) {
			reply=protologin(db, &args);
			if (reply) {
				MemHandleFree(amh);

				ap=MemHandleLock(reply);
				MemPtrSetOwner(ap, 0);
				SysUIAppSwitch(card, dbid, sysAppLaunchCmdGoToURL, ap);
				return;
			}
		} else if (args.protocol==PSTORE)
			protostore(db, &args);
		else if (args.protocol==PDELETE)
			protodelete(db, &args);
	}

	MemHandleFree(amh);
}
