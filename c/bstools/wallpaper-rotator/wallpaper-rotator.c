#include <dconf/dconf.h>
#include <dconf/dconf-client.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#include "wallpaper-rotator.h"

#define HISTORY_LEN (5)

struct node {
	struct node *next;
	short locked;
	char *name;
};

static struct node *get_available_files(char *picture_path);
static int get_picture_count(struct node *pictures);
static struct node *select_picture(struct node *pictures, struct node *history[]);
static int lock_unavailable_pictures(struct node *pictures, struct node *history[]);
static int is_on_history(struct node *picture, struct node *history[]);
static int reset_pictures_list(struct node *pictures, struct node *first);
static struct node *get_picture_by_index(struct node *pictures, int index);
static void unlock_all_pictures(struct node *pictures);
static void set_background_picture(GSettings *settings, char *picture_path, char *name);
static void destroy_pictures_list(struct node *pictures);

static gchar *schema = "org.gnome.desktop.background";
static gchar *key = "picture-uri";

void rotate(char *picture_path, int rotation_speed)
{
	struct node *history[HISTORY_LEN] = { NULL, NULL, NULL, NULL, NULL };
	struct node *pictures, *picture;
	int count, idx;
	GSettings *settings;

	srandom(time(NULL));

	g_type_init();

	settings = g_settings_new(schema);
	if (settings == NULL) {
		syslog(LOG_ERR, "Cannot allocat settings schema");
		exit(1);
	}

	for (;;) {
		pictures = get_available_files(picture_path);
		count = get_picture_count(pictures);

		if (count == 0) {
			syslog(LOG_ERR, "No pictures found in %s\n", picture_path);
			exit(1);
		}

		if (count == 1) {
			set_background_picture(settings, picture_path, pictures->name);
		} else {
			picture = select_picture(pictures, history);

			if (history[HISTORY_LEN - 1]) {
				free(history[HISTORY_LEN - 1]);
			}

			for (idx = HISTORY_LEN -1; idx > 0; idx--) {
				history[idx] = history[idx - 1];
			}

			history[0] = picture;

			set_background_picture(settings, picture_path, picture->name);
		}

		destroy_pictures_list(pictures);

		sleep(rotation_speed);
	}
}

static struct node *get_available_files(char *picture_path)
{
	DIR *dir = opendir(picture_path);
	struct node *nodes = NULL;
	struct node *node;
	struct dirent *dir_entry;
	char *name;
	int len;

	if (dir == NULL) {
		syslog(LOG_ERR, "Failed to open %s for reading", picture_path);
		exit(1);
	}

	while ((dir_entry = readdir(dir)) != NULL) {
		if (!strcmp(dir_entry->d_name, ".") || !strcmp(dir_entry->d_name, "..")) {
			continue;
		}

		len = sizeof(struct node) + 1;
		len += strlen(dir_entry->d_name);

		if ((node = calloc(len, 1)) == NULL) {
			syslog(LOG_ERR, "Failed to allocate %i bytes", len);
			exit(1);
		}

		name = ((char *) node) + sizeof(struct node);
		strncpy(name, dir_entry->d_name, strlen(dir_entry->d_name));
		node->name = name;
		node->next = nodes;
		nodes = node;
	}

	closedir(dir);
	return nodes;
}

static int get_picture_count(struct node *pictures)
{
	int count = 0;

	while (pictures) {
		count++;
		pictures = pictures->next;
	}

	return count;
}

static struct node *select_picture(struct node *pictures, struct node *history[])
{
	int count = 0;
	struct node *picture;

	count = lock_unavailable_pictures(pictures, history);

	if (count == 0) {
		count = reset_pictures_list(pictures, history[0]);
	}

	picture = get_picture_by_index(pictures, random() % count);
	unlock_all_pictures(pictures);

	picture->locked = 1;
	return picture;
}

static int lock_unavailable_pictures(struct node *pictures, struct node *history[])
{
	int count = 0;

	while (pictures) {
		if (is_on_history(pictures, history)) {
			pictures->locked = 1;
		} else {
			count++;
		}

		pictures = pictures->next;
	}

	return count;
}

static int is_on_history(struct node *picture, struct node *history[])
{
	int i;

	for (i = 0; i < HISTORY_LEN; i++) {
		if (history[i] == NULL) {
			continue;
		}

		if (!strcmp(history[i]->name, picture->name)) {
			return 1;
		}
	}

	return 0;
}

static int reset_pictures_list(struct node *pictures, struct node *first)
{
	int count = 0;

	while (pictures) {
		pictures->locked = 0;

		if (first) {
			if (!strcmp(first->name, pictures->name)) {
				pictures->locked = 1;
				count--;
			}
		}

		count++;
		pictures = pictures->next;
	}

	return count;
}

static struct node *get_picture_by_index(struct node *pictures, int index)
{
	while (pictures) {
		if (!pictures->locked) {
			if (index == 0) {
				break;
			}
			index--;
		}

		pictures = pictures->next;
	}

	return pictures;
}

static void unlock_all_pictures(struct node *pictures)
{
	while (pictures) {
		pictures->locked = 0;
		pictures = pictures->next;
	}
}

static void set_background_picture(GSettings *settings, char *picture_path, char *name)
{
	int len = 9;
	char *path, *p;

	if (picture_path[0] != '/') {
		len++;
	}

	len += strlen(picture_path);
	len += strlen(name);

	if ((path = (char *) calloc(len, 1)) == NULL) {
		syslog(LOG_ERR, "Failed to allocate %i bytes", len);
		exit(1);
	}

	p = path;
	memmove(p, "file://", 7);
	p += 7;
	if (picture_path[0] != '/') {
		*p++ = '/';
	}

	memmove(p, picture_path, strlen(picture_path));
	p += strlen(picture_path);
	*p++ = '/';
	memmove(p, name, strlen(name));

	printf("setting: %s\n", path);

	if (g_settings_set_string(settings, key, (gchar *) path)) {
		g_settings_sync();
	} else {
		syslog(LOG_WARNING, "Cannot set %s in %s to %s", key, schema, path);
	}

	free(path);
}

static void destroy_pictures_list(struct node *pictures)
{
	struct node *next;

	while (pictures) {
		next = pictures->next;

		if (!pictures->locked) {
			free(pictures);
		}

		pictures = next;
	}
}
