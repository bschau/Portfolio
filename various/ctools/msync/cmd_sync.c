#include <curl/curl.h>
#include <dirent.h>
#include <libgen.h>
#include <pwd.h>
#include "common.h"
#include "include.h"

/* forward decls */
static void ftp_path_add(char *p);
static void ftp_path_dirname(void);
static void file_sync(char *f_p, char *dir);
static void ftp_file(char *p);
static void copy_file(char *p);
static void mkdirs(char *p);

/* globals */
static int files_copied = 0, skip_path;
static CURL *curl_h = NULL;
static char full_path[PATH_MAX + 1], ftp_path[PATH_MAX + 1], curl_url[1024];
static int ftp_path_len, real_sync;

/**
 * Sync command.
 *
 * @param copy Copy (real sync)?  1 to copy, 0 to show.
 */
void cmd_sync(int copy) {
	int uid;

	real_sync = copy;

	config_find();
	config_read();
	ignore_list_read();

	if ((uid = is_locked()) >= 0) {
		struct passwd *pwd = getpwuid((uid_t) uid);

		real_sync = 0;

		fprintf(stderr, "Tree locked");
		if (pwd) {
			fprintf(stderr, " by %s", pwd->pw_name);
		}

		fprintf(stderr, "\n");
		exit(1);
	}

	ftp_path_len = 0;
	if (real_sync) {
		if (path) {
			ftp_path_add(path);
		} else {
			if ((curl_h = curl_easy_init()) == NULL) {
				fprintf(stderr, "Cannot initialize curl\n");
				exit(1);
			}

			ftp_path_add("/");
			ftp_path_add(remote_dir);
		}
	}


	skip_path = strlen(ftp_path) + 1;

	file_sync(".", ".");

	if (real_sync) {
		if (!path) {
			curl_easy_cleanup(curl_h);
		}

		if (files_copied > 0) {
			if (path) {
				printf("%i file%s copied\n", files_copied, (files_copied == 1 ? "" : "s"));
			} else {
				printf("%i file%s sent to remote\n", files_copied, (files_copied == 1 ? "" : "s"));
			}
			config_write_stamp();
		}
	}
}

/**
 * Add the path data to the ftp_path.
 *
 * @param p Path.
 */
static void ftp_path_add(char *p) {
	int len = strlen(p);

	if (ftp_path_len + len + 1 > sizeof(ftp_path)) {
		fprintf(stderr, "Out of memory adding to ftp_path\n");
		exit(1);
	}

	strcpy(ftp_path + ftp_path_len, p);
	ftp_path_len += len;
}

/**
 * Remove last path from ftp_path.
 */
static void ftp_path_dirname(void) {
	char *s = dirname(ftp_path);

	ftp_path_len = strlen(s);
}

/**
 * Synchronize (or query).
 *
 * @param f_p Full path.
 * @param dir This dir.
 */
static void file_sync(char *f_p, char *dir) {
	struct dirent *de;
	struct stat buf;
	DIR *dh;

	if (chdir(f_p) != 0) {
		perror(f_p);
		return;
	}

	get_cwd(cur_path);

	if ((dh = opendir(".")) == NULL) {
		perror(cur_path);
		return;
	}

	if (strcmp(dir, ".")) {
		ftp_path_add("/");
		ftp_path_add(dir);
	}

	while ((de = readdir(dh)) != NULL) {
		if ((strcmp(de->d_name, ".") == 0) ||
		    (strcmp(de->d_name, "..") == 0) ||
		    (strcmp(de->d_name, ".MSYNC") == 0)) {
			continue;
		}

		snprintf(full_path, PATH_MAX, "%s/%s", cur_path, de->d_name);
		if (lstat(full_path, &buf) == 0) {
			if (S_ISDIR(buf.st_mode)) {
				if (!ignore_list_match(full_path)) {
					file_sync(full_path, de->d_name);

					if (chdir("..") != 0) {
						fprintf(stderr, "parent directory '%s' gone\n", dirname(full_path));
						exit(1);
					}

					ftp_path_dirname();

					get_cwd(cur_path);
				}
			} else {
				if (!ignore_list_match(full_path)) {
					if ((force) || (stamp < buf.st_mtime)) {
						if (path) {
							copy_file(full_path);
						} else {
							ftp_file(full_path);
						}
					}
				}
			}
		} else {
			perror(full_path);
		}
	}

	closedir(dh);
}

/**
 * Synchronize file - ftp.
 *
 * @param p Full path.
 */
static void ftp_file(char *p) {
	char *sp = xstrdup(p);

	sp = basename(sp);

	if (real_sync) {
		FILE *fp = fopen(p, "rb");
		CURLcode ec;

		printf("%s => %s\n", sp, ftp_path);

		if (fp == NULL) {
			perror(p);
			exit(1);
		}

		curl_easy_setopt(curl_h, CURLOPT_READDATA, fp);
		snprintf(curl_url, sizeof(curl_url), "ftp://%s%s/%s", host, ftp_path, sp);
		curl_easy_setopt(curl_h, CURLOPT_URL, curl_url);
		curl_easy_setopt(curl_h, CURLOPT_USERNAME, user);
		curl_easy_setopt(curl_h, CURLOPT_PASSWORD, pass);
		curl_easy_setopt(curl_h, CURLOPT_UPLOAD, 1);
		curl_easy_setopt(curl_h, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);

		if ((ec = curl_easy_perform(curl_h)) > 0) {
			fprintf(stderr, "%s\n", curl_easy_strerror(ec));
		}

		fclose(fp);
		files_copied++;
	} else {
		printf("%s/%s\n", ftp_path + skip_path, sp);
	}
}

/**
 * Synchronize file - file.
 *
 * @param p Full path.
 */
static void copy_file(char *p) {
	char *sp = xstrdup(p), *src = xstrdup(p);

	sp = basename(sp);

	if (real_sync) {
		int left = sizeof(full_path) - 1, l;

		snprintf(full_path, sizeof(full_path), "%s", ftp_path);
		collapse_slashes(full_path);

		mkdirs(full_path);

		l = strlen(full_path);
		if (full_path[l - 1] == '/') {
			l--;
			full_path[l] = 0;
		}

		left -= l;
		if (left > 0) {
			strncat(full_path, "/", left);
			left--;
		}

		if (left > 0) {
			strncat(full_path, sp, left);
			left -= strlen(sp);
		}

		if (left < 0) {
			fprintf(stderr, "Not copied: %s - file path too long", p);
			return;
		}

		printf("%s => %s\n", sp, full_path);

		if (copy(src, full_path) == 0) {
			files_copied++;
		}
	} else {
		printf("%s/%s\n", ftp_path + skip_path, sp);
	}
}

/**
 * Make a directory structure.
 *
 * @param p Path.
 */
static void mkdirs(char *p) {
	char *d = p + 1;
	char old = ' ';
	struct stat s_buf;

	if (lstat(p, &s_buf) == 0) {
		if (S_ISDIR(s_buf.st_mode)) {
			return;
		}

		fprintf(stderr, "Not a directory: %s\n", p);
		exit(1);
	}


	while (old) {
		for (; (*d) && (*d != '/'); d++);

		old = *d;
		*d = 0;

		if (lstat(p, &s_buf) == 0) {
			if (!S_ISDIR(s_buf.st_mode)) {
				fprintf(stderr, "%s exists but is not a directory\n", p);
				exit(1);
			}
		} else {
			if (mkdir(p, 0755) != 0) {
				perror(p);
				exit(1);
			}
		}

		*d++ = old;
	}
}
