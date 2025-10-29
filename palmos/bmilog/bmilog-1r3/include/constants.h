#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "resources.h"

/* macros */
#define EVER ;;
#define CRID 'BgSc'
#define DBNAME "BgSc_BMILogData"
#define DBVERSION 1
#define APPVER 1
#define MAGICYEAR 1904

enum { FormatKgCm=0, FormatLbsInch };
enum { BmiTableEuropean=0, BmiTableAmerican };
enum { RPerson='1', RBmi='2' };

#endif
