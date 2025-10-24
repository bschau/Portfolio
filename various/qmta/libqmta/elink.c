#include "private.h"

static mxnode *el, *elptr;

/*******************************************************************************
*
*	elinkopen
*
*	Opens elink system.
*/
void
elinkopen(void)
{
	el=NULL;	/* not much here */
}

/*******************************************************************************
*
*	elinkclose
*
*	Closes elink system.
*/
void
elinkclose(void)
{
	mxdestroylist(&el);
}

/*******************************************************************************
*
*	elinkadd
*
*	Add a elink message.
*
*	Input:
*		src - message to add.
*	Output:
*		s: 0.
*		f: !0.
*/
int
elinkadd(char *src)
{
	mxnode *m;

	m=mxalloc(0, src);
	if (!m)
		return -1;

	el=mxinsert(el, m);

	return 0;
}

/*******************************************************************************
*
*	elinktravstart
*
*	Prepare to traverse elink.
*/
void
elinktravstart(void)
{
	elptr=el;
}

/*******************************************************************************
*
*	elinknext
*
*	Get next elink message.
*
*	Output:
*		s: message.
*		f: NULL (end of list).
*/
char *
elinknext(void)
{
	char *m=NULL;

	if (elptr) {
		m=elptr->exchange;
		elptr=elptr->next;
	}

	return m;
}

/*******************************************************************************
*
*	elinkstatus
*
*	Return elinks.
*
*	Output:
*		no elinks: 0.
*		elinks: !0.
*/
int
elinkstatus(void)
{
	return ((el) ? 1 : 0);
}
