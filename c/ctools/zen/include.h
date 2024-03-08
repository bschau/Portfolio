#ifndef INCLUDE_H
#define INCLUDE_H

#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* structs */
struct zen_node {
	struct zen_node *next;
	int id;
	time_t created;
	time_t modified;
	time_t closed;
	char *story;
};
#define ZN_LEN (sizeof(struct zen_node))
#define ZN_STORY (((char *) x) + ZN_LEN)
#define ZEN_VERSION "1.2"

/* macros */
#define ZEN_MAGIC "#ZEN1"

/* cmd_add.c */
void cmd_add(int argc, char *argv[], int optind);

/* cmd_close.c */
void cmd_close(int argc, char *argv[], int optind);

/* cmd_count.c */
void cmd_count(int argc, char *argv[], int optind, int count0);

/* cmd_delete.c */
void cmd_delete(int argc, char *argv[], int optind);

/* cmd_edit.c */
void cmd_edit(int argc, char *argv[], int optind);

/* cmd_init.c */
void cmd_init(void);

/* cmd_list.c */
void cmd_list(int argc, char *argv[], int optind);

/* cmd_reopen.c */
void cmd_reopen(int argc, char *argv[], int optind);

/* cmd_status.c */
void cmd_status(int argc, char *argv[], int optind);

/* cmd_view.c */
void cmd_view(int argc, char *argv[], int optind);

/* editor.c */
char *editor(char *story);

/* stringbuilder.c */
typedef void * StringBuilder;

StringBuilder stringbuilder_new(int g);
void stringbuilder_free(StringBuilder s);
void stringbuilder_append(StringBuilder s, char *src, int len);
void stringbuilder_append_char(StringBuilder s, char src);
char *stringbuilder_to_string(StringBuilder s);
int stringbuilder_length(StringBuilder s);
void stringbuilder_set_length(StringBuilder s, int len);
void stringbuilder_reset(StringBuilder s);
void stringbuilder_copy_to(StringBuilder src, StringBuilder dst);
int stringbuilder_position(StringBuilder s);

/* utils.c */
void set_file_permissions(char *path, int mode);
char *read_file(char *file);

/* zen_file.c */
void zen_file_init(void);
void zen_file_read(void);
void zen_file_write(void);
void zen_add_story(char *story);
char *zen_trim_story(char *story);
struct zen_node *zen_next(void);
struct zen_node *zen_node_by_id(int id, struct zen_node **prev);
char *zen_encode(char *src);
char *zen_decode(char *src);
int zen_count(void);

#ifndef WIN32
extern int errno;
#endif

#endif
