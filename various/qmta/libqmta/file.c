#include "private.h"

/*******************************************************************************
*
*	writefd
*
*	Write to fd descriptor.
*
*	Input:
*		f - fd.
*		src - what to write.
*		cnt - how much to write.
*	Output:
*		s: 0.
*		f: !0.
*/
int
writefd(int f,
        char *src,
        int cnt)
{
	int err;
	
	if (!cnt)
		return 0;

	for (EVER) {
		err=write(f, src, cnt);
		if (err==-1)
			return -1;

		cnt-=err;
		src+=err;
		if (cnt<=0)
			return 0;
	}
}

/*******************************************************************************
*
*	writefile
*
*	Write to FILE descriptor.
*
*	Input:
*		f - FILE.
*		src - what to write.
*		cnt - how much to write.
*	Output:
*		s: 0.
*		f: !0.
*/
int
writefile(FILE *f,
          char *src,
          int cnt)
{
	size_t err;
	
	if (!cnt)
		return 0;

	for (EVER) {
		err=fwrite(src, 1, cnt, f);
		if (err==cnt)
			return 0;	/* most likely :) */

		if (ferror(f))
			return -1;

		cnt-=err;
		src+=err;
		if (cnt<=0)
			return 0;
	}
}

/*******************************************************************************
*
*	getfilesize
*
*	Returns file size.
*
*	Input:
*		f - file to get size for.
*	Output:
*		s: size.
*		f: -1.
*/
int
getfilesize(char *f)
{
	struct stat buf;
	int err;

	err=stat(f, &buf);
	if (err)
		return -1;

	return buf.st_size;
}

/*******************************************************************************
*
*	lockfile
*
*	Lock a file.
*
*	Input:
*		fd - file descriptor.
*	Output:
*		file is locked: 0.
*		file is not locked: !0.
*/
int
lockfile(int fd)
{
	struct flock fl;
	int res;

	fl.l_type=F_WRLCK;
	fl.l_whence=SEEK_SET;
	fl.l_start=0;
	fl.l_len=1;
	fl.l_pid=-1;

	res=fcntl(fd, F_SETLK, &fl);
	if (res==-1)
		return -1;

	return 0;
}

/*******************************************************************************
*
*	unlockfile
*
*	Unlock a file.
*
*	Input:
*		fd - file descriptor.
*/
void
unlockfile(int fd)
{
	struct flock fl;

	fl.l_type=F_UNLCK;
	fl.l_whence=SEEK_SET;
	fl.l_start=0;
	fl.l_len=1;
	fl.l_pid=-1;

	fcntl(fd, F_SETLK, &fl);
}

/*******************************************************************************
*
*	getfile
*
*	Load a file into memory.   Memory is dynamically allocated.
*
*	Input:
*		fn - file name.
*	Output:
*		s: memory block.
*		f: NULL.
*/
char *
getfile(char *fn)
{
	FILE *f;
	char *mem, *p;
	int fsize, len;

	fsize=getfilesize(fn);
	if (fsize==-1) {
		qlog(QINFO, "(getfile) cannot get filesize for %s", fn);
		return NULL;
	}

	f=fopen(fn, "r");
	if (!f) {
		qlog(QINFO, "(getfile) cannot open file %s (%i)", fn, errno);
		return NULL;
	}

	mem=(char *)calloc(fsize+1, 1);
	if (!mem) {
		qlog(QWARN, "(getfile) out of memory (%i bytes)", fsize+1);
		fclose(f);
		return NULL;
	}

	/* this is special */
	if (!fsize) {
		fclose(f);
		return mem;
	}

	/* read file */
	p=mem;
	while (fsize>0) {
		len=fread(p, 1, fsize, f);
		if (len==fsize)
			break;

		if (feof(f) || ferror(f)) {
			free(mem);
			mem=NULL;
			break;
		}

		fsize-=len;
		p+=len;
	}

	fclose(f);
	return mem;
}

/*******************************************************************************
*
*	relpath
*
*	Create relative filepath.
*
*	Input:
*		p - prepath.
*		f - relative file.
*	Output:
*		s: path.
*		f: NULL.
*/
char *
relpath(char *p,
        char *f)
{
	int len=strlen(p)+strlen(f)+2;
	char *path;

	path=(char *)calloc(len, 1);
	if (!path)
		return NULL;

	strcat(path, p);
	strcat(path, "/");
	strcat(path, f);

	return path;
}

/*******************************************************************************
*
*	fopenrel
*
*	fopen relative path.
*
*	Input:
*		p - prepath.
*		f - relative file.
*		m - mode.
*	Output:
*		s: file handle.
*		f: NULL.
*/
FILE *
fopenrel(char *p,
         char *f,
         char *m)
{
	FILE *hd=NULL;
	char *path;

	path=relpath(p, f);
	if (path) {
		hd=fopen(path, m);
		free(path);
	}

	return hd;
}
