#include "include.h"

/* forward decls */
static void run_discover(char *p);

/**
 * Discover path.
 */
void walk_dst(void) {
	/* initial one million buckets */
	if ((dst_ht = hash_new(20)) == NULL) {
		fprintf(stderr, "Failed to allocate hashtable\n");
		exit(1);
	}

	ch_dir(d_path);
	path_init();

	run_discover(".");
}

/**
 * Tree walk ..
 *
 * @param ht HashTable.
 * @param p Path component.
 */
static void run_discover(char *p) {
	struct stat buf;
	DIR *dh;
	struct dirent *de;

	ch_dir(p);

	if ((dh = opendir(".")) == NULL) {
		perror(p);
		exit(1);
	}

	if (strcmp(p, ".")) {
		path_add(p);
	}

	while ((de = readdir(dh)) != NULL) {
		if ((strcmp(de->d_name, ".") == 0) ||
		    (strcmp(de->d_name, "..") == 0)) {
			continue;
		}

#ifdef WIN32
		if (stat(de->d_name, &buf) == 0) {
#else
		if (lstat(de->d_name, &buf) == 0) {
#endif
			if (S_ISDIR(buf.st_mode)) {
				run_discover(de->d_name);
				ch_dir("..");

				path_up();
			} else {
				if (S_ISREG(buf.st_mode)) {
					snprintf(full_path, sizeof(full_path), "%s%c%s", path_get(), SEPARATOR_CHAR, de->d_name);

					if (hash_add(dst_ht, full_path, NULL, 0, HT_FREE_VALUE) != 0) {
						fprintf(stderr, "Failed to add node to hashtable\n");
						exit(1);
					}
#ifndef WIN32
				} else if (S_ISLNK(buf.st_mode)) {
					fprintf(stderr, "Skipping link: %s%c%s\n", path_get(), SEPARATOR_CHAR, de->d_name);
#endif
				} else {
					fprintf(stderr, "Skipping unknown filetype: %s%c%s\n", path_get(), SEPARATOR_CHAR, de->d_name);
				}	
			}
		} else {
			fprintf(stderr, "Skipping unreadable file: %s%c%s\n", path_get(), SEPARATOR_CHAR, de->d_name);
		}
	}

	closedir(dh);
}
