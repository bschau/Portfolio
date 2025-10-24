#include <proc/readproc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include "wallpaper-rotator.h"

static void usage(int exit_code);
static int am_i_the_only_one(void);

int main(int argc, char* const argv[])
{
	int rotation_speed = 1800, opt;
	char *picture_path = NULL;

	while ((opt = getopt(argc, argv, "hr:")) != -1) {
		switch (opt) {
			case 'h':
				usage(0);

			case 'r':
				rotation_speed = atoi(optarg);
				break;

			default:
				fprintf(stderr, "Unknown argument '%c'\n", opt);
				exit(1);
		}
	}

	if (argc - optind < 1) {
		fprintf(stderr, "must specify picture path\n");
		usage(1);
	}

	picture_path = argv[optind];

	daemonize();
	openlog("wallpaper-rotator", LOG_PERROR|LOG_CONS|LOG_PID, LOG_USER);
	if (!am_i_the_only_one()) {
		syslog(LOG_ERR, "Only one instance of wallpaper-rotator per user is allowed");
		exit(1);
	}

	rotate(picture_path, rotation_speed);
	closelog();
	exit(0);
}

static void usage(int exit_code)
{
	FILE *out = exit_code == 0 ? stdout : stderr;

	fprintf(out, "wallpaper-rotator v0.1\n");
	fprintf(out, "\nUsage: wallpaper-rotator [OPTIONS] picture-path\n\n");
	fprintf(out, "[OPTIONS]\n");
	fprintf(out, "  -r rotation         Delay between rotation in seconds\n");
	exit(exit_code);
}

static int am_i_the_only_one(void)
{
	int my_uid = getuid();
	int my_pid = getpid();
	proc_t **procs;
	int idx;

	if ((procs = readproctab(PROC_FILLCOM | PROC_FILLSTAT | PROC_FILLSTATUS)) == NULL) {
		syslog(LOG_ERR, "Failed to open /proc");
		exit(1);
	}

	for (idx = 0; procs[idx]; idx++) {
		if (!procs[idx]->cmdline || procs[idx]->euid != my_uid || procs[idx]->tid == my_pid)  {
			continue;
		}

		if (strstr(procs[idx]->cmdline[0], "wallpaper-rotator")) {
			return 0;
		}
	}

	free(procs);
	return 1;
}
