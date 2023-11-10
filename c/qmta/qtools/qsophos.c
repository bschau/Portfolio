#define MAIN
#include "libqmta.h"

/* protos */
static void cleanup(void);

/*******************************************************************************
*
*	cleanup
*
*	Perform global cleanup.
*/
static void
cleanup(void)
{
	sophosclose();
	syscleanup();
}

/*******************************************************************************
*
*	main
*
*	Entry point - uses standard switches.
*/
int
main(int argc,
     char *argv[])
{
	int rest;

	rest=common(argc, argv, NULL, "hr:", NULL, &cleanup, NULL, NORMAL, ALLUSERS);
	if (rest)
		usage(1);

	sophoslist();
	exit(0);
}
