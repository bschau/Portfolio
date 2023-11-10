#include "loginman.h"

/*******************************************************************************
*
*	getformobject
*
*	Return pointer to object.
*/
void *
getformobject(FormPtr form,
              UInt16 obj)
{
	if (!form)
		form=FrmGetActiveForm();

	return FrmGetObjectPtr(form, FrmGetObjectIndex(form, obj));
}

/*******************************************************************************
*
*	refocus
*
*	Refocus given a table.
*/
Boolean
refocus(FormPtr form,
        refocustab *rt)
{
	Int16 fidx=FrmGetObjectId(form, FrmGetFocus(form));
	UInt16 i;

	for (i=0; rt[i].this; i++) {
		if (rt[i].this==fidx) {
			FrmSetFocus(form, FrmGetObjectIndex(form, rt[i].other));
			return true;
		}
	}

	return false;
}

/*******************************************************************************
*
*	isfieldblank
*
*	Check to see if field is blank.
*/
Boolean
isfieldblank(UInt8 *fld)
{
	if (!fld)
		return true;	/* yes, field is blank */

	while (*fld) {
		if (*fld>' ')
			return false;	/* no, field is not blank */

		fld++;
	}

	return true;	/* yes, field is blank */
}

/*******************************************************************************
*
*	clonestring
*
*	Clone a string.
*/
MemHandle
clonestring(UInt8 *src)
{
	MemHandle mh=NULL;
	UInt16 len;
	UInt8 *mp;

	if (src) {
		len=StrLen(src);
		mh=MemHandleNew(len+1);
		ErrFatalDisplayIf(mh==NULL, "(clonestring) Out of memory");

		mp=MemHandleLock(mh);
		MemMove(mp, src, len);
		*(mp+len)='\x00';
		MemHandleUnlock(mh);
	}

	return mh;
}

/*******************************************************************************
*
*	setfieldtext
*
*	Sets and redraw a field text.
*/
void
setfieldtext(FormPtr form,
             UInt16 fldobj,
             UInt8 *txt)
{
	FieldType *fld;

	fld=getformobject(form, fldobj);
	FldSetTextPtr(fld, txt);
	FldRecalculateField(fld, true);
}

/*******************************************************************************
*
*	setfieldhandle
*
*	Sets text as a field handle.
*/
void
setfieldhandle(FormPtr form,
               UInt16 fldobj,
               UInt8 *txt)
{
	FieldType *fld=getformobject(form, fldobj);
	MemHandle mh, oh;
	UInt16 len;
	UInt8 *mp;

	if (txt) {
		len=StrLen(txt);
		mh=MemHandleNew(len+1);
		ErrFatalDisplayIf(mh==NULL, "(setfieldhandle) Out of memory");

		mp=MemHandleLock(mh);
		MemMove(mp, txt, len);
		*(mp+len)='\x00';
		MemHandleUnlock(mh);

		oh=FldGetTextHandle(fld);
		FldSetTextHandle(fld, mh);

		if (oh)
			MemHandleFree(oh);
	}
}

/*******************************************************************************
*
*	isxdigit
*
*	Is char a hexdigit?
*/
Boolean
isxdigit(UInt8 digit)
{
	UInt8 d=toupper(digit);

	if (d>='0' && d<='9') 
		return true;

	if (d>='A' && d<='F')
		return true;

	return false;
}

/*******************************************************************************
*
*	isalnum
*
*	Is char an alphanumeric char?
*/
Boolean
isalnum(UInt8 cu)
{
	UInt8 c=toupper(cu);

	if (c>='A' && c<='Z')
		return true;

	if (c>='0' && c<='9')
		return true;

	return false;
}

/*******************************************************************************
*
*	htoi
*
*	Convert two hex digits to integer.
*/
UInt16
htoi(UInt8 *src)
{
	UInt16 v1, v2, c;

	c=toupper(*src);
	if (c>='0' && c<='9')
		v2=c-'0';
	else if (c>='A' && c<='F')
		v2=c-'A'+10;
	else
		v2=0;

	v1=v2<<4;
	src++;

	c=toupper(*src);
	if (c>='0' && c<='9')
		v2=c-'0';
	else if (c>='A' && c<='F')
		v2=c-'A'+10;
	else
		v2=0;

	v1|=v2;

	return v1;
}
