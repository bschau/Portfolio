/*
 * List.c
 */
#include "Include.h"

/* macros */
#define DefaultPath "/PALM/Launcher/"

/* protos */
static void AddNode(List *, Char *);

/*
 * ListInit
 *
 * Initialize a list.
 *
 *  -> l		List.
 */
void
ListInit(List *l)
{	
	l->head=(Node *)&l->tail;
	l->tail=NULL;
	l->tailPred=(Node *)&l->head;
	l->count=0;
}

/*
 * ListDestroy
 *
 * Delete all entries on a list.
 *
 *  -> l		List.
 */
void
ListDestroy(List *l)
{
	Node *n, *t;

	for (n=l->head; n->next; ) {
		t=n->next;
		MemPtrFree(n);
		n=t;
	}
}

/*
 * AddNode
 *
 * Insert a node in the list.
 *
 *  -> l		List.
 *  -> name		Name to insert.
 */
static void
AddNode(List *l, Char *name)
{
	MemHandle mh=MemHandleNew(sizeof(Node));
	Node *mp, *n, *n1;
	Int16 ret;

	ErrFatalDisplayIf(mh==NULL, "(AddNode) Out of memory.");

	mp=MemHandleLock(mh);
	MemSet(mp->name, NameLength, 0);
	StrNCopy(mp->name, name, NameLength);

	for (n=l->head; n->next; n=n->next) {
		if ((ret=StrCaselessCompare(mp->name, n->name))<0)
			break;

		if (!ret) {
			MemHandleFree(mh);
			return;
		}
	}

	n1=n->prev;
	n->prev=mp;
	n1->next=mp;
	mp->next=n;
	mp->prev=n1;

	l->count++;
}

/*
 * ListAppDiscover
 *
 * Build a list of all applications.
 *
 *  -> l		List.
 */
void
ListAppDiscover(List *l)
{
	Boolean firstSearch=true;
	DmSearchStateType state;
	UInt16 vRefNum, card, meCard;
	LocalID dbId, meLid;
	Char dbName[NameLength];
	Err err;
	UInt32 v, d, fileSize;
	FileRef dirRef, fr;
	FileInfoType fileInfo;
	MemHandle pathH;
	Char *path;
	UInt8 pdb[78];

	SysCurAppDatabase(&meCard, &meLid);
	for (EVER) {
		if ((err=DmGetNextDatabaseByTypeCreator(firstSearch, &state, 'appl', 0, false, &card, &dbId))!=errNone)
			break;

		firstSearch=false;
		if (card==meCard && dbId==meLid)
			continue;

		if ((err=DmDatabaseInfo(card, dbId, dbName, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL))==errNone)
			AddNode(l, dbName);
	}

	/* Apps on card */
	if (FtrGet(sysFileCVFSMgr, vfsFtrIDVersion, &v)!=errNone)
		return;

	pathH=MemHandleNew(256+StrLen(DefaultPath)+1);
	ErrFatalDisplayIf(pathH==NULL, "(ListDiscover) Out of memory");

	path=MemHandleLock(pathH);
	StrCopy(path, DefaultPath);

	fileInfo.nameP=path+StrLen(DefaultPath);
	fileInfo.nameBufLen=MemHandleSize(pathH)-StrLen(fileInfo.nameP)-1;

	v=vfsIteratorStart;
	while (v!=vfsIteratorStop) {
		if (VFSVolumeEnumerate(&vRefNum, &v)!=errNone)
			break;

		if (VFSFileOpen(vRefNum, DefaultPath, vfsModeRead, &dirRef)!=errNone)
			break;

		d=vfsIteratorStart;
		while (d!=vfsIteratorStop) {
			if (VFSDirEntryEnumerate(dirRef, &d, &fileInfo)!=errNone)
				break;

			if (!(fileInfo.attributes&vfsFileAttrDirectory)) {
				if (VFSFileOpen(vRefNum, path, vfsModeRead, &fr)!=errNone)
					continue;

				fileSize=0;
				VFSFileSize(fr, &fileSize);
				if (fileSize>=78) {
					err=VFSFileRead(fr, 78, &pdb, NULL);
					VFSFileClose(fr);

					if (err==errNone) {
						if (pdb[60]=='a' && pdb[61]=='p' &&
						    pdb[62]=='p' && pdb[63]=='l')
							AddNode(l, (Char *)pdb);
					}
				}

				VFSFileClose(fr);
			}
		}

		VFSFileClose(dirRef);
	}

	MemHandleFree(pathH);
}
