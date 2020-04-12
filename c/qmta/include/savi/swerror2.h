/*
 * swerror2.h (26-NOV-1999)
 *
 * This file is a part of the Sophos Anti-Virus Interface (SAVI)(tm).
 *
 * Copyright (C) 1997,2000 Sophos Plc, Oxford, England.
 * All rights reserved.
 *
 * This source code is only intended as a supplement to the
 * SAVI(tm) Reference and related documentation for the library.
 *
 * NB Only use these error codes with SAVI2.
 *
 */

#ifndef __SWERROR2_DOT_H__
#define __SWERROR2_DOT_H__

/* Check that we aren't trying to mix SAVI1 and SAVI2 interfaces: */
#ifdef _SOPHOS_SAVI1
#  error Attempting to mix SAVI1 and SAVI2 include files. Include only isavi2.h for SAVI2.
#endif
#define _SOPHOS_SAVI2

/*
 * SAVI return codes.
 */

#include "savitype.h"   

#ifdef __SOPHOS_WIN32__

/* Windows standard error codes: */
#include <winerror.h>

/* On win32 we have aliases for the Windows definitions, to remain COM-compliant: */
#define SOPHOS_S_OK                    S_OK
#define SOPHOS_S_FALSE                 S_FALSE
#define SOPHOS_E_NOINTERFACE           E_NOINTERFACE
#define SOPHOS_CLASS_E_NOAGGREGATION   CLASS_E_NOAGGREGATION
#define SOPHOS_E_UNEXPECTED            E_UNEXPECTED
#define SOPHOS_E_OUTOFMEMORY           E_OUTOFMEMORY
#define SOPHOS_E_INVALIDARG            E_INVALIDARG
#define SOPHOS_E_NOTIMPL               E_NOTIMPL
#define SOPHOS_E_OUT_OF_DISK           STG_E_MEDIUMFULL
#define SOPHOS_MAKE_HRESULT(sev,fac,code) (MAKE_HRESULT(sev,fac,code) | (1<<29))
#define SOPHOS_SEVERITY_SUCCESS        SEVERITY_SUCCESS
#define SOPHOS_SEVERITY_ERROR          SEVERITY_ERROR
#define SOPHOS_FACILITY_ITF            FACILITY_ITF
#define SOPHOS_SUCCEEDED(Status)       SUCCEEDED(Status)
#define SOPHOS_FAILED(Status)          FAILED(Status)
#define SOPHOS_CODE(Status)            HRESULT_CODE(Status)
#define SOPHOS_FACILITY(Status)        HRESULT_FACILITY(Status)
#define SOPHOS_SEVERITY(Status)        HRESULT_SEVERITY(Status)

#else    /* __SOPHOS_WIN32__ */

/* Supply some error codes analogous to Windows error codes: */
/* For IUnknown: */
#define SOPHOS_S_OK                    ((HRESULT)0x00000000L)
#define SOPHOS_S_FALSE                 ((HRESULT)0x00000001L)
#define SOPHOS_E_NOINTERFACE           ((HRESULT)0x80004002L)
/* For IClassFactory: */
#define SOPHOS_CLASS_E_NOAGGREGATION   ((HRESULT)0x80040110L)
#define SOPHOS_E_UNEXPECTED            ((HRESULT)0x8000FFFFL)
#define SOPHOS_E_OUTOFMEMORY           ((HRESULT)0x8007000EL)
#define SOPHOS_E_INVALIDARG            ((HRESULT)0x80070057L)
#define SOPHOS_E_NOTIMPL               ((HRESULT)0x80004001L)
#define SOPHOS_E_OUT_OF_DISK           ((HRESULT)0x80030070L)

/* Macros for defining error codes: */
#define SOPHOS_MAKE_HRESULT(sev,fac,code) \
    ((HRESULT) (((U32)(sev)<<31) | ((U32)(fac)<<16) | ((U32)(code))) )
#define SOPHOS_SEVERITY_SUCCESS    0
#define SOPHOS_SEVERITY_ERROR      1

#define SOPHOS_FACILITY_ITF        4

/* Macros for testing error results: */
#define SOPHOS_SUCCEEDED(Status)  ((HRESULT)(Status) >= 0)
#define SOPHOS_FAILED(Status)     ((HRESULT)(Status) < 0)
#define SOPHOS_CODE(Status)       ((HRESULT)(Status) & 0xFFFF)
#define SOPHOS_FACILITY(Status)   ((((HRESULT)(Status)) >> 16) & 0x1FFF)
#define SOPHOS_SEVERITY(Status)   ((((HRESULT)(Status)) >> 31) & 0x1)

#endif   /* __SOPHOS_WIN32__ */

/* Sophos-specific error messages: */

#define SOPHOS_SAVI2_ERROR_INITIALISING SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x200)
                        /* DLL failed to intialise */

#define SOPHOS_SAVI2_ERROR_TERMINATING SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x201)
                        /* Error while unloading */

#define SOPHOS_SAVI2_ERROR_SWEEPFAILURE SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x202)
                        /* The virus scan failed */

#define SOPHOS_SAVI2_ERROR_VIRUSPRESENT SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_SUCCESS, SOPHOS_FACILITY_ITF, 0x203)
                        /* A virus was detected */

#define SOPHOS_SAVI2_ERROR_NOT_INITIALISED SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x204)
                        /* Attempt to use virus engine without initialising it */

#define SOPHOS_SAVI2_ERROR_IC_INCOMPATIBLE_VERSION SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x205)
                        /* The installed version of SAV is running an */
                        /* incompatible version of the InterCheck client */

#define SOPHOS_SAVI2_ERROR_IC_ACCESS_DENIED SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x206)
                        /* The process does not have sufficient rights */
                        /* to disable the InterCheck client */

#define SOPHOS_SAVI2_ERROR_IC_SCAN_PREVENTED SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x207)
                        /* The InterCheck client could not be disabled.  */
                        /* The request to scan the file has been denied. */

#define SOPHOS_SAVI2_ERROR_DISINFECTION_FAILED SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x208)
                        /* The disinfection failed */

#define SOPHOS_SAVI2_ERROR_DISINFECTION_UNAVAILABLE SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x209)
                        /* Disinfection was attempted on an uninfected file. */
                        /* library data */

#define SOPHOS_SAVI2_ERROR_UPGRADE_FAILED SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x20A)
                        /* An attempted upgrade to the virus engine failed */

#define SOPHOS_SAVI2_ERROR_SAV_NOT_INSTALLED SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x20B)
                        /* Sophos Anti Virus has been removed from this */
                        /* machine */

#define SOPHOS_SAVI2_ERROR_INVALID_CONFIG_NAME SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x20C)
                        /* Attempt to get/set SAVI configuration with incorrect name */

#define SOPHOS_SAVI2_ERROR_INVALID_CONFIG_TYPE SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x20D)
                        /* Attempt to get/set SAVI configuration with incorrect type */

#define SOPHOS_SAVI2_ERROR_INIT_CONFIGURATION SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x20E)
                        /* Could not configure SAVI */

#define SOPHOS_SAVI2_ERROR_NOT_SUPPORTED SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x20F)
                        /* Not supported in this SAVI implementation */

#define SOPHOS_SAVI2_ERROR_COULD_NOT_OPEN SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x210)
                        /* File couldn't be accessed */

#define SOPHOS_SAVI2_ERROR_FILE_COMPRESSED SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x211)
                        /* File was compressed, but no virus was found on the outer level */

#define SOPHOS_SAVI2_ERROR_FILE_ENCRYPTED SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x212)
                        /* File was encrypted */

#define SOPHOS_SAVI2_ERROR_INFORMATION_NOT_AVAILABLE SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x213)
                        /* Additional virus location is unavailable*/

#define SOPHOS_SAVI2_ERROR_ALREADY_INIT SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x214)
                        /* Attempt to initialise when already initialised */

#define SOPHOS_SAVI2_ERROR_STUB SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x215)
                        /* Attempt to use a stub library */

#define SOPHOS_SAVI2_ERROR_BUFFER_TOO_SMALL SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x216)
                        /* Buffer supplied was too small */

#define SOPHOS_SAVI2_CBCK_CONTINUE_THIS SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_SUCCESS, SOPHOS_FACILITY_ITF, 0x217)
                        /* Returned from a callback function to continue with the current file. */

#define SOPHOS_SAVI2_CBCK_CONTINUE_NEXT SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_SUCCESS, SOPHOS_FACILITY_ITF, 0x218)
                        /* Returned from a callback function to skip to the next file. */

#define SOPHOS_SAVI2_CBCK_STOP SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_SUCCESS, SOPHOS_FACILITY_ITF, 0x219)
                        /* Returned from a callback function to stop the current operation. */

#define SOPHOS_SAVI2_ERROR_CORRUPT SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x21A)
                        /* Sweep could not proceed, the file was corrupted. */

#define SOPHOS_SAVI2_ERROR_REENTRANCY SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x21B)
                        /* An attempt to re-enter SAVI from a callback notification was detected. */
 
#define SOPHOS_SAVI2_ERROR_CALLBACK SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x21C)
                        /* An error was encountered in the SAVI client's callback function. */

#define SOPHOS_SAVI2_ERROR_PARTIAL_INFORMATION SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_SUCCESS, SOPHOS_FACILITY_ITF, 0x21D)
                        /* A call requesting several pieces of information did not return them all. */

#define SOPHOS_SAVI2_ERROR_OLD_VIRUS_DATA SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_SUCCESS, SOPHOS_FACILITY_ITF, 0x21E)
                        /* The main body of virus data is out of date. */

#define SOPHOS_SAVI2_ERROR_INVALID_TMP SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_SUCCESS, SOPHOS_FACILITY_ITF, 0x21F)
                        /* No valid temporary directory found. */

#define SOPHOS_SAVI2_ERROR_MISSING_MAIN_VIRUS_DATA SOPHOS_MAKE_HRESULT(SOPHOS_SEVERITY_ERROR, SOPHOS_FACILITY_ITF, 0x220)
                        /* The main body of virus data is missing. */

#endif   /*__SWERROR2_DOT_H__*/
