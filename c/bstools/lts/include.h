#ifndef INCLUDE_H
#define INCLUDE_H

#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#ifdef WIN32
#include <limits.h>
#endif
#ifdef MACOSX
#include <limits.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "common.h"

/* macros */
#define BUF_SIZE 20480

/* cleanup.c */
void delete_files(void);
void delete_empty_dirs(char *p);

/* path.c */
void path_init(void);
char *path_get(void);
void path_add(char *p);
void path_up(void);

/* synchronize.c */
void synchronize_files(char *p);

/* utils.c */
void collapse_slashes(char *src);
void get_cwd(char *dst);
void ch_dir(char *dir);

/* walk_dst.c */
void walk_dst(void);

#ifdef MAIN
	int dry_run = 0;
	char full_path[PATH_MAX + 1];
	char s_path[PATH_MAX + 1], d_path[PATH_MAX + 1];
	char copy_buf[BUF_SIZE], diff_buf[BUF_SIZE];
	HashTable dst_ht;
#else
	extern int dry_run;
	extern char full_path[PATH_MAX + 1];
	extern char s_path[PATH_MAX + 1], d_path[PATH_MAX + 1];
	extern char copy_buf[BUF_SIZE], diff_buf[BUF_SIZE];
	extern HashTable dst_ht;
#endif

extern int errno;

#endif
