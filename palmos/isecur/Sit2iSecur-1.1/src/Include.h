#ifndef INCLUDE_H
#define INCLUDE_H

#include <PalmOS.h>
#include "Resources.h"

/* macros */
#define EVER ;;
#define AESKeyLength 32
#define PasswordLength 32

enum { FDFound, FDNotFound, FDError };

typedef struct {
	Char name[dmDBNameLength];
	DmOpenRef db;
	LocalID id;
       	UInt16 card;
} DB;

typedef struct {
	FormType *oldForm;
	UInt16 oldId;
	FormActiveStateType state;
} FormSave;

typedef struct {
	UInt32 encKey[64];
	UInt32 decKey[64];
} AESKeys;

typedef struct {
	UInt32 parray[18];
	UInt32 sbox1[256];
	UInt32 sbox2[256];
	UInt32 sbox3[256];
	UInt32 sbox4[256];
} BlowfishKey;

typedef struct {
	UInt32 state[8];
	UInt32 cnt[2];
	UInt8 buffer[64];
	UInt8 digest[32];
} SHA256;

typedef struct {
	AppInfoType appInfo;
	UInt16 pwdGiven;
	UInt32 md5[4];
} SitAppInfoBlock;

typedef struct {
	AppInfoType appInfo;
	UInt8 digest[32];
	UInt8 key[AESKeyLength];
} ISecurAppInfoBlock;

typedef struct {
	UInt16 flags;
	DmResID iconID;
} RecordHdr;
#define noIconSelected 0xffff
#define RHTemplate 1
#define RHHasSecret 0x8000
#define RHHasTitle 0x4000

/* Sections */
#ifdef _AES
#undef _AES
#endif
#define _AES(x) x __attribute__ ((section("aes")));

#ifdef _BF
#undef _BF
#endif
#define _BF(x) x __attribute__ ((section("bf")));

#ifdef _SHA
#undef _SHA
#endif
#define _SHA(x) x __attribute__ ((section("sha")));

#ifdef _UI
#undef _UI
#endif
#define _UI(x) x __attribute__ ((section("ui")));

/* AES.c */
_AES(void AESOpen(void))
_AES(void AESClose(void))
_AES(void AESCreateKeys(AESKeys *, UInt8 *))
_AES(void AESEncrypt(AESKeys *, UInt8 *, UInt8 *))
_AES(void AESDecrypt(AESKeys *, UInt8 *, UInt8 *))

/* Blowfish.c */
_BF(void BlowfishMakeKey(BlowfishKey *, UInt8 *, UInt32))
_BF(void BlowfishEncrypt(BlowfishKey *, UInt8 *))
_BF(void BlowfishDecrypt(BlowfishKey *, UInt8 *))

/* DB.c */
UInt16 DBFindDatabase(DB *);
void DBInit(DB *, Char *);
Boolean DBOpen(DB *, UInt16, Boolean);
void DBClose(DB *);
void DBClear(DB *);
Int16 DBSort(void *, void *, Int16, SortRecordInfoType *, SortRecordInfoType *, MemHandle);
UInt32 DBSetRecord(DB *, UInt16, UInt16, UInt16, DmResID, Char *, Char *, DmComparF);

/* MD5.c */
void MD5Sum(UInt32 *, UInt8 *);

/* PilotMain.c */
#ifdef DEBUG
void HexDump(UInt8 *, UInt16);
#else
#define HexDump(x, y)	;
#endif

/* SHA256.c */
_SHA(void SHA256Init(SHA256 *))
_SHA(void SHA256Transform(SHA256 *, UInt8 *, UInt32 ))
_SHA(void SHA256Final(SHA256 *))

/* UI.c */
_UI(void *UIObjectGet(UInt16))
_UI(void UIFieldFocus(UInt16))
_UI(Boolean UIFieldRefocus(UInt16 *, WChar))
_UI(Char *UIFieldGetText(UInt16))
_UI(void UIFieldSetText(UInt16, Char *))
_UI(Boolean UIFieldScrollBarHandler(EventType *, UInt16, UInt16))
_UI(Boolean UIFieldScrollBarKeyHandler(EventType *, UInt16, UInt16))
_UI(void UIFieldUpdateScrollBar(UInt16, UInt16))
_UI(void UIFieldScrollLines(UInt16, Int16))
_UI(void UIFieldPageScroll(UInt16, UInt16, WinDirectionType))

/* fDone.c */
Boolean fDoneEH(EventType *);

/* fMain.c */
Boolean fMainInit(void);
Boolean fMainEH(EventType *);

/* fPasswords.c */
Boolean fPasswordsEH(EventType *);

/* fReo.c */
void fReoClose(void);
void fReoRun(Char *, Char *);

/* Debug.c */
#if DEBUG==1
#include <HostControl.h>
void DAlert(UInt8 *, ...);
#define D(x)		x
#define DPrint(x...)	HostFPrintF(gHostFile, x); \
			HostFFlush(gHostFile);
#else
#define DAlert(x, ...)	;
#define D(x)		;
#define DPrint(x...)	;
#endif

#ifdef MAIN
	UInt16 currentFormID, recordIdx;
	FormType *currentForm;
	Boolean appStopped;
	AESKeys aes;
#ifdef DEBUG
	HostFILE *gHostFile;
#endif
#else
	extern UInt16 currentFormID, recordIdx;
	extern FormType *currentForm;
	extern Boolean appStopped;
	extern AESKeys aes;
#ifdef DEBUG
	extern HostFILE *gHostFile;
#endif
#endif

#endif
