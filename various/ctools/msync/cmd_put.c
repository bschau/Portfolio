#include <curl/curl.h>
#include <libgen.h>
#include "common.h"
#include "include.h"

/* forward decls */
static void ftp_file(char *file);

/* globals */
static char ftp_path[PATH_MAX + 1], curl_url[1024];
static CURL *curl_h = NULL;

/**
 * Put command.
 */
void cmd_put(int argc, char *argv[], int optind) {
	get_cwd(cur_path);

	config_find();
	config_read();
	
	if (chdir(cur_path)) {
		perror(cur_path);
		exit(1);
	}

	if (path) {
		snprintf(ftp_path, sizeof(ftp_path), "%s/%s", path, cur_path + root_dir_len);
		collapse_slashes(ftp_path);

		for (; optind < argc; optind++) {
			snprintf(curl_url, sizeof(curl_url), "%s/%s", ftp_path, basename(argv[optind]));
			collapse_slashes(curl_url);

			if (copy(argv[optind], curl_url) == 0) {
				printf("%s => %s\n", argv[optind], curl_url);
			}
		}
	} else {
		if ((curl_h = curl_easy_init()) == NULL) {
			fprintf(stderr, "Cannot initialize curl\n");
			exit(1);
		}

		snprintf(ftp_path, sizeof(ftp_path), "/%s/%s", remote_dir, cur_path + root_dir_len);
		collapse_slashes(ftp_path);

		for (; optind < argc; optind++) {
			ftp_file(argv[optind]);
		}

		curl_easy_cleanup(curl_h);
	}
}

/**
 * Transfer file to server.
 *
 * @param file File to transfer.
 */
static void ftp_file(char *file) {
	struct stat s_buf;
	char *f;
	FILE *fp;
	CURLcode ec;

	if (lstat(file, &s_buf)) {
		fprintf(stderr, "No such file: %s\n", file);
		return;
	}

	if ((!S_ISREG(s_buf.st_mode)) && (!S_ISLNK(s_buf.st_mode))) {
		fprintf(stderr, "Not at regular file or a link: %s\n", file);
		return;
	}

	if (strchr(file, '/')) {
		fprintf(stderr, "Warning: %s will be transferred to %s\n", file, ftp_path);
	}

	f = basename(file);
	snprintf(curl_url, sizeof(curl_url), "ftp://%s/%s/%s", host, ftp_path, f);
	collapse_slashes(curl_url + 6);

	printf("%s => %s\n", file, curl_url);

	if ((fp = fopen(file, "rb")) == NULL) {
		perror(file);
		return;
	}

	curl_easy_setopt(curl_h, CURLOPT_READDATA, fp);
	curl_easy_setopt(curl_h, CURLOPT_URL, curl_url);
	curl_easy_setopt(curl_h, CURLOPT_USERNAME, user);
	curl_easy_setopt(curl_h, CURLOPT_PASSWORD, pass);
	curl_easy_setopt(curl_h, CURLOPT_UPLOAD, 1);
	curl_easy_setopt(curl_h, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);
	
	if ((ec = curl_easy_perform(curl_h)) > 0) {
		fprintf(stderr, "%s\n", curl_easy_strerror(ec));
	}

	fclose(fp);
}
