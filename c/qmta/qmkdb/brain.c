#include "private.h"

/* protos */
static int qrename(char *, char *);

/*******************************************************************************
*
*	brain
*
*	Recreates the databases - this is the control part.
*
*	Input:
*		db - database name.
*		fn - file name.
*		parse - parse function.
*/
void
brain(char *db,
      char *fn,
      int (*parse)())
{
	char tpn[14]="etc/dbaXXXXXX";
	char rtn[14]="etc/tmpXXXXXX";
	int err, rtnr;
	FILE *file;

	initdblist(&dbl);
	file=fopen(fn, "r");
	if (!file) {
		fprintf(stderr, "cannot open %s (%i)\n", fn, errno);
		return;
	}

	err=(parse)(file);
	fclose(file);
	if (err)
		return;

	umask(0);
	err=mkstemp(tpn);
	if (err==-1) {
		fprintf(stderr, "cannot create temporary file\n");
		return;
	}
	close(err);

	err=populate(&dbl, tpn);
	if (err) {
		fprintf(stderr, "cannot populate %s - file deleted\n", tpn);
		unlink(tpn);
		return;
	}

	rtnr=qrename(db, rtn);
	err=rename(tpn, db);
	if (err) {
		fprintf(stderr, "cannot move %s to %s\n", tpn, db);
		unlink(tpn);
		return;
	}

	if (rtnr) {
		err=unlink(rtn);
		if (err)
			fprintf(stderr, "cannot delete %s - it is safe to do it yourself\n", rtn);
	}

	freedblist(&dbl);
}

/*******************************************************************************
*
*	qrename
*
*	Renames a file to a new temporary name.
*
*	Input:
*		fn - file to rename.
*		rtn - random tmp name (template).
*	Output:
*		file renamed: !0.
*		file not renamed: 0.
*/
static int
qrename(char *fn,
        char *rtn)
{
	char *f;
	int fd, err;

	fd=open(fn, O_RDONLY);
	if (fd==-1) {
		if (errno!=ENOENT)
			fprintf(stderr, "general file error on %s\n", fn);

		return 0;
	}
	close(fd);

	f=mktemp(rtn);
	if (!f) {
		fprintf(stderr, "cannot generate random name");
		return 0;
	}

	err=rename(fn, f);
	if (err) {
		fprintf(stderr, "cannot rename %s to %s\n", fn, f);
		return 0;
	}

	return 1;
}
