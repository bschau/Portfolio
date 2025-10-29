#include "include.h"

/* forward decls */
static long should_copy_file(char *f);
static int file_compare(char *src, char *dst, size_t size);
static void copy_file(char *src, char *dst, size_t size);

/* globals */
static char f_path[PATH_MAX + 1];
static char file_status;

/**
 * Run synchronization.
 *
 * @param p Path.
 */
void synchronize_files(char *p) {
	struct stat buf;
	DIR *dh;
	struct dirent *de;

	if (chdir(p)) {
		perror(p);
		exit(1);
	}

	if ((dh = opendir(".")) == NULL) {
		perror(p);
		exit(1);
	}

	if (strcmp(p, ".")) {
		path_add(p);

		snprintf(full_path, sizeof(full_path), "%s%c%s", d_path, SEPARATOR_CHAR, path_get());
		collapse_slashes(full_path);

#ifdef WIN32
		if (mkdir(full_path)) {
#else
		if (mkdir(full_path, 0700)) {
#endif
			if (errno != EEXIST) {
				perror(full_path);
			}
		}
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
				synchronize_files(de->d_name);

				ch_dir("..");

				path_up();
			} else {
				if (S_ISREG(buf.st_mode)) {
					long t;

					snprintf(full_path, sizeof(full_path), "%s%c%s", path_get(), SEPARATOR_CHAR, de->d_name);
					collapse_slashes(full_path);
					hash_delete(dst_ht, full_path);

					if ((t = should_copy_file(de->d_name)) > -1) {
						copy_file(de->d_name, f_path, t);
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

/**
 * @return -1 if file shouldn't be copied, other values is file size.
 */
static long should_copy_file(char *f) {
	struct stat d_buf, s_buf;

	get_cwd(f_path);

	snprintf(f_path, sizeof(f_path), "%s%c%s%c%s", d_path, SEPARATOR_CHAR, path_get(), SEPARATOR_CHAR, f);
	collapse_slashes(f_path);

	if (stat(f, &s_buf)) {
		perror(f);
		exit(1);
	}

#ifdef WIN32
	if (stat(f_path, &d_buf)) {
#else
	if (lstat(f_path, &d_buf)) {
#endif
		if (errno == ENOENT) {
			file_status = 'A';
			return (long) s_buf.st_size;
		}

		perror(f_path);
		exit(1);
	}

	if (!S_ISREG(d_buf.st_mode)) {
		fprintf(stderr, "Will not copy %s as dst is not a regular file\n", f);
		return -1;
	}

	file_status = 'U';
	if (s_buf.st_size != d_buf.st_size) {
		return (long) s_buf.st_size;
	}

	if (file_compare(f, f_path, s_buf.st_size) != 0) {
		return (long) s_buf.st_size;
	}

	return -1;
}

/**
 * @param src Path.
 * @param dst Path
 * @param size Filesize.
 * @return 0 if the file is equal in both folders, !0 otherwise.
 */
static int file_compare(char *src, char *dst, size_t size) {
	int ret = 0;
	FILE *src_p, *dst_p;
	size_t to_read;

	if ((src_p = fopen(src, "rb")) == NULL) {
		perror(src);
		exit(1);
	}

	if ((dst_p = fopen(dst, "rb")) == NULL) {
		perror(dst);
		exit(1);
	}

	while (size > 0) {
		to_read = size > sizeof(copy_buf) ? sizeof(copy_buf) : size;
		if (fread(copy_buf, to_read, 1, src_p) != 1) {
			perror(src);
			exit(1);
		}

		if (fread(diff_buf, to_read, 1, dst_p) != 1) {
			perror(dst);
			exit(1);
		}

		if (memcmp(copy_buf, diff_buf, to_read)) {
			ret = 1;
			break;
		}

		size -= to_read;
	}

	fclose(dst_p);
	fclose(src_p);

	return ret;
}

/**
 * Copy file from src to dst.
 *
 * @param src Path.
 * @param dst Path
 * @param size Filesize.
 */
static void copy_file(char *src, char *dst, size_t size) {
	char *p = path_get();
	FILE *src_p, *dst_p;
	struct stat buf;
	size_t to_read;

#ifdef WIN32
	if (stat(dst, &buf) == 0) {
#else
	if (lstat(dst, &buf) == 0) {
#endif
		if (!S_ISREG(buf.st_mode)) {
			fprintf(stderr, "Will not copy %s as it is a link in %s\n", p, d_path);
			return;
		}
	}

	printf("%c   %s%c%s\n", file_status, p, SEPARATOR_CHAR, src);

	if (dry_run) {
		return;
	}

	if ((src_p = fopen(src, "rb")) == NULL) {
		perror(src);
		exit(1);
	}

	if ((dst_p = fopen(dst, "wb")) == NULL) {
		perror(dst);
		exit(1);
	}

	while (size > 0) {
		to_read = size > sizeof(copy_buf) ? sizeof(copy_buf) : size;
		if (fread(copy_buf, to_read, 1, src_p) != 1) {
			perror(src);
			exit(1);
		}

		if (fwrite(copy_buf, to_read, 1, dst_p) != 1) {
			perror(dst);
			exit(1);
		}

		size -= to_read;
	}

	fclose(dst_p);
	fclose(src_p);
}
