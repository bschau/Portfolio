/*******************************************************************************
 *
 *	main.c
 *
 *	Main driver for blowfish and md5.
 */
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
	unsigned long l1=1, l2=2;
	unsigned char *key="MyLongKey-longish";
	int keylen;
	blowfishkey ks;
	unsigned char digest[16];

	printf("MD5:\n\n");
	md5run("");
	md5run("a");
	md5run("abc");
	md5run("message digest");
	md5run("abcdefghijklmnopqrstuvwxyz");

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

	printf("\n\nBlowfish:\n\n");
	keylen=strlen(key);
	blowfishmakekey(&ks, key, keylen);

	printf("    Test-set:  %08lx %08lx\n", l1, l2);
	blowfishencrypt(&ks, &l1, &l2);
	printf("    Encrypted: %08lx %08lx\n", l1, l2);
	blowfishdecrypt(&ks, &l1, &l2);
	printf("    Decrypted: %08lx %08lx\n", l1, l2);

	printf("    Running 10 seconds test ... ");
	fflush(stdout);
	signal(SIGALRM, sigalarm);
	loops=0;
	run=1;
	alarm(10);
	while (run) {
		blowfishmakekey(&ks, key, keylen);
		blowfishencrypt(&ks, &l1, &l2);
		blowfishdecrypt(&ks, &l1, &l2);
		loops++;
	}

	printf("%i loops.\n", loops);

	alarm(0);

	exit(0);
}
