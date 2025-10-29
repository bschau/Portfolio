#ifndef SIT_H
#define SIT_H

#include <PalmOS.h>
#include "resources.h"

/* macros */
#define EVER ;;
#define SECRETSDATABASENAME "Secure It Secrets"
#define TEMPLATESDATABASENAME "Secure It Templates"
#define CREATORID 'SeSt'
#define DATABASETYPE 'DATA'
#define DATABASEVERSION 1
#define CATNONEDITABLES 1
#define FORMTITLELEN 18
#define SEVERR -1	/* severe error */
#define DISPGETNEXTNEWDB 1
#define DISPDONENEWDB 2
#define DISPGETPASSWORD 3
#define DISPGETNEXTCHPWD 4
#define DISPDONECHPWD 5
#define ROMNONE 0
#define ROMV2 2
#define ROMV3 3
#define PASSDOUBLE 0
#define PASSSINGLE 1
#define PINSHUFFLE 0
#define PINPHONEPAD 1
#define PINCALCULATOR 2
#define PINTEXTUAL 3
#define LOGOUT30 0
#define LOGOUT1 1
#define LOGOUT2 2
#define LOGOUT3 3
#define LOGOUTPREFS 4
#define KABOOMNEVER 0
#define KABOOM1 1
#define KABOOM3 2
#define KABOOM5 3
#define GETOFFSET(x) ((((UInt32)*(x))<<8)|((UInt32)*(x+1)))

enum { PrfApplication=0, PrfSaveType, PrfSaveData };
enum { TypeNone=0, TypeSecret, TypeTemplate };

/* structures */
/* might want to expand on this later ... */
typedef struct {
	AppInfoType appinfo;
	UInt16 pwdgiven;
	UInt32 md5[4];
} aiblock;

/* this is obsolete (v1 AppInfoBlock) */
typedef struct {
	AppInfoType appinfo;
	UInt16 pwdgiven;
	UInt32 md5[4];
} oldaiblock;

typedef struct {
	UInt32 flags;
	UInt16 catidx;
	UInt16 pinidx;
	UInt16 alaidx;
	UInt16 kaboomidx;
	UInt16 pinstars;
	/* add preferences here! */
} sitpref;

#define PREFBITQUICKACCESS 1
#define PREFQUICKACCESS (1<<PREFBITQUICKACCESS)
#define PREFBITCONFIRMDELETE 2
#define PREFCONFIRMDELETE (1<<PREFBITCONFIRMDELETE)
#define PREFBITPROTECTDB 3
#define PREFPROTECTDB (1<<PREFBITPROTECTDB)
#define PREFBITPAGESCROLL 4
#define PREFPAGESCROLL (1<<PREFBITPAGESCROLL)
#define PREFBITLOCKDEVICE 5
#define PREFLOCKDEVICE (1<<PREFBITLOCKDEVICE)
#define PREFBITLOGOUTPD 6
#define PREFLOGOUTPD (1<<PREFBITLOGOUTPD)

typedef struct {
	UInt16 idx;
	UInt8 name[dmCategoryLength];
} sitlist;

typedef struct {
	UInt32 parray[18];
	UInt32 sbox1[256];
	UInt32 sbox2[256];
	UInt32 sbox3[256];
	UInt32 sbox4[256];
} blowfishkey;

typedef struct {
	UInt8 *secret;
	UInt8 *title;
	UInt32 secretlen;
	UInt32 titlelen;
} recdesc;

/* about.c */
void aboutinit(FormPtr);

/* blowfish.c */
void blowfishmakekey(blowfishkey *, UInt8 *, UInt32);
void blowfishencrypt(blowfishkey *, UInt8 *);
void blowfishdecrypt(blowfishkey *, UInt8 *);

/* database.c */
Int16 opendbs(void);
void closedbs(void);
Boolean storerecord(DmOpenRef, UInt8 *, UInt8 *, UInt16, UInt16);
void recorddesc(recdesc *, UInt8 *);
MemHandle formrecord(Boolean, UInt32 *, UInt8 *, UInt8 *);
Int16 recordcompare(UInt8 *, UInt8 *, Int16, SortRecordInfoPtr, SortRecordInfoPtr, MemHandle);

/* editentry.c */
void preparetoedit(UInt16, MemHandle);
void editentryinit(FormPtr);

/* export.c */
void exporttomemo(FormPtr, UInt16);
#define EXPORTFROMSECRETS 0
#define EXPORTFROMTEMPLATES 1

/* import.c */
UInt16 importmemo(FormPtr, UInt16, UInt16 *);
#define IMPORTTOSECRETS 0
#define IMPORTTOTEMPLATES 1

/* main.c */
void enabletimeout(void);
void disabletimeout(void);

/* md5.c */
void md5(UInt32 *, UInt8 *);

/* password.c */
void getpassword(UInt16);
Int16 doublepassword(UInt8 *, UInt8 *);
Int16 singlepassword(UInt8 *);
Int16 setpwdindb(UInt16);
void changepassword(UInt8 *);

/* pincode.c */
UInt16 initshuffle(UInt16);
UInt16 initphonepad(UInt16);
UInt16 initcalculator(UInt16);
void pincodeinit(FormPtr);

/* preferences.c */
void visual(FormPtr);
void security(FormPtr);
void rethinkpreferences(void);
void setdbprotect(void);

/* reo.c */
DmOpenRef reo1to2(void);
void reoto2r5(void);

/* scrollbar.c */
Boolean scrollbarhandler(EventPtr, UInt16, UInt16);
Boolean scrollbarkeyhandler(EventPtr, UInt16, UInt16);
void updatescrollbar(FormPtr, UInt16, UInt16);

/* showentry.c */
void showrecord(UInt16);
void showentryinit(FormPtr);

/* start.c */
void doneshow(void);
void startinit(FormPtr);
void infobox(FormPtr, UInt16);
void gotostart(void);

/* templates.c */
void templateeditorinit(FormPtr);
void edittemplate(UInt16, MemHandle);
void edittemplateinit(FormPtr);
void selecttemplate(FormPtr, UInt16);

/* textual.c */
UInt16 inittextual(UInt16);
void textualinit(FormPtr);

/* utils.c */
MemPtr objectinform(FormPtr, UInt16);
Int16 generatekey(MemHandle *, UInt8 *);
void setfieldtext(MemHandle, UInt8 *, UInt16, UInt16, UInt16, FormPtr);
void buildlist(FormPtr, UInt16, DmOpenRef, UInt16, void (*)(Int16, RectangleType *, Char **));
void dynamiclistdraw(DmOpenRef, UInt16, RectangleType *);
Boolean populate2fieldsmemhandle(FormPtr, MemHandle, UInt16, UInt16);
Boolean populate2fields(FormPtr, DmOpenRef, UInt16, UInt16, UInt16);
Boolean refocus(EventPtr, FormPtr, UInt16, UInt16);
Int16 fieldblank(UInt8 *);
DmOpenRef openmemodb(void);
Int16 deletedialog(FormPtr);
void updatetoggle(UInt16, UInt16, UInt16);
void decrypt(UInt8 *, UInt32);
Boolean saveentry(UInt16, UInt16, UInt8 *, UInt8 *);
void loadentry(void);

/* working.c */
void working(FormPtr, UInt16);
void stopworking(void);
void updatevisual(UInt8 *);

/* debug.c */
#ifdef DEBUG
void dalert(UInt8 *, ...);
#endif

#ifdef MAIN
	DmOpenRef sitdb, tmpldb, memodb;
	sitlist catlist;
	UInt8 formtitle[FORMTITLELEN+2];
	sitpref prefs;
	UInt16 kaboom, autooff, romv, pwdgiven, nopwd, preloadtmpl;
	MemHandle mainkeyh;
	UInt32 md5pwd[4];
	CustomPatternType blankpattern={ 0, 0, 0, 0, 0, 0, 0, 0 };
#else
	extern DmOpenRef sitdb, tmpldb, memodb;
	extern sitlist catlist;
	extern UInt8 formtitle[FORMTITLELEN+2];
	extern sitpref prefs;
	extern UInt16 kaboom, autooff, romv, pwdgiven, nopwd, preloadtmpl;
	extern MemHandle mainkeyh;
	extern UInt32 md5pwd[4];	
	extern CustomPatternType blankpattern;
#endif

#endif
