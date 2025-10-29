#ifndef BMILOG_H
#define BMILOG_H

#include <PalmOS.h>
#include <libraries/PalmOSGlue/TxtGlue.h>
#include <libraries/PalmOSGlue/BmpGlue.h>
#include "resources.h"
#include "constants.h"
#include "structures.h"
#include "protos.h"

#ifdef MAIN
	Boolean findReturn=false;
	DB dbData;
	FormPtr currentForm;
	UInt16 currentFormID, recordIdx;
	MemHandle resStrSelectDate;
	MemHandle resStrWhatIsTitle, resStrWhatIsText;
	MemHandle resStrBMITableTitle;
	MemHandle resStrCm, resStrFeet, resStrInch;
	MemHandle resStrKg, resStrLbs, resStrAbout;
	MemHandle resStrBMITableTextAmerican, resStrBMITableTextEuropean;
	MemHandle resStrName, resStrDate, resStrBMI, resStrFeetInch;
	MemHandle bmpNegativeH, bmpNullH, bmpPositiveH;
	UInt32 personId;
	Preferences prefs;
#else
	extern Boolean findReturn;
	extern DB dbData;
	extern FormPtr currentForm;
	extern UInt16 currentFormID, recordIdx;
	extern MemHandle resStrSelectDate;
	extern MemHandle resStrWhatIsTitle, resStrWhatIsText;
	extern MemHandle resStrBMITableTitle;
	extern MemHandle resStrCm, resStrFeet, resStrInch;
	extern MemHandle resStrKg, resStrLbs, resStrAbout;
	extern MemHandle resStrBMITableTextAmerican, resStrBMITableTextEuropean;
	extern MemHandle resStrName, resStrDate, resStrBMI, resStrFeetInch;
	extern MemHandle bmpNegativeH, bmpNullH, bmpPositiveH;
	extern UInt32 personId;
	extern Preferences prefs;
#endif

#endif
