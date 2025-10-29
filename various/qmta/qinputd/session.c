#include "private.h"

/*******************************************************************************
*
*	createsession
*
*	Prepares a new session.
*
*	Input:
*		ch - child object.
*	Output:
*		s: 0.
*		f: !0.
*/
int
createsession(child *ch)
{
	int err;

	ch->validmail=0;

	err=newsession(ch->sess);
	if (err) {
		qlog(QFATAL, "(createsession) cannot create session");
		return -1;
	}

	return 0;
}

/*******************************************************************************
*
*	destroysession
*
*	Deallocates and zeros structures - this will also take care of any
*	broken sessions.
*
*	Input:
*		ch - child object
*/
void
destroysession(child *ch)
{
	freesession(ch->sess, ch->validmail);
	ch->validmail=0;
}
