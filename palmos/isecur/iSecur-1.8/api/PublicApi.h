#ifndef PUBLICAPI_H
#define PUBLICAPI_H

#include <PalmTypes.h>
#include <LibTraps.h>
#include "PublicResources.h"

/* globals */
#define CRID 'iSec'

/* Keyboard */
#define KeyboardLibName "iSecur Keyboard"
#define KeyboardAPIVersion 1
#define KeyboardLibCRID 'iSec'
#define KeyboardLibType 'kbde'
#define KeyboardPrfStart 1000
enum { KeyboardFtrStart=1000, KeyboardFtrClose };
enum { erriSecurKeyboardNone=0, erriSecurKeyboardAbort, erriSecurKeyboardOther };
enum { kbfCloseOK=0, kbfCloseCancel };

/* KeyboardLib.c */
Err KeyboardLibOpen(UInt16, UInt16) SYS_TRAP(sysLibTrapOpen);
Err KeyboardLibClose(UInt16, UInt16 *) SYS_TRAP(sysLibTrapClose);
Err KeyboardLibSleep(UInt16) SYS_TRAP(sysLibTrapSleep);
Err KeyboardLibWake(UInt16) SYS_TRAP(sysLibTrapWake);
void KeyboardLibVersion(UInt16, UInt32 *) SYS_TRAP(sysLibTrapCustom);
void KeyboardLibAPIVersion(UInt16, UInt32 *) SYS_TRAP(sysLibTrapCustom+1);
Err KeyboardLibRun(UInt16, MemHandle *, UInt16 *) SYS_TRAP(sysLibTrapCustom+2);

/* Debug */
#ifdef DEBUG
#define D(x)		x
#include <HostControl.h>
void DAlert(UInt8 *, ...);
void DHexDump(UInt8 *, UInt16);
#ifdef MAIN
	HostFILE *gHostFile;
#else
	extern HostFILE *gHostFile;
#endif
#define DPrint(x...)	HostFPrintF(gHostFile, x); \
			HostFFlush(gHostFile);
#else
#define DAlert(x, ...)	;
#define D(x)		;
#define DPrint(x...)	;
#define DHexDump(x, y)	;
#endif

#endif
