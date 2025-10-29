#include <sys/resource.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#define MAIN
#include "include.h"

/** forward decls */
static void daemonize(void);
static void write_pid(void);
static void remove_pid(void);
static void exit_handler(void);
static void sigterm_handler(int signum);
static void usage(int ec);

/** globals */
static char *program_name = "grab";
static int my_pid = 0, run_in_background = 1;
static int in_loop = 1;
extern int errno;
static struct {
	char *name;
	int level;
} levels[] = {
	{ "LOG_DAEMON", LOG_DAEMON },
	{ "LOG_LOCAL0", LOG_LOCAL0 },
	{ "LOG_LOCAL1", LOG_LOCAL1 },
	{ "LOG_LOCAL2", LOG_LOCAL2 },
	{ "LOG_LOCAL3", LOG_LOCAL3 },
	{ "LOG_LOCAL4", LOG_LOCAL4 },
	{ "LOG_LOCAL5", LOG_LOCAL5 },
	{ "LOG_LOCAL6", LOG_LOCAL6 },
	{ "LOG_LOCAL7", LOG_LOCAL7 },
	{ "LOG_USER", LOG_USER },
	{ NULL, 0 }
};

/**
 * Go into background.
 */
static void daemonize(void) {
	struct rlimit resource_limit = { 0 };
	int file_desc = -1, status;
	struct stat s_buf;

	if (stat(pid_file, &s_buf) == 0) {
		fprintf(stderr, "Another instance of grab seems to be running\n");
		exit(1);
	}

	switch (fork()) {
	case -1:
		perror("fork() - first");
		exit(1);
	case 0:
		break;
	default:
		exit(0);
	}

	resource_limit.rlim_max = 0;
	if ((status = getrlimit(RLIMIT_NOFILE, &resource_limit)) == -1) {
		perror("Warning: getrlimit()");
	} else {
		if (resource_limit.rlim_max == 0) {
			fprintf(stderr, "Warning: rlim_max == 0\n");
		} else {
			for (status = 0; status < resource_limit.rlim_max; status++) {
				close(status);
			}
		}
	}

	if (setsid() == -1) {
		perror("Warning: setsid() failed");
	}

	switch (fork()) {
	case -1:
		perror("fork() - second");
		exit(1);
	case 0:	
		break;
	default:
		exit(0);
	}

	if (chdir("/") != 0) {
 		perror("Warning: / not found");
	}

	umask(0);

	if ((file_desc = open("/dev/null", O_RDWR)) == -1) {
		perror("Warning: no /dev/null");
	} else {
		status = dup(file_desc);		/* stdout */
		status = dup(file_desc);		/* stderr */
	}
}

/**
 * Write the PID file.
 */
static void write_pid(void) {
	char buffer[32];
	int fd, len;

	if (run_in_background) {
		if ((fd = open(pid_file, O_WRONLY|O_CREAT|O_EXCL, 0600)) == -1) {
			syslog(LOG_ERR, "cannot create .pid file");
			exit(1);
		}
	
		my_pid = 1;

		snprintf(buffer, sizeof(buffer), "%li\n", (long int) getpid());
		len = strlen(buffer);
		if (write(fd, buffer, len) != len) {
			syslog(LOG_ERR, "cannot write .pid file");
			close(fd);
			exit(1);
		}

		close(fd);
	}
}

/**
 * Remove the PID file.
 */
static void remove_pid(void) {
	if (run_in_background) {
		if (my_pid) {
			if (unlink(pid_file)) {
				syslog(LOG_INFO, "cannot remove .pid file");
			} else {
				my_pid = 0;
			}
		}
	}
}

/**
 * Exit handler.
 */
static void exit_handler(void) {
	device_uninit();
	remove_pid();
	closelog();
}

/**
 * Signal handler (only for SIGTERM).
 *
 * @param signum Signal sent.
 */
static void sigterm_handler(int signum) {
	in_loop = 0;
	exit(0);
}

/**
 * Usage.
 */
static void usage(int ec) {
	FILE *f = (ec) ? stderr : stdout;
	int i, j;
	char pre;

	fprintf(f, "Usage: grab [OPTIONS]\n\n");
	fprintf(f, "[OPTIONS]\n");
	fprintf(f, "  -D           Dimension (%i) - one of:", dimension);
	i = 0;
	for (EVER) {
		if ((j = device_get_dimension_width(i)) == -1) {
			break;
		}

		fprintf(f, "\n               %i: %ix%i", i, j, device_get_dimension_height(i));
		i++;
	}
	fprintf(f, "\n");

	fprintf(f, "  -d device    Webcam device (%s)\n", device);
	fprintf(f, "  -f rate      FPS rate (%i)\n", fps);
	fprintf(f, "  -h           Help (this page)\n");
	fprintf(f, "  -l level     Log level (%s) - one of:", log_level);

	pre = ' ';
	for (i = 0; levels[i].name; i++) {
		if ((i % 4) == 0) {
			fprintf(f, "\n             ");
			pre = ' ';
		} else {
			pre = ',';
		}

		fprintf(f, "%c %s", pre, levels[i].name);
	}

	fprintf(f, "\n");

	fprintf(f, "  -O           Optimize output jpegs (%s)\n", (optimize ? "yes" : "no"));
	fprintf(f, "  -o output    Destination directory (%s)\n", dst_dir);
	fprintf(f, "  -p file      PID file (%s)\n", pid_file);
	fprintf(f, "  -q quality   JPEG quality (%i)\n", quality);
	fprintf(f, "  -s file      Single grab to file (no)\n");
	fprintf(f, "  -t type      How to grab (%s) - one of:\n", grab_type);
	fprintf(f, "               mmap, read, userp\n");
	fprintf(f, "  -X           Stay in foreground\n");

	exit(ec);
}

/**
 * Main
 */
int main(int argc, char *argv[]) {
	char *single_grab = NULL;
	int lvl = LOG_LOCAL0, i = 0, opt;
	int logopt = LOG_PID;
	struct sigaction sigact;

	for (EVER) {
		if ((opt = device_get_dimension_width(i)) == -1) {
			break;
		}

		if (opt == 640) {
			dimension = i;
			break;
		}

		i++;
	}

	while ((opt = getopt(argc, argv, "D:d:f:hl:Oo:p:q:s:t:X")) != -1) {
		switch (opt) {
			case 'D':
				dimension = atoi(optarg);
				break;

			case 'd':
				device = optarg;
				break;

			case 'f':
				fps = atoi(optarg);
				break;

			case 'h':
				usage(0);

			case 'l':
				log_level = optarg;
				break;

			case 'O':
				optimize = 1;
				break;

			case 'o':
				dst_dir = optarg;
				break;

			case 'p':
				pid_file = optarg;
				break;

			case 'q':
				quality = atoi(optarg);
				break;

			case 's':
				single_grab = optarg;
				run_in_background = 0;
				break;

			case 't':
				if ((strcasecmp(optarg, "mmap") == 0) ||
				    (strcasecmp(optarg, "read") == 0) ||
				    (strcasecmp(optarg, "userp") == 0)) {
					grab_type = optarg;
				} else {
					fprintf(stderr, "Unknown grab type %s\n", optarg);
					usage(1);
				}
				break;

			case 'X':
				run_in_background = 0;
				logopt |= (LOG_CONS | LOG_PERROR);
				break;

			default:
				fprintf(stderr, "Unknown option %c or no value to option given\n", (char) opt);
				usage(1);
		}
	}

	if ((quality < 1) || (quality > 100)) {
		fprintf(stderr, "Warning: jpeg-quality not within allowed range (1-100).\nWarning: Setting jpeg-quality to 75%%.\n");
		quality = 60;
	}

	for (opt = 0; levels[opt].name; opt++) {
		if (strcasecmp(levels[opt].name, log_level) == 0) {
			lvl = levels[opt].level;
			break;
		}
	}

	if (run_in_background) {
		daemonize();
	}

	openlog(program_name, logopt, lvl);

	if (atexit(exit_handler) != 0) {
		syslog(LOG_ERR, "failed to install exit_handler");
	}

	memset(&sigact, 0, sizeof(struct sigaction));
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigact.sa_handler = sigterm_handler;
	sigaction(SIGTERM, &sigact, NULL);

	write_pid();

	if (single_grab == NULL) {
		if (chdir(dst_dir)) {
			syslog(LOG_ERR, "cannot change directory to grab_dir");
			exit(1);
		}
	}

	device_init();

	/* discard first frame */
	while (in_loop) {
		if (read_frame("-")) {
			break;
		}
	}

	while (in_loop) {
		if ((read_frame(single_grab)) && (single_grab != NULL)) {
			break;
		}
	}

	remove_pid();
	exit(0);
}
