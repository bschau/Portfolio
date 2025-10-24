#define MAIN
#include "libqmta.h"

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

	rest=common(argc, argv, NULL, "hr:", NULL, NULL, NULL, NORMAL, ALLUSERS);
	if (rest)
		usage(1);

	dumpconfig();
	exit(0);
}
