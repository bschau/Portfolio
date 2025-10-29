#include <linux/videodev2.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <jpeglib.h>
#include "include.h"

/* forwd decls */
static void save_image(char *fn, void *img, unsigned int len);

/* globals */
static char buffer[64];
static unsigned char *data_ptr[MAX_PICTURE_HEIGHT];
static unsigned char data_lines[MAX_PICTURE_HEIGHT * MAX_PICTURE_WIDTH * 3];
#define YUV422RGB888CLIP(x) ((x) >= 0xFF ? 0xFF : ((x) <= 0x00 ? 0x00 : (x)))

/**
 * Read a frame using mmap.
 *
 * @param fn File name (if single grab) or NULL.
 * @return 0 if EAGAIN encountered, !0 otherwise.
 */
int fr_mmap(char *fn) {
	struct v4l2_buffer buf;

	memset(&buf, 0, sizeof(struct v4l2_buffer));

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	if (x_ioctl(wc_fd, VIDIOC_DQBUF, &buf) == -1) {
		if (errno == EAGAIN) {
			syslog(LOG_ERR, "mmap EAGAIN");
			return 0;
		}

		syslog(LOG_ERR, "VIDIOC_DQBUF in fr_mmap failed (%s)", strerror(errno));
		exit(1);
	}

	save_image(fn, buffers[buf.index].start, buffers[buf.index].length);

	if (x_ioctl(wc_fd, VIDIOC_QBUF, &buf) == -1) {
		syslog(LOG_ERR, "VIDIOC_QBUF in fr_mmap failed (%s)", strerror(errno));
		exit(1);
	}

	return 1;
}

/**
 * Read a frame using read.
 *
 * @param fn File name (if single grab) or NULL.
 * @return 0 if EAGAIN encountered, !0 otherwise.
 */
int fr_read(char *fn) {
	if (read(wc_fd, buffers[0].start, buffers[0].length) == -1) {
		if (errno == EAGAIN) {
			return 0;
		}

		syslog(LOG_ERR, "Read error (%s)", strerror(errno));
	} else {
		save_image(fn, buffers[0].start, buffers[0].length);
	}

	return 1;
}

/**
 * Read a frame using userp.
 *
 * @param fn File name (if single grab) or NULL.
 * @return 0 if EAGAIN encountered, !0 otherwise.
 */
int fr_userp(char *fn) {
	struct v4l2_buffer buf;
	int idx;

	memset(&buf, 0, sizeof(struct v4l2_buffer));

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_USERPTR;

	if (x_ioctl(wc_fd, VIDIOC_DQBUF, &buf) == -1) {
		if (errno == EAGAIN) {
			return 0;
		}

		syslog(LOG_ERR, "VIDIOC_DQBUF in fr_userp failed (%s)", strerror(errno));
		exit(1);
	}

	for (idx = 0; idx < USERP_BUFFERS; idx++) {
		if ((buf.m.userptr == (unsigned long) buffers[idx].start) &&
		    (buf.length == buffers[idx].length))
			break;
	}

	if (idx < USERP_BUFFERS) {
		save_image(fn, (void *) buf.m.userptr, buf.length);
	}
	
	if (x_ioctl(wc_fd, VIDIOC_QBUF, &buf) == -1) {
		syslog(LOG_ERR, "VIDIOC_QBUF in fr_userp failed (%s)", strerror(errno));
		exit(1);
	}

	return 1;
}

/**
 * Save the image.
 *
 * @param fn File name (if single grab) or NULL.
 * @param img Image data.
 * @param len Length.
 */
static void save_image(char *fn, void *img, unsigned int len) {
	unsigned char *py = img, *pu = img + 1, *pv = img + 3, *tmp;
	struct timeb tb;
	FILE *fh;
	struct jpeg_compress_struct c_info;
	struct jpeg_error_mgr j_err;
	int row, col;

	if (fn == NULL) {
		ftime(&tb);
		snprintf(buffer, sizeof(buffer), "%li-%.4i.jpg", (long int) tb.time, tb.millitm);

		fh = fopen(buffer, "w");
	} else {
		if (*fn == '-') {
			return;
		}

		fh = fopen(fn, "w");
	}

	if (fh == NULL) {
		syslog(LOG_ERR, "cannot create %s (%s)", (fn == NULL) ? buffer : fn, strerror(errno));
		exit(1);
	}

	c_info.err = jpeg_std_error(&j_err);
	jpeg_create_compress(&c_info);
	jpeg_stdio_dest(&c_info, fh);

	c_info.input_components = 3;
	c_info.in_color_space = JCS_RGB;
	c_info.image_width = pic_w;
	c_info.image_height = pic_h;
	c_info.smoothing_factor = 0;
	c_info.optimize_coding = optimize;

	jpeg_set_defaults(&c_info);
	jpeg_set_quality(&c_info, quality, TRUE);
	jpeg_start_compress(&c_info, TRUE);

	/* Conversion from YUV422 to RGB888, see:
	 *
	 * http://en.wikipedia.org/wiki/YUV
	 *
	 * This converter uses the ITU-R-INT algorithm.
	 */
	tmp = data_lines;
	for (row = 0; row < pic_h; row++) {
		data_ptr[row] = tmp;
		for (col = 0; col < pic_w; col++) {
			*tmp++ = YUV422RGB888CLIP(*py + (*pv - 128) + ((*pv - 128) >> 2) + ((*pv - 128) >> 3) + ((*pv - 128) >> 5));
			*tmp++ = YUV422RGB888CLIP(*py - (((*pu - 128) >> 2) + ((*pu - 128) >> 4) + ((*pu - 128) >> 5)) - (((*pv - 128) >> 1) + ((*pv - 128) >> 3) + ((*pv - 128) >> 4) + ((*pv - 128) >> 5)));
			*tmp++ = YUV422RGB888CLIP(*py + (*pu - 128) + ((*pu - 128) >> 1) + ((*pu - 128) >> 2) + ((*pu - 128) >> 6));

#if 0
			// This is the ITU_R_FLOAT conversion
			*tmp++ = YUV422RGB888CLIP((double) *py + 1.402 * ((double) *pv - 128.0));
			*tmp++ = YUV422RGB888CLIP((double) *py - 0.344 * ((double) *pu - 128.0) - 0.714 * ((double) *pv - 128.0));      
			*tmp++ = YUV422RGB888CLIP((double) *py + 1.772 * ((double) *pu - 128.0));
#endif

			py += 2;

			if ((col & 1) == 1) {
				pu += 4;
				pv += 4;
			}
		}

	}

	jpeg_write_scanlines(&c_info, data_ptr, pic_h);
	jpeg_finish_compress(&c_info);
	jpeg_destroy_compress(&c_info);
	fclose(fh);
}
