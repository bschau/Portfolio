#include "libqmta.h"

/* globals */
static dblist dbl;

/* protos */
static void runtest(database *, char *);

/*******************************************************************************
*
*	main
*/
int
main(int argc,
     char *argv[])
{
	int err;
	database mindb;

	initdblist(&dbl);
	adddblistentry(&dbl, "brisse", "data1", 5);
	adddblistentry(&dbl, "huj", "data huj", 8);
	adddblistentry(&dbl, "gdf", "data gdf", 8);
	adddblistentry(&dbl, "aggg", "data aggg", 9);
	adddblistentry(&dbl, "_129", "data _129", 9);
	printf("%i items\n", dbl.items);

	err=populate(&dbl, "test.db");
	if (err)
		printf("bummer\n");
		
	freedblist(&dbl);

	printf("\nStarting test\n");
	err=qdbopen(&mindb, "test.db");
	if (err)
		printf("bummer");
	else {
		runtest(&mindb, "nosuchkey");
		runtest(&mindb, NULL);
		runtest(&mindb, "yetanotherunknown key");
		runtest(&mindb, "brisse");
		runtest(&mindb, "huj");
		runtest(&mindb, "gdf");
		runtest(&mindb, "aggg");
		runtest(&mindb, "_129");
	}
	qdbclose(&mindb);
	
	exit(0);
}

/*******************************************************************************
*
*	runtest
*
*	Run a (simple) test.   Look up a value to a key.
*
*	Input:
*		d - database.
*		k - key.
*/
static void
runtest(database *d,
        char *k)
{
	int err;

	err=qdbfetch(d, k);
	if (err)
		printf("Error, %s\n", ((d->dptr) ? d->dptr : "(none)"));
	else 
		printf("Key: %s = %s\n", ((k) ? k : "(none)"), ((d->dptr) ? d->dptr : "(none)"));
}
