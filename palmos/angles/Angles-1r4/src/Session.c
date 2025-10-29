/*
 * Session.c
 */
#include "Include.h"

/* globals */
static Session s;

/*
 * SssGet
 *
 * Open or create a new session.
 */
void
SssGet(void)
{
	if (UtilsGetPref(&s, sizeof(Session), PrfSession)==true)
		session=&s;
	else
		session=NULL;
}

/*
 * SssNew
 *
 * Create a new session.
 */
void
SssNew(void)
{
	s.round=0;
	s.score=0;
	SssUpdate(0);

	session=&s;
}

/*
 * SssUpdate
 *
 * Update session.
 *
 *  -> p	Points.
 */
void
SssUpdate(UInt16 p)
{
	s.round++;
	s.score+=p;

	if (prefs.flags&PrfIncludeReflex)
		s.angle=(UInt16)(RANDOM(360));
	else
		s.angle=(UInt16)(RANDOM(180));

	if (s.angle<5)
		s.angle+=5;
	else if (s.angle>359)
		s.angle-=5;

	if ((s.start=(UInt16)(RANDOM(360)))>359)
		s.start-=5;
}	

/*
 * SssSet
 *
 * Save session.
 */
void
SssSet(void)
{
	if (session)
		UtilsSetPref(&s, sizeof(Session), PrfSession);
	else
		UtilsSetPref(NULL, 0, PrfSession);
}

/*
 * SssDestroy
 *
 * Destroy session.
 */
void
SssDestroy(void)
{
	session=NULL;
	SssSet();
}
