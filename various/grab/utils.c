#include <sys/ioctl.h>
#include <errno.h>
#include "include.h"

/**
 * Wrapper around ioctl.
 */
int x_ioctl(int fd, int req, void *arg) {
	int r;

	do {
		r = ioctl(fd, req, arg);
	} while ((r == -1) && (errno == EINTR));

	return r;
}
