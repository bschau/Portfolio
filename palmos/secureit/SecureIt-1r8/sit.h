#ifndef SIT_H
#define SIT_H

#include <PalmOS.h>
#include "resources.h"

/* macros */
#define EVER ;;
#define DATABASENAME "secureit-db"
#define DATABASETYPE "cntn"
#define DATABASEVERSION 1
#define CATNONEDITABLES 1
#define FORMTITLELEN 18
#define SEVERR -1	/* severe error */
#define EKEYLEN 32
#define ETXTLEN 64
#define BSSIZE (EKEYLEN+ETXTLEN)
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
#define KABOOMNEVER 0
#define KABOOM1 1
#define KABOOM3 2
#define KABOOM5 3

/* structures */
/* might want to expand on this later ... */
typedef struct {
	AppInfoType appinfo;
	UInt16 pwdgiven;
	unsigned long md5[4];
} aiblock;

typedef struct {
	char quickaccess;
	char confirmdelete;
	char protectdb;
	UInt16 catidx;
	UInt16 pinidx;
	UInt16 alaidx;
	UInt16 kaboomidx;
	UInt16 pinstars;
} sitpref;

typedef struct {
	ListType list;
	UInt16 valid;
	UInt16 idx;
	Char name[dmCategoryLength];
} sitlist;

typedef struct {
	Char key[EKEYLEN+1];
	Char txt[ETXTLEN+1];
} record;

typedef struct {
	unsigned long parray[18];
	unsigned long sbox1[256];
	unsigned long sbox2[256];
	unsigned long sbox3[256];
	unsigned long sbox4[256];
} blowfishkey;

/* blowfish.c */
void blowfishmakekey(blowfishkey *, unsigned char *, unsigned int);
void blowfishencrypt(blowfishkey *, unsigned long *, unsigned long *);
void blowfishdecrypt(blowfishkey *, unsigned long *, unsigned long *);

/* editentry.c */
void editentryinit(FormPtr);

/* main.c */
void enabletimeout(void);
void disabletimeout(void);

/* md5.c */
void md5(unsigned long *, unsigned char *);

/* password.c */
void getpassword(UInt16);
Int16 doublepassword(Char *, Char *);
Int16 singlepassword(Char *);
void changedbpwd(Char *);

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

/* showentry.c */
int showrecord(Int16, UInt16, FormPtr);

/* start.c */
void startinit(FormPtr);
void about(FormPtr, UInt16);
void gotostart(void);

/* textual.c */
UInt16 inittextual(UInt16);
void textualinit(FormPtr);

/* utils.c */
Int16 insertrecord(char *, UInt16);
void newlist(sitlist *, UInt16);
void freelist(sitlist *);
MemPtr findobject(UInt16);
MemPtr objectinform(FormPtr, UInt16);
void recpack(char *, char *, char *);
void recunpack(record *, char *);
int setfieldtext(FormPtr, UInt16, Char *);
Boolean handleeditmenu(EventPtr, FormPtr);

#ifdef MAIN
	DmOpenRef sitdb;
	sitlist catlist, entrylist;
	Char formtitle[FORMTITLELEN+2], editflag;
	sitpref prefs;
	record trec;
	UInt16 editindex, kaboom, autooff, romv, pwdgiven;
	MemHandle keyh, mainkeyh;
	Char *keyp;
	unsigned long md5pwd[4];
	Char password[PASSWORDLEN+2];
#else
	extern DmOpenRef sitdb;
	extern sitlist catlist, entrylist;
	extern Char formtitle[FORMTITLELEN+2], editflag;
	extern sitpref prefs;
	extern record trec;
	extern UInt16 editindex, kaboom, autooff, romv, pwdgiven;
	extern MemHandle keyh, mainkeyh;
	extern Char *keyp;
	extern unsigned long md5pwd[4];
	Char password[PASSWORDLEN+2];
#endif

#endif
