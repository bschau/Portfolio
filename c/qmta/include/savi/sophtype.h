/*
 * sophtype.h (26-NOV-1999)
 *
 * This file is a part of the Sophos Anti-Virus Interface (SAVI)(tm).
 *
 * Copyright (C) 1997,2000 Sophos Plc, Oxford, England.
 * All rights reserved.
 *
 * This source code is only intended as a supplement to the
 * SAVI(tm) Reference and related documentation for the library.
 *
 * These are the sophos type (re)defintions for use with basic types
 */

#ifndef __SOPHTYPE_DOT_H__
#define __SOPHTYPE_DOT_H__

#include "compute.h"

/*
 * Standard sizes.
 */
#ifndef __SOPHOS_TYPES_DEFINED__
#define __SOPHOS_TYPES_DEFINED__
#  if (((defined(__SOPHOS_DEC_UNIX__)) && (defined(__SOPHOS_ALPHA__))) || \
        ((defined(__SOPHOS_LINUX__))    && (defined(__SOPHOS_ALPHA__))))
   typedef unsigned char      U08;
   typedef unsigned short int U16;
   typedef unsigned       int U32;
   typedef signed   char      S08;
   typedef signed   short int S16;
   typedef signed         int S32;
#  else
   typedef unsigned char      U08;
   typedef unsigned short int U16;
   typedef unsigned long  int U32;
   typedef signed   char      S08;
   typedef signed   short int S16;
   typedef signed   long  int S32;
#  endif
#endif /* __SOPHOS_TYPES_DEFINED__ */

#endif /* __SOPHTYPE_DOT_H__ */
