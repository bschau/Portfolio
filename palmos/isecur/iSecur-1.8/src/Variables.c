#include "Include.h"

/* PilotMain.c */
UInt16 libRefNum, kbdRefNum;
MemHandle aesData, sha256Data;

/* fMain.c */
Int16 mainLstIndex;

/* fCreateDatabase.c */
UInt16 sCnt, rounds, lastX, lastY;

/* fEdit.c */
UInt16 category, forceLoad;
DmResID iconID;
UInt8 startChar;

/* fIcon.c */
Int32 sIdx, iIdx, maxLines;
Boolean bIconSelected;
DmResID *dstIconID;
UInt16 inIcons, iX, iY;

/* fIE.c */
Boolean isImport;

/* fKeywords.c */
MemHandle keywordsH;
UInt16 keywordsCnt, keywordsIdx;
Int16 current;
Boolean keywordsDone, useKeywords;
Char bufDate[longDateStrLength+1], bufTime[timeStringLength+1];

/* fLetCreate.c */
Char name[dmDBNameLength];

/* fLetManage.c */
MemHandle sletH;

/* fPassword.c */
UInt16 dialogType, maxAttempts, errY, titleIdx, srcVersion;
Boolean runLoop, pwdValid;
Int32 timeOut, deleteStringDelay;
AppInfoBlock *appInfo;

/* fPreferences.c */
Prefs gPrefs;

/* fPwdGen.c */
//UInt8 *dstPwd;
Boolean bPwdGen;

/* fShow.c */
DmResID iconID;
UInt8 *gTitle;
UInt16 gTitleIdx, catIdx;
Boolean editable, isTemplate;

/* fTemplate.c */
Char catName[dmCategoryLength];
UInt16 *dstRecIdx;
UInt16 tmplIdx;
Boolean tReturn;

/* Include.h */
Char *tArrUp="\x01", *tArrDown="\x02", *tArrUpDis="\x03", *tArrDownDis="\x04";
UI *ui;
DB dbData, dbMemo;
UInt16 currentFormID, recordIdx;
FormType *currentForm;
Boolean appStopped, notificationsEnabled;
Prefs prefs;
State state;
UInt8 gKey[AESKeyLength];
Char catName1[dmCategoryLength];
DmOpenRef dbRefIcons;
UInt16 cntIcons;
UInt8 loginDigest[SHA256DigestLength];
UInt8 *pool;
Char gPwd[PasswordLength+2];
#ifdef DEBUG
HostFILE *gHostFile;
#endif
