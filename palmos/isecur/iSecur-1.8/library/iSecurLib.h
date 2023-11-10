#ifndef ISECURLIB_H
#define ISECURLIB_H

#include <LibTraps.h>

/* public constants */
#define iSecurLibName "iSecur Library"
#define iSecurLibCRID 'iSec'
#define iSecurLibType 'libr'

/* iSecurLib.c */
Err iSecurLibOpen(UInt16, UInt16) SYS_TRAP(sysLibTrapOpen);
Err iSecurLibClose(UInt16, UInt16 *) SYS_TRAP(sysLibTrapClose);
Err iSecurLibSleep(UInt16) SYS_TRAP(sysLibTrapSleep);
Err iSecurLibWake(UInt16) SYS_TRAP(sysLibTrapWake);
void iSecurLibVersion(UInt16, UInt32 *) SYS_TRAP(sysLibTrapCustom);

/* AES.c */
#define AESKeyLength 32
MemHandle iSecurLibAESOpen(UInt16) SYS_TRAP(sysLibTrapCustom+1);
void iSecurLibAESClose(UInt16, MemHandle) SYS_TRAP(sysLibTrapCustom+2);
void iSecurLibAESCreateKeys(UInt16, MemHandle, UInt8 *) SYS_TRAP(sysLibTrapCustom+3);
void iSecurLibAESEncrypt(UInt16, MemHandle, UInt8 *, UInt8 *) SYS_TRAP(sysLibTrapCustom+4);
void iSecurLibAESDecrypt(UInt16, MemHandle, UInt8 *, UInt8 *) SYS_TRAP(sysLibTrapCustom+5);

/* SHA256.c */
#define SHA256DigestLength 32
MemHandle iSecurLibSHA256Open(UInt16) SYS_TRAP(sysLibTrapCustom+6);
void iSecurLibSHA256Close(UInt16, MemHandle) SYS_TRAP(sysLibTrapCustom+7);
void iSecurLibSHA256Init(UInt16, MemHandle) SYS_TRAP(sysLibTrapCustom+8);
void iSecurLibSHA256Transform(UInt16, MemHandle, UInt8 *, UInt32) SYS_TRAP(sysLibTrapCustom+9);
void iSecurLibSHA256Final(UInt16, MemHandle) SYS_TRAP(sysLibTrapCustom+10);

#endif
