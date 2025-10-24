#ifndef QALM_H
#define QALM_H

#include <PalmOS.h>
#include <unix_stdarg.h>
#include "resources.h"

/* macros */
#define EVER ;;
#define DATABASEVERSION 1
#define QALMDATABASENAME "LoginMan Realms"
#define CREATORID 'QALM'
#define DATABASETYPE 'DATA'
#define GETOFFSET(x) ((((UInt32)*(x))<<8)|((UInt32)*(x+1)))

#define toupper(x) (((x)>=96) ? (x)-32 : x)

/* structs */
typedef struct {
	UInt16 this;
	UInt16 other;
} refocustab;

typedef struct {
	UInt8 *realm;
	UInt8 *login;
	UInt8 *password;
} realmrecord;

typedef struct {
	UInt16 size;
	MemHandle mh;
} realmpacked;

typedef struct {
	UInt16 protocol;
	UInt8 *realm;
	UInt8 *login;
	UInt8 *password;
	UInt8 *url;
} arguments;
#define PLOGIN 1
#define PSTORE 2
#define PDELETE 3

/* clipping.c */
Boolean popup(UInt8 *, UInt8 *, UInt8 *, MemHandle *, MemHandle *, MemHandle *);
void clipping(DmOpenRef, UInt8 *, Boolean);

/* db.c */
DmOpenRef opendatabase(void);
void closedatabase(DmOpenRef);
void qpack(realmpacked *, UInt8 *, UInt8 *, UInt8 *);
void qunpack(realmrecord *, UInt8 *);
void storerecord(DmOpenRef, realmpacked *, UInt16);
UInt16 findrecord(DmOpenRef, MemHandle);
MemHandle clonerecord(DmOpenRef, UInt16);
MemHandle fakerecord(UInt8 *);

/* editform.c */
void preparetoedit(UInt16, UInt16);
void formeditinit(FormPtr);
#define EEDIT 0
#define ENEW 1

/* mainform.c */
void formmaininit(FormPtr);

/* protocol.c */
Boolean getprotorealm(arguments *, UInt8 *);
MemHandle protologin(DmOpenRef, arguments *);
void protostore(DmOpenRef, arguments *);
void protodelete(DmOpenRef, arguments *);

/* url.c */
UInt16 urldecode(UInt8 *);
UInt16 urlencodehandle(MemHandle *);

/* utils.c */
void *getformobject(FormPtr, UInt16);
Boolean refocus(FormPtr, refocustab *);
Boolean isfieldblank(UInt8 *);
MemHandle clonestring(UInt8 *);
void setfieldtext(FormPtr, UInt16, UInt8 *);
void setfieldhandle(FormPtr, UInt16, UInt8 *);
Boolean isxdigit(UInt8);
Boolean isalnum(UInt8);
UInt16 htoi(UInt8 *);

/* viewform.c */
void preparetoview(UInt16);
void formviewinit(FormPtr);

#if DEBUG==1
#define DEBUGCOMMAND(x) x
void dalert(UInt8 *, ...);
void preload(DmOpenRef);
#else
#define DEBUGCOMMAND(x) ;
#endif

#ifdef MAIN
	DmOpenRef qdb;
#else
	extern DmOpenRef qdb;
#endif

#endif
