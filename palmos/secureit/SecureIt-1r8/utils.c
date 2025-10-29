/*******************************************************************************
*
*	utils
*
*	Utilities ...
*/
#include "sit.h"

/* protos */
static void setrecord(UInt16, MemHandle, char *, UInt16);
static int ircompare(unsigned char *, unsigned char *, Int16, SortRecordInfoPtr, SortRecordInfoPtr, MemHandle);

/*******************************************************************************
*
*	insertrecord
*
*	Inserts record in database.
*
*	Input:
*		bs - bytestream.
*		cat - category.
*	Output:
*		true/false.
*/
Int16
insertrecord(char *bs,
             UInt16 cat)
{
	UInt16 recindex, previndex, attrs;
	MemHandle rec;
	unsigned char *kptr;
	Int16 t;

	/* look for record in same category */
	recindex=DmFindSortPosition(sitdb, bs, 0, (DmComparF *)ircompare, EKEYLEN);
	if (recindex) {
		/* we have a match */
		for (previndex=recindex-1; previndex>0; previndex--) {
			rec=DmGetRecord(sitdb, previndex);
			kptr=MemHandleLock(rec);
			t=MemCmp(bs, kptr, EKEYLEN);
			MemHandleUnlock(rec);
			if (t) {
				/* key mismatch */
				DmReleaseRecord(sitdb, previndex, false);
				break;
			}

			DmRecordInfo(sitdb, previndex, &attrs, NULL, NULL);
			if (cat==(attrs&dmRecAttrCategoryMask)) {
				/* overwrite record */
				DmRecordInfo(sitdb, previndex, &t, NULL, NULL);
				setrecord(previndex, rec, bs, t|cat);
				return true;
			}
			DmReleaseRecord(sitdb, previndex, false);
		}
	}
			
	rec=DmNewRecord(sitdb, &recindex, BSSIZE);
	if (rec) {
		setrecord(recindex, rec, bs, cat);
		return true;
	}

	return false;
}

/*******************************************************************************
*
*	setrecord
*
*	Write a given record to it's index.
*
*	Input:
*		idx - index.
*		rec - memhandle to record.
*		record - record data.
*		attr - record attributes.
*/
static void
setrecord(UInt16 idx,
          MemHandle rec,
          char *bs,
          UInt16 attr)
{
	unsigned char *kptr;

	kptr=MemHandleLock(rec);
	DmWrite(kptr, 0, bs, BSSIZE);
	MemHandleUnlock(rec);
	DmSetRecordInfo(sitdb, idx, &attr, NULL);
	DmReleaseRecord(sitdb, idx, true);
}

/*******************************************************************************
*
*	ircompare
*
*	Compare function for insertrecord.
*
*	See reference manual for arguments.
*/
static int
ircompare(unsigned char *r1,
          unsigned char *r2,
          Int16 reclen,
          SortRecordInfoPtr unused1,
          SortRecordInfoPtr unused2,
          MemHandle unused3)
{
	return StrNCompare(r1, r2, reclen);
}

/*******************************************************************************
*
*	newlist
*
*	Create a new list from the category database.
*
*	Input:
*		list - sit list.
*		idx - index to select.
*/
void
newlist(sitlist *list,
        UInt16 idx)
{
	freelist(list);

	list->idx=idx;

	CategoryCreateList(sitdb, &(list->list), idx, false, true, CATNONEDITABLES, 0, true);
	CategoryGetName(sitdb, idx, list->name);
	list->valid=1;
}

/*******************************************************************************
*
*	freelist
*
*	Frees a list resource.
*
*	Input:
*		list - sit list.
*/
void
freelist(sitlist *list)
{
	if (list->valid) {
		CategoryFreeList(sitdb, &(list->list), false, categoryDefaultEditCategoryString);
		list->valid=0;
	}
}

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
*	recpack
*
* 	Pack structure into "bytestream" :)
*
* 	Input:
*		bs - bytestream to pack to.
* 		key - key to pack.
* 		txt - text to pack.
*/
void
recpack(char *bs,
	char *key,
        char *txt)
{
	unsigned char t[ETXTLEN], *p;
	unsigned long *lp, l1, l2;
	UInt16 cnt, i;
	blowfishkey *k;

	MemSet(bs, EKEYLEN, 0);
	MemSet(t, ETXTLEN, 0);
	cnt=StrLen(txt);
	StrNCopy(t, txt, ETXTLEN);

	/* pad with random bytes */
	p=t+cnt+1;
	for (i=cnt+1; i<ETXTLEN; i++)
		*p++=SysRandom(0);

	k=MemHandleLock(mainkeyh);
	lp=(unsigned long *)t;
	for (i=0; i<(ETXTLEN/8); i++) {
		l1=*lp;
		l2=*(lp+1);
		blowfishencrypt(k, &l1, &l2);
		*lp=l1;
		*(lp+1)=l2;
		lp+=2;
	}

	MemHandleUnlock(mainkeyh);

	MemMove(bs, key, StrLen(key));
	MemMove(bs+EKEYLEN, t, ETXTLEN);
}

/*******************************************************************************
*
* 	recunpack
*
* 	Unpack bytestream to record ptr.
*
* 	Input:
* 		rp - record pointer.
*		bs - bytestream to unpack from.
*/
void
recunpack(record *rp,
          char *bs)
{
	unsigned long *lp, l1, l2;
	UInt16 i;
	blowfishkey *k;
	char *p;

	MemMove(rp->key, bs, EKEYLEN);
	*(rp->key+EKEYLEN)='\x00';

	lp=(unsigned long *)(bs+EKEYLEN);
	k=MemHandleLock(mainkeyh);
	for (i=0; i<(ETXTLEN/8); i++) {
		l1=*lp;
		l2=*(lp+1);
		blowfishdecrypt(k, &l1, &l2);
		*lp=l1;
		*(lp+1)=l2;
		lp+=2;
	}
	MemHandleUnlock(mainkeyh);

	/* remove padding */
	p=bs+EKEYLEN;
	for (i=0; i<ETXTLEN; i++) {
		if (!*p)
			break;
	}

	for (; i<ETXTLEN; i++)
		*p++='\x00';

	MemMove(rp->txt, bs+EKEYLEN, ETXTLEN);
	*(rp->txt+ETXTLEN)='\x00';
}

/*******************************************************************************
 *
 *	setfieldtext
 *
 * 	Sets new text in the specified field in the specified form.
 *
 * 	Input:
 * 		form - form.
 * 		fld - field id.
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
*	handleeditmenu
*
*	Handle edit menu events.
*
*	Input:
*		ev - event.
*		form - form.
*	Output:
*		handled: true.
*		not handled: false.
*/
Boolean
handleeditmenu(EventPtr ev,
               FormPtr form)
{
	UInt16 mid=ev->data.menu.itemID, focus;
	FieldPtr fld;

	if (mid>=globaleditundo && mid<=globaleditselectall) {
		focus=FrmGetFocus(form);
		if (focus==noFocus)
			return false;

		fld=FrmGetObjectPtr(form, focus);

		if (mid==globaleditundo)
			FldUndo(fld);
		else if (mid==globaleditcut)
			FldCut(fld);
		else if (mid==globaleditcopy)
			FldCopy(fld);
		else if (mid==globaleditpaste)
			FldPaste(fld);
		else
			FldSetSelection(fld, 0, FldGetTextLength(fld));

		return true;
	}
	MenuEraseStatus(NULL);

	disabletimeout();
	if (mid==globaleditkeyboard)
		SysKeyboardDialog(kbdDefault);
	else
		SysGraffitiReferenceDialog(referenceDefault);

	enabletimeout();
	return true;
}
