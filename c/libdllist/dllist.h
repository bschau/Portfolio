/*******************************************************************************
*
* dllist.h
*
* Functions to manipulate doubly linked lists.
*/

#ifndef DLLIST_H
#define DLLIST_H

/* structs */
struct _Node {
	struct _Node *next;
	struct _Node *prev;
};

typedef struct _Node Node;

struct _List {
	Node *head;
	Node *tail;
	Node *tailpred;
};

typedef struct _List List;

#define IsListEmpty(x) ( (x->tailpred) == (Node *)(x) )

/* protos */
void InitList(List *);
void InsertHead(List *, Node *);
void InsertTail(List *, Node *);
void InsertAfter(Node *, Node *);
void InsertBefore(Node *, Node *);
Node *RemoveHead(List *);
Node *RemoveTail(List *);
void Remove(Node *);

#endif /* DLLIST_H */
