#include <sys/stat.h>
#include "include.h"

/** forward decls */
static void usage(void);

/**
 * Local usage
 */
static void usage(void) {
	fprintf(stderr, "init ftp://user:pass@host/dir\n");
	fprintf(stderr, " or  file:///path/to/dest\n");
	exit(1);
}

/**
 * Init command.
 */
void cmd_init(int argc, char *argv[], int optind) {
	struct stat s_buf;
	FILE *fp;
	char *s, *t;

	if ((argc - optind) < 1) {
		usage();
	}

	if (stat(RCFILE, &s_buf) == 0) {
		fprintf(stderr, "This root is already initialized\n");
		exit(1);
	}

	if (mkdir(".MSYNC", 0700)) {
		perror(".MSYNC");
		exit(1);
	}

	if ((fp = fopen(RCFILE, "wb")) == NULL) {
		perror(RCFILE);
		exit(1);
	}

	if (strncmp(argv[optind], "ftp://", 6) == 0) {
		if ((s = strchr(argv[optind] + 6, ':')) == NULL) {
			fprintf(stderr, "Argument must be in the form user:pass@host/dir\n");
			exit(1);
		}
		*s++ = 0;
		fprintf(fp, "user %s\n", argv[optind] + 6);
		t = s;

		if ((s = strchr(s, '@')) == NULL) {
			fprintf(stderr, "Argument must be in the form user:pass@host/dir\n");
			exit(1);
		}
		*s++ = 0;
		fprintf(fp, "pass %s\n", t);
		t = s;

		if ((s = strchr(s, '/')) == NULL) {
			fprintf(stderr, "Argument must be in the form user:pass@host/dir\n");
			exit(1);
		}
		*s++ = 0;
		fprintf(fp, "host %s\n", t);

		if (!*s) {
			fprintf(stderr, "Argument must be in the form user:pass@host/dir\n");
			exit(1);
		}
		fprintf(fp, "remote %s\n", s);
	} else if (strncmp(argv[optind], "file://", 7) == 0) {
		s = argv[optind] + 7;
		if (lstat(s, &s_buf) != 0) {
			fprintf(stderr, "%s does not exist\n", s);
			exit(1);
		}

		if (!S_ISDIR(s_buf.st_mode)) {
			fprintf(stderr, "%s is not a directory\n", s);
			exit(1);
		}

		fprintf(fp, "path %s\n", s);
	} else {
		usage();
	}

	fclose(fp);

	set_file_permissions(RCFILE, 0600);

	config_read();

	if ((fp = fopen(STAMPFILE, "wb")) == NULL) {
		perror(STAMPFILE);
		exit(1);
	}

	fprintf(fp, "0\n");
	fclose(fp);

	set_file_permissions(STAMPFILE, 0600);
}
