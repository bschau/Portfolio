/**
 * Variable functions.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "Include.h"

/* structs */
typedef struct {
	HashTable ht;
	Var parent;
} VAR;

/* protos */
static void MergeVar(HashTable, Var);

/* globals */
static char ticksBuffer[32];
extern char *P;

/**
 * Create a new Variables block.
 *
 * @param parent Parent variables or NULL.
 * @return Variables block.
 */
Var
VarNew(Var parent)
{
	VAR *var=calloc(1, sizeof(VAR));

	if (!var) {
		XError(NULL, "OOM while allocating VAR");
	}

	if ((var->ht=HashNew(8))==NULL) {
		XError(NULL, "OOM while allocating VAR HashTable");
	}

	var->parent=parent;

	return var;
}

/**
 * Free variables.
 *
 * @param v Variables.
 */
void
VarFree(Var v)
{
	if (v) {
		VAR *var=(VAR *)v;

		HashFree(var->ht, HTFreeValue);
		free(var);
	}
}

/**
 * Initialize variables.
 *
 * @param v Variables.
 */
void
VarInitialize(Var v)
{
	char *cwd=XGetCwd(NULL);
	int cLen;

	cLen=strlen(cwd)+1;
	VarSetSystem(v, "CWD", cwd, cLen, 0, NULL);
	VarSetSystem(v, "PWD", cwd, cLen, 0, NULL);
	free(cwd);

	VarSetSystem(v, "OS", P, strlen(P)+1, 0, NULL);
	InitVars(v);
}

/**
 * Get a entry from the variables.
 *
 * @param v Variables.
 * @param key Key.
 * @param dvd Where to store variable desc data.
 * @return !0 if var was found, 0 otherwise.
 */
int
VarGet(Var v, char *key, VARDesc *dvd)
{
	VAR *var=(VAR *)v;
	VARDesc *vd;
	int cnt;
	char *s;

	if (strcmp(key, "userid")==0) {
		dvd->data=OSUserId(); 
		dvd->dLen=strlen(dvd->data)+1;
		dvd->file=NULL;
		dvd->lineNo=0;
		return 1;
	} else if (strcmp(key, "seconds")==0) {
		time_t t=time(NULL);

		sprintf(ticksBuffer, "%lu", t);
		dvd->data=ticksBuffer;
		dvd->dLen=strlen(ticksBuffer)+1;
		dvd->file=NULL;
		dvd->lineNo=0;
		return 1;
	}

	while (var) {
		if (var->ht) {
			if ((vd=HashGet(var->ht, key, &cnt))!=NULL) {
				dvd->data=vd->data;
				dvd->dLen=vd->dLen;
				dvd->file=vd->file;
				dvd->lineNo=vd->lineNo;
				return 1;
			}
		}

		var=var->parent;
	}

	if ((s=getenv(key))!=NULL) {
		dvd->data=s;
		dvd->dLen=strlen(s)+1;
		dvd->file=NULL;
		dvd->lineNo=0;
		return 1;
	}

	dvd->data=NULL;
	dvd->dLen=0;
	dvd->file=NULL;
	dvd->lineNo=0;
	return 0;
}

/**
 * Set a entry in the variables - by user assignment.
 *
 * @param v Variables.
 * @param key Key.
 * @param src Data.
 * @param srcLen Data length.
 * @param lineNo Line number of declaration.
 * @param file File in which var is declared (or NULL).
 */
void
VarSet(Var v, char *key, void *src, int srcLen, int lineNo, char *file)
{
	if ((strcmp(key, "-")==0) ||
	    (strcmp(key, "<")==0) ||
	    (strcmp(key, "?")==0) ||
	    (strcmp(key, "OS")==0) ||
	    (strcmp(key, "PWD")==0) ||
	    (strcmp(key, "CWD")==0) ||
	    (strcmp(key, "userid")==0) ||
	    (strcmp(key, "seconds")==0)) {
		XError(NULL, "Cannot redefine %s", key);
	}

	VarSetSystem(v, key, src, srcLen, lineNo, file);
}

/**
 * Set a entry in the variables.
 *
 * @param v Variables.
 * @param key Key.
 * @param src Data.
 * @param srcLen Data length.
 * @param lineNo Line number of declaration.
 * @param file File in which var is declared (or NULL).
 */
void
VarSetSystem(Var v, char *key, void *src, int srcLen, int lineNo, char *file)
{
	int fLen=0, size=sizeof(VARDesc);
	VARDesc *vd;
	void *p;

	size+=srcLen;

	if (file) {
		fLen=strlen(file);
		size+=fLen+1;
	}

	if ((vd=(VARDesc *)malloc(size))==NULL) {
		XError(NULL, "OOM when allocating VARDesc");
	}

	p=((char *)vd)+sizeof(VARDesc);
	vd->data=p;
	vd->dLen=srcLen;
	memmove(p, src, srcLen);
	if (file) {
		p+=srcLen;

		vd->file=p;
		memmove(p, file, fLen+1);
	} else {
		vd->file=NULL;
	}

	vd->lineNo=lineNo;

	if (HashAdd(((VAR *)v)->ht, key, vd, size, HTFreeValue)!=0) {
		XError(NULL, "Failed to add '%s'", key);
	}
}

/**
 * Delete a variable.
 *
 * @param v Variables.
 * @param key.
 * @return 0 if succesful, !0 otherwise.
 */
int
VarDelete(Var v, char *key)
{
	VAR *var=(VAR *)v;

	if (HashDelete(var->ht, key)==0) {
		return 0;
	}

	return -1;
}

/**
 * Dump the variables.
 *
 * @param va Variables.
 */
void
VarDump(Var va)
{
	HashTable ht=HashNew(8);
	VARDesc *vd;
	int size;
	const char *key;

	if (!ht) {
		XError(NULL, "OOM while allocating VarDump HashTable");
	}

	MergeVar(ht, va);

	if (!HashFindFirst(ht, &key, (void *)&vd, &size)) {
		printf("%s -> '%s'\n", key, vd->data);

		while (!HashFindNext(ht, &key, (void *)&vd, &size)) {
			printf("%s -> '%s'\n", key, vd->data);
		}
	}

	HashFree(ht, HTPreserveValue);
}

/**
 * Merge hashtables.
 *
 * @param ht Destination hash table.
 * @param v Variables.
 */
static void
MergeVar(HashTable ht, Var v)
{
	VAR *var=(VAR *)v;
	VARDesc *vd;
	int size;
	const char *key;

	if (var->parent) {
		MergeVar(ht, var->parent);
	}

	if (!HashFindFirst(var->ht, &key, (void *)&vd, &size)) {
		if (HashAdd(ht, key, vd, size, HTPreserveValue)) {
			XError(NULL, "OOM while inserting first key");
		}

		while (!HashFindNext(var->ht, &key, (void *)&vd, &size)) {
			if (HashAdd(ht, key, vd, size, HTPreserveValue)) {
				XError(NULL, "OOM while inserting key");
			}
		}
	}
}
