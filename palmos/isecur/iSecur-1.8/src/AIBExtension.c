/**
 * Routines to handle extensions in the application info block.
 */
#include "Include.h"

/* globals */
extern DB dbData;

/**
 * Set an extension in the application info block.
 *
 * @param ext Extension.
 * @param val Value (NULL to delete extension).
 */
void
AIBExtensionSet(Char ext, Char *val)
{
	LocalID appInfoID=DmGetAppInfoID(dbData.db);
	Boolean gotExtensions=false;
	UInt32 size=0, aIdx;
	UInt16 extLen, s;
	AppInfoBlock *a;
	MemPtr extP;
	Char *e, *d;
	UInt8 v;
	MemHandle mh;
	LocalID newID;

	if (val)
		extLen=StrLen(val);
	else
		extLen=0;

	ErrFatalDisplayIf(appInfoID==0, "(AIBExtensionSet) appInfoID == NULL ... nederen!");
	a=MemLocalIDToLockedPtr(appInfoID, dbData.card);
	extP=((Char *)a)+sizeof(AppInfoBlock);
	if (MemPtrSize(a)>sizeof(AppInfoBlock)) {
		for (e=extP; *e; ) {
			s=(e[1]<<8)|e[2];
			if (*e==ext) {
				aIdx=e-((Char *)a);
				if (s>=extLen) {
					aIdx+=3;
					DmSet(a, aIdx, s, 0);
					DmWrite(a, aIdx, val, extLen);
					MemPtrUnlock(a);
					return;
				} else if (!extLen) {
					v=AIBUnused;
					DmWrite(a, aIdx, &v, 1);
					MemPtrUnlock(a);
					return;
				}

				v=AIBUnused;
				DmWrite(a, aIdx, &v, 1);
			}

			e+=s+3;
		}

		if (!extLen) {
			MemPtrUnlock(a);
			return;
		}

		for (e=extP; *e; ) {
			s=(e[1]<<8)|e[2];
			if ((*e==AIBUnused) && (extLen<=s)) {
				aIdx=(e-((Char *)a))+3;
				DmSet(a, aIdx, s, 0);
				DmWrite(a, aIdx, val, extLen);
				MemPtrUnlock(a);
				return;
			}

			e+=s+3;
		}


		for (e=extP; *e; ) {
			s=((e[1]<<8)|e[2])+3;
			if (*e!=AIBUnused)
				size+=s;
	
			e+=s;
		}

		gotExtensions=true;
	} else if (!extLen) {
		MemPtrUnlock(a);
		return;
	}

	size+=sizeof(AppInfoBlock)+3+extLen+1;
	mh=DmNewHandle(dbData.db, size);
	ErrFatalDisplayIf(mh==NULL, "(AIBExtensionSet) Cannot allocate AppInfoBlock");

	newID=MemHandleToLocalID(mh);
	d=MemLocalIDToLockedPtr(newID, dbData.card);
	aIdx=sizeof(AppInfoBlock);
	DmWrite(d, 0, a, aIdx);
	if (gotExtensions) {
		for (e=extP; *e; ) {
			s=((e[1]<<8)|e[2])+3;
			if (*e!=AIBUnused) {
				DmWrite(d, aIdx, e, s);
				aIdx+=s;
			}

			e+=s;
		}
	}

	DmWrite(d, aIdx, &ext, 1);
	aIdx++;
	v=(UInt8)((extLen>>8)&0xff);
	DmWrite(d, aIdx, &v, 1);
	aIdx++;
	v=(UInt8)(extLen&0xff);
	DmWrite(d, aIdx, &v, 1);
	aIdx++;
	DmWrite(d, aIdx, val, extLen);
	aIdx+=extLen;
	v=0;
	DmWrite(d, aIdx, &v, 1);

	if (DmSetDatabaseInfo(dbData.card, dbData.id, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &newID, NULL, NULL, NULL)!=errNone) {
		MemPtrUnlock(d);
		MemHandleFree(mh);
		MemPtrUnlock(a);
		ErrFatalDisplay("(AIBExtensionSet) Failed to attach new AppInfoBlock");
	}

	MemPtrUnlock(d);
	MemPtrFree(a);
}

/**
 * Get an extension from the application info block.
 *
 * @param ext Extension.
 * @return MemHandle with extension data or NULL if extension wasn't found.
 */
MemHandle
AIBExtensionGet(Char ext)
{
	LocalID appInfoID=DmGetAppInfoID(dbData.db);
	AppInfoBlock *a;
	Char *e, *d;
	MemHandle mh;
	UInt16 s;

	ErrFatalDisplayIf(appInfoID==0, "(AIBExtensionGet) appInfoID == NULL ... nederen!");
	a=MemLocalIDToLockedPtr(appInfoID, dbData.card);
	if (MemPtrSize(a)>sizeof(AppInfoBlock)) {
		e=((Char *)a)+sizeof(AppInfoBlock);
		for (; *e; ) {
			s=(e[1]<<8)|e[2];
			if (*e==ext) {
				mh=MemHandleNew(s+1);
				ErrFatalDisplayIf(mh==NULL, "(AIBExtensionGet) Out of memory");
				d=MemHandleLock(mh);
				MemMove(d, e+3, s);
				d[s]=0;
				MemHandleUnlock(mh);
				MemPtrUnlock(a);
				return mh;
			}

			e+=s+3;
		}
	}

	MemPtrUnlock(a);
	return NULL;
}
