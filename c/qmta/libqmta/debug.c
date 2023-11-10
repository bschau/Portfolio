#include "private.h"

#ifdef DEBUG
/*******************************************************************************
*
*	dumpsession
*
*	Dumps the session.
*
*	Input:
*		s - session.
*/
void
dumpsession(session *s)
{
	printf("session->cntlname:    %s\n", s->cntlname);
	printf("session->rcptname:    %s\n", s->rcptname);
	printf("session->mailname:    %s\n", s->mailname);
	printf("session->created:     %i\n", (int)s->created);
	printf("session->from:        %s\n", s->from);
	printf("session->messagetype: %i\n", s->messagetype);
	printf("session->access:      %i\n", s->access);
	printf("session->dequeue:     %i\n", s->dequeue);
/*	printf("session->time:        %i\n", (int)s->time);	*/
	printf("session->hostname:    %s\n", s->hostname);
	printf("session->domainname:  %s\n", s->domainname);
	printf("session->fqdn:        %s\n", s->fqdn);
	printf("session->postmaster:  %s\n", s->postmaster);
	printf("session->remoteip:    %0X\n", (unsigned int)s->remoteip);
	printf("session->remotename:  %s\n", s->remotename);
	printf("session->remoteaddr:  %s\n", s->remoteaddr);
	printf("session->to:          %s\n", s->to);
}

#endif
