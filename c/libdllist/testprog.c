#include "dllist.h"
#include <stdio.h>
#include <stdlib.h>

static void listtest(List *);
static void dumplist(List *);

typedef struct {
	Node n;
	char data[20];
} minnode;

static void
listtest(List *l)
{
	if (IsListEmpty(l))
		printf("list is empty\n");
	else
		printf("list is NOT empty\n");
}

static void
dumplist(List *l)
{
	minnode *n;

	for (n=(minnode *)l->head; n->n.next; n=(minnode *)n->n.next)
		printf("%s\n", n->data);
}

int
main(int argc,
     char *argv[])
{
	List mainlist;
	minnode arr[100];
	int i;

	InitList(&mainlist);

	InsertHead(&mainlist, (Node *)&arr[0]);
	for (i=1; i<100; i++) {
		sprintf(arr[i].data, "Node %i", i);
		InsertAfter((Node *)&arr[i], (Node *)&arr[i-1]);
	}

	dumplist(&mainlist);

/*
	for (i=0; i<100; i++) {
		RemoveTail(&mainlist);
	}

	listtest(&mainlist);
	dumplist(&mainlist);
	listtest(&mainlist);
*/

	exit(0);
}
