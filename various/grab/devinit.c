#include <linux/videodev2.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include "include.h"

/* forward decls */
static void init_mmap(void);
static void init_read(unsigned int size);
static void init_userp(unsigned int size);

/* globals */
/**
 * If you add a dimension with sizes larger the MAX_PICTURE_HEIGHT or
 * MAX_PICTURE_WIDTH then adjust those constants in include.h.
 */
static struct {
	int width;
	int height;
} dimensions[] = {
	{ 352, 288 },
	{ 640, 480 },
	{ 704, 576 },
	{ -1, -1 }
};

/**
 * Get width of dimension idx.
 *
 * @param idx Index.
 * @return width or -1 (index out of bounds).
 */
int device_get_dimension_width(int idx) {
	int i = 0;

	while (idx > 0) {
		if (dimensions[i].width == -1) {
			return -1;
		}
		i++;
		idx--;
	}

	return dimensions[i].width;
}

/**
 * Get height of dimension idx.
 *
 * @param idx Index.
 * @return height or -1 (index out of bounds).
 */
int device_get_dimension_height(int idx) {
	int i = 0;

	while (idx > 0) {
		if (dimensions[i].height == -1) {
			return -1;
		}
		i++;
		idx--;
	}

	return dimensions[i].height;
}

/**
 * Initialize grabber device.
 */
void device_init(void) {
	struct v4l2_capability v_cap;
	struct v4l2_cropcap v_cropcap;
	struct v4l2_crop v_crop;
	struct v4l2_format v_fmt;
	unsigned int min;

	if ((wc_fd = open(device, O_RDWR)) == -1) {
		syslog(LOG_ERR, "cannot open webcam %s (%s)", device, strerror(errno));
		exit(1);
	}

	if (x_ioctl(wc_fd, VIDIOC_QUERYCAP, &v_cap) == -1) {
		syslog(LOG_ERR, "cannot get webcam capabilities (%s)", strerror(errno));
		exit(1);
	}

	if ((v_cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) != V4L2_CAP_VIDEO_CAPTURE) {
		syslog(LOG_ERR, "%s is not a video capture device", device);
		exit(1);
	}

	/* cropping .. errors silently ignored */
	memset(&v_cropcap, 0, sizeof(struct v4l2_cropcap));

	v_cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (x_ioctl(wc_fd, VIDIOC_CROPCAP, &v_crop) == 0) {
		memset(&v_crop, 0, sizeof(struct v4l2_crop));

		v_crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		v_crop.c = v_cropcap.defrect;

		x_ioctl(wc_fd, VIDIOC_S_CROP, &v_crop);
	}

	/* format */
	pic_w = dimensions[dimension].width;
	pic_h = dimensions[dimension].height;

	memset(&v_fmt, 0, sizeof(struct v4l2_format));
	v_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v_fmt.fmt.pix.width = pic_w;
	v_fmt.fmt.pix.height = pic_h;
	v_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	v_fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

	if (x_ioctl(wc_fd, VIDIOC_S_FMT, &v_fmt) == -1) {
		syslog(LOG_ERR, "cannot set webcam output formats (%s)", strerror(errno));
		exit(1);
	}

	/* buggy driver paranoia - VIDIOC_S_FMT may change dimensions*/
	min = v_fmt.fmt.pix.width * 2;
	if (v_fmt.fmt.pix.bytesperline < min) {
		v_fmt.fmt.pix.bytesperline = min;
	}

	min = v_fmt.fmt.pix.bytesperline * v_fmt.fmt.pix.height;
	if (v_fmt.fmt.pix.sizeimage < min) {
		v_fmt.fmt.pix.sizeimage = min;
	}

	if (strcasecmp(grab_type, "mmap") == 0) {
		if ((v_cap.capabilities & V4L2_CAP_STREAMING) != V4L2_CAP_STREAMING) {
			syslog(LOG_ERR, "%s does not support mmap streaming", device);
			exit(1);
		}

		init_mmap();
	} else if (strcasecmp(grab_type, "read") == 0) {
		if ((v_cap.capabilities & V4L2_CAP_READWRITE) != V4L2_CAP_READWRITE) {
			syslog(LOG_ERR, "%s does not support read I/O", device);
			exit(1);
		}

		init_read(v_fmt.fmt.pix.sizeimage);
	} else {
		if ((v_cap.capabilities & V4L2_CAP_STREAMING) != V4L2_CAP_STREAMING) {
			syslog(LOG_ERR, "%s does not support userp streaming", device);
			exit(1);
		}

		init_userp(v_fmt.fmt.pix.sizeimage);
	}
}

/**
 * Initialize mmap grab.
 */
static void init_mmap(void) {
	struct v4l2_requestbuffers req;
	struct v4l2_buffer buf;
	enum v4l2_buf_type type;
	int idx;

	memset(&req, 0, sizeof(struct v4l2_requestbuffers));

	req.count = MMAP_BUFFERS;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (x_ioctl(wc_fd, VIDIOC_REQBUFS, &req) == -1) {
		if (errno == EINVAL) {
			syslog(LOG_ERR, "%s does not support memory mapping", device);
		} else {
			syslog(LOG_ERR, "VIDIOC_REQBUFS failed on %s (%s)", device, strerror(errno));
		}
		exit(1);
	}

	if (req.count < 2) {
		syslog(LOG_ERR, "insufficient buffer memory on %s", device);
		exit(1);
	}

	if ((buffers = calloc(req.count, sizeof(*buffers))) == NULL) {
		syslog(LOG_ERR, "out of memory allocating MMAP buffers");
		exit(1);
	}

	for (idx = 0; idx < MMAP_BUFFERS; idx++) {
		memset(&buf, 0, sizeof(struct v4l2_buffer));

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = idx;

		if (x_ioctl(wc_fd, VIDIOC_QUERYBUF, &buf) == -1) {
			syslog(LOG_ERR, "VIDIOC_QUERYBUF on %s:%i failed (%s)", device, idx, strerror(errno));
			exit(1);
		}

		buffers[idx].length = buf.length;
		if ((buffers[idx].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, wc_fd, buf.m.offset)) == MAP_FAILED) {
			syslog(LOG_ERR, "cannot MMAP on %s:%i (%s)", device, idx, strerror(errno));
			exit(1);
		}

		memset(&buf, 0, sizeof(struct v4l2_buffer));

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = idx;

		if (x_ioctl(wc_fd, VIDIOC_QBUF, &buf) == -1) {
			syslog(LOG_ERR, "VIDIOC_QBUF on %s:%i failed (%s)", device, idx, strerror(errno));
			exit(1);
		}
	}

	read_frame = fr_mmap;

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (x_ioctl(wc_fd, VIDIOC_STREAMON, &type) == -1) {
		syslog(LOG_ERR, "Cannot start streaming (%s)", strerror(errno));
		exit(1);
	}
}

/**
 * Initialize read grab.
 *
 * @param size Max image size.
 */
static void init_read(unsigned int size) {
	if ((buffers = calloc(1, sizeof(struct buffer))) == NULL) {
		syslog(LOG_ERR, "Out of memory allocating read buffer structure");
		exit(1);
	}

	buffers[0].length = size;
	if ((buffers[0].start = malloc(size)) == NULL) {
		syslog(LOG_ERR, "Out of memory allocating read buffer");
		exit(1);
	}

	read_frame = fr_read;
}

/**
 * Initialize user pointer grab.
 *
 * @param size Max image size.
 */
static void init_userp(unsigned int size) {
	struct v4l2_requestbuffers req;
	struct v4l2_buffer buf;
	enum v4l2_buf_type type;
	int idx;

	memset(&req, 0, sizeof(struct v4l2_requestbuffers));

	req.count = USERP_BUFFERS;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_USERPTR;

	if (x_ioctl(wc_fd, VIDIOC_REQBUFS, &req) == -1) {
		if (errno == EINVAL) {
			syslog(LOG_ERR, "%s does not support user pointer I/O", device);
			exit(1);
		}

		syslog(LOG_ERR, "ioctl on VIDIOC_REQBUFS failed (%s)", strerror(errno));
		exit(1);
	}

	if ((buffers = calloc(USERP_BUFFERS, sizeof(struct buffer))) == NULL) {
		syslog(LOG_ERR, "Out of memory allocating user pointer buffer structure");
		exit(1);
	}

	for (idx = 0; idx < USERP_BUFFERS; idx++) {
		buffers[idx].length = size;
		if ((buffers[idx].start = malloc(size)) == NULL) {
			syslog(LOG_ERR, "Out of memory allocating read buffer");
			exit(1);
		}

		memset(&buf, 0, sizeof(struct v4l2_buffer));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_USERPTR;
		buf.m.userptr = (unsigned long) buffers[idx].start;
		buf.length = buffers[idx].length;

		if (x_ioctl(wc_fd, VIDIOC_QBUF, &buf) == -1) {
			syslog(LOG_ERR, "VIDIOC_QBUF on %s:%i failed (%s)", device, idx, strerror(errno));
			exit(1);
		}
	}

	read_frame = fr_userp;

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (x_ioctl(wc_fd, VIDIOC_STREAMON, &type) == -1) {
		syslog(LOG_ERR, "Cannot start streaming (%s)", strerror(errno));
		exit(1);
	}
}

/**
 * Unitialize device.
 */
void device_uninit(void) {
	int idx;

	if (buffers) {
		if (strcasecmp(grab_type, "mmap") == 0) {
			for (idx = 0; idx < MMAP_BUFFERS; idx++) {
				if (buffers[idx].start) {
					munmap(buffers[idx].start, buffers[idx].length);
				}
			}
		} else if (strcasecmp(grab_type, "read") == 0) {
			if (buffers[0].start) {
				free(buffers[0].start);
			}
		} else {
			for (idx = 0; idx < USERP_BUFFERS; idx++) {
				if (buffers[idx].start) {
					free(buffers[idx].start);
				}
			}
		}

		free(buffers);
	}
}
