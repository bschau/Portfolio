#ifndef PUBLIC_H
#define PUBLIC_H

#include <string.h>

/* macros - PalmOS to World conversion */
#define MemSet(x, y, z) memset(x, z, y)
#define StrNCopy strncpy
#define StrLen strlen
#define MemMove memmove

/* structs */
typedef struct {
	unsigned long parray[18];
	unsigned long sbox1[256];
	unsigned long sbox2[256];
	unsigned long sbox3[256];
	unsigned long sbox4[256];
} blowfishkey;

/* blowfish.c */
void bfenc(blowfishkey *, unsigned long *, unsigned long *);
void bfdec(blowfishkey *, unsigned long *, unsigned long *);
void blowfishmakekey(blowfishkey *, unsigned char *, unsigned int);
void blowfishencrypt(blowfishkey *, unsigned char *);
void blowfishdecrypt(blowfishkey *, unsigned char *);

/* md5.c */
void md5(unsigned long *dst, unsigned char *src);

#endif
