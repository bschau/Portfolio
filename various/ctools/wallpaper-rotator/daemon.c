#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void daemonize(void)
{
	struct rlimit resource_limit = { 0 };
	int fd = -1, status;

	status = fork();
	if (status == -1) {
		perror("cannot fork - first");
		exit(1);
	} else if (status != 0) {
		exit(0);
	}

	resource_limit.rlim_max = 0;
	if ((status = getrlimit(RLIMIT_NOFILE, &resource_limit)) == -1) {
		perror("getrlimit()");
		exit(1);
	}

	if (resource_limit.rlim_max == 0) {
		fprintf(stderr, "max number of open file descriptors is 0!\n");
		exit(1);
	}

	for (status = 0; status < resource_limit.rlim_max; status++) {
		close(status);
	}

	if ((status = setsid()) == -1) {
		perror("setsid()");
		exit(1);
	}

	status = fork();
	if (status == -1) {
		perror("cannot fork - second");
		exit(1);
	} else if (status != 0) {
		exit(0);
	}

	if (chdir("/") != 0) {
		exit(1);
	}

	umask(0);
	if ((fd = open("/dev/null", O_RDWR)) == -1) {
		exit(1);
	}
		
	status = dup(fd);
	status = dup(fd);
}
