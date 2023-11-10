#include "private.h"

/*******************************************************************************
*
*	mxdestroylist
*
*	Destroys mxlist and content.
*
*	Input:
*		mxl - mxlist.
*/
void
mxdestroylist(mxnode **mxl)
{
	mxnode *m, *n;

	if (*mxl) {
		m=*mxl;
		*mxl=NULL;
		
		for (; m; ) {
			n=m->next;
			mxdestroy(m);
			m=n;
		}
	}
}

/*******************************************************************************
*
*	mxalloc
*
*	Allocates and initializes a new mxnode.
*
*	Input:
*		w - weight.
*		e - exchange.
*	Output:
*		s: mxnode.
*		f: NULL.
*/
mxnode *
mxalloc(unsigned short w,
        char *e)
{
	int nlen=strlen(e);
	mxnode *m;
	char *s;

	s=(char *)calloc(nlen+1, 1);
	if (!s)
		return NULL;

	m=(mxnode *)calloc(sizeof(mxnode)+nlen+2, 1);
	if (!m) {
		free(s);
		return NULL;
	}

	memcpy(s, e, nlen);
	m->weight=w;
	m->exchange=s;

	return m;
}

/*******************************************************************************
*
*	mxdestroy
*
*	Destroys an mxnode and its contents.
*
*	Input:
*		m - mxnode.
*/
void
mxdestroy(mxnode *m)
{
	if (m) {
		if (m->exchange)
			free(m->exchange);

		free(m);
	}
}

/*******************************************************************************
*
*	mxinsert
*
*	Insert mxnode in list.
*
*	Input:
*		mxl - mxlist to insert in.
*		n - node to insert.
*	Output:
*		start of new mxlist.
*/
mxnode *
mxinsert(mxnode *mxl,
         mxnode *n)
{
	mxnode *start=mxl, *prev;

	if (!mxl)		/* start of list */
		return n;

	prev=mxl;
	for (; mxl; ) {
		if ((mxl->weight)>(n->weight)) {
			n->next=mxl;
			if (n->next==start)
				return n;

			prev->next=n;
			return start;
		}

		prev=mxl;
		mxl=mxl->next;
	}

	prev->next=n;

	return start;
}
