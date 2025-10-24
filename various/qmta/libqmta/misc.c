#include "private.h"

/* macros */
#define LARGEBUFFER 256

/* globals */
extern char *whoami;
extern int logfacility;

/*******************************************************************************
*
*	qlog
*
*	Logs the specified message at the specified priority to the system
*	logger.
*
*	Input:
*		pri - priority.
*		format - format string followed by arguments.
*
*	'man 3 syslog' for more ...
*/
void
qlog(int pri,
     const char *format,
     ...)
{
	va_list vargs;
	char msg[LARGEBUFFER];

	va_start(vargs, format);
	vsnprintf(msg, LARGEBUFFER-1, format, vargs);
	va_end(vargs);

	openlog(whoami, LOG_PID, logfacility);
	syslog(pri, msg);
	closelog();
}

/*******************************************************************************
*
*	nsleep
*
*	Sleep for a period of time.   If interrupted by a signal resume sleep.
*
*	Input:
*		sec - seconds to sleep.
*		nsec - nanoseconds to sleep.
*	Output:
*		slept for the whole period: 0.
*		interrupted during sleep: !0.
*/
int
nsleep(time_t sec,
       long nsec)
{
	int err;
	struct timespec ts;

	ts.tv_sec=sec;
	ts.tv_nsec=nsec;

	for (EVER) {
		err=nanosleep(&ts, &ts);
		if (err==-1) {
			if (errno!=EINTR)
				return -1;
		} else
			break;
	}

	return 0;
}

/*******************************************************************************
*
*	xconcat
*
*	Concatenates two strings.
*
*	Input:
*		str1 - string 1.
*		str2 - string 2 (appended to string 1).
*	Output:
*		s: new string.
*		f: NULL.
*/
char *
xconcat(char *str1,
        char *str2)
{
	char *dst;
	int len;

	len=strlen(str1)+strlen(str2)+1;
	dst=(char *)calloc(len, 1);
	if (!dst)
		return NULL;

	strcat(dst, str1);
	strcat(dst, str2);

	return dst;
}

/*******************************************************************************
*
*	safefree
*
*	Safely free destination.
*
*	Input:
*		dst - destination.
*/
void
safefree(char **dst)
{
	char *p;

	if (*dst) {
		p=*dst;
		*dst=NULL;
		free(p);
	}
}
