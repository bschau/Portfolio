#include "private.h"

/* protos */
static int loadavg(void);

/*******************************************************************************
*
*	isbusy
*
*	Return busy status.    Busy means:
*
*		runningchilds=maxchilds
*		loadavg>X
*		etc
*
*	Output:
*		server not busy: 0.
*		server busy: !0.
*/
int
isbusy(void)
{
	int load=loadavg();

	if (runningchilds>=maxchilds || load==-1 || load>maxload)
		return 1;

	return 0;
}

/*******************************************************************************
*
*	loadavg
*
*	Return load average.
*
*	Output:
*		current load.
*/
static int
loadavg(void)
{
/*
**	Linux
*/
#ifdef OSLinux
	FILE *f;
	double load;
	int res;

	f=fopen("/proc/loadavg", "r");
	if (!f) {
		qlog(QINFO, "(loadavg) couldn't get loadavg");
		return -1;
	}

	res=fscanf(f, "%lf", &load);
	fclose(f);

	if (res==1)
		return ((int)(load+0.5));

	qlog(QINFO, "(loadavg) broken /proc/loadavg");
	return -1;
#endif

/*
**	Alpha
*/
#ifdef OSOSF1
	int load, err;
	struct tbl_loadavg tla;

	err=table(TBL_LOADAVG, 0, &tla, 1, sizeof(tla));
	if (err==-1) {
		qlog(QINFO, "(loadavg) couldn't get loadavg");
		return -1;
	}

	if (tla.tl_lscale)
		load=(tla.tl_avenrun.l[2]+(tla.tl_lscale/2))/tla.tl_lscale;
	else
		load=(int)(tla.tl_avenrun.d[2]+0.5);

	return load;
#endif
}
