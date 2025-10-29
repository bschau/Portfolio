/*
**	editpersonform.c
**
**	Code to drive the EditPerson form
*/
#include "bmilog.h"

/* protos */
static Boolean EditPersonHandler(EventPtr);
static MemHandle PackPerson(UInt32, Char *, UInt16);
static UInt16 FindPerson(Char *, UInt16);
static UInt32 GetID(void);

/* globals */
static UInt16 personFormat;

/*
**	EditPerson
**
**	Show the Edit Person form.
**
**	 -> recIdx	Record index of person to edit (or dmMaxRecordIndex).
*/
Boolean
EditPerson(UInt16 recIdx)
{
	FormSave frm;
	MemHandle mh;
	Person *rp;

	recordIdx=recIdx;
	UIFormPrologue(&frm, fEditPerson, EditPersonHandler);

	if (recIdx==dmMaxRecordIndex) {
		personId=GetID();
		UIShowObject(fEPFormatLabel);
		UIShowObject(fEPFormatPopup);
		UIListSetPopup(fEPFormatList, fEPFormatPopup, prefs.format);
	} else {
		mh=DmQueryRecord(dbData.db, recIdx);
		ErrFatalDisplayIf(mh==NULL, "(EditPerson) Cannot query record.");
		rp=MemHandleLock(mh);
		UIFieldSetText(fEPName, rp->name);
		personId=rp->r.id;
		personFormat=rp->format;
		MemHandleUnlock(mh);
		UIShowObject(fEPDelete);
	}
		
	UIFieldFocus(fEPName);

	return UIFormEpilogue(&frm, NULL, fEPCancel);
}

/*
**	EditPersonHandler
**
**	Event handler for EditPerson form.
**
**	 -> ev		Event.
**
**	Returns true if event is handled, false otherwise.
*/
static Boolean
EditPersonHandler(EventPtr ev)
{
	Boolean retCode=true;
	UInt16 recIdx, format;
	Char *name;
	Char nameBuf[MaxNameLength+2];
	MemHandle nmh;
	Person *p;

	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case fEPOK:
			if ((nmh=UtilsTidyString(UIFieldGetText(fEPName)))) {
				name=MemHandleLock(nmh);
				format=(recordIdx==dmMaxRecordIndex) ? LstGetSelection(UIFormGetObject(fEPFormatList)) : personFormat;
				if ((recIdx=FindPerson(name, recordIdx))==dmMaxRecordIndex) {
					DBSetRecord(&dbData, PackPerson(personId, name, format), recordIdx);
					retCode=false;
				} else
					FrmAlert(aNameExists);

				MemHandleFree(nmh);
			} else
				FrmAlert(aNotNull);

			return retCode;

		case fEPDelete:
			nmh=DmQueryRecord(dbData.db, recordIdx);
			ErrFatalDisplayIf(nmh==NULL, "(EditPersonEventHandler) Cannot query record.");
			p=MemHandleLock(nmh);
			MemSet(nameBuf, sizeof(nameBuf), 0);
			StrNCopy(nameBuf, p->name, MaxNameLength);
			MemHandleUnlock(nmh);
			if (DBDeleteRecord(&dbData, recordIdx, aDeletePerson, nameBuf)==true) {
				BmiDeleteAll(personId);
				return false;
			}
			return true;
		}
		break;

	default:
		break;
	}

	return false;
}

/*
**	PackPerson
**
**	Pack a person record.
**
**	 -> id		Person ID.
**	 -> name	Name.
**	 -> format	Format (american/european).
**
**	Returns MemHandle.
*/
static MemHandle
PackPerson(UInt32 id, Char *name, UInt16 format)
{
	Person *p;
	MemHandle mh;

	mh=MemHandleNew(sizeof(Person));
	ErrFatalDisplayIf(mh==NULL, "(PackRecord) Out of memory.");

	p=MemHandleLock(mh);
	MemSet(p, sizeof(Person), 0);
	p->r.type=RPerson;
	p->r.id=id;
	StrNCopy(p->name, name, MaxNameLength);
	p->format=format;
	MemHandleUnlock(mh);

	return mh;
}

/*
**	FindPerson
**
**	Given a person name, do a lookup.
**
**	 -> name	Person name.
**	 -> eIdx	Existing index.
**
**	Returns index of person (if found), dmMaxRecordIndex otherwise.
*/
static UInt16
FindPerson(Char *name, UInt16 eIdx)
{
	DmOpenRef db=dbData.db;
	UInt16 recIdx=0, vLen=StrLen(name), pLen, m1l, m2l;
	MemHandle mh;
	Person *p;

	for (EVER) {
		if (recIdx!=eIdx) {	// Let's not find ourself!
			if ((mh=DmQueryNextInCategory(db, &recIdx, dmAllCategories))==NULL)
				break;
	
			p=MemHandleLock(mh);
			if (p->r.type>RPerson) {
				MemHandleUnlock(mh);
				break;
			}
			
			if (p->r.type==RPerson) {
				pLen=StrLen(p->name);
				if (pLen==vLen) {
					if (TxtGlueCaselessCompare(name, vLen, &m1l, p->name, pLen, &m2l)==0) {
						MemHandleUnlock(mh);
						return recIdx;
					}
				}
			}

			MemHandleUnlock(mh);
		}
		recIdx++;
	}

	return dmMaxRecordIndex;
}

/*
**	GetID
**
**	Get a unique ID for a person.
**
**	Returns ID.
*/
static UInt32
GetID(void)
{
	UInt32 c=TimGetSeconds();

	for (EVER) {
		if (DBTypeIdSearch(&dbData, RPerson, c, dmMaxRecordIndex)==dmMaxRecordIndex)
			break;

		c+=(UInt32)SysRandom(0);
	}

	return c;
}
