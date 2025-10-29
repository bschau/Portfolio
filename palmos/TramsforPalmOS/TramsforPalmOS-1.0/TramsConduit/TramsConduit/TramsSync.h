#include <Windows.h>
#include <syncmgr.h>
#include <condapi.h>
#include <iostream>
#include <fstream>

#define MAX_RECORD_SIZE 65535
#define MAX_HEADER_SIZE 256

using namespace std;

class TramsSync
{
private:
	CSyncProperties properties;
	CONDHANDLE cHandle;
	char dbName[SYNC_DB_NAMELEN];
	int card;
	BYTE hhDB;
	char *record;
	ofstream csvFile;
	char *header;

	void CleanUp(void);

public:
	TramsSync(CSyncProperties&);
	long Synchronize(void);

};
