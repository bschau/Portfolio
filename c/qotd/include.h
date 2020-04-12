#ifndef INCLUDE_H
#define INCLUDE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

struct qotd {
	struct qotd *next;
	char *text;
};

#define QOTD_VERSION "1.0"
#ifdef WIN32
#define QOTD_FILE "_qotd"
#else
#define QOTD_FILE ".qotd"
#endif

/* qotd_file.c */
void qotd_file_set(char *qotd_file);
void qotd_file_load(void);
char *qotd_file_pick_one(int preferred);
void qotd_file_output(FILE *sh, char *qotd);

#endif
