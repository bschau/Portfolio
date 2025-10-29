#ifndef EVENTOR_H
#define EVENTOR_H

#include <PalmOS.h>
#include <Libraries/PalmOSGlue/TxtGlue.h>
#include "constants.h"
#include "resources.h"
#include "structures.h"
#include "protos.h"

#ifdef MAIN
	UInt16 categoryIdx=dmUnfiledCategory;
	UInt16 recordIdx, dateFormat, viewIdx, currentFormID;
	Char dateSeparator;
	DB dbData;
	FormPtr currentForm;
	Preferences prefs;
	UInt8 categoryName[dmCategoryLength+2];
	Boolean viewReturn;
#else
	extern UInt16 categoryIdx, recordIdx, dateFormat, viewIdx;
	extern UInt16 currentFormID;
	extern Char dateSeparator;
	extern DB dbData;
	extern FormPtr currentForm;
	extern Preferences prefs;
	extern UInt8 categoryName[dmCategoryLength+2];
	extern Boolean viewReturn;
#endif

#endif
