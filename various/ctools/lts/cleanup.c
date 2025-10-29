#include "include.h"

/**
 * Delete unseen files.
 * 
 * @param ht Hashtable with files.
 */
void delete_files(void) {
	struct stat buf;
	const char *key;
	const void *val;
	char *s;
	int v_size;

	ch_dir(d_path);

	if (hash_find_first(dst_ht, &key, &val, &v_size) != 0) {
		return;
	}

	do {
		s = strchr((char *) key, SEPARATOR_CHAR) + 1;

#ifdef WIN32
		if (stat(s, &buf)) {
#else
		if (lstat(s, &buf)) {
#endif
			if (errno != EEXIST) {
				perror(s);
			}

			continue;
		}

		if (S_ISREG(buf.st_mode)) {
			if (dry_run) {
				printf("D   %s\n", key);
			} else {
				if (unlink(s)) {
					perror(s);
				} else {
					printf("D   %s\n", key);
				}
			}
		} else {
			fprintf(stderr, "%s%s is not a regular file\n", d_path, key);
		}
	} while (hash_find_next(dst_ht, &key, &val, &v_size) == 0);
}

/**
 * Delete empty directories in dst if not found in src.
 *
 * @param p Path.
 */
void delete_empty_dirs(char *p) {
	struct dirent *de;
	struct stat buf;
	DIR *dh;

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
				delete_empty_dirs(de->d_name);

				if (chdir("..")) {
					perror("..");
					exit(1);
				}

				path_up();

				snprintf(full_path, sizeof(full_path), "%s%c%s%c%s", s_path, SEPARATOR_CHAR, path_get(), SEPARATOR_CHAR, de->d_name);
				collapse_slashes(full_path);

#ifdef WIN32
				if (stat(full_path, &buf)) {
#else
				if (lstat(full_path, &buf)) {
#endif
					if (errno == ENOENT) {
						if (!dry_run) {
							rmdir(de->d_name);
						}

						printf("D   %s%c%s\n", path_get(), SEPARATOR_CHAR, de->d_name);
					} else {
						perror(de->d_name);
					}
				}
			}
		} else {
			perror(de->d_name);
		}
	}

	closedir(dh);
}
