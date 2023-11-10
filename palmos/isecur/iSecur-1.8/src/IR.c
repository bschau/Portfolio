/*
 * IR.c
 */
#include "Include.h"

/* protos */
static Err Reader(void *, UInt32 *, void *);
static Boolean Deleter(const char *, UInt16, UInt16, LocalID, void *);

/* globals */
extern DB dbData;
extern State state;

/*
 * Reader
 *
 * Receive beam data.
 */
static Err
Reader(void *src, UInt32 *len, void *sck)
{
	Err err;

	*len=ExgReceive((ExgSocketType *)sck, src, *len, &err);
	return err;
}

/*
 * Deleter
 *
 * Delete database!
 */
static Boolean
Deleter(const char *name, UInt16 version, UInt16 card, LocalID id, void *u)
{
	return (DmDeleteDatabase(card, id)==errNone) ? true : false;
}

/*
 * IRReceive
 *
 * Receive data.
 *
 *  -> sck		Socket.
 */
void
IRReceive(ExgSocketType *sck)
{
	UInt32 v=0;
	Err err, fErr;
	LocalID dbID;
	Boolean resetBit;

	if ((err=ExgAccept(sck))!=errNone) {
		FrmAlert(aIRSocketError);
		return;
	}

	fErr=FtrGet((UInt32)CRID, iSecurFtrHasGlobals, &v);

	if (StrCaselessCompare(sck->name, "Entry")!=0) {
		if (fErr==ftrErrNoSuchFeature)
			err=ExgDBRead(Reader, Deleter, sck, &dbID, 0, &resetBit, false);
		else
			FrmAlert(aIRReceiveNotInApp);
	} else {
		if (fErr==errNone) {
			Boolean goOn=true;
			UInt16 flags=0, tLen, sLen, len;
			MemHandle th=NULL, sh=NULL;
			UInt8 buffer[4];
			UInt8 *p;

			v=ExgReceive(sck, &buffer, 3, &err);
			if ((v==0) || (err!=errNone))
				goOn=false;

			if (goOn) {
				if (*buffer)
					flags|=RHTemplate;
				else
					flags=0;

				tLen=(buffer[1]<<8)|buffer[2];
				len=tLen+2;
				th=MemHandleNew(len);
				ErrFatalDisplayIf(th==NULL, "(IRReceive) Out of memory");
				p=MemHandleLock(th);
				v=ExgReceive(sck, p, len, &err);
				sLen=(p[tLen]<<8)|(p[tLen+1]);
				p[tLen]=0;
				MemHandleUnlock(th);

				if ((v!=len) || (err!=errNone))
					goOn=false;
			}

			if (goOn) {
				if (sLen>0) {
					sh=MemHandleNew(sLen+1);
					ErrFatalDisplayIf(sh==NULL, "(IRReceive) Out of memory");
					p=MemHandleLock(sh);
					v=ExgReceive(sck, p, sLen, &err);
					p[sLen]=0;
					MemHandleUnlock(sh);

					if ((v!=sLen) || (err!=errNone))
						goOn=false;

				} else
					sh=NULL;
			}

			if (goOn) {
				Char *s=NULL, *t=NULL;

				if (th) {
					flags|=RHHasTitle;
					t=MemHandleLock(th);
				}

				if (sh) {
					flags|=RHHasSecret;
					s=MemHandleLock(sh);
				}

				DBSetRecord(&dbData, dmMaxRecordIndex, dmUnfiledCategory, flags, noIconSelected, t, s, DBSort);
				if (sh)
					MemHandleUnlock(sh);

				if (th)
					MemHandleUnlock(th);

				state.category=dmUnfiledCategory;
				FrmGotoForm(fMain);
			}

			if (sh)
				MemHandleFree(sh);

			if (th)
				MemHandleFree(th);
		} else
			FrmAlert(aIRReceiveMustBeInApp);
	}
	ExgDisconnect(sck, err);
}
