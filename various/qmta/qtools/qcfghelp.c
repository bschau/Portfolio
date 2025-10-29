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

	rest=common(argc, argv, NULL, "hr:", "configuration-directive", NULL, NULL, NORMAL, ALLUSERS);
	if (rest!=1)
		usage(1);

	helpconfig(argv[optind]);
	exit(0);
}
