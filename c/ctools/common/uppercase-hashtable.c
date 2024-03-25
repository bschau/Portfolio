/**
 * This HashTable implementation uses Robert J. Jenkins Jr.'s Hash function.
 * More about the function can be found at:
 *
 * 	http://burtleburtle.net/bob/hash/evahash.html
 */
#include <stdlib.h>
#include <string.h>

typedef void * HashTable;

/* Structs */
struct Bucket {
	struct Bucket *next;
	char *key;
	void *value;
	int vSize;
};

typedef struct {
	unsigned long bits;
	unsigned long size;
	unsigned long mask;
	unsigned long count;
	unsigned long prev;
	unsigned int bucketIndex;
	struct Bucket *currBucket;
} HT;

#define MIX(a, b, c) \
	{ \
		a-=b; \
		a-=c; \
		a^=(c>>13); \
		b-=c; \
	       	b-=a; \
	       	b^=(a<<8); \
		c-=a; \
	       	c-=b; \
	       	c^=(b>>13); \
		a-=b; \
	       	a-=c; \
	       	a^=(c>>12); \
		b-=c; \
	       	b-=a; \
	       	b^=(a<<16); \
		c-=a; \
	       	c-=b; \
	       	c^=(b<<5); \
		a-=b; \
	       	a-=c; \
	       	a^=(c>>3); \
		b-=c; \
	       	b-=a; \
	       	b^=(a<<10); \
		c-=a; \
	       	c-=b; \
	       	c^=(b>>15); \
	}

/**
 * Allocate and initialize a HashTable.
 *
 * @param bits No of bits in table.
 * @return HashTable or NULL.
 */
HashTable
HashNew(int bits)
{
	unsigned long tSize=((1L<<bits)*sizeof(struct Bucket *))+sizeof(HT);
	HT *ht=NULL;

	if ((ht=(HT *)calloc(1, tSize))!=NULL) {
		ht->bits=bits;
		ht->size=(1L<<bits);
		ht->mask=ht->size-1;
		ht->prev=0xbaadcafe;
	}

	return ht;
}

/**
 * Destroys hash table and buckets (but not the data within the buckets).
 *
 * @param table HashTable.
 * @param freeVal Free value (yes if HTFreeValue).
 */
void
HashFree(HashTable table, int freeVal)
{
	HT *ht=(HT *)table;

	if (ht) {
		struct Bucket **d;
		int idx;

		d=((struct Bucket **)(((char *)ht)+sizeof(HT)));
		for (idx=0; idx<ht->size; idx++) {
			if (d[idx]) {
				struct Bucket *c, *t;

				for (c=d[idx]; c; ) {
					t=c->next;
					if (freeVal && c->value) {
						free(c->value);
					}

					free(c);
					c=t;
				}
			}
		}

		free(ht);
	}
}

/**
 * Calculate hash/index of the given source.
 *
 * @param ht HashTable.
 * @param src Source string.
 * @return Hash value.
 */
static unsigned long
Hash(HT *ht, const char *src)
{
	unsigned long length=strlen(src), a=0x9e3779b9, b=0x9e3779b9, c=ht->prev, len;

	len=length;
	while (len>=12) {
		a+=(src[0]+(((unsigned long)src[1])<<8)+(((unsigned long)src[2])<<16)+(((unsigned long)src[3])<<24));
		b+=(src[4]+(((unsigned long)src[5])<<8)+(((unsigned long)src[6])<<16)+(((unsigned long)src[7])<<24));
		c+=(src[8]+(((unsigned long)src[9])<<8)+(((unsigned long)src[10])<<16)+(((unsigned long)src[11])<<24));
		MIX(a, b, c);
		src+=12;
		len-=12;
	}

	c+=length;
	switch (len) {
	case 11:
		c+=((unsigned int)src[10]<<24);
	case 10:
		c+=((unsigned int)src[9]<<16);
	case 9:
		c+=((unsigned int)src[8]<<8);
	case 8:
		b+=((unsigned int)src[7]<<24);
	case 7:
		b+=((unsigned int)src[6]<<16);
	case 6:
		b+=((unsigned int)src[5]<<8);
	case 5:
		b+=((unsigned int)src[4]);
	case 4:
		a+=((unsigned int)src[3]<<24);
	case 3:
		a+=((unsigned int)src[2]<<16);
	case 2:
		a+=((unsigned int)src[1]<<8);
	case 1:
		a+=((unsigned int)src[0]);
	}

	MIX(a, b, c);
	return c;
}

/**
 * Add a key value pair to the HashTable.
 *
 * @param table HashTable.
 * @param key Key.
 * @param value Value.
 * @param vSize Size of value.
 * @param freeVal Free value (yes if HTFreeValue).
 * @return 0 if successful, !0 if unsuccessful.
 */
int
HashAdd(HashTable table, const char *key, const void *value, int vSize, int freeVal)
{
	HT *ht=(HT *)table;
	int kLen=strlen(key)+1;
	struct Bucket **d;
	struct Bucket *b;
	char *p;
	unsigned long hVal;

	if ((b=(struct Bucket *)calloc(1, kLen+sizeof(struct Bucket)))==NULL) {
		return -1;
	}

	p=((char *)b)+sizeof(struct Bucket);
	memmove(p, key, kLen);
	b->key=p;
	b->value=(void *)value;
	b->vSize=vSize;

	hVal=(ht->mask)&Hash(ht, key);

	d=((struct Bucket **)(((char *)ht)+sizeof(HT)));

	if (d[hVal]!=NULL) {
		struct Bucket **prev=&d[hVal];
		struct Bucket *l;
		int c;

		while ((l=*prev)!=NULL) {
			if ((c=strcmp(key, l->key))<0) {
				b->next=l;
				*prev=b;
				return 0;
			}

			if (!c) {
				b->next=l->next;
				*prev=b;
				if (freeVal && l->value) {
					free(l->value);
				}

				free(l);
				prev=&b->next;
				return 0;
			}

			prev=&l->next;
		}

		*prev=b;
		b->next=l;
	} else {
		d[hVal]=b;
	}

	return 0;
}

/**
 * Get the value to a key.
 *
 * @param table HashTable.
 * @param key Key to retrieve.
 * @param value Where to store value.
 * @param vSize Where to store value size (or NULL).
 * @return data if found or NULL.
 */
void *
HashGet(HashTable table, const char *key, int *vSize)
{
	HT *ht=(HT *)table;
	struct Bucket **d;
	struct Bucket *b;
	unsigned long hVal;

	hVal=(ht->mask)&Hash(ht, key);
	d=((struct Bucket **)(((char *)ht)+sizeof(HT)));

	for (b=d[hVal]; b; b=b->next) {
		if (strcmp(b->key, key)==0) {
			if (vSize) {
				*vSize=b->vSize;
			}

			return b->value;
		}
	}

	return NULL;
}

/**
 * Delete key and value.
 *
 * @param table HashTable.
 * @param key Key to delete.
 * @return 0 if found, !0 if not found.
 */
int
HashDelete(HashTable table, const char *key)
{
	HT *ht=(HT *)table;
	struct Bucket **d, **p;
	struct Bucket *b;
	unsigned long hVal;

	hVal=(ht->mask)&Hash(ht, key);
	d=((struct Bucket **)(((char *)ht)+sizeof(HT)));

	p=&d[hVal];

	for (b=d[hVal]; b; b=b->next) {
		if (strcmp(b->key, key)==0) {
			*p=b->next;
			if (b->value) {
				free(b->value);
			}

			free(b);
			return 0;
		}
	}

	return -1;
}


/**
 * Find first key, value pair.
 *
 * @param table HashTable.
 * @param key Where to store key pointer.
 * @param value Where to store value pointer.
 * @param vSize Where to store value len.
 * @return 0 if a pair was found, !0 otherwise.
 * @note Entries are returned in unsorted as-is order.
 */
int
HashFindFirst(HashTable table, const char **key, const void **value, int *vSize)
{
	HT *ht=(HT *)table;
	unsigned int idx;
	struct Bucket **d;

	ht->bucketIndex=0;
	ht->currBucket=NULL;

	d=((struct Bucket **)(((char *)ht)+sizeof(HT)));
	for (idx=0; idx<ht->size; idx++) {
		if (d[idx]) {
			struct Bucket *b=d[idx];

			*key=b->key;
			*value=b->value;
			*vSize=b->vSize;
			ht->currBucket=b;
			ht->bucketIndex=idx;
			return 0;
		}
	}

	return -1;
}

/**
 * Find next key, value pair.
 *
 * @param table HashTable.
 * @param key Where to store key pointer.
 * @param value Where to store value pointer.
 * @param vSize Where to store value len.
 * @return 0 if a pair was found, !0 otherwise.
 * @note Entries are returned in unsorted as-is order.
 */
int
HashFindNext(HashTable table, const char **key, const void **value, int *vSize)
{
	HT *ht=(HT *)table;
	struct Bucket **d;
	struct Bucket *b;
	unsigned int idx;

	b=ht->currBucket;
	if (b->next) {
		b=b->next;
		ht->currBucket=b;

		*key=b->key;
		*value=b->value;
		*vSize=b->vSize;
		return 0;
	}

	d=((struct Bucket **)(((char *)ht)+sizeof(HT)));
	for (idx=ht->bucketIndex+1; idx<ht->size; idx++) {
		if (d[idx]) {
			b=d[idx];

			*key=b->key;
			*value=b->value;
			*vSize=b->vSize;
			ht->currBucket=b;
			ht->bucketIndex=idx;
			return 0;
		}
	}

	return -1;
}
