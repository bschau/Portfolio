/*******************************************************************************
 *
 *	main.c
 *
 *	Main driver for blowfish and md5.
 */
#include <cygwin/in.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "public.h"

/* protos */
static void sigalarm(int);
static void md5run(unsigned char *);

/* globals */
static int run, loops;

/*******************************************************************************
 *
 *	sigalarm
 *
 *	Alarm handler - terminates blowfish test.
 *
 *	Input:
 *		signum - signal number (SIGALRM).
 */
static void
sigalarm(int signum)
{
	run=0;
	signal(SIGALRM, SIG_DFL);
}

/*******************************************************************************
 *
 *	md5run
 *
 *	Sub stub for md5 test.
 *
 *	Input:
 *		s - message to digest.
 */
static void
md5run(unsigned char *s)
{
	int i;
	unsigned char digest[16];

	if (strlen(s))
		printf("    %s:  ", s);
	else
		printf("    \"\":  ");

	md5((unsigned long *)digest, s);
	for (i=0; i<16; i++)
		printf("%02x", digest[i]);

	printf("\n");
}

/*******************************************************************************
 *
 *	main
 */
int
main(int argc,
     char *argv[])
{
	unsigned char text[16];
	unsigned char *key="TESTKEY";
	int keylen, textlen, i;
	blowfishkey ks;
	unsigned char digest[16];
	unsigned long l1, l2;

#if 0
	printf("MD5:\n\n");
	md5run("");
	md5run("a");
	md5run("abc");
	md5run("message digest");
	md5run("abcdefghijklmnopqrstuvwxyz");
#endif

/*
	printf("    Running 10 seconds test ... ");
	fflush(stdout);
	signal(SIGALRM, sigalarm);
	loops=0;
	run=1;
	alarm(10);
	while (run) {
		md5((unsigned long *)digest, key);
		loops++;
	}

	printf("%i loops.\n", loops);

	alarm(0);
*/

	printf("\n\nBlowfish:\n\n");
	keylen=strlen(key);
	blowfishmakekey(&ks, key, keylen);

/*
	for (i=0; i<sizeof(text); i++)
		text[i]='\x00';
*/

	l1=1;	//htonl(1);
	l2=2;	//htonl(2);
	bfenc(&ks, &l1, &l2);
/*
	memmove(text, &l1, 4);
	memmove(text+4, &l2, 4);

	textlen=8;
	blowfishencrypt(&ks, text);
*/
	// should output 0xdf333fd2 0x30a71bb4
	printf("    Encrypted: '%08lx %08lx'\n", l1, l2);
/*
	for (i=0; i<textlen; i++)
		printf("%02x", text[i]);

	printf("'\n");
*/
//	printf("%08lx %08lx\n", l1, l2);
#if 0
	blowfishdecrypt(&ks, text);

	for (i=0; i<textlen; i++)
		printf("%02x", text[i]);
//	printf("    Decrypted: '%s'\n", text);

/*
	printf("    Running 10 seconds test ... ");
	fflush(stdout);
	signal(SIGALRM, sigalarm);
	loops=0;
	run=1;
	alarm(10);
	while (run) {
		blowfishmakekey(&ks, key, keylen);
		blowfishencrypt(&ks, text);
		blowfishdecrypt(&ks, text);
		loops++;
	}

	printf("%i loops.\n", loops);

	alarm(0);
*/

	l1=0x00000001;
	l2=0x00000002;
	bfenc(&ks, &l1, &l2);
	printf("\n%08X %08X\n", l1, l2);
#endif
	exit(0);
}
