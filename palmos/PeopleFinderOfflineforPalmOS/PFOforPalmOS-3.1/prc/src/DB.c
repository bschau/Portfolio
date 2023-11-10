/*
 * DB.c
 */
#include "Include.h"

/*
 * DBFindDatabase
 *
 * Search all cards for a given database.
 *
 *  -> dbo	Database object.
 *
 * Returns FDFound, FDNotFound or FDError.
 */
UInt16
DBFindDatabase(DB *dbo)
{
	UInt16 cards=MemNumCards(), idx, vRefNum;
	UInt32 v=0, d;
	Char buffer[dmDBNameLength];
	FileRef dirRef;
	FileInfoType fileInfo;
	Int32 dbNameLen;

	for (idx=0; idx<cards; idx++) {
		if ((dbo->id=DmFindDatabase(idx, dbo->name))) {
			UInt16 version;

			if (DmDatabaseInfo(idx, dbo->id, NULL, NULL, &version, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)!=errNone || version!=DBVERSION)
				continue;

			dbo->card=idx;
			return FDFound;
		}

		if (DmGetLastErr()!=dmErrCantFind)
			return FDError;
	}

	/* Perhaps on expansion card? */
	if (FtrGet(sysFileCVFSMgr, vfsFtrIDVersion, &v)!=errNone)
		return FDNotFound;

	if (dbo->rootPath==NULL)
		return FDNotFound;

	dbNameLen=StrLen(dbo->name);
	fileInfo.nameP=buffer;
	fileInfo.nameBufLen=sizeof(buffer);
	v=vfsIteratorStart;
	while (v!=vfsIteratorStop) {
		if (VFSVolumeEnumerate(&vRefNum, &v)!=errNone)
			break;

		if (VFSFileOpen(vRefNum, dbo->rootPath, vfsModeRead, &dirRef)!=errNone)
			break;

		d=vfsIteratorStart;
		while (d!=vfsIteratorStop) {
			if (VFSDirEntryEnumerate(dirRef, &d, &fileInfo)!=errNone)
				break;

			if (StrLen(buffer)<dbNameLen)
				continue;
			
			if ((StrNCompare(dbo->name, buffer, dbNameLen)==0) && (StrCaselessCompare(buffer+dbNameLen, ".pdb")==0)) {
				UInt32 len=StrLen(dbo->rootPath)+1+StrLen(buffer)+1;
				MemHandle mh;
				FileRef fr;
				Char *p;

				VFSFileClose(dirRef);
				if ((mh=MemHandleNew(len))==NULL)
					continue;

				p=MemHandleLock(mh);
				StrPrintF(p, "%s/%s", dbo->rootPath, buffer);

				if (VFSFileOpen(vRefNum, p, vfsModeRead, &fr)!=errNone) {
					MemHandleFree(mh);
					continue;
				}

				MemHandleFree(mh);
				if (VFSFileSeek(fr, vfsOriginBeginning, 34)==errNone) {
					UInt16 version=0;

					if ((VFSFileRead(fr, 2, &version, NULL)==errNone) && (version==DBVERSION)) {
						VFSFileClose(fr);

						dbo->card=vRefNum;
						dbo->isVFS=true;
						return FDFound;
					}
				}

				VFSFileClose(fr);
			}
		}

		VFSFileClose(dirRef);
	}

	return FDNotFound;
}

/*
 * DBInit
 *
 * Initialize a database object.
 *
 *  -> dbo	Database object.
 *  -> name	Database name.
 *  -> rootPath	Root Path if on card. This ptr must be valid througout the
 *  		lifetime of the DB object!
 */
void
DBInit(DB *dbo, Char *name, Char *rootPath)
{
	MemSet(dbo, sizeof(DB), 0);
	StrNCopy(dbo->name, name, dmDBNameLength);
	dbo->rootPath=rootPath;
}

/*
 * DBOpen
 *
 * Open a database. Issue warning if unsuccesful (and if we were asked to do
 * so!).
 *
 *  -> dbo	Database object.
 *  -> mode	Open mode.
 *  -> verbose	Show alert if open fails.
 *
 * Returns true if database opened successfully, false otherwise.
 */
Boolean
DBOpen(DB *dbo, UInt16 mode, Boolean verbose)
{
	if (dbo->isOpen)
		return true;	/* Already open */

	if (DBFindDatabase(dbo)==FDFound) {
		if (dbo->isVFS==false) {
			if ((dbo->db=DmOpenDatabase(dbo->card, dbo->id, mode))!=NULL) {
				dbo->isOpen=true;
				return true;
			}
		} else {
			UInt32 len=StrLen(dbo->rootPath)+1+StrLen(dbo->name)+5;
			MemHandle mh;

			if ((mh=MemHandleNew(len))) {
				Char *p;
				FileRef fr;

				p=MemHandleLock(mh);
				StrPrintF(p, "%s/%s.pdb", dbo->rootPath, dbo->name);

				if (mode==dmModeReadWrite)
					mode=vfsModeReadWrite;
				else
					mode=vfsModeRead;

				if (VFSFileOpen(dbo->card, p, mode, &fr)==errNone) {
					dbo->fileRef=fr;
					MemHandleFree(mh);
					dbo->isOpen=true;
					return true;
				}

				MemHandleFree(mh);
			}
		}
	}

	if (verbose==true)
		FrmCustomAlert(aCannotOpenDatabase, dbo->name, NULL, NULL);

	return false;
}

/*
 * DBClose
 *
 * Close a database.
 *
 *  -> dbo	Database object.
 */
void
DBClose(DB *dbo)
{
	if (dbo->db)
		DmCloseDatabase(dbo->db);

	if (dbo->fileRef)
		VFSFileClose(dbo->fileRef);

	DBClear(dbo);
}

/*
 * DBClear
 *
 * Clears the database object - leaves the name intact.
 *
 *  -> dbo	Database object.
 */
void
DBClear(DB *dbo)
{
	dbo->db=NULL;
	dbo->id=0;
	dbo->card=0;
	dbo->isVFS=false;
	dbo->fileRef=0;
	dbo->isOpen=false;
}

/*
 * DBGetRecord
 *
 * Retrieves a record at a given index.
 *
 *  -> dbo	Database object.
 *  -> recIdx	Record Index.
 *
 *  Returns memhandle.
 */
MemHandle
DBGetRecord(DB *dbo, UInt16 recIdx)
{
	MemHandle mh=DBGetRecordSilent(dbo, recIdx);

	ErrFatalDisplayIf(mh==NULL, "(DBGetRecord) Cannot retrieve record.");
	return mh;
}

/*
 * DBGetRecordSilent
 *
 * Retrieves a record at a given index - this may return NULL.
 *
 *  -> dbo	Database object.
 *  -> recIdx	Record Index.
 *
 *  Returns memhandle or NULL if record was not found.
 */
MemHandle
DBGetRecordSilent(DB *dbo, UInt16 recIdx)
{
	MemHandle mh=NULL;

	if (dbo->isVFS==false) {
		MemHandle rh;
		UInt32 size;

		if ((rh=DmQueryRecord(dbo->db, recIdx))==NULL)
			return NULL;

		size=MemHandleSize(rh);
		if ((mh=MemHandleNew(size))!=NULL) {
			MemMove(MemHandleLock(mh), MemHandleLock(rh), size);
			MemHandleUnlock(rh);
			MemHandleUnlock(mh);
		}
	} else {
		/*
		 * v1.0 had this (which works):
		 *   VFSFileDBGetRecord(dbo->fileRef, recIdx, &mh, NULL, NULL);
		 *
		 * Unfortunately Palm in their infinite wisdom decided, that
		 * when a record needs to be read, the intire record index must
		 * be read to RAM beforehand.  This is slow when operating on
		 * databases the size we do.
		 *
		 * So we do it by hand ...
		 */
		UInt32 startOfRecord, endOfRecord, chunkSize, read;
		UInt16 cnt;
		UInt8 recEntry[8];

		if ((VFSFileDBInfo(dbo->fileRef, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &cnt)!=errNone) || (recIdx>=cnt))
			return NULL;

		if (VFSFileSeek(dbo->fileRef, vfsOriginBeginning, (((UInt32)recIdx)*8)+78)!=errNone)
			return NULL;

		if ((VFSFileRead(dbo->fileRef, 8, recEntry, &read)!=errNone) ||  (read!=8))
			return NULL;

		startOfRecord=*(UInt32 *)recEntry;
		if (recIdx==(cnt-1)) {
			if (VFSFileSize(dbo->fileRef, &endOfRecord)!=errNone)
				return NULL;
		} else {
			if (VFSFileSeek(dbo->fileRef, vfsOriginBeginning, ((((UInt32)recIdx+1))*8)+78)!=errNone)
				return NULL;

			if (VFSFileRead(dbo->fileRef, 8, recEntry, NULL)!=errNone)
				return NULL;

			endOfRecord=*(UInt32 *)recEntry;
		}

		if (VFSFileSeek(dbo->fileRef, vfsOriginBeginning, startOfRecord)!=errNone)
			return NULL;

		chunkSize=endOfRecord-startOfRecord;
		if ((mh=MemHandleNew(chunkSize))==NULL)
			return NULL;

		if ((VFSFileRead(dbo->fileRef, chunkSize, MemHandleLock(mh), &read)!=errNone) || (read!=chunkSize)) {
			MemHandleFree(mh);
			return NULL;
		}

		MemHandleUnlock(mh);
	}

	return mh;
}

/*
 * DBNumRecords
 *
 * Return number of records in database.
 *
 *  -> dbo		Database object.
 *
 * Returns number of records (or 0 if empty or error!)
 */
UInt16
DBNumRecords(DB *dbo)
{
	UInt16 cnt;

	if (dbo->isVFS==false)
		return DmNumRecordsInCategory(dbo->db, dmAllCategories);

	if (VFSFileDBInfo(dbo->fileRef, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &cnt)==errNone)
		return cnt;

	return 0;
}
