#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include "common.h"
#include "include.h"

/* macros */
#define LINE_LEN 1024
#define BUFFER_LEN (1024 * 1024)

/* globals */
static char buffer[BUFFER_LEN];
extern int errno;

/**
 * Read configuration file.
 */
void config_read(void) {
	FILE *fp = fopen(RCFILE, "rb");
	int line = 0;
	char buffer[LINE_LEN + 2];
	char *s, *token, *value;
	char tc;

	if (fp == NULL) {
		perror(RCFILE);
		exit(1);
	}

	while ((s = xgets(buffer, LINE_LEN, fp)) != NULL) {
		line++;
		if ((!*s) || (*s == '#')) {
			continue;
		}

		token = s;
		for (; (*s) && (isalnum((int) *s)); s++);
		if (!*s) {
			fprintf(stderr, "Token '%s' with no value on line %i\n", token, line);
			exit(1);
		}

		*s++ = 0;
		for (; (*s) && (isspace((int) *s)); s++);
		if (!*s) {
			fprintf(stderr, "Token '%s' with no value on line %i\n", token, line);
			exit(1);
		}

		if ((*s == '\'') || (*s == '"')) {
			tc = *s++;
		} else {
			tc = 0;
		}

		value = s;
		for (; *s; s++) {
			if (tc) {
				if (*s == tc) {
					break;
				}
			} else {
				if (isspace((int) *s)) {
					break;
				}
			}
		}

		*s = 0;
		if (strcmp(token, "remote") == 0) {
			remote_dir = xstrdup(value);
		} else if (strcmp(token, "host") == 0) {
			host = xstrdup(value);
		} else if (strcmp(token, "user") == 0) {
			user = xstrdup(value);
		} else if (strcmp(token, "pass") == 0) {
			pass = xstrdup(value);
		} else if (strcmp(token, "path") == 0) {
			path = xstrdup(value);
		} else {
			fprintf(stderr, "Unknown token '%s' on line %i\n", token, line);
			exit(1);
		}
	}

	fclose(fp);

	stamp = 0;
	if ((fp = fopen(STAMPFILE, "rb")) != NULL) {
		if ((s = xgets(buffer, LINE_LEN, fp)) != NULL) {
			stamp = (time_t) strtoul(s, NULL, 10);
		}

		fclose(fp);
	}
}

/**
 * Find root config folder.
 */
void config_find(void) {
	char path1[PATH_MAX + 1], path2[PATH_MAX + 1];
	struct stat s_buf;
	char *s1, *s2;

	/* find root */
	while (stat(RCFILE, &s_buf)) {
		s1 = getcwd(path1, PATH_MAX);
		if (chdir("..") != 0) {
			perror("..");
			exit(1);
		}

		s2 = getcwd(path2, PATH_MAX);
		if (strcmp(s1, s2) == 0) {
			fprintf(stderr, "Cannot find root .MSYNC\n");
			exit(1);
		}
	}

	if (getcwd(root_dir, PATH_MAX) == NULL) {
		fprintf(stderr, "Cannot get root directory\n");
		exit(1);
	}

	root_dir_len = strlen(root_dir);
}

/**
 * Write stamp file.
 */
void config_write_stamp(void) {
	time_t now = time(NULL);
	char p[PATH_MAX + 1];
	FILE *fp;

	snprintf(p, PATH_MAX, "%s/%s", root_dir, STAMPFILE);
	exit_if_link(p);

	if ((fp = fopen(p, "wb")) == NULL) {
		fprintf(stderr, "Cannot write to stamp file\n");
		exit(1);
	}

	fprintf(fp, "%i", (int) now);
	fclose(fp);
}

/**
 * Get and trim a line from fp.
 *
 * @param dst Where to store line.
 * @param len Dst len.
 * @param fp File pointer.
 * @return start of trimmed buffer or NULL (eof).
 */
char *xgets(char *dst, int len, FILE *fp) {
	char *s = fgets(dst, len, fp);

	if (s) {
		char *start;

		for (; (*s) && (isspace((int) *s)); s++);
		if (!*s) {
			return s;
		}

		start = s;
		s += strlen(s) - 1;
		while (s >= start) {
			if (isspace((int) *s)) {
				*s = 0;
			} else {
				break;
			}

			s--;
		}

		return start;
	}

	return NULL;
}

/**
 * Set permissions on file.
 *
 * @param path Path to file.
 * @param mode mode.
 */
void set_file_permissions(char *path, int mode) {
	if (chmod(path, mode)) {
		fprintf(stderr, "Warning: cannot set permissions on %s to %#o\n", path, mode);
	}
}

/**
 * Get current working directory.
 *
 * @param dst Store path in a pre-allocated buffer of PATH_MAX chars.
 */
void get_cwd(char *dst) {
	if (getcwd(dst, PATH_MAX) == NULL) {
		fprintf(stderr, "where am I?\n");
		exit(1);
	}
}

/**
 * @return -1 if tree is unlocked, uid of locker if locked.
 */
int is_locked(void) {
	char p[PATH_MAX + 1];
	char *s;
	FILE *fp;
	struct stat s_buf;

	snprintf(p, PATH_MAX, "%s/%s", root_dir, LOCKFILE);
	if (stat(p, &s_buf)) {
		return -1;
	}

	exit_if_link(p);

	if ((fp = fopen(p, "rb")) == NULL) {
		fprintf(stderr, "Cannot open lockfile\n");
		exit(1);
	}

	s = fgets(p, PATH_MAX, fp);
	fclose(fp);

	if (s) {
		return atoi(s);
	}

	return 0;
}

/**
 * Exit with error if file is a link.
 *
 * @param p Path.
 */
void exit_if_link(char *p) {
	struct stat s_buf;

	if (lstat(p, &s_buf) == 0) {
		if (S_ISLNK(s_buf.st_mode)) {
			fprintf(stderr, "%s is a symbolic link\n", p);
			fprintf(stderr, "Please remove the link or turn it into a regular file\n");
			exit(1);
		}
	}
}

/**
 * Collapse slashes into one.
 *
 * @param src Source/dst string.
 */
void collapse_slashes(char *src) {
	char *dst = src;
	int first = 0;

	while (*src) {
		if (*src == '/') {
			if (!first) {
				*dst++ = '/';
				first = 1;
			}
		} else {
			first = 0;
			*dst++ = *src;
		}

		src++;
	}

	*dst = 0;
}

/**
 * Copy a file from src to dst.
 *
 * @param src File to copy.
 * @param dst Where to copy to.
 * @return 0 if succesfull, !0 otherwise.
 */
int copy(char *src, char *dst) {
	int i_fd = -1, o_fd = -1, err = 0;
	struct stat s_buf;
	off_t size, r, a;

	if (lstat(src, &s_buf) != 0) {
		fprintf(stderr, "%s does not exist\n", src);
		return 1;
	}

	if (!S_ISREG(s_buf.st_mode)) {
		fprintf(stderr, "%s is not a regular file\n", src);
		return 1;
	}

	size = s_buf.st_size;
	if (lstat(dst, &s_buf) == 0) {
		if (!S_ISREG(s_buf.st_mode)) {
			fprintf(stderr, "%s exists but is not a regular file\n", dst);
			return 1;
		}
	}

	if ((i_fd = open(src, O_RDONLY)) == -1) {
		perror(src);
		return 1;
	}

	if ((o_fd = open(dst, O_WRONLY|O_CREAT|O_TRUNC, 0644)) == -1) {
		perror(dst);
		close(i_fd);
		return 1;
	}

	while (size > 0) {
		errno = 0;
		r = size <= BUFFER_LEN ? size : BUFFER_LEN;
		if ((r = read(i_fd, buffer, r)) > 0) {
			size -= r;
			while (r > 0) {
				if ((a = write(o_fd, buffer, r)) == -1) {
					if (errno != EAGAIN) {
						perror(dst);
						close(o_fd);
						close(i_fd);
						return 1;
					}
				} else {
					r -= a;
				}
			}
		} else if (r == 0) {
			break;
		} else {
			if (errno != EAGAIN) {
				err = 1;
				perror(src);
				break;
			}
		}
	}
		
	close(o_fd);
	close(i_fd);

	return err;
}
