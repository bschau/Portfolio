/**
 * This HashTable implementation uses Robert J. Jenkins Jr.'s Hash function.
 * More about the function can be found at:
 *
 * 	http://burtleburtle.net/bob/hash/evahash.html
 */
#include <stdlib.h>
#include <string.h>

typedef void *HashTable;

/* Structs */
struct bucket {
	struct bucket *next;
	char *key;
	void *value;
	int v_size;
};

typedef struct {
	unsigned long bits;
	unsigned long size;
	unsigned long mask;
	unsigned long count;
	unsigned long prev;
	unsigned int bucket_index;
	struct bucket *curr_bucket;
} HT;

#define MIX(a, b, c) \
	{ \
		a -= b; \
		a -= c; \
		a ^= (c >> 13); \
		b -= c; \
	       	b -= a; \
	       	b ^= (a << 8); \
		c -= a; \
	       	c -= b; \
	       	c ^= (b >> 13); \
		a -= b; \
	       	a -= c; \
	       	a ^= (c >> 12); \
		b -= c; \
	       	b -= a; \
	       	b ^= (a << 16); \
		c -= a; \
	       	c -= b; \
	       	c ^= (b << 5); \
		a -= b; \
	       	a -= c; \
	       	a ^= (c >> 3); \
		b -= c; \
	       	b -= a; \
	       	b ^= (a << 10); \
		c -= a; \
	       	c -= b; \
	       	c ^= (b >> 15); \
	}

/** forward decls */
static unsigned long hash(HT *ht, const char *src);

/**
 * Allocate and initialize a HashTable.
 *
 * @param bits Size of table (1 << bits).
 * @return HashTable or NULL.
 * @note Make sure that bits is a "power of" number (1, 2, 4, 8, 16, ...) or
 * the entire hashtable will fail miserably.
 */
HashTable hash_new(int bits) {
	unsigned long t_size = ((1L << bits) * sizeof(struct bucket *)) + sizeof(HT);
	HT *ht = NULL;

	if ((ht = (HT *) calloc(1, t_size)) != NULL) {
		ht->bits = bits;
		ht->size = (1L << bits);
		ht->mask = ht->size - 1;
		ht->prev = 0xbaadcafe;
	}

	return ht;
}

/**
 * Destroys hash table and buckets (but not the data within the buckets).
 *
 * @param table HashTable.
 * @param free_val Free value (yes if HTFreeValue).
 */
void hash_free(HashTable table, int free_val) {
	HT *ht = (HT *) table;

	if (ht) {
		struct bucket **d;
		int idx;

		d = ((struct bucket **) (((char *) ht) + sizeof(HT)));
		for (idx = 0; idx < ht->size; idx++) {
			if (d[idx]) {
				struct bucket *c, *t;

				for (c = d[idx]; c; ) {
					t = c->next;
					if ((free_val) && (c->value)) {
						free(c->value);
					}

					free(c);
					c = t;
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
static unsigned long hash(HT *ht, const char *src) {
	unsigned long length = strlen(src), a = 0x9e3779b9, b = 0x9e3779b9, c = ht->prev, len;

	len = length;
	while (len >= 12) {
		a += (src[0] + (((unsigned long) src[1]) << 8) + (((unsigned long) src[2]) << 16) + (((unsigned long) src[3]) << 24));
		b += (src[4] + (((unsigned long) src[5]) << 8) + (((unsigned long) src[6]) << 16) + (((unsigned long) src[7]) << 24));
		c += (src[8] + (((unsigned long) src[9]) << 8) + (((unsigned long) src[10]) << 16) + (((unsigned long) src[11]) << 24));
		MIX(a, b, c);
		src += 12;
		len -= 12;
	}

	c += length;
	switch (len) {
	case 11:
		c += ((unsigned int) src[10] << 24);
	case 10:
		c += ((unsigned int) src[9] << 16);
	case 9:
		c += ((unsigned int) src[8] << 8);
	case 8:
		b += ((unsigned int) src[7] << 24);
	case 7:
		b += ((unsigned int) src[6] << 16);
	case 6:
		b += ((unsigned int) src[5] << 8);
	case 5:
		b += ((unsigned int) src[4]);
	case 4:
		a += ((unsigned int) src[3] << 24);
	case 3:
		a += ((unsigned int) src[2] << 16);
	case 2:
		a += ((unsigned int) src[1] << 8);
	case 1:
		a += ((unsigned int) src[0]);
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
 * @param v_size Size of value.
 * @param free_val Free value (yes if HT_FREE_VALUE).
 * @return 0 if successful, !0 if unsuccessful.
 */
int hash_add(HashTable table, const char *key, const void *value, int v_size, int free_val) {
	HT *ht = (HT *) table;
	int k_len = strlen(key) + 1;
	struct bucket **d;
	struct bucket *b;
	char *p;
	unsigned long h_val;

	if ((b = (struct bucket *) calloc(1, k_len + sizeof(struct bucket))) == NULL) {
		return -1;
	}

	p = ((char *) b) + sizeof(struct bucket);
	memmove(p, key, k_len);
	b->key = p;
	b->value = (void *) value;
	b->v_size = v_size;

	h_val = (ht->mask) & hash(ht, key);

	d = ((struct bucket **) (((char *) ht) + sizeof(HT)));

	if (d[h_val] != NULL) {
		struct bucket **prev = &d[h_val];
		struct bucket *l;
		int c;

		while ((l = *prev) != NULL) {
			if ((c = strcmp(key, l->key)) < 0) {
				b->next = l;
				*prev = b;
				return 0;
			}

			if (!c) {
				b->next = l->next;
				*prev = b;
				if (free_val && l->value) {
					free(l->value);
				}

				free(l);
				prev = &b->next;
				return 0;
			}

			prev = &l->next;
		}

		*prev = b;
		b->next = l;
	} else {
		d[h_val] = b;
	}

	return 0;
}

/**
 * Get the value to a key.
 *
 * @param table HashTable.
 * @param key Key to retrieve.
 * @param value Where to store value.
 * @param v_size Where to store value size (or NULL).
 * @return data if found or NULL.
 */
void *hash_get(HashTable table, const char *key, int *v_size) {
	HT *ht = (HT *)table;
	struct bucket **d;
	struct bucket *b;
	unsigned long h_val;

	h_val = (ht->mask) & hash(ht, key);
	d = ((struct bucket **) (((char *) ht) + sizeof(HT)));

	for (b = d[h_val]; b; b = b->next) {
		if (strcmp(b->key, key) == 0) {
			if (v_size) {
				*v_size = b->v_size;
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
int hash_delete(HashTable table, const char *key) {
	HT *ht = (HT *) table;
	struct bucket **d, **p;
	struct bucket *b;
	unsigned long h_val;

	h_val = (ht->mask) & hash(ht, key);
	d = ((struct bucket **) (((char *) ht) + sizeof(HT)));

	p = &d[h_val];

	for (b = d[h_val]; b; b = b->next) {
		if (strcmp(b->key, key) == 0) {
			*p = b->next;
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
 * @param v_size Where to store value len.
 * @return 0 if a pair was found, !0 otherwise.
 * @note Entries are returned in unsorted as-is order.
 */
int hash_find_first(HashTable table, const char **key, const void **value, int *v_size) {
	HT *ht = (HT *)table;
	unsigned int idx;
	struct bucket **d;

	ht->bucket_index = 0;
	ht->curr_bucket = NULL;

	d = ((struct bucket **) (((char *) ht) + sizeof(HT)));
	for (idx = 0; idx < ht->size; idx++) {
		if (d[idx]) {
			struct bucket *b = d[idx];

			*key = b->key;
			*value = b->value;
			*v_size = b->v_size;
			ht->curr_bucket = b;
			ht->bucket_index = idx;
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
 * @param v_size Where to store value len.
 * @return 0 if a pair was found, !0 otherwise.
 * @note Entries are returned in unsorted as-is order.
 */
int hash_find_next(HashTable table, const char **key, const void **value, int *v_size) {
	HT *ht = (HT *) table;
	struct bucket **d;
	struct bucket *b;
	unsigned int idx;

	b = ht->curr_bucket;
	if (b->next) {
		b = b->next;
		ht->curr_bucket = b;

		*key = b->key;
		*value = b->value;
		*v_size = b->v_size;
		return 0;
	}

	d = ((struct bucket **) (((char *) ht) + sizeof(HT)));
	for (idx = ht->bucket_index + 1; idx < ht->size; idx++) {
		if (d[idx]) {
			b = d[idx];

			*key = b->key;
			*value = b->value;
			*v_size = b->v_size;
			ht->curr_bucket = b;
			ht->bucket_index = idx;
			return 0;
		}
	}

	return -1;
}
