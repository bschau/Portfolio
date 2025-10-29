#include <windows.h>
#include <UserData.h>
#include "TramsSync.h"
#include <HSLog.h>
#include <Pgenerr.h>
#include <stdio.h>
#include <time.h>

/* globals */
static const char *txtHeader="TRAMSMOB`400\r\nOP`U`%d-%b-%y %H:%m\r\nTH``DD/MM/YY`DD/MM/YY`O`\r\n";

/*
 * TramsSync
 */
TramsSync::TramsSync(CSyncProperties& rProps)
{
	memcpy(&properties, &rProps, sizeof(CSyncProperties));
}

/*
 * CleanUp
 *
 * Clean up dynamic allocations.
 */
void TramsSync::CleanUp(void)
{
	if (hhDB)
		SyncCloseDB(hhDB);

	if (cHandle)
		SyncUnRegisterConduit(cHandle);

	if (record)
		delete[] record;

	if (header)
		delete[] header;

	if (csvFile.is_open())
		csvFile.close();
}

/*
 * Synchronize
 *
 * Handle actual synchronization logic.
 *
 * Returns 0 if successful, !0 otherwise.
 */
long TramsSync::Synchronize(void)
{
	bool pathSet=false;
	DWORD uid;
	long rVal;
	char path[300];
	long pathSize;
	struct tm now;
	__time32_t nowSecs;

	if (properties.m_SyncType==eDoNothing) {
		LogAddFormattedEntry(slText, false, "Trams Conduit - conduit configured to Do Nothing");
		return 0;
	}

	if (properties.m_SyncType!=eHHtoPC)
		return GEN_ERR_BAD_SYNC_TYPE;

	if (UmGetIDFromName(properties.m_UserName, &uid)==0) {
		pathSize=sizeof(path);
		if (UmGetString(uid, "Trams", "DataPath", path, &pathSize, "")==0)
			pathSize=strlen(path);
		else {
			strcpy_s(path, sizeof(path),"C:\\TRAMS\\mobile\\data");
			pathSize=strlen(path);
		}

		if (pathSize>0) {
			path[pathSize++]='\\';
			pathSet=true;
		}
	}

	if (!pathSet) {
		LogAddFormattedEntry(slText, false, "Trams Conduit - path not configured");
		return 0;
	}

	// Initialize stuff which might go oos early
	cHandle=NULL;
	hhDB=0;
	header=NULL;
	record=new char[MAX_RECORD_SIZE];
	if (!record) {
		LogAddFormattedEntry(slText, false, "Trams Conduit - low memory");
		return GEN_ERR_LOW_MEMORY;
	}

	if ((rVal=SyncRegisterConduit(cHandle))) {
		LogAddFormattedEntry(slText, false, "Trams Conduit - failed to register conduit with SyncManager");
		return rVal;
	}

	header=(char *)new char[MAX_HEADER_SIZE];
	if (!header) {
		LogAddFormattedEntry(slText, false, "Trams Conduit - low memory");
		return GEN_ERR_LOW_MEMORY;
	}

	_time32(&nowSecs);
	_localtime32_s(&now, &nowSecs);
	strftime(header, MAX_HEADER_SIZE-1, txtHeader, &now);

	for (int dbNo=0; dbNo<properties.m_nRemoteCount; dbNo++) {
		CRawRecordInfo cRaw;
		char buffer[20], countryCode[2];
		WORD numRecs, recNo, date, sD, sM, sY, eD, eM, eY;
		bool badFile, handled;
		int yield, i, j;
		char *ter;

		SyncYieldCycles(1);

		if (properties.m_RemoteDbList) {
			strncpy_s(dbName, sizeof(dbName), properties.m_RemoteDbList[dbNo]->m_Name, SYNC_DB_NAMELEN);
			card=properties.m_RemoteDbList[dbNo]->m_CardNum;
		} else {
			strncpy_s(dbName, sizeof(dbName), properties.m_RemoteName[dbNo], SYNC_DB_NAMELEN);
			card=0;
		}

		if (hhDB) {
			SyncCloseDB(hhDB);
			hhDB=0;
		}

		ter=&dbName[6];
		if (strncmp(dbName, "Trms_L", 6)==0) {
			LogAddFormattedEntry(slText, false, "Trams Conduit - TER '%s' is locked", ter);
			continue;
		}

		if ((rVal=SyncOpenDB(dbName, card, hhDB, eDbRead))) {
			LogAddFormattedEntry(slText, false, "Trams Conduit - failed to open TER '%s'", ter);
			continue;
		}

		if ((SyncGetDBRecordCount(hhDB, numRecs))) {
			LogAddFormattedEntry(slText, false, "Trams Conduit - cannot retrieve number of expenses on TER '%s'", ter);
			continue;
		}

		if (!numRecs) {
			LogAddFormattedEntry(slText, false, "Trams Conduit - no expenses on TER '%s'", ter);
			continue;
		}

		if (strncmp(dbName, "Trms_", 5)==0)
			j=6;
		else
			j=0;

		for (i=pathSize; i<sizeof(path)-6; i++) {
			path[i]=dbName[j++]&0x7f;
			if (!path[i])
				break;
		}

		path[i++]='.';
		path[i++]='e';
		path[i++]='x';
		path[i++]='p';
		path[i]=0;

		LogAddFormattedEntry(slText, false, "Trams Conduit - writing to local CSV file: '%s'", path);

		csvFile.open(path, ios::out|ios::binary);
		if (csvFile.is_open()==false) {
			LogAddFormattedEntry(slText, false, "Trams Conduit - failed to open '%s'", path);
			continue;
		}

		badFile=false;
		yield=0;

		csvFile.write(header, strlen(header));

		for (recNo=0; recNo<numRecs; recNo++) {
			memset(&cRaw, 0, sizeof(CRawRecordInfo));

			cRaw.m_pBytes=(BYTE *)record;
			cRaw.m_TotalBytes=MAX_RECORD_SIZE;
			cRaw.m_FileHandle=hhDB;
			cRaw.m_RecIndex=recNo;

			if (SyncReadRecordByIndex(cRaw)) {
				LogAddFormattedEntry(slText, false, "Trams Conduit - error on record %u", recNo);
				badFile=true;
				break;
			}

			date=SyncHHToHostWord(*(WORD *)record);
			if (!recNo) {
				sD=date&0x1f;
				sM=(date>>5)&0x0f;
				sY=(((date>>9)&0x7f)+1904)%100;
			}

			if (recNo==(numRecs-1)) {
				eD=date&0x1f;
				eM=(date>>5)&0x0f;
				eY=(((date>>9)&0x7f)+1904)%100;
			}

			handled=false;

			// Record layout:
			//
			//	Pos	Field						Size
			//	00	Datetype					2
			//	02	Country index				2	<-- not used
			//	04	Country code				2
			//	06	Currency index				2	<-- not used
			//	08	Currency string				4	(3 + nul)
			//	12	Flags						2	(EFlgOriginalReceipt 1)
			//	14	Expense category code		4
			//	18	Amount						zero term
			//		Comment						zero term
			//		Expense category string		zero term


			/* FIXME: if ExpCat == 'xxxx', handle type xxxx */

			// Default handler
			if (!handled) {
				char *comment, *expCat;
				WORD flags;

				csvFile.write("CE`", 3);
				csvFile.write(record+14, 4);
				csvFile.write("`", 1);

				for (comment=record+18; *comment; comment++);
				if (!*comment)
					comment++;

				for (expCat=comment; *expCat; expCat++);
				if (!*expCat)
					expCat++;

				if (*expCat)
					csvFile.write(expCat, strlen(expCat));
				else
					csvFile.write(" ", 1);

				sprintf_s(buffer, sizeof(buffer), "`%02i/%02i/%02i```", date&0x1f, (date>>5)&0x0f, (((date>>9)&0x7f)+1904)%100);
				csvFile.write(buffer, strlen(buffer));
				if (record[18]) {
					csvFile.write(record+18, strlen(record+18));
					csvFile.write("`", 1);
					csvFile.write(record+18, strlen(record+18));
					csvFile.write("`", 1);
				} else
					csvFile.write("0`0`", 4);

				csvFile.write(record+8, 3);
				csvFile.write("`1`N`", 5);
				
				flags=SyncHHToHostWord(*(WORD *)(record+12));
				if (flags&1)
					csvFile.write("Y`N```", 6);
				else
					csvFile.write("N`N```", 6);

				if (*comment)
					csvFile.write(comment, strlen(comment));

				csvFile.write("`````` ` `", 10);
				csvFile.write(record+4, 2);
				csvFile.write("``````````````` ````` ````` ` ```` ` \r\n", 39);
			}

			yield++;
			if (!(yield%16))
				SyncYieldCycles(1);
		}

		// 41 + 50
		sprintf_s(buffer, sizeof(buffer), "%02i/%02i/%02i`%02i/%02i/%02i", sD, sM, sY, eD, eM, eY);

		csvFile.seekp(40);
		csvFile.write(buffer, 17);
		csvFile.close();

		if (badFile)
			_unlink(path);
	}

	CleanUp();
	LogAddFormattedEntry(slSyncFinished, false, "Trams Conduit");
	return 0;
}