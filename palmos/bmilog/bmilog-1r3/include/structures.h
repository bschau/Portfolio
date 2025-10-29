#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <PalmOS.h>
#include "resources.h"

typedef struct {
	UInt16 bmiTable;
	UInt16 format;
} Preferences;

typedef struct {
	UInt8 type;
	UInt8 filler;
	UInt32 id;
} Record;

typedef struct {
	Record r;
	UInt16 format;
	Char name[MaxNameLength+2];
} Person;

typedef struct {
	Record r;
	UInt16 date;
	UInt16 height;
	UInt16 weight;
} BMI;

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
	Boolean usable;
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

#endif
