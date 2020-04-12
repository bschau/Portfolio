/*
 * savichar.h (26-NOV-1999)
 *
 * This file is a part of the Sophos Anti-Virus Interface (SAVI)(tm).
 *
 * Copyright (C) 1997,2000 Sophos Plc, Oxford, England.
 * All rights reserved.
 *
 * This source code is only intended as a supplement to the
 * SAVI(tm) Reference and related documentation for the library.
 *
 * Cross platform type definitions
 */

#ifndef __SAVICHAR_DOT_H__
#define __SAVICHAR_DOT_H__

#include "compute.h"

#if defined(__SOPHOS_WIN32__)
#  include <tchar.h>
#  ifndef __SOPHOS_TCHAR_DEFINED__
#     define __SOPHOS_TCHAR_DEFINED__
#  endif /* __SOPHOS_TCHAR_DEFINED__ */
#else  /* __SOPHOS_WIN32__ */

#  ifndef __SOPHOS_TCHAR_DEFINED__
#    define __SOPHOS_TCHAR_DEFINED__

#    if  defined(__SOPHOS_OS2__)
#      include <tchar.h>
#    else
        typedef char TCHAR;
#       if defined(_T)
#          undef _T
#       endif /* defined(_T) */
#       define _T(x) x
#    endif /* __SOPHOS_OS2__ */

#  endif /* __SOPHOS_TCHAR_DEFINED__ */

   typedef char OLECHAR;
   typedef char * LPOLESTR;
   typedef const char * LPCOLESTR;
#  define OLESTR(str) str

#endif /* __SOPHOS_WIN32__ */

#endif /* __SAVICHAR_DOT_H__ */
