#include "libqmta.h"

/* globals */
static int fd=-1;
static database db;

/* protos */
static void cleanup(void);
static void graceful(int);

/*******************************************************************************
*
*	cleanup
*
*	Perform global cleanup.
*/
static void
cleanup(void)
{
	donemail();
	qdbclose(&db);

	if (fd>-1)
		close(fd);

	deconfigure();
}

/*******************************************************************************
*
*	graceful
*
*	Major shutdown.
*
*	Input:
*		unused - just that.
*/
static void
graceful(int unused)
{
	exit(0);
}

/*******************************************************************************
*
*	main
*/
int
main(int argc,
     char *argv[])
{
	int err;

	err=common(argc, argv, NULL, "hr:", NULL, &cleanup, &graceful, NORMAL, ALLUSERS);
	if (err!=1) {
		fprintf(stderr, "please specify control file\n");
		exit(1);
	}

	err=qdbopen(&db, QDOMAINDB);
	if (err) {
		fprintf(stderr, "cannot open %s", QDOMAINDB);
		exit(1);
	}

	err=chdir(spooldir);
	if (err) {
		fprintf(stderr, "cannot change to %s (%i)\n", spooldir, errno);
		exit(1);
	}

	fd=open(argv[optind], O_RDWR);
	if (fd==-1) {
		fprintf(stderr, "cannot open %s (%i)\n", argv[optind], errno);
		exit(1);
	}

	err=lockfile(fd);
	if (err) {
		fprintf(stderr, "file %s is already in use\n", argv[optind]);
		exit(1);
	}

	fflush(NULL);
	err=sendmail(argv[optind], fd, &db, 0, SMVERBOSE);
	exit(0);
}
