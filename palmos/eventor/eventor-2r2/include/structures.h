#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <PalmOS.h>
#include "resources.h"

typedef struct {
	UInt16 id;
	UInt16 scrollBarId;
	TableType *tbl;
	Int16 top;
	Int16 records;
	Int16 rows;
} Table;

typedef struct {
	Int16 column;
	TableItemStyleType style;
} TableStyle;

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

/* pre-2r1 App Info Block */
typedef struct {
	AppInfoType appInfo;
} EventorAppInfo;

typedef struct {
	AppInfoType appInfo;
} AppInfoBlock;

typedef struct {
	UInt16 categoryIdx;
	Int16 alarmHour;
	Int16 alarmMinute;
	struct {
		UInt16 showNotifications:1;
		UInt16 soundAlarm:1;
	} flags;
} Preferences;

typedef struct {
	Char event[EventLength];
	UInt16 date;
	UInt16 flags;
} TEvent;

#endif
