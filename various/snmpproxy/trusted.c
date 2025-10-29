/*
**	trusted.c
**
**	Handle trusted access list.
*/
#include "include.h"

/* structs */
struct TNode {
	struct TNode *next;
	Network n;
};
typedef struct TNode TNode;

/* globals */
static TNode *trustedList;

/*
**	TrustedDump
**
**	Dump trusted list.
*/
void
TrustedDump(void)
{
	TNode *t;
	char buffer[20];

	printf("Security settings:\n\n");
	printf("  Run-as user:                     '%s'\n", pUser);
	printf("  Community:                       '%s'\n", pCommunity);

	printf("  Trusted list:\n");
	if (trustedList) {
		for (t=trustedList; t; t=t->next) {
			XIPToDottedAscii(buffer, &t->n.network);
			printf("                                   %s/", buffer);

			XIPToDottedAscii(buffer, &t->n.mask);
			printf("%s\n", buffer);
		}
	} else
		printf("  ** Warning **  The trusted list is empty.  Proxy disabled.\n");
}

/*
**	TrustedAdd
**
**	Add network/host to the trusted list.
**
**	 -> n		Network.
*/
void
TrustedAdd(Network *net)
{
	TNode *n;

	for (n=trustedList; n; n=n->next) {
		if (n->n.network.s_addr==net->network.s_addr && n->n.mask.s_addr==net->mask.s_addr) {
			XLog(LOG_WARNING, "(TrustedAdd) %0X/%0X already present on list - ignoring", net->network.s_addr, net->mask.s_addr);
			return;
		}
	}

	n=XMalloc(sizeof(TNode));

	n->next=trustedList;
	n->n.network.s_addr=net->network.s_addr;
	n->n.mask.s_addr=net->mask.s_addr;

	trustedList=n;
}

/*
**	TrustedAccess
**
**	Allow or deny access based on the input ip-address.
**
**	 -> ip		IP Address (in host order).
**
**	Return 0 if IP Address is trusted, !0 otherwise.
*/
int
TrustedAccess(struct in_addr *ip)
{	
	TNode *n;

	for (n=trustedList; n; n=n->next) {
		if ((ip->s_addr&n->n.mask.s_addr)==n->n.network.s_addr)
			return 0;
	}

	return -1;
}
