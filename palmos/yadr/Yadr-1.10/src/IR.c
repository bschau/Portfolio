/*
 * IR.c
 */
#include "Include.h"

/* structs */
typedef struct {
	Char docName[DocNameLength];
	UInt16 version;
	UInt16 records;
} IRHeader;

/* protos */
static Boolean GetHdrInfo(DB *, UInt16 *, UInt16 *);

/*
 * GetHdrInfo
 *
 * Retrieve various fields to be beamed to the other side from the header.
 *
 *  -> db		Database object.
 * <-  attrs		Where to store attributes.
 * <-  version		Where to store database version.
 *
 * Returns true if data was retrieved successfully, false otherwise.
 */
static Boolean
GetHdrInfo(DB *db, UInt16 *attrs, UInt16 *version)
{
	if (db->path) {
		UInt32 read;

		if (VFSFileSeek(db->fileRef, vfsOriginBeginning, 20)!=errNone)
			return false;

		if ((VFSFileRead(db->fileRef, 2, attrs, &read)!=errNone) || (read!=2))
			return false;

		if ((VFSFileRead(db->fileRef, 2, version, &read)!=errNone) || (read!=2))
			return false;
	} else {
		if (DmDatabaseInfo(db->card, db->id, NULL, attrs, version, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)!=errNone)
			return false;
	}

	return true;
}

/*
 * IRBeam
 *
 * Beam a database to the other end.
 *
 *  -> n		DocNode.
 */
void
IRBeam(DocNode *n)
{
	UInt32 maxSize=0, read;
	DB db;
	UInt16 attrs, cnt, chunkSize;
	MemHandle mh;
	IRHeader h;
	ExgSocketType sck;
	Err err;

	MemSet(h.docName, DocNameLength, 0);
	StrNCopy(h.docName, n->docName, DocNameLength-1);

	DBInit(&db, n->docName, n->fileName);
	if (DBOpen(&db, dmModeReadOnly, true)==false)
		return;

	if (GetHdrInfo(&db, &attrs, &h.version)==false) {
		FrmAlert(aNoHeaderInfo);
		return;
	}

	if (attrs&dmHdrAttrCopyPrevention) {
		FrmAlert(aDBCopyPrevent);
		return;
	}
	
	h.records=DBNumRecords(&db);
	for (cnt=0; cnt<h.records; cnt++) {
		mh=DBGetRecord(&db, cnt);
		read=MemHandleSize(mh);
		if (read>maxSize)
			maxSize=read;

		MemHandleFree(mh);
	}

	MemSet(&sck, sizeof(ExgSocketType), 0);
	sck.target=(UInt32)CRID;
	sck.count=1+h.records;
	sck.description=n->docName;
	sck.name=n->docName;
	sck.noGoTo=1;
	if ((err=ExgPut(&sck))==errNone) {
		if (ExgSend(&sck, &h, sizeof(IRHeader), &err)==sizeof(IRHeader)) {
			for (cnt=0; cnt<h.records; cnt++) {
				mh=DBGetRecord(&db, cnt);
				read=MemHandleSize(mh);
				chunkSize=(UInt16)read;
				if (ExgSend(&sck, &chunkSize, sizeof(UInt16), &err)!=sizeof(UInt16)) {
					MemHandleFree(mh);
					break;
				}
				
				maxSize=ExgSend(&sck, MemHandleLock(mh), read, &err);
				MemHandleFree(mh);
				if (maxSize!=read)
					break;
			}
		}

		err=ExgDisconnect(&sck, err);
	}
		
	if (err!=errNone)
		FrmAlert(aIRSendError);
}

/*
 * IRReceive
 *
 * Receive a document.
 *
 *  -> sck		Socket.
 * <-  disconnect	True if caller should disconnect socket,
 * 			false otherwise.
 * <-  err		Error.
 *
 * Returns true if document was received, false otherwise.
 */
Boolean
IRReceive(ExgSocketType *sck, Boolean *disconnect, Err *err)
{
	UInt32 mhSize=0;
	MemHandle mh=NULL, rh;
	UInt16 attrs=dmHdrAttrBackup, cnt, chunkSize, rIdx;
	IRHeader h;
	DB db;

	*disconnect=false;
	if (ExgAccept(sck)!=errNone) {
		FrmCustomAlert(aIRReceiveError, "Cannot accept connection", NULL, NULL);
		return false;
	}

	*disconnect=true;
	if (ExgReceive(sck, &h, sizeof(IRHeader), err)!=sizeof(IRHeader)) {
		FrmCustomAlert(aIRReceiveError, "Cannot receive header", NULL, NULL);
		return false;
	}

	DBInit(&db, h.docName, NULL);
	switch (DBFindDatabase(&db)) {
	case FDFound:
		if (FrmAlert(aIRDeleteDB)==1)
			return false;

		if (DmDeleteDatabase(db.card, db.id)!=errNone) {
			FrmCustomAlert(aIRReceiveError, "Cannot delete database - this is usually means that the document is already open", NULL, NULL);
			return false;
		}
	case FDNotFound:	/* FALL-THRU */
		break;
	default:
		FrmCustomAlert(aIRReceiveError, "Cannot search databases", NULL, NULL);
		return false;
	}

	if (DmCreateDatabase(0, db.name, (UInt32)'REAd', (UInt32)'TEXt', false)!=errNone) {
		FrmCustomAlert(aIRReceiveError, "Cannot create database", NULL, NULL);
		return false;
	}

	if (DBOpen(&db, dmModeReadWrite, true)!=true)
		return false;

	if (DmSetDatabaseInfo(db.card, db.id, NULL, &attrs, &h.version, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)!=errNone) {
		FrmCustomAlert(aIRReceiveError, "Cannot set database info", NULL, NULL);
		return false;
	}

	for (cnt=0; cnt<h.records; cnt++) {
		if (ExgReceive(sck, &chunkSize, sizeof(UInt16), err)!=sizeof(UInt16)) {
			DmDeleteDatabase(db.card, db.id);
			FrmCustomAlert(aIRReceiveError, "Cannot receive chunk size", NULL, NULL);
			return false;
		}

		if (chunkSize>mhSize) {
			if (mh)
				MemHandleFree(mh);

			mh=MemHandleNew(chunkSize);
			if (mh==NULL) {
				FrmCustomAlert(aIRReceiveError, "Out of memory", NULL, NULL);
				return false;
			}
			mhSize=chunkSize;
		}

		if (ExgReceive(sck, MemHandleLock(mh), chunkSize, err)!=chunkSize) {
			DmDeleteDatabase(db.card, db.id);
			MemHandleFree(mh);
			FrmCustomAlert(aIRReceiveError, "Cannot receive document chunk", NULL, NULL);
			return false;
		}
		MemHandleUnlock(mh);

		rIdx=dmMaxRecordIndex;
		if ((rh=DmNewRecord(db.db, &rIdx, (UInt32)chunkSize))==NULL) {
			DmDeleteDatabase(db.card, db.id);
			MemHandleFree(mh);
			FrmCustomAlert(aIRReceiveError, "Out of memory", NULL, NULL);
			return false;
		}

		DmWrite(MemHandleLock(rh), 0, MemHandleLock(mh), chunkSize);
		MemHandleUnlock(mh);
		MemHandleUnlock(rh);

		DmReleaseRecord(db.db, rIdx, true);
	}

	sck->noGoTo=0;
	sck->goToCreator=(UInt32)CRID;
	MemSet(&sck->goToParams, sizeof(ExgGoToType), 0);
	sck->goToParams.dbCardNo=db.card;
	sck->goToParams.dbID=db.id;

	DBClose(&db);
	return true;
}
