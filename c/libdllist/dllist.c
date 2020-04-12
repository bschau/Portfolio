/*******************************************************************************
*
* Doubly Linked Lists library (dllist)
*
* Functions to manipulate doubly linked lists.
*
* Authors
*
* BS - Brian Schau <brian@schau.dk>
* 
* Changehist
*
* 1.0	Initial release -BS
*/
#include "dllist.h"

/* Special define */
#ifndef NULL
#define NULL ((void *) 0)
#endif

/*******************************************************************************
*
* InitList
*
* Initializes a listheader.
*
* I:	listheader
* O:	-
*/
void
InitList(List *l)
{
	l->head=(Node *)&(l->tail);
	l->tailpred=(Node *)l;
	l->tail=NULL;
}

/*******************************************************************************
*
* InsertHead
*
* Inserts the node at the start of the list.
*
* I:	listheader, node	
* O:	-
*/
void
InsertHead(List *l,
           Node *n)
{
	Node *t;

	n->next=l->head;		/* new node */
	n->prev=(Node *)l;

	l->head=n;			/* list */

	t=n->next;			/* other nodes */
	t->prev=n;
}

/******************************************************************************
*
* InsertTail
*
* Inserts the node at the end of the list.
*
* I:	listheader, node
* O:	-
*/
void
InsertTail(List *l,
           Node *n)
{
	Node *t;

	n->next=(Node *)&l->tail;	/* new node */
	n->prev=l->tailpred;

	l->tailpred=n;			/* list */

	t=n->prev;			/* other nodes */
	t->next=n;
}

/*******************************************************************************
*
* InsertAfter
*
* Inserts the node after the given node.
*
* I:	node, listnode
* O:	-
*/
void
InsertAfter(Node *n,
            Node *ln)
{
	Node *t;

	n->next=ln->next;		/* node */
	n->prev=ln;

	ln->next=n;			/* list node */

	t=n->next;			/* other nodes */
	t->prev=n;
}

/*******************************************************************************
*
* InsertBefore
*
* Inserts the node before the given node.
*
* I:	node, listnode
* O:	-
*/
void
InsertBefore(Node *n,
             Node *ln)
{
	Node *t;

	n->next=ln;			/* node */
	n->prev=ln->prev;

	ln->prev=n;			/* list node */

	t=n->prev;			/* other nodes */
	t->next=n;
}
/*******************************************************************************
*
* RemoveHead
*
* Removes the first node from the list.
*
* I:	list
* O:	the removed node
*/
Node *
RemoveHead(List *l)
{
	Node *t, *n;

	if (IsListEmpty(l)) return NULL;	/* guard */

	n=l->head;				/* list */
	l->head=n->next;

	t=n->next;				/* other nodes */
	t->prev=(Node *)&l->head;
	return n;
}

/*******************************************************************************
*
* RemoveTail
*
* Removes the last node from the list.
*
* I:	list
* O:	the removed node	
*/
Node *
RemoveTail(List *l)
{
	Node *t, *n;

	if (IsListEmpty(l)) return NULL;	/* guard */

	n=l->tailpred;				/* list */
	t=n->prev;
	l->tailpred=t;

	t->next=(Node *)&l->tail;		/* other nodes */
	return n;
}

/*******************************************************************************
*
* Remove
*
* Removes the node from the list.
*
* I:	node
* O:	-
*/
void
Remove(Node *n)
{
	Node *t;

	t=n->next;			/* next node */
	t->prev=n->prev;

	t=n->prev;			/* previous node */
	t->next=n->next;
}
