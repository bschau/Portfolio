#include "private.h"

/* globals */
static char cntldir[CNTLFILENAMELEN+1];
static DIR *dir;

/* protos */
static int travstart(void);
static char *traverse(void);

/*******************************************************************************
*
*	travstart
*
*	Start traverse in current directory.
*
*	Output:
*		s: 0.
*		f: !0.
*/
static int
travstart(void)
{
	if (dir) {
		closedir(dir);
		dir=NULL;
	}

	dir=opendir(".");
	if (dir)
		return 0;

	qlog(QWARN, "(travstart) cannot start traversal");
	return -1;
}

/*******************************************************************************
*
*	traverse
*
*	Traverse a previously opened directory.
*
*	Output:
*		s: next entry.
*		f: NULL (no more entries).
*/
static char *
traverse(void)
{
	struct dirent *de;

	for (EVER) {
		de=readdir(dir);
		if (!de)
			return NULL;

		if (strcmp(de->d_name, ".") && strcmp(de->d_name, ".."))
			return de->d_name;
	}
}

/*******************************************************************************
*
*	sweepdone
*
*	Clean up after sweep.
*/
void
sweepdone(void)
{
	int removed=0, err;
	char *f;

	doneinfected();
	sophosclose();

	if (*cntldir) {
		if (!chdir(cntldir)) {
			err=travstart();
			if (!err) {
				for (f=traverse(); f; f=traverse())
					unlink(f);
			}
		}

		if (!chdir(".."))  {
			err=rmdir(cntldir);
			if (!err)
				removed=1;
		}	

		if (!removed)
			qlog(QWARN, "(sweepdone) cannot perform cleanup on %s (%i)", cntldir, errno);
	}
}

/*******************************************************************************
*
*	sweep
*
*	Sweep file.
*
*	Input:
*		cntl - control file name.
*/
void
sweep(char *cntl)
{
	int clean=SCLEAN, err;
	char renbuf[CNTLFILENAMELEN+1];
	char *f, *msg;

	memmove(cntldir+1, cntl+1, CNTLFILENAMELEN-1);
	*cntldir='D';
	cntldir[CNTLFILENAMELEN]='\x00';
	
	err=mkdir(cntldir, 0700);
	if (err && errno!=EEXIST) {
		*cntldir='\x00';
		qlog(QWARN, "(sweep) cannot create directory (%i)", errno);
		return;
	}

	err=chdir(cntldir);
	if (err) {
		qlog(QWARN, "(sweep) cannot change directory to %s (%i)", cntldir, errno);
		return;
	}

	alarm(smaxsweep);
	err=munpack(cntl);
	alarm(0);
	if (err)
		return;

	err=sophosopen(0);
	if (err)
		return;

	err=travstart();
	if (err)
		return;

	msg=QTXTSCANNERPROBLEMS;
	for (f=traverse(); f; f=traverse()) {
		err=sophos(f);
/*		unlink(f);	*/
		if (err==SCLEAN)
			continue;

		if (err!=-1)
			msg=QTXTVIRUS;

		clean=SINFECTED;
		break;
	}

	err=chdir("..");
	if (err) {
		qlog(QWARN, "(sweep) cannot break out (%i)", errno);
		return;
	}

	if (clean==SCLEAN) {
		memmove(renbuf+1, cntl+1, CNTLFILENAMELEN-1);
		*renbuf='c';
		err=rename(cntl, renbuf);
		if (err)
			qlog(QINFO, "(sweep) cannot rename from %s to %s (%i)", cntl, renbuf, errno);
	} else
		infected(cntl, msg);
}
