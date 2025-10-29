#ifndef INCLUDE_H
#define INCLUDE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fnmatch.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef WIN32
#include "fnmatch.h"
#endif
#include "common.h"

/* structs */
struct ignore_node {
	struct ignore_node *next;
};
#define IN_LEN (sizeof(struct ignore_node))
#define IN_ENTRY(x) (((char *) x) + IN_LEN)

/* macros */
#define IGNOREFILE ".RSC/ignore"
#define LOCKFILE ".RSC/locked"
#define RCFILE ".RSC/rc"
#define STAMPFILE ".RSC/stamp"

/* cmd_ignore.c */
void cmd_ignore(int argc, char *argv[], int optind);
void ignore_list_read(void);
void ignore_list_write(void);
void ignore_list_destroy(void);
struct ignore_node *ignore_list_find(char *e);
void ignore_list_add(char *e);
int ignore_list_match(char *e);
void ignore_list_print(char sep);

/* cmd_info.c */
void cmd_info(void);

/* cmd_init.c */
void cmd_init(int argc, char *argv[], int optind);

/* cmd_lock.c */
void cmd_lock(void);

/* cmd_unlock.c */
void cmd_unlock(void);

/* cmd_set.c */
void cmd_set(int argc, char *argv[], int optind);

/* cmd_sync.c */
void cmd_sync(int copy);

/* utils.c */
void config_read(void);
void config_write(void);
void config_find(void);
void config_write_stamp(void);
char *xgets(char *dst, int len, FILE *fp);
void set_file_permissions(char *path, int mode);
void get_cwd(char *dst);
int is_locked(void);
void exit_if_link(char *p);
StringBuilder command_expand(int show);

#ifdef MAIN
	char root_dir[PATH_MAX + 1], cur_path[PATH_MAX + 1];
	int force = 0, root_dir_len;
	time_t stamp;
	char *command = "rsync -rltvz $RSCSHOW --omit-dir-times --rsync-path=\"$RSCREMOTERSYNC --server --daemon --config=$RSCREMOTECONFIG\" --rsh=ssh --delete --exclude-from=.RSC/ignore . $RSCMODULE", *module = NULL, *remote_config = "/etc/rsync.cfg", *remote_rsync = "/usr/bin/rsync";
#else
	extern char root_dir[PATH_MAX + 1], cur_path[PATH_MAX + 1];
	extern int force, root_dir_len;
	extern time_t stamp;
	extern char *command, *module, *remote_config, *remote_rsync;
#endif

#endif
