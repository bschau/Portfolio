/*
 * fMain.c
 */
#include "Include.h"

/* macros */
#define ComponentLength 128

/* protos */
static void DrawTable(void *, Int16, Int16, RectangleType *);
static void UpdateDocStatus(void);
static void DrawUpButton(void);
static void AddNode(Char *, Char *, UInt8);
static void DeleteNode(DocNode *);
static void LocateDocuments(void);
static void MemoryLocate(void);
static void DrawPath(void);
static void CardLocate(void);
static Char *GetFullName(Char *, UInt16);
static Boolean Edit(DocNode *);
static Boolean EEH(EventType *);
static Boolean Rename(DocNode *, Char *);
static void FixupNode(DocNode *, Char *);
static Boolean Delete(DocNode *);
static void EHook(void);

/* globals */
static Char *txtFinding="Finding Documents ...", *txtUpdateDoc="No Documents ...";
static DocList docList;
static Table tblMainMemory, tblMainCard;
static Int16 returnTop;
static MemHandle pathH;
static Char *path;
static Boolean dChanged;
static DocNode *docNode;

/*
 * fMainAlloc
 */
void
fMainAlloc(void)
{
	docList.head=(DocNode *)&docList.tail;
	docList.tail=NULL;
	docList.tailPred=(DocNode *)&docList.head;
	docList.count=0;
}

/*
 * fMainFree
 */
void
fMainFree(void)
{
	DocNode *n, *t;

	for (n=docList.head; n->next; ) {
		t=n->next;
		DeleteNode(n);
		n=t;
	}
}

/*
 * fMainClose
 *
 * Call in PilotMain::Stop.
 */
void
fMainClose(void)
{
	fMainFree();

	if (pathH) {
		PMSetPref(path, StrLen(path), PrfVFSPath);
		MemHandleFree(pathH);
	}
}

/*
 * DrawTable
 */
static void
DrawTable(void *tbl, Int16 row, Int16 column, RectangleType *r)
{
	DocNode *n=docList.head;
	UInt16 len;
	Int16 cnt;
	Boolean trunc;

	if (!docList.count)
		return;

	if (prefs.searchDocs==PSDMemory)
		cnt=tblMainMemory.top;
	else
		cnt=tblMainCard.top;

	cnt+=row;
	for (; cnt>0; cnt--) {
		if ((n=n->next)==NULL)
			return;
	}

	TblSetRowData(tbl, row, (UInt32)n);
	WinEraseRectangle(r, 0);
	
	len=StrLen(n->docName);
	if (prefs.searchDocs==PSDMemory) {
		UInt16 pW=150;

		FntCharsInWidth(n->docName, &pW, &len, &trunc);
		WinDrawChars(n->docName, len, r->topLeft.x, r->topLeft.y);
	} else {
		UInt16 pW=135;

		FntCharsInWidth(n->docName, &pW, &len, &trunc);
		WinDrawChars(n->docName, len, r->topLeft.x+15, r->topLeft.y);

		if (n->type==NodeTypeDirectory)
			WinDrawBitmap(folderBitmap, 1, r->topLeft.y);
	}
}

/*
 * fMainInit
 */
Boolean
fMainInit(void)
{
	UInt32 v;

	PMSetPref(NULL, 0, PrfDocument);
	DocClose(&doc);

	UITableInit(&tblMainMemory, cMainTableMemory, cMainScrollBarMemory, DrawTable, style1, 0);
	UITableInit(&tblMainCard, cMainTableCard, cMainScrollBarCard, DrawTable, style1, 0);

	UIObjectHide(cMainTableMemory);
	UIObjectHide(cMainScrollBarMemory);
	UIObjectHide(cMainTableCard);
	UIObjectHide(cMainScrollBarCard);
	UIObjectHide(cMainPath);
	UIObjectHide(cMainUp);

	if (FtrGet(sysFileCVFSMgr, vfsFtrIDVersion, &v)==errNone) {
		ListType *l=UIObjectGet(cMainList);

		LstSetSelection(l, (Int16)prefs.searchDocs);
		CtlSetLabel(UIObjectGet(cMainPopup), LstGetSelectionText(l, (Int16)prefs.searchDocs));
		UIObjectShow(cMainPopup);

		if (!pathH) {
			UInt16 pSize=0;

			pathH=MemHandleNew(MaxPathLen+1);
			ErrFatalDisplayIf(pathH==NULL, "(fMainInit) Out of memory");
			path=MemHandleLock(pathH);
			MemSet(path, MaxPathLen+1, 0);

			if (PrefGetAppPreferences((UInt32)CRID, PrfVFSPath, NULL, &pSize, false)!=noPreferenceFound) {
				UInt16 l=pSize;

				PrefGetAppPreferences((UInt32)CRID, PrfVFSPath, path, &l, false);
			} else
				*path='/';
		}
	} else
		prefs.searchDocs=PSDMemory;

	return true;
}

/*
 * fMainEH
 */
Boolean
fMainEH(EventType *ev)
{
	DocNode *n;
	Char *p;
	UInt32 pathLen;
	Int16 row, column;
	TableType *tbl;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		LocateDocuments();
	case frmUpdateEvent:
		UpdateDocStatus();
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cMainUp:
			for (p=path+StrLen(path); p>path; ) {
				p--;
				if (*p=='/') {
					*p=0;
					break;
				}
			}

			if (StrLen(path)==0) {
				*path='/';
				*(path+1)=0;
			}

			LocateDocuments();
			return true;

		case cMainRead:
			tbl=prefs.searchDocs==PSDMemory ? tblMainMemory.tbl : tblMainCard.tbl;
			if (TblGetSelection(tbl, &row, &column)==true) {
				TblUnhighlightSelection(tbl);
				n=(DocNode *)TblGetRowData(tbl, row);
				if (n->type==NodeTypeFile) {
					Char *p;

					DocClose(&doc);
					fFindReset();
					if (n->fileName) {
						UInt32 len=StrLen(n->fileName);
						MemHandle mh;

						mh=MemHandleNew(len+1);
						ErrFatalDisplayIf(mh==NULL, "(EH) Out of memory");
						p=MemHandleLock(mh);
						MemMove(p, n->fileName, len);
						p[len]=0;
					} else
						p=NULL;

					fMainStatus(txtPleaseWait);
					if ((doc=DocOpen(n->docName, p, 0))!=NULL)
						fReaderRun();
				}
			} else
				FrmAlert(aNothingSelected);

			return true;

		case cMainEdit:
			tbl=prefs.searchDocs==PSDMemory ? tblMainMemory.tbl : tblMainCard.tbl;
			if (TblGetSelection(tbl, &row, &column)==true) {
				TblUnhighlightSelection(tbl);
				n=(DocNode *)TblGetRowData(tbl, row);
				if (n->type==NodeTypeFile) {
					DocClose(&doc);
					if (Edit(n)==true)
						LocateDocuments();
				}
			} else
				FrmAlert(aNothingSelected);

			return true;
		}
		break;

	case popSelectEvent:
		if (ev->data.popSelect.listID==cMainList) {
			if ((ev->data.popSelect.selection!=ev->data.popSelect.priorSelection) && (ev->data.popSelect.selection!=noListSelection)) {
				prefs.searchDocs=(UInt8)ev->data.popSelect.selection;
				returnTop=0;
				LocateDocuments();
			}
		}
		break;

	case tblSelectEvent:
		tbl=prefs.searchDocs==PSDMemory ? tblMainMemory.tbl : tblMainCard.tbl;
		returnTop=(prefs.searchDocs==PSDMemory) ? tblMainMemory.top : tblMainCard.top;
		n=(DocNode *)TblGetRowData(tbl, ev->data.tblSelect.row);
		if (n->type==NodeTypeDirectory) {
			UInt32 cLen=StrLen(n->docName);

			TblUnhighlightSelection(tbl);
			pathLen=StrLen(path);
			if ((pathLen+cLen+1)<=MaxPathLen) {
				p=path+pathLen;
				if (*(p-1)!='/')
					*p++='/';

				MemMove(p, n->docName, cLen);
				p+=cLen;
				*p=0;

				LocateDocuments();
			} else
				FrmAlert(aMaxPathReached);
		}
		return true;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mMainAbout:
			fAboutRun();
			return true;
		case mMainBookmarks:
			fBookmarksRun(fMain);
			return true;
		case mMainHistory:
			if (DBOpen(&dbHistory, dmModeReadWrite, false)==true)
				FrmGotoForm(fHistory);
			else
				FrmAlert(aNoHistoryDB);
			return true;
		case mMainPreferences:
			fPreferencesRun();
			return true;
		}
		break;
	default:
		UITableEvents((prefs.searchDocs==PSDMemory) ? &tblMainMemory : &tblMainCard, ev);
		break;
	}

	return false;
}

/*
 * fMainStatus
 *
 * Draw the status text in a frame.
 *
 *  -> txt		Text.
 */
void
fMainStatus(Char *txt)
{
	FontID fID=FntSetFont(boldFont);
	UInt16 len=(UInt16)StrLen(txt), w, h;
	RectangleType r;

	RctSetRectangle(&r, 0, 15, dispW, dispH-34);
	WinEraseRectangle(&r, 0);

	h=FntCharHeight();
	w=FntCharsWidth(txt, len);
	RctSetRectangle(&r, (dispW-w-6)/2, 15+((dispH-h-6-15)/2), w+6, h+6);
	WinEraseRectangle(&r, 0);
	WinDrawRectangleFrame(boldRoundFrame, &r);
	WinDrawChars(txt, len, r.topLeft.x+3, r.topLeft.y+3);

	FntSetFont(fID);
}

/*
 * UpdateDocStatus
 *
 * Draw the "No documents ..." blurb.
 */
static void
UpdateDocStatus(void)
{
	if (docList.count==0) {
		FontID fID=FntSetFont(boldFont);
		UInt16 len=(UInt16)StrLen(txtUpdateDoc), w, h;
		RectangleType r;

		h=FntCharHeight();
		w=FntCharsWidth(txtUpdateDoc, len);
		RctSetRectangle(&r, 0, 15+((dispH-h-10-15)/2), 160, h+10);
		WinEraseRectangle(&r, 0);
		RctSetRectangle(&r, (dispW-w-6)/2, 15+((dispH-h-6-15)/2), w+6, h+6);
		WinDrawRectangleFrame(boldRoundFrame, &r);
		WinDrawChars(txtUpdateDoc, len, r.topLeft.x+3, r.topLeft.y+3);

		FntSetFont(fID);
	}
}

/*
 * LocateDocuments
 *
 * Find all documents on card or in memory.
 */
static void
LocateDocuments(void)
{
	fMainFree();
	fMainAlloc();

	switch (prefs.searchDocs) {
	case PSDCard:
		UIObjectHide(cMainTableMemory);
		UIObjectHide(cMainScrollBarMemory);
		fMainStatus(txtFinding);

		CardLocate();
		tblMainCard.records=docList.count;
		tblMainCard.top=returnTop;
		UITableUpdateValues(&tblMainCard, false);
		if (docList.count>0) {
			RectangleType r;
			RctSetRectangle(&r, 0, 15, dispW, dispH-34);
			WinEraseRectangle(&r, 0);

			UIObjectShow(cMainScrollBarCard);
			UIObjectShow(cMainTableCard);
			TblDrawTable(tblMainCard.tbl);
		}

		DrawPath();
		UIObjectShow(cMainPath);
		UIObjectShow(cMainUp);
		DrawUpButton();
		break;
	default:
		UIObjectHide(cMainTableCard);
		UIObjectHide(cMainScrollBarCard);
		UIObjectHide(cMainPath);
		UIObjectHide(cMainUp);

		fMainStatus(txtFinding);

		MemoryLocate();

		tblMainMemory.records=docList.count;
		tblMainMemory.top=returnTop;
		UITableUpdateValues(&tblMainMemory, false);
		if (docList.count>0) {
			RectangleType r;
			RctSetRectangle(&r, 0, 15, dispW, dispH-34);
			WinEraseRectangle(&r, 0);

			UIObjectShow(cMainScrollBarMemory);
			UIObjectShow(cMainTableMemory);
			TblDrawTable(tblMainMemory.tbl);
		}
		break;
	}

	if (!docList.count)
		FrmUpdateForm(fMain, frmRedrawUpdateCode);
}

/*
 * DrawUpButton
 *
 * Draw the Up button.
 */
static void
DrawUpButton(void)
{
	MemHandle mh=DmGetResource(bitmapRsc, bmpUp);
	RectangleType r;

	ErrFatalDisplayIf(mh==NULL, "(DrawUpButton) Cannot find up button");

	FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cMainUp), &r);
	WinDrawBitmap(MemHandleLock(mh), r.topLeft.x, r.topLeft.y);

	MemHandleUnlock(mh);
	DmReleaseResource(mh);
}

/*
 * AddNode
 *
 * Add a node to the docList.
 *
 *  -> docName		Document name.
 *  -> fileName		Name of file.
 *  -> type		File type.
 */
static void
AddNode(Char *docName, Char *fileName, UInt8 type)
{
	UInt32 docNameLen=StrLen(docName);
	MemHandle mh;
	DocNode *mp, *n, *n1;
	Char *p;

	mh=MemHandleNew(sizeof(DocNode)+docNameLen+1);
	ErrFatalDisplayIf(mh==NULL, "(AddNode) Out of memory");

	mp=MemHandleLock(mh);

	p=((Char *)mp)+sizeof(DocNode);
	mp->docName=p;
	MemMove(p, docName, docNameLen+1);

	mp->fileName=fileName;

	if (type==NodeTypeFile)
		for (n=docList.head; ((n->next) && (n->type==NodeTypeDirectory)); n=n->next);
	else
		n=docList.head;

	for (; ((n->next) && (n->type==type)); n=n->next) {
		if (StrCaselessCompare(mp->docName, n->docName)<0)
			break;
	}

	mp->type=type;
	n1=n->prev;
	n->prev=mp;
	n1->next=mp;
	mp->next=n;
	mp->prev=n1;

	docList.count++;
}

/*
 * DeleteNode
 *
 * Delete node and node data.
 *
 *  -> n		DocNode.
 */
static void
DeleteNode(DocNode *n)
{
	if (n) {
		if (n->fileName)
			MemPtrFree(n->fileName);

		MemPtrFree(n);
	}
}

/*
 * MemoryLocate
 *
 * Find documents stored in memory.
 */
static void
MemoryLocate(void)
{
	Boolean firstSearch=true;
	DmSearchStateType state;
	UInt16 card;
	LocalID dbID;
	Char dbName[DocNameLength];

	while (docList.count<32768) {
		if (DmGetNextDatabaseByTypeCreator(firstSearch, &state, (UInt32)'TEXt', (UInt32)'REAd', false, &card, &dbID)!=errNone)
			break;

		firstSearch=false;
		if (DmDatabaseInfo(card, dbID, dbName, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)!=errNone)
			continue;

		AddNode(dbName, NULL, NodeTypeFile);
	}
}

/*
 * DrawPath
 *
 * Draw current path on card.
 */
static void
DrawPath(void)
{
	RectangleType r;
	Int16 pW, w;
	Char *p;


	FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cMainPath), &r);
	r.topLeft.x+=r.extent.x+2;
	r.extent.x=dispW-r.topLeft.x;

	WinEraseRectangle(&r, 0);
	pW=r.extent.x-2;
	for (p=path; *p; p++);

	--p;
	for (; p>path; ) {
		w=FntCharWidth(*p);
		if ((pW-=w)<0) {
			p++;
			break;
		} else
			--p;
	}

	WinDrawChars(p, StrLen(p), r.topLeft.x, r.topLeft.y);
}

/*
 * CardLocate
 *
 * Find documents stored on card.
 */
static void
CardLocate(void)
{
	MemHandle ch=MemHandleNew(ComponentLength);
	UInt16 vRefNum;
	UInt32 v, d, fileSize;
	FileRef dirRef, fr;
	FileInfoType fileInfo;
	Char *component, *name;
	UInt8 pdb[78];
	Err err;

	ErrFatalDisplayIf(ch==NULL, "(CardLocate) Out of memory");
	component=MemHandleLock(ch);

	v=vfsIteratorStart;
	while (v!=vfsIteratorStop) {
		if (VFSVolumeEnumerate(&vRefNum, &v)!=errNone)
			break;

		if (VFSFileOpen(vRefNum, path, vfsModeRead, &dirRef)!=errNone)
			break;

		d=vfsIteratorStart;
		fileInfo.nameP=component;
		fileInfo.nameBufLen=ComponentLength;
		while (d!=vfsIteratorStop) {
			if (VFSDirEntryEnumerate(dirRef, &d, &fileInfo)!=errNone)
				break;

			if ((fileInfo.attributes&vfsFileAttrHidden))
				continue;

			if (!(fileInfo.attributes&vfsFileAttrDirectory)) {
				name=GetFullName(fileInfo.nameP, fileInfo.nameBufLen);
				if (VFSFileOpen(vRefNum, name, vfsModeRead, &fr)!=errNone) {
					MemPtrFree(name);
					continue;
				}

				fileSize=0;
				VFSFileSize(fr, &fileSize);
	
				if (fileSize<78) {
					VFSFileClose(fr);
					MemPtrFree(name);
					continue;
				}

				err=VFSFileRead(fr, 78, &pdb, NULL);
				VFSFileClose(fr);

				if (err!=errNone) {
					MemPtrFree(name);
					continue;
				}

				if (pdb[60]=='T' && pdb[61]=='E' &&
				    pdb[62]=='X' && pdb[63]=='t' &&
				    pdb[64]=='R' && pdb[65]=='E' &&
				    pdb[66]=='A' && pdb[67]=='d')
					AddNode((Char *)pdb, name, NodeTypeFile);
				else
					MemPtrFree(name);
			} else
				AddNode(component, NULL, NodeTypeDirectory);
		}
		VFSFileClose(dirRef);

		break;		/* Only enum first card! */
	}

	MemHandleFree(ch);
}

/*
 * GetFullName
 *
 * Add the path component to the current path.
 *
 *  -> c		Path component.
 *  -> len		Path component length (or 0 to calculate).
 *
 * Returns full path. Caller must free.
 */
static Char *
GetFullName(Char *c, UInt16 len)
{
	UInt32 pLen=StrLen(path);
	MemHandle mh;
	Char *p, *s;

	if (!len)
		len=StrLen(c);

	mh=MemHandleNew(pLen+2+len);
	ErrFatalDisplayIf(mh==NULL, "(GetFullName) Out of memory");

	s=MemHandleLock(mh);
	p=s;
	MemMove(p, path, pLen);
	p+=pLen;
	if (*(p-1)!='/')
		*p++='/';

	MemMove(p, c, len);
	p[len]=0;

	return s;
}

/*
 * Edit
 *
 * Edit metadata for this document.
 *
 *  -> n		Node.
 *
 * Returns true if document details was changed/deleted, false otherwise.
 */
static Boolean
Edit(DocNode *n)
{
	FormSave frm;

	docNode=n;
	dChanged=false;

	UIFormPrologue(&frm, fMainEdit, EEH);
	UIFieldSelectAll(cMainEditName, n->docName, true);

	if (UIFormEpilogue(&frm, EHook, cMainEditCancel)==false)
		return false;

	return dChanged;
}

/*
 * EEH
 */
static Boolean
EEH(EventType *ev)
{
	if (ev->eType==ctlSelectEvent) {
		Char *p=UIFieldGetText(cMainEditName);

		if ((p==NULL) || (StrLen(p)==0))
			return false;

		switch (ev->data.ctlSelect.controlID) {
		case cMainEditOK:
			if (StrCaselessCompare(p, docNode->docName)!=0)
				dChanged=Rename(docNode, p);

			break;

		case cMainEditDelete:
			if (FrmAlert(aDeleteDoc)==1)
				return true;

			dChanged=Delete(docNode);
			break;

		case cMainEditBeam:
			IRBeam(docNode);
			return true;
		}
	}

	return false;
}

/*
 * Rename
 *
 * Rename the database pointed to by the DocNode.
 *
 *  -> n		DocNode.
 *  -> newName		New name.
 *
 * Returns true if database was renamed, false otherwise.
 */
static Boolean
Rename(DocNode *n, Char *newName)
{
	if (n->fileName) {
		UInt32 v=vfsIteratorStart;
		Char *errText="Unknown error.";
		UInt16 vRefNum;
		FileRef fileRef;

		if (VFSVolumeEnumerate(&vRefNum, &v)==errNone) {
			if (VFSFileOpen(vRefNum, n->fileName, vfsModeReadWrite, &fileRef)==errNone) {
				if (VFSFileSeek(fileRef, vfsOriginBeginning, 0)==errNone) {
					UInt32 len=StrLen(newName), read;

					if ((VFSFileWrite(fileRef, len+1, newName, &read)==errNone) && (read==len+1)) {
						VFSFileClose(fileRef);
						FixupNode(n, newName);
						return true;
					} else
						errText="Failed to write to file";

				} else
					errText="Cannot seek";

				VFSFileClose(fileRef);
			} else
				errText="Cannot open file";
		
		} else
			errText="Cannot enumerate cards";

		FrmCustomAlert(aCannotRename, errText, NULL, NULL);
	} else {
		DB db;
		Char *p;

		DBInit(&db, newName, NULL);
		if (DBFindDatabase(&db)==FDNotFound) {
			DBInit(&db, n->docName, NULL);
			if (DBFindDatabase(&db)==FDFound) {
				switch (DmSetDatabaseInfo(db.card, db.id, newName, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)) {
				case dmErrInvalidDatabaseName:
					p="Invalid Database Name";
					break;
				case dmErrAlreadyExists:
					p="Database Already Exists";
					break;
				case dmErrInvalidParam:
					p="Invalid Paramter";
					break;
				default:
					FixupNode(n, newName);
					return true;
				}

				FrmCustomAlert(aCannotRename, p, NULL, NULL);
			} else
				FrmCustomAlert(aDatabaseNotFound, n->docName, NULL, NULL);
		} else
			FrmAlert(aAlreadyExist);
	}

	return false;
}

/*
 * FixupNode
 *
 * Re-insert node.
 *
 *  -> n		DocNode.
 *  -> newName		New name.
 */
static void
FixupNode(DocNode *n, Char *newName)
{
	DocNode *nxt=n->next, *prv=n->prev;
	
	StrNCopy(n->docName, newName, DocNameLength);

	prv->next=nxt;
	nxt->prev=prv;

	n->next=NULL;
	n->prev=NULL;

	for (nxt=docList.head; ((nxt->next) && (nxt->type==NodeTypeDirectory)); nxt=nxt->next);

	for (; nxt->next; nxt=nxt->next) {
		if (StrCaselessCompare(n->docName, nxt->docName)<0)
			break;
	}

	prv=nxt->prev;
	nxt->prev=n;
	prv->next=n;
	n->next=nxt;
	n->prev=prv;
}

/*
 * Delete
 *
 * Delete the document pointed to by DocNode.
 *
 *  -> n		DocNode.
 *
 * Returns true if document was deleted, false otherwise.
 */
static Boolean
Delete(DocNode *n)
{
	DocNode *nxt=n->next, *prv=n->prev;
	Char buffer[64];
	UInt16 err;

	if (n->fileName) {
		UInt32 v=vfsIteratorStart;
		UInt16 vRefNum;

		if (VFSVolumeEnumerate(&vRefNum, &v)!=errNone) {
			FrmCustomAlert(aCannotDelete, "Cannot enumerate card", NULL, NULL);
			return false;
		}

		if ((err=VFSFileDelete(vRefNum, n->fileName))!=errNone) {
			StrPrintF(buffer, "VFSFileDelete error: %u", err);
			FrmCustomAlert(aCannotDelete, buffer, NULL, NULL);
			return false;
		}
	} else {
		DB db;

		DBInit(&db, n->docName, NULL);
		if (DBFindDatabase(&db)!=FDFound) {
			FrmCustomAlert(aCannotDelete, "Cannot find database", NULL, NULL);
			return false;
		}

		if ((err=DmDeleteDatabase(db.card, db.id))!=errNone) {
			StrPrintF(buffer, "DmDeleteDatabase error: %u", err);
			FrmCustomAlert(aCannotDelete, buffer, NULL, NULL);
			return false;
		}
	}

	prv->next=nxt;
	nxt->prev=prv;

	DeleteNode(n);
	docList.count--;
	return true;
}

/*
 * EHook
 *
 * Edit Hook.
 */
static void
EHook(void)
{
	if (prefs.searchDocs==PSDMemory) {
		tblMainMemory.records=docList.count;
		UITableUpdateValues(&tblMainMemory, false);
	} else {
		tblMainCard.records=docList.count;
		UITableUpdateValues(&tblMainCard, false);
	}
}

/*
 * fMainRethink
 *
 * Rethink main screen.
 */
void
fMainRethink(void)
{
	if (FrmGetActiveFormID()==fMain)
		LocateDocuments();
}
