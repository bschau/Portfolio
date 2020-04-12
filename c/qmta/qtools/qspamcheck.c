#define MAIN
#include "libqmta.h"

/* protos */

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
	int rest, err, cnt;

	rest=common(argc, argv, NULL, "hr:", "ip-address1 ip-address2 ... ip-addressX", NULL, NULL, NORMAL, ALLUSERS);

	err=dropprivs(user);
	if (err) {
		fprintf(stderr, "cannot drop privileges to %s\n", user);
		exit(1);
	}

	if (!rest)
		usage(1);

	for (cnt=0; cnt<rest; cnt++) {
		spamcheck(argv[optind], SCVERBOSE);
		optind++;
	}

	exit(0);
}
