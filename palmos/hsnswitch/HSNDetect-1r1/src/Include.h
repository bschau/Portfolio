#ifndef INCLUDE_H
#define INCLUDE_H

#include <PalmOS.h>
#include <DLServer.h>
#include "Resources.h"

/* macros */
#define EVER ;;
#define CRID 'HSND'
#define DBNameDetect "HSND_HSNDetectData"

/* fMain.c */
Boolean fMainEH(EventType *);

#ifdef MAIN
	FormPtr currentForm;
#else
	extern FormPtr currentForm;
#endif

#endif
