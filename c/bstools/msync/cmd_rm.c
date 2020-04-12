#include <curl/curl.h>
#include <libgen.h>
#include "common.h"
#include "include.h"

/* forward decls */
static size_t silent_writer(void *ptr, size_t size, size_t nmemb, void *userdata);

/* globals */
static char ftp_path[PATH_MAX + 1], curl_url[1024];
static CURL *curl_h = NULL;

/**
 * Rm command.
 */
void cmd_rm(int argc, char *argv[], int optind) {
	get_cwd(cur_path);

	config_find();
	config_read();
	
	if (chdir(cur_path)) {
		perror(cur_path);
		exit(1);
	}

	if (path) {
		struct stat s_buf;

		for (; optind < argc; optind++) {
			snprintf(ftp_path, sizeof(ftp_path), "%s/%s/%s", path, cur_path + root_dir_len, argv[optind]);
			collapse_slashes(ftp_path);

			if (lstat(ftp_path, &s_buf) == 0) {
				if (unlink(ftp_path)) {
					perror(ftp_path);
				}
			}
		}
	} else {
		struct curl_slist *sl = NULL;
		CURLcode ec;
		char *f, *p;
		int len;

		snprintf(ftp_path, sizeof(ftp_path), "/%s/%s", remote_dir, cur_path + root_dir_len);
		collapse_slashes(ftp_path);

		if ((curl_h = curl_easy_init()) == NULL) {
			fprintf(stderr, "Cannot initialize curl\n");
			exit(1);
		}

		snprintf(curl_url, sizeof(curl_url), "ftp://%s/%s", host, ftp_path);
		collapse_slashes(curl_url + 6);

		for (; optind < argc; optind++) {
			f = basename(argv[optind]);
			len = strlen(f);
			if ((p = malloc(len + 6)) == NULL) {
				fprintf(stderr, "OOM - Failed to allocate buffer\n");
				exit(1);
			}

			sprintf(p, "dele %s", f);

			sl = curl_slist_append(sl, p);

			printf("%s\n", p);
		}

		curl_easy_setopt(curl_h, CURLOPT_WRITEFUNCTION, silent_writer);
		curl_easy_setopt(curl_h, CURLOPT_POSTQUOTE, sl);
		curl_easy_setopt(curl_h, CURLOPT_URL, curl_url);
		curl_easy_setopt(curl_h, CURLOPT_USERNAME, user);
		curl_easy_setopt(curl_h, CURLOPT_PASSWORD, pass);

		ec = curl_easy_perform(curl_h);
		curl_slist_free_all(sl);
		curl_easy_cleanup(curl_h);
	
		if (ec) {
			fprintf(stderr, "%s\n", curl_easy_strerror(ec));
		}
	}
}

/**
 * Discard data.
 */
static size_t silent_writer(void *ptr, size_t size, size_t nmemb, void *userdata) {
	return (size * nmemb);
}
