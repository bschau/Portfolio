/*******************************************************************************
*
*	utils
*
*	Utilities ...
*/
#include "p0ls.h"

/* protos */

/*******************************************************************************
*
*	findobject
*
*	Returns pointer to the object in the active form.
*
*	Input:
*		id - object id.
*	Output:
*		objectptr.
*/
MemPtr
findobject(UInt16 id)
{
	FormPtr form;
	UInt16 index;

	form=FrmGetActiveForm();
	index=FrmGetObjectIndex(form, id);

	return FrmGetObjectPtr(form, index);
}

/*******************************************************************************
 *
 *	objectinform
 *
 *	Returns pointer to the object in the given form.
 *
 *	Input:
 *		form - form.
 *		id - object id.
 *	Output:
 *		objectptr.
 */
MemPtr
objectinform(FormPtr form,
             UInt16 id)
{
	UInt16 index;

	index=FrmGetObjectIndex(form, id);
	return FrmGetObjectPtr(form, index);
}

/*******************************************************************************
 *
 *	setfieldtext
 *
 * 	Sets new text in the specified field in the specified form.
 *
 * 	Input:
 * 		form - form.
 * 		fid - field id.
 * 		txt - new text to set.
 * 	Output:
 * 		s: 0.
 * 		f: !0.
 */
int
setfieldtext(FormPtr form,
             UInt16 fid,
             Char *txt)
{
	MemHandle mh, omh;
	FieldType *fld;
	Char *k;

	mh=MemHandleNew(StrLen(txt)+1);
	if (mh) {
		k=MemHandleLock(mh);
		StrCopy(k, txt);
		MemHandleUnlock(mh);
	} else {
		FrmCustomAlert(alertoom, NULL, NULL, NULL);
		return -1;
	}

	fld=objectinform(form, fid);
	omh=FldGetTextHandle(fld);
	FldSetTextHandle(fld, mh);

	if (omh)
		MemHandleFree(omh);

	return 0;
}

/*******************************************************************************
 *
 *	getfieldtext
 *
 *	Get copy of field text.
 *
 *	Input:
 *		form - form.
 * 		fid - field id.
 * 		dst - where to store text.
 *              num - max bytes (size of dst).
 */
void
getfieldtext(FormPtr form,
             UInt16 fid,
             Char *dst,
             UInt16 num)
{
	FieldType *fld;
	Char *fp;

	fld=objectinform(form, fid);
	fp=FldGetTextPtr(fld);
	if (!fp)
		*dst='\x00';
	else {
		MemSet(dst, num, 0);
		StrNCopy(dst, fp, num);
	}
}

/*******************************************************************************
 *
 *	about
 *
 *	Handle abouts.
 *
 * 	Input:
 * 		parform - parent form.
 * 		adf - about dialog form.
 */
void
about(FormPtr parform,
      UInt16 adf)
{
	FormPtr tfrm;
	
	tfrm=FrmInitForm(adf);
	FrmSetActiveForm(tfrm);

	FrmDoDialog(tfrm);
	if (parform)
		FrmSetActiveForm(parform);

	FrmDeleteForm(tfrm);
}

/*******************************************************************************
 *
 *	globalmenu
 * 
 *	Handle global menu.  This app shares the same menu across all forms.
 *
 *	Input:
 *		ev - event.
 *		form - form.
 *	Output:
 *		handled: true.
 *		not handled: false.
 */
Boolean
globalmenu(EventPtr ev,
           FormPtr form)
{
	UInt16 mid=ev->data.menu.itemID, focus;
	Boolean handled=true;
	FieldPtr fld;

	if (mid>=menuundo && mid<=menuselectall) {
		focus=FrmGetFocus(form);
		if (focus==noFocus)
			return false;

		fld=FrmGetObjectPtr(form, focus);

		if (mid==menuundo)
			FldUndo(fld);
		else if (mid==menucut)
			FldCut(fld);
		else if (mid==menucopy)
			FldCopy(fld);
		else if (mid==menupaste)
			FldPaste(fld);
		else
			FldSetSelection(fld, 0, FldGetTextLength(fld));

		return true;
	}
	MenuEraseStatus(NULL);

	switch (mid) {
	case menukeyboard:
		SysKeyboardDialog(kbdDefault);
		break;
	case menugraffiti:
		SysGraffitiReferenceDialog(referenceDefault);
		break;
	case menuabout:
		about(form, formabout);
		break;
	default:
		handled=false;
		break;
	}

	return handled;
}

/*******************************************************************************
 *
 *	normalizekey
 *
 *	Normalizes key.  Collapses whitespaces.  Uppercase first letter in
 *	each word.
 *
 *	Input:
 *		dst - destination.
 *		src - source.
 */
void
normalizekey(Char *dst,
             Char *src)
{
	int uc=1, len=KEYSIZE;
	Char c;

	for (; *src && *src<=' '; src++);

	while (*src && len>0) {
		c=*src++;
		if (c<=' ') {
			for (; *src && *src<=' '; src++);
			if (!*src) {
				*dst='\x00';
				return;
			}

			c=' ';
			uc=1;
		} else {
			if (uc) {
				if (c>='a' && c<='z')
					c-=('a'-'A');
				uc=0;
			}
		}
		*dst++=c;
		len--;
	}

	*dst='\x00';
}
