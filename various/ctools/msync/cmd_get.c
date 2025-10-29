#include <curl/curl.h>
#include <libgen.h>
#include "common.h"
#include "include.h"

/* forward decls */
static void get_file(char *file);

/* globals */
static char ftp_path[PATH_MAX + 1], curl_url[1024];
static CURL *curl_h = NULL;

/**
 * Get command.
 */
void cmd_get(int argc, char *argv[], int optind) {
	get_cwd(cur_path);

	config_find();
	config_read();
	
	if (chdir(cur_path)) {
		perror(cur_path);
		exit(1);
	}

	if (path) {
		char *s;

		snprintf(ftp_path, sizeof(ftp_path), "%s/%s", path, cur_path + root_dir_len);
		collapse_slashes(ftp_path);

		for (; optind < argc; optind++) {
			s = basename(argv[optind]);
			snprintf(curl_url, sizeof(curl_url), "%s/%s", ftp_path, s);
			collapse_slashes(curl_url);

			if (copy(curl_url, s) == 0) {
				printf("%s => %s\n", curl_url, s);
			}
		}
	} else {
		snprintf(ftp_path, sizeof(ftp_path), "/%s/%s", remote_dir, cur_path + root_dir_len);
		collapse_slashes(ftp_path);

		if ((curl_h = curl_easy_init()) == NULL) {
			fprintf(stderr, "Cannot initialize curl\n");
			exit(1);
		}

		for (; optind < argc; optind++) {
			get_file(argv[optind]);
		}

		curl_easy_cleanup(curl_h);
	}
}

/**
 * Transfer file from server.
 *
 * @param file File to transfer.
 */
static void get_file(char *file) {
	struct stat s_buf;
	char *f;
	FILE *fp;
	CURLcode ec;

	f = basename(file);
	if (lstat(f, &s_buf) == 0) {
		if (!S_ISREG(s_buf.st_mode)) {
			fprintf(stderr, "Will not overwrite a non-regular file: %s\n", f);
			return;
		}
	}

	snprintf(curl_url, sizeof(curl_url), "ftp://%s/%s/%s", host, ftp_path, file);
	collapse_slashes(curl_url + 6);

	printf("%s => %s\n", curl_url, f);

	if ((fp = fopen(f, "wb")) == NULL) {
		perror(f);
		return;
	}

	curl_easy_setopt(curl_h, CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(curl_h, CURLOPT_URL, curl_url);
	curl_easy_setopt(curl_h, CURLOPT_USERNAME, user);
	curl_easy_setopt(curl_h, CURLOPT_PASSWORD, pass);

	ec = curl_easy_perform(curl_h);
	fclose(fp);
	if (ec) {
		fprintf(stderr, "%s\n", curl_easy_strerror(ec));
		unlink(f);
	}
}
