/*
 * Beam.c
 */
#include "Include.h"

/* protos */
static Err BeamWriter(const void *, UInt32 *, void *);

/*
 * Beam
 *
 * Prepare database to beam & then beam it.
 *
 *  -> groupName	Group name.
 *  -> dbX		XRef database.
 *  -> dbS		Sentence database.
 */
void
Beam(Char *groupName, DB *dbX, DB *dbS)
{
	UInt16 grpLen=StrLen(groupName), sLen, sNo, sentences, rIdx;
	MemHandle bufferH, mh;
	Char *buffer, *p, *b;
	DmOpenRef db;
	UInt8 gL1, gL2;
	Err err;
	ExgSocketType sck;

	DBCreateDatabase(DBNameBeam, 0);
	DBClear(&dbBeam);
	if (DBOpen(&dbBeam, dmModeReadWrite, true)==false)
		return;

	db=dbBeam.db;
	while (DmNumRecords(db)>0)
		ErrFatalDisplayIf(DmRemoveRecord(db, 0)!=errNone, "(Beam) Cannot empty database.");

	bufferH=MemHandleNew(GroupNameLength+3+SentenceLength+3);
	ErrFatalDisplayIf(bufferH==NULL, "(Beam) Out of memory.");

	buffer=MemHandleLock(bufferH);
	sentences=XRefCount(dbX, groupUid);
	gL1=(grpLen&0xff00)>>8;
	gL2=grpLen&0xff;
	for (sNo=0; sNo<sentences; sNo++) {
		b=buffer;
		*b++=gL1;
		*b++=gL2;
		MemMove(b, groupName, grpLen);
		b+=grpLen;
		*b++='\x00';

		rIdx=XRefIndex(dbX, groupUid, sNo);
		ErrFatalDisplayIf(rIdx==dmMaxRecordIndex, "(Beam) Database inconsistency (Xref)");

		mh=DBGetRecord(dbX, rIdx);
		err=DmFindRecordByID(dbS->db, ((XRefRecord *)MemHandleLock(mh))->suid, &rIdx);
		MemHandleUnlock(mh);

		ErrFatalDisplayIf(err!=errNone, "(Beam) Cannot look up sentence.");

		mh=DBGetRecord(dbS, rIdx);
		p=MemHandleLock(mh);
		sLen=StrLen(p);

		*b++=(sLen&0xff00)>>8;
		*b++=sLen&0xff;
		MemMove(b, p, sLen);
		MemHandleUnlock(mh);
		b+=sLen;
		*b='\x00';

		DBSetRecord(&dbBeam, dmMaxRecordIndex, dmUnfiledCategory, buffer, grpLen+sLen+6, NULL);
	}

	MemHandleFree(bufferH);

	MemSet(&sck, sizeof(ExgSocketType), 0);
	sck.description="Dictation Sentences";
	sck.name=DBNameBeam;
	if ((err=ExgPut(&sck))==errNone) {
		err=ExgDBWrite(BeamWriter, &sck, NULL, dbBeam.id, dbBeam.card);
		err=ExgDisconnect(&sck, err);
	}
	DBClose(&dbBeam);

	if (err!=errNone)
		FrmAlert(aIRSendError);

	if (DBFindDatabase(&dbBeam)==FDFound)
		DmDeleteDatabase(dbBeam.card, dbBeam.id);
}

/*
 * BeamWriter
 *
 * Beam callback.
 */
static Err
BeamWriter(const void *src, UInt32 *len, void *sck)
{
	Err err;

	*len=ExgSend((ExgSocketType *)sck, src, *len, &err);
	return err;
}
