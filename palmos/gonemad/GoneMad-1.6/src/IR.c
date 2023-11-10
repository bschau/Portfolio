/*
 * IR.c
 */
#include "Include.h"

/*
 * IRBeam
 *
 * Beam a story.
 *
 *  -> src		Text to beam.
 */
void
IRBeam(Char *src)
{
	UInt16 len=StrLen(src);
	ExgSocketType sck;
	Err err;
	MemHandle memoH;
	Char *memo;

	memoH=MemHandleNew(MemoSize);
	ErrFatalDisplayIf(memoH==NULL, "(IRBeam) Out of memory.");
	memo=MemHandleLock(memoH);

	MemSet(memo, MemoSize, 0);
	if (len>(MemoSize-1))
		len=MemoSize-1;

	MemMove(memo, src, len);

	MemSet(&sck, sizeof(ExgSocketType), 0);
	sck.target=(UInt32)CRID;
	sck.count=1;
	sck.length=MemoSize;
	sck.noGoTo=1;
	sck.description="Gone Mad! Story Template";
	sck.name=DBNameTemplate;
	if ((err=ExgPut(&sck))==errNone) {
		ExgSend(&sck, memo, MemoSize, &err);
		err=ExgDisconnect(&sck, err);
	}

	MemHandleFree(memoH);

	if (err!=errNone)
		FrmAlert(aIRSendError);
}

/*
 * IRReceive
 *
 * Receive a record.
 *
 *  -> sck		Socket.
 *  -> dbo		Database object.
 */
void
IRReceive(ExgSocketType *sck, DB *dbo)
{
	MemHandle memoH=MemHandleNew(MemoSize);
	Char *memo;
	Err err;

	ErrFatalDisplayIf(memoH==NULL, "(IRReceive) Out of memory.");
	memo=MemHandleLock(memoH);

	if ((err=ExgAccept(sck))==errNone) {
		if (ExgReceive(sck, memo, MemoSize, &err)==MemoSize) {
			UInt32 uid;

			uid=DBSetRecord(dbo, dmMaxRecordIndex, dmUnfiledCategory, memo, StrLen(memo)+1, SFString);
			MemSet(&(sck->goToParams), sizeof(sck->goToParams), 0);
			if (DmFindRecordByID(dbo->db, uid, &sck->goToParams.recordNum)==errNone)
				sck->goToCreator=(UInt32)CRID;
		}
		err=ExgDisconnect(sck, err);
	}

	MemHandleFree(memoH);
	if (err)
		FrmAlert(aIRReceiveError);
}
