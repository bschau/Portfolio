#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "resources.h"

/* macros */
#define EVER ;;
#define CRID 'PERV'
#define DBNAME "PERV_EventorData"
#define OLDDBNAME "Eventor Data"
#define DBTYPE 'DATA'
#define DBVERSION 2
#define APPVER 2
#define CATNONEDITABLES 1
#define MAGICYEAR 1904
#define FlgRemindMe 0x8000
#define FlgRepeat 0x4000
#define MaskOnlyDate 0x1ff	/* lower 9 bits */
#define MaskFLGDays 0x3ff	/* lower 10 bits */
#define eventorLateWakeup 'ELWU'

enum { DateFormatDM, DateFormatMD };
enum { FtrRecords=0 };

#endif
