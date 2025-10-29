#include <curl/curl.h>
#include <libgen.h>
#include "common.h"
#include "include.h"

/* globals */
static char ftp_path[PATH_MAX + 1], curl_url[1024];
static CURL *curl_h = NULL;

/**
 * Ls command.
 */
void cmd_ls(void) {
	CURLcode ec;

	get_cwd(cur_path);

	config_find();
	config_read();
	
	if (chdir(cur_path)) {
		perror(cur_path);
		exit(1);
	}

	if (path) {
		/* I should really write a proper directory traversal routine ... */
		snprintf(ftp_path, sizeof(ftp_path), "ls -l %s/%s", path, cur_path + root_dir_len);
		collapse_slashes(ftp_path);
		if (system(ftp_path)) {
			perror("system");
		}
	} else {
		snprintf(ftp_path, sizeof(ftp_path), "/%s/%s", remote_dir, cur_path + root_dir_len);
		collapse_slashes(ftp_path);

		if ((curl_h = curl_easy_init()) == NULL) {
			fprintf(stderr, "Cannot initialize curl\n");
			exit(1);
		}

		snprintf(curl_url, sizeof(curl_url), "ftp://%s/%s", host, ftp_path);
		collapse_slashes(curl_url + 6);

		curl_easy_setopt(curl_h, CURLOPT_URL, curl_url);
		curl_easy_setopt(curl_h, CURLOPT_USERNAME, user);
		curl_easy_setopt(curl_h, CURLOPT_PASSWORD, pass);

		ec = curl_easy_perform(curl_h);
		curl_easy_cleanup(curl_h);
		if (ec) {
			fprintf(stderr, "%s\n", curl_easy_strerror(ec));
			exit(1);
		}
	}
}
