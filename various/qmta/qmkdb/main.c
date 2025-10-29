#define MAIN
#include "private.h"

/*******************************************************************************
*
*	main
* 
*		-h - request help.
*		-r root - root directory (qroot).
*/
int
main(int argc,
     char *argv[])
{
	int rest;
	struct {
		unsigned int doaccess:1;
		unsigned int dodomain:1;
		unsigned int dospammer:1;
	} whattodo;

	rest=common(argc, argv, NULL, "hr:", "all | access | domain | spammer", NULL, NULL, NORMAL, ROOTONLY);
	if (rest!=1)
		usage(1);

	whattodo.doaccess=0;
	whattodo.dodomain=0;
	whattodo.dospammer=0;

	if (!strcasecmp(argv[optind], "all")) {
		whattodo.doaccess=1;
		whattodo.dodomain=1;
		whattodo.dospammer=1;
	} else if (!strcasecmp(argv[rest], "access"))
		whattodo.doaccess=1;
	else if (!strcasecmp(argv[rest], "domain"))
		whattodo.dodomain=1;
	else if (!strcasecmp(argv[rest], "spammer"))
		whattodo.dospammer=1;
	else {
		fprintf(stderr, "unknown argument %s\n", argv[rest]);
		usage(1);
	}

	rest=dropprivs(user);
	if (rest) {
		fprintf(stderr, "cannot drop privileges to %s\n", user);
		exit(1);
	} 

	rest=chdir(qroot);
	if (rest) {
		fprintf(stderr, "failed to change directory to %s\n", qroot);
		exit(1);
	}

	if (whattodo.doaccess)
		mkdbaccess();

	if (whattodo.dodomain)
		mkdbdomain();

	if (whattodo.dospammer)
		mkdbspammer();

	exit(0);
}
