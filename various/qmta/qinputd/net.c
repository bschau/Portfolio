#include "private.h"

/* globals */
static int sfd=-1;

/*******************************************************************************
*
*	initnet
*
*	Initializes network.
*/
void
initnet(void)
{
	struct sockaddr_in srvadr;
	int err;

	sfd=socket(AF_INET, SOCK_STREAM, 0);
	if (sfd==-1) {
		qlog(QFATAL, "(initnet) couldn't create network socket (%i)", errno);
		exit(1);
	}

	srvadr.sin_family=AF_INET;
	srvadr.sin_addr.s_addr=htonl(INADDR_ANY);
	srvadr.sin_port=getsmtpport();

	err=bind(sfd, (struct sockaddr *)&srvadr, sizeof(srvadr));
	if (err==-1) {
		qlog(QFATAL, "(initnet) couldn't bind network socket (%i)", errno);
		exit(1);
	}

	err=listen(sfd, SOMAXCONN);
	if (err==-1) {
		qlog(QFATAL, "(initnet) couldn't create backlog queue (%i)", errno);
		exit(1);
	}
}

/*******************************************************************************
*
*	closenet
*
*	Closes network connections.
*/
void
closenet(void)
{
	if (sfd>-1)
		close(sfd);
}

/*******************************************************************************
*
*	getserverfd
*
*	Return current server file descriptor.
*
*	Output:
*		server fd.
*/
int
getserverfd(void)
{
	return sfd;
}
