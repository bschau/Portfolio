/*
 * fDetect.c
 */
#include "Include.h"

/* protos */
static Boolean EH(EventType *);

/*
 * fDetectRun
 *
 * Detect what slot/index the hotsync name is stored in.
 */
void
fDetectRun(void)
{
	MemHandle mh=NULL;
	UInt16 mhSize=0, nameLen, slot, len;
	FormSave frm;
	Char buffer[HotSyncNameLength+2];
	UInt8 *p;
	Char hexBuffer[200];
	UInt16 x, y;
	Char *h;

	UIFormPrologue(&frm, fDetect, EH);
	StrPrintF(buffer, "%d", prefs.slots);
	UIFieldSetText(cDetectField, buffer);
	UIFieldFocus(cDetectField);
	if (UIFormEpilogue(&frm, NULL, cDetectCancel)==false)
		return;

	if (!prefs.slots) {
		prefs.slots=30;
		return;
	}

	DlkGetSyncInfo(NULL, NULL, NULL, buffer, NULL, NULL);
	if ((nameLen=StrLen(buffer))<1) {
		FrmAlert(aHotSyncPlease);
		return;
	}

	for (slot=0; slot<prefs.slots; slot++) {
		len=0;
		if (PrefGetAppPreferences((UInt32)'psys', slot, NULL, &len, false)==noPreferenceFound)
			continue;

		if (len>(SHSSizeOS50-1) && len<9999) {
			if (len>mhSize) {
				if (mh)
					MemHandleFree(mh);

				mh=MemHandleNew(len);
				ErrFatalDisplayIf(mh==NULL, "(fDetect) Device is full.");
				mhSize=len;
			}

			p=MemHandleLock(mh);
			PrefGetAppPreferences((UInt32)'psys', slot, p, &len, false);
			h=hexBuffer;
			for (x=0; x<60; x++) {
				Char hexChars[]="0123456789ABCDEF";
				y=(UInt16)((Char *)p)[x];
				*h++=hexChars[(y>>4)&0x0f];
				*h++=hexChars[y&0x0f];
			}
			*h='\x00';

			if (nameLen==p[gShs.nameIdx]-1) {
				if (StrCompare(p+gShs.size, buffer)==0) {
					MemHandleFree(mh);
					prefs.prefIdx=slot;
					prefs.trustPrefIdx=true;
					StrPrintF(buffer, "%d", slot);
					FrmCustomAlert(aDetectSlot, buffer, NULL, NULL);
					return;
				}
			}

			MemHandleUnlock(mh);
		}
	}

	if (mh)
		MemHandleFree(mh);

	FrmAlert(aDetectNotFound);
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	if (ev->eType==ctlSelectEvent && ev->data.ctlSelect.controlID==cDetectDetect) {
		Char *p;
		
		if ((p=UIFieldGetText(cDetectField)))
			prefs.slots=StrAToI(p);
		else
			prefs.slots=0;
	}

	return false;
}
