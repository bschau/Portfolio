#ifndef INCLUDE_H
#define INCLUDE_H

#include <sys/types.h>

/* macros */
#define EVER ;;
#define MMAP_BUFFERS (4)
#define USERP_BUFFERS (4)
#define MAX_PICTURE_WIDTH (600)
#define MAX_PICTURE_HEIGHT (800)

/* struct */
struct buffer {
	void *start;
	size_t length;
};

/* devinit.c */
int device_get_dimension_width(int idx);
int device_get_dimension_height(int idx);
void device_init(void);
void device_uninit(void);

/* framereaders.c */
int fr_mmap(char *fn);
int fr_read(char *fn);
int fr_userp(char *fn);

/* utils.c */
int x_ioctl(int fd, int req, void *arg);

/* globals */
#ifdef MAIN
	int quality = 70, optimize = 0, fps = 15, dimension = 0;
	char *dst_dir = "/var/grab", *log_level = "LOG_LOCAL0";
	char *pid_file = "/var/run/grab.pid", *device = "/dev/video0";
	char *grab_type = "mmap";
	int wc_fd = -1, pic_w, pic_h;
	struct buffer *buffers = NULL;
	int (*read_frame)(char *) = NULL;
#else
	extern int quality, optimize, fps, dimension, wc_fd;
	extern char *dst_dir, *log_level, *pid_file, *device, *grab_type;
	extern struct buffer *buffers;
	extern int (*read_frame)(char *);
	extern int pic_w, pic_h;
#endif

#endif
