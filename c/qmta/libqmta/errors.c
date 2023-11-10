#include "private.h"

/* globals */
static int errtype, erraction;

/*******************************************************************************
*
*	erroropen
*
*	Open and initialize error system.
*/
void
erroropen(void)
{
	errtype=ERRNOERR;
	erraction=ERRNOTHING;
}

/*******************************************************************************
*
*	errorraise
*
*	Raise error condition.
*
*	Input:
*		t - type.
*		a - action.
*/
void
errorraise(int t,
           int a)
{
	errtype=t;
	erraction=a;
}

/*******************************************************************************
*
*	errorget
*
*	Get part of error.
*
*	Input:
*		w - what to get.
*	Output:
*		requested error field.
*/
int
errorget(int w)
{
	if (w==EGTYPE)
		return errtype;

	return erraction;
}
