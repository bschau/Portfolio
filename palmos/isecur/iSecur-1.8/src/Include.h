#ifndef INCLUDE_H
#define INCLUDE_H

#include <PalmOS.h>
#include <FrmGlue.h>
#include <TxtGlue.h>
#include "Resources.h"
#include "iSecurLib.h"
#include "PublicApi.h"

/* macros */
#define EVER ;;
#define PREFSVER 1
#define DBVERSION 2
#define DBNameData "iSec_iSecur Data"
#define EntropyPoolLength 64
#define iSecurAppLaunchCmdChangePassword 0x8000
#define iSecurFtrHasGlobals 1
#define iSecurLetResID 100
#define AIBUnused ' '
#define AIBPasswordHint 'H'

enum { PrfApplication=0, PrfState, PrfPwdChangeAlarm, PrfRetentionList, PrfEntropy, PrfSaveData };
enum { FDFound, FDNotFound, FDError };
enum { PwdDialogNormal, PwdDialogConfirm };

typedef struct {
	Char name[dmDBNameLength];
	DmOpenRef db;
	LocalID id;
       	UInt16 card;
} DB;

typedef struct {
	UInt32 dispW;
	UInt32 dispH;
	UInt32 depth;
	Boolean colorEnabled;
	Boolean navSupported;
	IndexedColorType backColor;
	IndexedColorType foreColor;
} UI;

typedef struct {
	FormType *oldForm;
	UInt16 oldId;
	FormActiveStateType state;
} FormSave;

typedef struct {
	UInt16 id;
	Char *button;
} Gadget;

typedef struct {
	UInt32 flags;
	UInt16 pwdRetention;
	UInt16 maxFailedLogins;
	UInt16 pwdChangeDays;
} Prefs;
#define PFlgPwdShowText 1
#define PFlgSecretOpenEdit 2
#define PFlgEnableFind 4
#define PFlgLogoutPower 8
#define PFlgLogoutTimeout 16
#define PFlgEmptyClipboard 32
#define PFlgDestroyDB 64
#define PFlgNoBeam 128
#define PFlgNewSecret 256
#define PFlgSecretStay 512
#define PFlgTemplateInherit 1024
#define PFlg5Way 2048
#define PFlgAutoPopupKbd 4096

typedef struct {
	UInt16 category;
	UInt16 filter;
	UInt16 pwdGenLength;
	UInt16 pwdGenContains;
	UInt16 prfPage;
	UInt16 memoCat;
	UInt16 impCat;
	UInt16 expCat;
	UInt16 impExpFlags;
} State;
#define STIEFImpDelete 1
#define STIEFImpTemplate 2
#define STIEFExpDelete 4

typedef struct {
	AppInfoType appInfo;
	UInt8 digest[SHA256DigestLength];
	UInt8 key[AESKeyLength];
} AppInfoBlock;

typedef struct {
	UInt16 flags;
	DmResID iconID;
} RecordHdr;
#define noIconSelected 0xffff
#define RHTemplate 1
#define RHHasSecret 0x8000
#define RHHasTitle 0x4000

/* Sections */
#ifdef _FRM
#undef _FRM
#endif
#define _FRM(x) x __attribute__ ((section("frm")))

#ifdef _PWD
#undef _PWD
#endif
#define _PWD(x) x __attribute__ ((section("pwd")))

#ifdef _UI
#undef _UI
#endif
#define _UI(x) x __attribute__ ((section("ui")))

#ifdef _UTL
#undef _UTL
#endif
#define _UTL(x) x __attribute__ ((section("utl")))

#define AESOpen()	iSecurLibAESOpen(libRefNum)
#define AESClose()	iSecurLibAESClose(libRefNum, aesData)
#define AESCreateKeys(key)	iSecurLibAESCreateKeys(libRefNum, aesData, key)
#define AESEncrypt(src, dst)	iSecurLibAESEncrypt(libRefNum, aesData, src, dst)
#define AESDecrypt(src, dst)	iSecurLibAESDecrypt(libRefNum, aesData, src, dst)

#define SHA256Open()	iSecurLibSHA256Open(libRefNum)
#define SHA256Close()	iSecurLibSHA256Close(libRefNum, sha256Data)
#define SHA256Init()	iSecurLibSHA256Init(libRefNum, sha256Data)
#define SHA256Transform(src, len)	iSecurLibSHA256Transform(libRefNum, sha256Data, src, len)
#define SHA256Final()	iSecurLibSHA256Final(libRefNum, sha256Data)

/* AIBExtension.c */
_UTL(void AIBExtensionSet(Char, Char *));
_UTL(MemHandle AIBExtensionGet(Char));

/* DB.c */
/* Do not put these in their own segment! */	
Int16 DBSort(void *, void *, Int16, SortRecordInfoType *, SortRecordInfoType *, MemHandle);
UInt16 DBFindDatabase(DB *);
void DBInit(DB *, Char *);
Boolean DBOpen(DB *, UInt16, Boolean);
void DBClose(DB *);
void DBClear(DB *);
UInt32 DBSetRecord(DB *, UInt16, UInt16, UInt16, DmResID, Char *, Char *, DmComparF);
MemHandle DBGetRecord(DB *, UInt16);
Err DBFindRecordByID(DB *, UInt32, UInt16 *);

/* Entropy.c */
_UTL(UInt8 *EntropyOpen(void));
_UTL(void EntropyClose(UInt8 *));
_UTL(void EntropyAdd(UInt8 *, EventType *));
_UTL(UInt8 EntropyGet(UInt8 *));

/* Find.c */
/* Must be in same segment as PilotMain */
void FindSearch(FindParamsPtr);

/* IR.c */
/* Do not put these in their own segment! */	
void IRReceive(ExgSocketType *);
void IRImport(void);

/* PilotMain.c */
/* Do not put these in their own segment! */	
Boolean PMGetPref(void *, UInt16, UInt16, Boolean);
void PMSetPref(void *, UInt16, UInt16, Boolean);
void PMOpenSecret(UInt16);
Boolean PMHardPower(EventType *);
//void PMFPCSetup(void);
//void PMFPCSetAlarm(void);
void PMQueueAppStopEvent(void);
void PMPwdChgAlarm(void);

/* Random.c */
UInt32 Random(UInt32);

/* UI.c */
_UI(UI *UIInit(void));
_UI(void *UIObjectGet(UInt16));
_UI(void UIObjectShow(UInt16));
_UI(void UIObjectHide(UInt16));
_UI(void UIFormPrologue(FormSave *, UInt16, FormEventHandlerType *));
_UI(Boolean UIFormEpilogue(FormSave *, void (*)(), UInt16));
_UI(void UIFormRedraw(void));
_UI(void UIFieldFocus(UInt16));
_UI(Boolean UIFieldRefocus(UInt16 *, WChar));
_UI(Char *UIFieldGetText(UInt16));
_UI(void UIFieldSetText(UInt16, Char *));
_UI(void UIFieldClear(UInt16));
_UI(void UIFieldSelectAll(UInt16, Char *, Boolean));
_UI(Boolean UIFieldScrollBarHandler(EventType *, UInt16, UInt16));
_UI(Boolean UIFieldScrollBarKeyHandler(EventType *, UInt16, UInt16));
_UI(void UIFieldUpdateScrollBar(UInt16, UInt16));
_UI(void UIFieldScrollLines(UInt16, Int16));
_UI(void UIFieldPageScroll(UInt16, UInt16, WinDirectionType));
_UI(void UIPopupSet(DmOpenRef , UInt16, Char *, UInt16));
_UI(UInt16 UIGadgetFontHandler(Gadget *, FontID, UInt16, UInt16));
_UI(void UIListInit(UInt16, void (*)(Int16, RectangleType *, Char **), Int16, Int16));
_UI(Boolean UIListKeyHandler(EventType *, UInt16, UInt16));
_UI(void UIListUpdateFocus(UInt16, Int16));

/* fAbout.c */
_UTL(void fAboutRun(void));

/* fCreateDatabase.c */
_UTL(Boolean fCreateDatabaseInit(void));
_UTL(Boolean fCreateDatabaseEH(EventType *));
_UTL(void SetPassword(AppInfoBlock *, Char *, Int16));

/* fEdit.c */
_FRM(void fEditRun(UInt16, WChar, Boolean));
_FRM(Boolean fEditInit(void));
_FRM(Boolean fEditEH(EventType *));

/* fIcon.c */
_UTL(void fIconOpen(void));
_UTL(void fIconClose(void));
_UTL(Boolean fIconRun(DmResID *));

/* fIE.c */
_FRM(void fIERun(Boolean));
_FRM(void fIEClose(void));
_FRM(Boolean fIEInit(void));
_FRM(Boolean fIEEH(EventType *));

/* fLetCreate.c */
_UTL(void fLetCreateRun(UInt16));

/* fLetManage.c */
_UTL(void fLetManageRun(void));

/* fKeywords.c */
_FRM(void fKeywordsInit(void));
_FRM(Boolean fKeywordsOpen(Char *, Char *));
_FRM(void fKeywordsClose(void));
_FRM(Boolean fKeywordsRun(Char *, Char *, MemHandle *, MemHandle *));

/* fMain.c */
_FRM(Boolean fMainInit(void));
_FRM(Boolean fMainEH(EventType *));
_FRM(void fMainChangePassword(void));
_FRM(void fMainDrawIcon(UInt16, UInt16, UInt16));
_FRM(UInt16 fMainFindByLetter(Char, UInt16, UInt16));

/* fNewDatabase.c */
_UTL(Boolean fNewDatabaseEH(EventType *));

/* fPassword.c */
_PWD(Boolean fPasswordRun(UInt16, AppInfoBlock *, UInt16, UInt16, Boolean, UInt16));
_PWD(void fPasswordRunMullify(UInt8 *));

/* fPasswordHint.c */
_PWD(void fPasswordHintRun(void));

/* fPwdGen.c */
_PWD(Boolean fPwdGenRun(void));
_PWD(void fPwdGenGenerate(UInt16, UInt16));
_PWD(void fPwdGenRequester(void));
#define PGLower 1
#define PGUpper 2
#define PGDigits 4
#define PGSpecials 8

/* fPreferences.c */
_UTL(void fPreferencesRun(void));

/* fShow.c */
_FRM(void fShowOpen(void));
_FRM(void fShowClose(void));
_FRM(void fShowRun(UInt16, Boolean));
_FRM(Boolean fShowInit(void));
_FRM(Boolean fShowEH(EventType *));

/* fTemplate.c */
_UTL(Boolean fTemplateRun(UInt16, UInt16 *));

/* globals */
	extern FormType *currentForm;
	extern UInt16 currentFormID;
	extern UInt8 *pool;
	extern Boolean appStopped;

#endif
