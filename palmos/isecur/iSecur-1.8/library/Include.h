#ifndef INCLUDE_H
#define INCLUDE_H

#include "iSecurLib.h"

/* structs */
typedef struct {
	Int16 refCount;
	MemHandle mhs[10];
} iSecurLibGlobals;

/* iSecurLib.c */
iSecurLibGlobals *LockGlobals(UInt16);

#endif
