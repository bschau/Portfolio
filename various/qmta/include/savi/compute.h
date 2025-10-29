/*
 * compute.h (26-NOV-1999)
 *
 * This file is a part of the Sophos Anti-Virus Interface (SAVI)(tm).
 *
 * Copyright (C) 1997,2000 Sophos Plc, Oxford, England.
 * All rights reserved.
 *
 * This source code is only intended as a supplement to the
 * SAVI(tm) Reference and related documentation for the library.
 *
 * Determines the platform and compiler in use.
 */

#ifndef __COMPUTE_DOT_H__
#define __COMPUTE_DOT_H__

/*
 *  Compiler:
 *   __SOPHOS_WC__             Watcom
 *   __SOPHOS_MS__             Microsoft
 *   __SOPHOS_BC__             Borland
 *   __SOPHOS_GC__             GNU
 *   __SOPHOS_DECC__           Digital C/C++
 *   __SOPHOS_MWERKS__         Metrowerks Codewarrior C/C++ compiler
 *   __SOPHOS_IBMC__           IBM
 *
 * Hardware platform:
 *   __SOPHOS_I86__            Intel 8086+
 *   __SOPHOS_I386__           Intel 80386+
 *   __SOPHOS_ALPHA__          Alpha AXP
 *   __SOPHOS_SPARC__          Sun SPARC
 *   __SOPHOS_POWERPC__        IBM PowerPC/RS6000/PowerMac
 *   __SOPHOS_68000__          Macintosh 68000 (PowerMac can emulate this)
 *   __SOPHOS_HP_PA__          HP Precision Architecture
 *
 * Operating system:
 *   __SOPHOS_DOS__            DOS
 *   __SOPHOS_DOS32__          Extended DOS
 *   __SOPHOS_DOS4GW__         Extended DOS using 4GW Extender
 *   __SOPHOS_WIN16__          16-bit Windows
 *   __SOPHOS_WIN32__          32-bit Windows
 *   __SOPHOS_OS2__            OS/2
 *   __SOPHOS_NW__             NetWare
 *   __SOPHOS_SOL2__           Solaris 2
 *   __SOPHOS_LINUX__          Linux
 *   __SOPHOS_DEC_UNIX__       Digital Unix
 *   __SOPHOS_SCO_UNIXWARE__   SCO UnixWare
 *   __SOPHOS_SCO_OPENSERVER__ SCO OpenServer
 *   __SOPHOS_HP_UX__          HP-UX
 *   __SOPHOS_FREEBSD__        FreeBSD
 *   __SOPHOS_AIX__            IBM AIX
 *   __SOPHOS_BANYAN_VINES__   Banyan VINES Server
 *   __SOPHOS_MACOS__          Macintosh OS 7.5+
 *   __SOPHOS_VMS__            VMS
 *
 * Special features:
 *   __SOPHOS_ALIGNED_ACCESS__ Requires data accesses to be aligned in some way.
 *
 * Operating system features:
 *   __SOPHOS_YIELD__          Requires periodic task switching for smooth performance
 *
 * Declaration specifications:
 *   SOPHOS_EXPORT             Export declaration
 *                            (controlled by __SOPHOS_EXPORT__, or
 *                             automatically when compiling as DLL)
 *
 *   SOPHOS_EXPORTC            C function export declaration
 *                            (controlled by __SOPHOS_EXPORT__, or
 *                             automatically when compiling as DLL)
 *
 *   SOPHOS_PUBLIC             Similar to SOPHOS_EXPORT but
 *                             different (static functions, but
 *                             available for calling from outside
 *                             a DLL via function pointers).
 *
 *   SOPHOS_PUBLIC_PTR         Is a pointer to a SOPHOS_PUBLIC.
 *
 *   SOPHOS_FAR                Far declaration (used for Win16 and DOS)
 *                            (automatically controlled by target platform)
 *
 *   SOPHOS_INLINEC            Inline C function declaration
 *                            (controlled by __SOPHOS_INLINE__)
 *
 */

/* ----- */

/*
 * Clear everything to start with, apart from
 * those things that the build environment must
 * define by hand.  These are:
 *   __SOPHOS_BANYAN_VINES__
 *   __SOPHOS_WINNT__
 *   __SOPHOS_WIN95__
 */
#undef SOPHOS_EXPORT
#undef SOPHOS_EXPORTC
#undef SOPHOS_FAR
#undef SOPHOS_INLINEC
#undef __SOPHOS_I86__
#undef __SOPHOS_I386__
#undef __SOPHOS_ALPHA__
#undef __SOPHOS_SPARC__
#undef __SOPHOS_POWERPC__
#undef __SOPHOS_68000__
#undef __SOPHOS_HP_PA__
#undef __SOPHOS_DOS__
#undef __SOPHOS_DOS32__
#undef __SOPHOS_DOS4GW__
#undef __SOPHOS_WIN16__
#undef __SOPHOS_WIN32__
#undef __SOPHOS_OS2__
#undef __SOPHOS_NW__
#undef __SOPHOS_SOL2__
#undef __SOPHOS_LINUX__
#undef __SOPHOS_DEC_UNIX__
#undef __SOPHOS_SCO_UNIXWARE__
#undef __SOPHOS_SCO_OPENSERVER__
#undef __SOPHOS_HP_UX__
#undef __SOPHOS_FREEBSD__
#undef __SOPHOS_MACOS__
#undef __SOPHOS_VMS__
#undef __SOPHOS_WC__
#undef __SOPHOS_MS__
#undef __SOPHOS_BC__
#undef __SOPHOS_GC__
#undef __SOPHOS_DECC__
#undef __SOPHOS_MWERKS__
#undef __SOPHOS_IBMC__
#undef __SOPHOS_YIELD__
#undef __SOPHOS_BIG_ENDIAN__
#undef __SOPHOS_LITTLE_ENDIAN__
#undef __SOPHOS_ALIGNED_ACCESS__

/* ----- */

/*
 * Watcom C/C++.
 */

#if defined(__WATCOMC__)
# define __SOPHOS_WC__

/*
 * Export keywords.
 */

# if defined(__SW_BD) || defined(__SOPHOS_EXPORT__)
#  if defined(__WINDOWS__)
#   define SOPHOS_EXPORT        __export __cdecl
#   define SOPHOS_EXPORTC       __export __far __pascal
#   define SOPHOS_FAR           __far
#   error Partially supported platform, please #define SOPHOS_PUBLIC/_PTR for Watcom Windows!
#  elif defined(__NT__)
#   define SOPHOS_EXPORT        __export
#   define SOPHOS_EXPORTC       __export __stdcall
#   define SOPHOS_FAR
#   error Partially supported platform, please #define SOPHOS_PUBLIC/_PTR for Watcom NT!
#  elif defined(__OS2__)
#   if defined(__386__)
#    define SOPHOS_EXPORT      __export
#    define SOPHOS_EXPORTC     __export __syscall
#    define SOPHOS_PUBLIC      __syscall
#    define SOPHOS_PUBLIC_PTR  __syscall * 
#    define SOPHOS_FAR
#   else  /* defined(__386__) */
#    define SOPHOS_EXPORT      __export
#    define SOPHOS_EXPORTC     __export __pascal
#    define SOPHOS_PUBLIC      __far __pascal
#    define SOPHOS_PUBLIC_PTR  __far __pascal * 
#    define SOPHOS_FAR         __far
#   endif /* defined(__386__) */
#  else
#   define SOPHOS_EXPORT        __export
#   define SOPHOS_EXPORTC       __export
#   define SOPHOS_FAR
#  endif
# else  /* defined(__SW_BD) || defined(__SOPHOS_EXPORT__) */
#  define SOPHOS_EXPORT
#  define SOPHOS_FAR
#  if defined(__OS2__)
#   if defined(__386__)
#    define SOPHOS_EXPORTC      __syscall
#    define SOPHOS_PUBLIC       __syscall
#    define SOPHOS_PUBLIC_PTR   __syscall * 
#   else /* defined(__386__) */
#    define SOPHOS_EXPORTC      __far __pascal
#    define SOPHOS_PUBLIC       __far __pascal
#    define SOPHOS_PUBLIC_PTR   __far __pascal * 
#   endif /* defined(__386__) */
#  elif defined(__DOS4GW__)
#   define SOPHOS_EXPORTC
#   define SOPHOS_PUBLIC
#   define SOPHOS_PUBLIC_PTR   *
#  elif defined(__NT__)  
#   define SOPHOS_EXPORTC      _stdcall 
#   error Partially supported platform, please #define SOPHOS_PUBLIC/_PTR for Watcom NT!
#  elif defined(__NETWARE_386__)  
#   define SOPHOS_EXPORTC
#   define SOPHOS_PUBLIC
#   define SOPHOS_PUBLIC_PTR   *
#  else
#   define SOPHOS_EXPORTC       __far __pascal
#   error Partially supported platform, please #define SOPHOS_PUBLIC/_PTR for Watcom on your OS!
#  endif
# endif /* defined(__SW_BD) || defined(__SOPHOS_EXPORT__) */

/*
 * Inline keyword.
 */
# if defined(__SOPHOS_INLINE__)
#  define SOPHOS_INLINEC  __inline
# else  /* defined(__SOPHOS_INLINE__) */
#  define SOPHOS_INLINEC
# endif /* defined(__SOPHOS_INLINE__) */

/*
 * Operating system.
 */
# if defined(__DOS__) || defined(__DOS4GW__) /* DOS */
#  if defined(__FLAT__)
#   if defined(__DOS4GW__)
#    define __SOPHOS_DOS4GW__
#   else
#    define __SOPHOS_DOS32__
#   endif
#  else
#   define __SOPHOS_DOS__
#  endif
# elif defined(__WINDOWS__)     /* Windows 3.x */
#  define __SOPHOS_WIN16__
#  define __SOPHOS_YIELD__
# elif defined(__NT__)          /* Win32 (95/98/NT) */
#  define __SOPHOS_WIN32__
# elif defined(__OS2__)         /* OS/2 2.x and 3.x */
#  define __SOPHOS_OS2__
# elif defined(__NETWARE_386__) /* Netware 386 */
#  define __SOPHOS_NW__
#  define __SOPHOS_YIELD__
# else                          /* Unsupported operating system */
#  error Unsupported Watcom C/C++ target operating system
# endif

/*
 * Hardware platform
 */
# if defined(M_I86) && ( defined(__DOS__) || defined(__DOS4GW__) || defined(__WINDOWS__) || defined(__OS2__)) /* Intel x86 16-bit */
#  define __SOPHOS_I86__
# elif defined(M_I386)                                              /* Intel x86 32-bit */
#  define __SOPHOS_I386__
# else                                                              /* Unsupported hardware platform */
#  error Unsupported Watcom C/C++ target hardware platform
# endif

/* ----- */

/*
 * Microsoft C/C++.
 */

#elif defined(_MSC_VER)
# define __SOPHOS_MS__

/*
 * Export keyword.
 */

# if defined(_WINDLL) || defined(__SOPHOS_EXPORT__)
#  if defined(_M_IX86)
#   define SOPHOS_EXPORT  __declspec( dllexport )           /* The Visual C++ 2.0 Compiler obsoletes the __export */
#   define SOPHOS_EXPORTC __declspec( dllexport ) __stdcall /* keyword for the dllexport storage class modifier   */
#   define SOPHOS_FAR
#  else
#   define SOPHOS_EXPORT  __export
#   define SOPHOS_EXPORTC __export __far __pascal
#   define SOPHOS_FAR     __far
#  endif
# else
#  define SOPHOS_EXPORT
#  define SOPHOS_EXPORTC
#  define SOPHOS_FAR
# endif

#define SOPHOS_PUBLIC
#define SOPHOS_PUBLIC_PTR *

/*
 * Inline keyword.
 */
# if defined(__SOPHOS_INLINE__)
#  define SOPHOS_INLINEC  __inline
# else  /* defined(__SOPHOS_INLINE__) */
#  define SOPHOS_INLINEC
# endif /* defined(__SOPHOS_INLINE__) */

/*
 * Operating system
 */
# if defined(_WIN32)              /* Win32 (95/98/NT) */
#  define __SOPHOS_WIN32__
# elif defined(_WINDOWS)          /* Windows 3.x */
#  define __SOPHOS_WIN16__
#  define __SOPHOS_YIELD__
# elif( _MSC_VER == 700 )         /* DOS */
#  define __SOPHOS_DOS__
# elif( _MSC_VER == 800 )         /* DOS */
#  define __SOPHOS_DOS__
# else                            /* Unsupported operating system */
#  error Unsupported Microsoft C/C++ target operating system
# endif

/*
 * Hardware platform
 */
# if defined(_M_I86)              /* Intel x86 16-bit */
#  define __SOPHOS_I86__
# elif defined(_M_IX86)           /* Intel x386 32-bit */
#  define __SOPHOS_I386__
# elif defined(_M_ALPHA)          /* DEC Alpha */
#  define __SOPHOS_ALPHA__
# else                            /* Unsupported hardware platform */
#  error Unsupported Microsoft C/C++ target hardware platform
# endif

/* ----- */

/*
 * Borland C/C++.
 */

#elif defined(__BORLANDC__)
# define __SOPHOS_BC__

/*
 * Export keyword.
 */

# if defined(__DLL__) || defined(__SOPHOS_EXPORT__)
#  if defined(__OS2__)
#   define SOPHOS_EXPORT      __export
#   define SOPHOS_EXPORTC     __export __far16 __pascal
#   define SOPHOS_PUBLIC      __far16 __pascal
#   define SOPHOS_PUBLIC_PTR  __far16 __pascal *
#   define SOPHOS_FAR         __far16
#  elif defined(__WIN32__)
#   define SOPHOS_EXPORT      __export
#   define SOPHOS_EXPORTC     __export __stdcall
#   define SOPHOS_FAR
#   define SOPHOS_PUBLIC
#   define SOPHOS_PUBLIC_PTR  *
#  elif defined(_Windows)
#   define SOPHOS_EXPORT      __export
#   define SOPHOS_EXPORTC     __export __far __pascal
#   define SOPHOS_FAR         __far
#   define SOPHOS_PUBLIC
#   define SOPHOS_PUBLIC_PTR  *
#  else
#   define SOPHOS_EXPORT      __export
#   define SOPHOS_EXPORTC     __export
#   define SOPHOS_FAR
#   define SOPHOS_PUBLIC
#   define SOPHOS_PUBLIC_PTR  *
#  endif
# else
#  define SOPHOS_EXPORT
#  define SOPHOS_EXPORTC
#  define SOPHOS_FAR
#  define SOPHOS_PUBLIC
#  define SOPHOS_PUBLIC_PTR  *
# endif

/*
 * Inline keyword.
 */

# if defined(__SOPHOS_INLINE__)
#  define SOPHOS_INLINEC inline
# else
#  define SOPHOS_INLINEC
# endif

/*
 * Operating system.
 */

# if defined(__OS2__)                          /* OS/2 */
#  define __SOPHOS_OS2__
# elif defined(__WIN32__)                      /* Win32 (Windows 95 and Windows NT) */
#  define __SOPHOS_WIN32__
# elif defined(_Windows)                       /* Windows 3.x */
#  define __SOPHOS_WIN16__
#  define __SOPHOS_YIELD__
# elif defined(__MSDOS__)                      /* DOS */
#  define __SOPHOS_DOS__
# else                                         /* Unsupported operating system */
#  error Unsupported Borland C/C++ target operating system
# endif

/*
 * Hardware platform.
 */

# if defined(__OS2__)                          /* Intel x86 32-bit */
#  define __SOPHOS_I386__
# elif defined(__WIN32__)                      /* Intel x86 32-bit */
#  define __SOPHOS_I386__
# elif defined(_Windows) || defined(__MSDOS__) /* Intel x86 16-bit */
#  define __SOPHOS_I86__
# else                                         /* Unsupported hardware platform */
#  error Unsupported Borland C/C++ target hardware platform
# endif

/* ----- */

/*
 * Digital C/C++.
 */

#elif defined(__DECC)
# define __SOPHOS_DECC__

/*
 * Export keyword.
 */
#define SOPHOS_PUBLIC
#define SOPHOS_PUBLIC_PTR *
#define SOPHOS_EXPORT
#define SOPHOS_EXPORTC

/*
 * Far keyword.
 */
#define SOPHOS_FAR

/*
 * Inline keyword.
 */
# if defined(__SOPHOS_INLINE__)
#  define SOPHOS_INLINEC
# else
#  define SOPHOS_INLINEC
# endif

/*
 * Operating system
 */
# if defined(__unix__)
#  define __SOPHOS_DEC_UNIX__
# elif defined(__VMS__)
#  define __SOPHOS_VMS__
# else
#  error Unsupported Digital C/C++ target operating system
# endif

/*
 * Hardware platform.
 */
# if defined(__alpha)
#  define __SOPHOS_ALPHA__
# elif defined(__VAX)
#  define __SOPHOS_VAX__
# else
#  error Unsupported Digital C/C++ target hardware platform
# endif


/* ----- */

/*
 * GNU C/C++.
 */

#elif defined(__GNUC__)
# define __SOPHOS_GC__

/*
 * Export keyword.
 */
#define SOPHOS_PUBLIC
#define SOPHOS_PUBLIC_PTR *
#define SOPHOS_EXPORT
#define SOPHOS_EXPORTC

/*
 * Far keyword.
 */

#define SOPHOS_FAR

/*
 * Inline keyword.
 */

# if defined(__SOPHOS_INLINE__)
#  define SOPHOS_INLINEC  __inline__
# else
#  define SOPHOS_INLINEC
# endif


/*
 * Operating system
 */

# if defined(__linux__)
#  define __SOPHOS_LINUX__
# elif defined(__hpux__)
#  define __SOPHOS_HP_UX__
# elif defined(__sun__)
#  define __SOPHOS_SOL2__
# elif defined(__svr4__)
#  define __SOPHOS_SCO_UNIXWARE__
# elif defined(_AIX)
#  define __SOPHOS_AIX__
# elif defined(__FreeBSD__)
#  define __SOPHOS_FREEBSD__
# elif (defined(_SCO_DS) && defined(_SCO_ELF) && defined(_SCO_XPG_VERS) && defined(_SCO_C_DIALECT))
#  define __SOPHOS_SCO_OPENSERVER__
# elif defined(__SOPHOS_BANYAN_VINES__)
/* Intentionally empty section! */
# else
#  error Unsupported GNU C target operating system
# endif


/*
 * Hardware platform.
 */

# if defined(__i386__)
#  define __SOPHOS_I386__
# elif (defined(__hppa__) && defined(_PA_RISC1_1))
#  define __SOPHOS_HP_PA__
# elif defined(__sparc__)
#  define __SOPHOS_SPARC__
# elif defined(_POWER)
#  define __SOPHOS_POWERPC__
# elif defined(__alpha__)
#  define __SOPHOS_ALPHA__
# else
#  error Unsupported GNU C/C++ target hardware platform
# endif

/* ----- */

/*
 * Metrowerks Codewarrior C/C++
 */

#elif defined(__MWERKS__)
# define __SOPHOS_MWERKS__

/*
 * Export keyword.
 */
# define SOPHOS_EXPORT
# define SOPHOS_EXPORTC
# define SOPHOS_PUBLIC
# define SOPHOS_PUBLIC_PTR *
# define SOPHOS_FAR

/*
 * Inline keyword.
 */
#if defined(__SOPHOS_INLINE__)
# define SOPHOS_INLINEC  inline
#else  /* defined(__SOPHOS_INLINE__) */
# define SOPHOS_INLINEC
#endif /* defined(__SOPHOS_INLINE__) */

/*
 * Operating system.
 */
# if macintosh
#  define __SOPHOS_MACOS__
# else
#  error Unsupported Metrowerks target OS
# endif

/*
 * Hardware platform.
 */
# if __POWERPC__
#  define __SOPHOS_POWERPC__
# elif __MC68K__
#  define __SOPHOS_68000__
# else
#  error Unsupported Metrowerks target hardware platform.
# endif

/* ----- */

#elif defined(__IBMC__)
# define __SOPHOS_IBMC__

/*
 * Export keywords - IBMC
 */

# if defined(__SW_BD) || defined(__SOPHOS_EXPORT__)
#  if defined(__OS2__)
#   if defined(_M_I386)
#     define SOPHOS_EXPORT  
#     define SOPHOS_EXPORTC      _System
#     define SOPHOS_PUBLIC       _System
#     define SOPHOS_PUBLIC_PTR * _System 
#     define SOPHOS_FAR
#   else
#     define SOPHOS_EXPORT     __export
#     define SOPHOS_EXPORTC    __export __pascal
#     define SOPHOS_PUBLIC     __far __pascal
#     define SOPHOS_PUBLIC_PTR __far __pascal * 
#     define SOPHOS_FAR        __far
#   endif
#  else
#   define SOPHOS_EXPORT       __export
#   define SOPHOS_EXPORTC      __export
#   define SOPHOS_FAR
#  endif
# else
#  define SOPHOS_EXPORT
#  define SOPHOS_EXPORTC
#  define SOPHOS_FAR
#  define SOPHOS_PUBLIC        _System
#  define SOPHOS_PUBLIC_PTR  * _System 
# endif

/*
 * Inline keyword.
 */

#if defined(__SOPHOS_INLINE__)
# define SOPHOS_INLINEC
#else  /* defined(__SOPHOS_INLINE__) */
# define SOPHOS_INLINEC
#endif /* defined(__SOPHOS_INLINE__) */

/*
 * Operating system.
 */

# if defined(__DOS__)           /* DOS */
#  if defined(__FLAT__)
#   define __SOPHOS_DOS32__
#  else
#   define __SOPHOS_DOS__
#  endif
# elif defined(__WINDOWS__)     /* Windows 3.x */
#  define __SOPHOS_WIN16__
#  define __SOPHOS_YIELD__
# elif defined(__NT__)          /* Win32 (95/98/NT) */
#  define __SOPHOS_WIN32__
# elif defined(__OS2__)         /* OS/2 2.x and 3.x */
#  define __SOPHOS_OS2__
# else                          /* Unsupported operating system */
#  error Unsupported IBM C/C++ target operating system
# endif


/*
 * Hardware platform
 */

# if defined(M_I86) && ( defined(__DOS__) || defined(__WINDOWS__) || defined(__OS2__)) /* Intel x86 16-bit */
#  define __SOPHOS_I86__
# elif defined(_M_I386)                                              /* Intel x86 32-bit */
#  define __SOPHOS_I386__
# else                                                              /* Unsupported hardware platform */
#  error Unsupported IBM C/C++ target hardware platform
# endif


/* ----- */

/*
 * Unknown compiler.
 */

#else
# error Unsupported compiler
#endif

/* ----- */

/*
 * Set macros describing the endian nature of the platform.
 */
#if (defined(__SOPHOS_VMS__))
# undef  __SOPHOS_BIG_ENDIAN__
# define __SOPHOS_LITTLE_ENDIAN__
#else
# if ((defined(__SOPHOS_SPARC__)   && defined(__SOPHOS_SOL2__))  || \
     (defined(__SOPHOS_HP_UX__)   && defined(__SOPHOS_HP_PA__)) || \
     (defined(__SOPHOS_POWERPC__) && defined(__SOPHOS_AIX__))   || \
     (defined(__SOPHOS_POWERPC__) && defined(__SOPHOS_MACOS__)) || \
     (defined(__SOPHOS_68000__)   && defined(__SOPHOS_MACOS__)) )
#  define __SOPHOS_BIG_ENDIAN__
#  undef  __SOPHOS_LITTLE_ENDIAN__
# else
#  undef  __SOPHOS_BIG_ENDIAN__
#  define __SOPHOS_LITTLE_ENDIAN__
# endif
#endif

/*
 * Set another macro concerning aligned data access if necessary.
 */
#if (!defined(__SOPHOS_VMS__))
# if ((defined(__SOPHOS_SPARC__) && defined(__SOPHOS_SOL2__))     || \
     (defined(__SOPHOS_ALPHA__) && defined(__SOPHOS_LINUX__))    || \
     (defined(__SOPHOS_HP_UX__) && defined(__SOPHOS_HP_PA__))    || \
     (defined(__SOPHOS_ALPHA__) && defined(__SOPHOS_DEC_UNIX__)) )
#  define __SOPHOS_ALIGNED_ACCESS__
# endif
#endif

/*
 * Group all the Unix variants together under one define.
 */
#if (!defined(__SOPHOS_VMS__))
# if (defined(__SOPHOS_SOL2__)            || \
     defined(__SOPHOS_LINUX__)           || \
     defined(__SOPHOS_DEC_UNIX__)        || \
     defined(__SOPHOS_AIX__)             || \
     defined(__SOPHOS_FREEBSD__)         || \
     defined(__SOPHOS_SCO_OPENSERVER__)  || \
     defined(__SOPHOS_SCO_UNIXWARE__)    || \
     defined(__SOPHOS_HP_UX__))
#  define __SOPHOS_UNIX__
# endif
#endif

#endif /* __COMPUTE_DOT_H__ */
