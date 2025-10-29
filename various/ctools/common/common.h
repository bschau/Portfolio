#ifndef COMMON_H
#define COMMON_H

#ifdef WIN32
#define SEPARATOR_CHAR '\\'
#else
#define SEPARATOR_CHAR '/'
#endif

/* hashtable.c */
typedef void * HashTable;

HashTable hash_new(int bits);
void hash_free(HashTable table, int free_val);
#define HT_PRESERVE_VALUE 0
#define HT_FREE_VALUE 1
int hash_add(HashTable table, const char *key, const void *value, int v_size, int free_val);
void *hash_get(HashTable table, const char *key, int *v_size);
int hash_delete(HashTable table, const char *key);
int hash_find_first(HashTable table, const char **key, const void **value, int *v_size);
int hash_find_next(HashTable table, const char **key, const void **value, int *v_size);

/* stringbuilder.c */
typedef void * StringBuilder;

StringBuilder stringbuilder_new(int g);
void stringbuilder_free(StringBuilder s);
void stringbuilder_append(StringBuilder s, char *src, int len);
void stringbuilder_append_char(StringBuilder s, char src);
char *stringbuilder_to_string(StringBuilder s);
int stringbuilder_length(StringBuilder s);
void stringbuilder_set_length(StringBuilder s, int len);
void stringbuilder_reset(StringBuilder s);
void stringbuilder_copy_to(StringBuilder src, StringBuilder dst);
int stringbuilder_position(StringBuilder s);

/* strings.h */
char *xstrdup(const char *str);

#ifdef MAIN
	char *bstools_version = "from bstools-v3.10";
#else
	extern char *bstools_version;
#endif

#endif
