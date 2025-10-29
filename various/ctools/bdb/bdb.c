/**
 * bdb - GDBM database driver.
 */
#include <errno.h>
#include <gdbm.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAIN
#include "common.h"

/* macros */
#define RET_OK (0)
#define RET_OKNOEXIST (1)
#define RET_NOTOK (2)
#define RET_UNKNOWNCOMMAND (128)
#define RET_WRONGARGCOUNT (129)
#define RET_NOOUTFILE (130)
#define RET_NODBM (131)
#define RET_NOSETOPTS (132)
#define RET_NOVALUE (133)
#define RET_NOVALFILE (134)
#define RET_NOKEY (135)
#define RET_SIGNALS (140)
#define DEFAULT_BLOCK_SIZE (0)
#define DEFAULT_CACHE_SIZE (100)
#define DEFAULT_TIMEOUT (10)

/* structs */
typedef struct {
	void (*func)();
	char *func_name;
	int need_key;
} f_table;

/* protos */
static void usage(char *);
static void signal_handler(int);
static char get_hex(char);
static char get_oct(char);
static char conv_char(char *);
static void set_opts(void);
static void fputsn(datum, char);
static void get_value(void);
static void db_store(int);
static void open_dbm_read(void);
static void open_dbm_write(void);
static void dbm_insert(void);
static void dbm_replace(void);
static void dbm_fetch(void);
static void dbm_exists(void);
static void dbm_delete(void);
static void dbm_dump(void);
static void dbm_reorganize(void);

/* globals */
extern int errno;
static char *database, *command, *key, *value, *file_ptr, *mem_value;
static FILE *file_hd, *tp_file;
static GDBM_FILE dbf;
static int block_size = DEFAULT_BLOCK_SIZE, cache_size = DEFAULT_CACHE_SIZE;
static int timeout = DEFAULT_TIMEOUT;
static char sep_char = ' ', end_char = '\n';
static datum k_datum, d_datum;

/**
 * Usage.
 *
 * @param progname Program path/name.
 */
static void usage(char *progname) {
	char *ptr = progname;

	while (*ptr) {
		ptr++;
	}

	while ((ptr >= progname) && (*ptr != '/')) {
		ptr--;
	}

	if (*ptr == '/') {
		ptr++;
	}
	
	fprintf(stderr, "bdb %s\n", bstools_version);
	fprintf(stderr, "Usage: %s [OPTIONS]\n"
"Options:\n\n"
"  -b block_size     set default block_size (default is %i)\n"
"  -c cache_size     set default cache_size (default is %i)\n"
"  -f file           read [value] from file or output to file\n"
"  -t timeout        timeout in writer problems (default is %i)\n"
"  database          database to work on\n"
"  command           see below\n"
"  [key]             key, optional for some commands\n"
"  [value]           value, optional for some commands\n\n"
"Commands:\n\n"
"  insert            insert (key, value) - generate error if key exists\n"
"  replace           replace (key, value)\n"
"  fetch             get value of (key)\n"
"  exists            check if (key) exists\n"
"  delete            delete (key)\n"
"  dump              dumps database\n"
"  reorganize        reorganize database\n"
, ptr, DEFAULT_BLOCK_SIZE, DEFAULT_CACHE_SIZE, DEFAULT_TIMEOUT);

	exit(RET_NOTOK);
}

/**
 * Handle signal.
 *
 * @param sig_num Signal number received.
 */
static void signal_handler(int sig_num)
{
	fprintf(stderr, "Caught signal %i.\n", sig_num);
	exit(sig_num + RET_SIGNALS);
}

/**
 * Get value of hex digit.
 *
 * @param sec Hex digit.
 * @return value.
 */
static char get_hex(char sec) {
	if ((sec >= '0') && (sec <= '9')) {
		return (char) sec - '0';
	}

	if ((sec >= 'a') && (sec <= 'f')) {
		return (char) (sec - 'a' + 10);
	}

	if ((sec >= 'A') && (sec <= 'F')) {
		return (char) (sec - 'A' + 10);
	}

	fprintf(stderr, "Error in hex format.\n");
	exit(RET_NOTOK);
}

/**
 * Get value of octal digit.
 *
 * @param sec Octal digit.
 * @return value.
 */
static char get_oct(char sec)
{
	if ((sec >= '0') && (sec <= '7')) {
	      return (char) sec - '0';
	}

	fprintf(stderr, "Error in octal format.\n");
	exit(RET_NOTOK);
}

/**
 * Convert character to hex.
 * 
 * @param src Source (possible escaped) string.
 * @return value.
 */
static char conv_char(char *src) {
	char chr = 0;

	if (*src != '\\') {
		return (char) *src;
	}

	src++;
	switch (*src) {
		case '?':
			return (char) 63;

		case '0':
			if (*(src + 1)) {
				break;
			}

			return (char) 0;

		case '\"':
			return (char) 34;

		case '\'':
			return (char) 39;

		case '\\':
			return (char) 92;

		case 'a':
			return (char) 7;

		case 'b':
			return (char) 8;

		case 'f':
			return (char) 12;

		case 'n':
			return (char) 10;

		case 'r':
			return (char) 13;

		case 't':
			return (char) 9;

		case 'v':
			return (char) 11;

		case 'x':
			src++;
			chr = get_hex(*src++);
			chr <<= 4;
			chr |= get_hex(*src);
			return (char) chr;
	}

	chr = get_oct(*src++);
	chr <<= 3;
	chr |= get_oct(*src++);
	chr <<= 3;
	chr |= get_oct(*src);
	return (char) chr;
} 

/**
 * Main.
 */
int main(int argc, char *argv[]) {
	f_table functable[] = {
		{ dbm_replace, "replace", 1 },
		{ dbm_fetch, "fetch", 1 },
		{ dbm_insert, "insert", 1 },
		{ dbm_delete, "delete", 1 },
		{ dbm_exists, "exists", 1 },
		{ dbm_dump, "dump", 0 },
		{ dbm_reorganize, "reorganize", 0 }, 
		{ NULL, "", 0 },
	};
	f_table *fptr = functable;
	int option;

	while ((option = getopt(argc, argv, "b:c:e:f:s:t:")) != -1) {
		switch (option) {
			case 'b':
				block_size = atoi(optarg);
				break;

			case 'c':
				cache_size = atoi(optarg);
				break;

			case 'e':
				end_char = conv_char(optarg);
				break;

			case 'f':
				file_ptr = optarg;
				break;

			case 's':
				sep_char = conv_char(optarg);
				break;

			case 't':
				timeout = atoi(optarg);
				break;
		}
	}

	if (block_size < 0) {
		fprintf(stderr, "Negative block-size (%i) reset.\n", block_size);
		block_size = DEFAULT_BLOCK_SIZE;
	}

	if (cache_size < 0) {
		fprintf(stderr, "Negative cache-size (%i) reset.\n", cache_size);
		cache_size = DEFAULT_CACHE_SIZE;
	}

	if (timeout < 0) {
		fprintf(stderr, "Negative timeout (%i) reset.\n", timeout);
		timeout = DEFAULT_TIMEOUT;
	}

	option = argc - optind;
	if (option < 2) {
		usage(argv[0]);
	}

	database = argv[optind];
	command = argv[optind + 1];
	switch (option) {
		case 2:
			break;

		case 3:
			key = argv[optind + 2];
			break;

		case 4:
			key = argv[optind + 2];
			value = argv[optind + 3];
			break;

		default:
			fprintf(stderr, "Too many arguments!\n");
			exit(RET_WRONGARGCOUNT);
	}

	signal(SIGHUP, &signal_handler);
	signal(SIGINT, &signal_handler);
	signal(SIGQUIT, &signal_handler);
	signal(SIGILL, &signal_handler);
	signal(SIGTRAP, &signal_handler);
	signal(SIGABRT, &signal_handler);
	signal(SIGBUS, &signal_handler);
	signal(SIGFPE, &signal_handler);
	signal(SIGSEGV, &signal_handler);
	signal(SIGALRM, &signal_handler);
	signal(SIGTERM, &signal_handler);
#ifdef Linux
	signal(SIGPWR, &signal_handler);
#endif

	if (key) {
		k_datum.dptr = key;
		k_datum.dsize = strlen(key);
	}

	while (fptr->func) {
		if (!(strcmp(command, fptr->func_name))) {
			if (fptr->need_key) {
				if (key) {
					(*fptr->func)();
				} else {
					fprintf(stderr, "No key given to %s.\n", fptr->func_name);
					exit(RET_NOKEY);
				}
			} else {
				(*fptr->func)();
			}
		}

		fptr++;
	}
	fprintf(stderr, "Unknown function '%s'.\n", command);
	exit(RET_UNKNOWNCOMMAND);
}

/**
 * Set gdbm options.
 */
static void set_opts(void) {
	if (!gdbm_setopt(dbf, GDBM_CACHESIZE, &cache_size, sizeof(int))) {
		return;
	}

	fprintf(stderr, "Couldn't set options (%s).\n", gdbm_strerror(gdbm_errno));
	exit(RET_NOSETOPTS);
}

/**
 * Open database file in read only mode.
 */
static void open_dbm_read(void) {
	char *out_std = "/dev/stdout";

	while (1) {
		if (!(dbf = gdbm_open(database, block_size, GDBM_READER, 0, 0))) {
			if ((gdbm_errno == GDBM_CANT_BE_READER) && (timeout)) {
				timeout--;
				sleep(1);
			} else {
				fprintf(stderr, "Couldn't open %s (%s).\n", database, gdbm_strerror(gdbm_errno));
				exit(RET_NODBM);
			}
		} else {
			break;
		}
	}

	set_opts();

	if (file_ptr == NULL) {
		file_ptr = out_std;
	}

	if ((file_hd = fopen(file_ptr, "w")) < 0) {
		fprintf(stderr, "Couldn't open %s (%i).\n", file_ptr, errno);
		exit(RET_NOOUTFILE);
	}
}

/**
 * Open database file in read/write mode.
 */
static void open_dbm_write(void) {
	while (1) {
		if (!(dbf = gdbm_open(database, block_size, GDBM_WRCREAT, 0644, 0))) {
			if ((gdbm_errno == GDBM_CANT_BE_WRITER) && (timeout)) {
				timeout--;
				sleep(1);
			} else {
				fprintf(stderr, "Couldn't open %s (%s).\n", database, gdbm_strerror(gdbm_errno));
				exit(RET_NODBM);
			}
		} else {
			break;
		}
	}

	set_opts();
}

/**
 * Write to entry to file.
 *
 * @param content Content to write.
 * @param term Terminator character.
 */
static void fputsn(datum content, char term)
{
	char *ptr = content.dptr;
	int size = content.dsize, nw;

	if (ptr == NULL) {
		return;
	}

	while (size > 0) {
		nw = fwrite(ptr, 1, size, file_hd);
		if (ferror(file_hd)) {
			fprintf(stderr, "Couldn't write output (%i).", errno);
			fflush(file_hd);
			free(content.dptr);
			exit(RET_NOOUTFILE);
		}

		size -= nw;
		ptr += nw;
	}

	fflush(file_hd);

	if ((nw = fwrite(&term, 1, 1, file_hd)) != 1) {
		fprintf(stderr, "Couldn't write output (%i).", errno);
		exit(RET_NOOUTFILE);
	}
}

/**
 * Read value from file into datum.
 */
static void get_value(void) {
	long len, r_len;
	char *ptr;

	if ((file_ptr == NULL) && (value == NULL)) {
		fprintf(stderr, "No 'value' given.\n");
		exit(RET_NOVALUE);
	}

	if (file_ptr == NULL) {
		d_datum.dptr = value;
		d_datum.dsize = strlen(value);
		return;
	}

	if ((tp_file = fopen(file_ptr, "r")) == NULL) {
		fprintf(stderr, "Couldn't open %s (%i).\n", file_ptr, errno);
		exit(RET_NOVALFILE);
	}

	if (fseek(tp_file, 0, SEEK_END)) {
		fprintf(stderr, "Couldn't seek %s (%i).\n", file_ptr, errno);
		exit(RET_NOVALFILE);
	}

	len = ftell(tp_file);
	rewind(tp_file);

	if ((mem_value = (char *) calloc(len + 1, 1)) == NULL) {
		fprintf(stderr, "Couldn't allocate %li bytes.", len + 1);
		exit(RET_NOVALFILE);
	}

	d_datum.dptr = mem_value;
	d_datum.dsize = len;

	ptr = mem_value;
	while (len > 0) {
		r_len = fread(ptr, 1, len, tp_file);
		if (ferror(tp_file)) {
			fprintf(stderr, "Read error on %s (%i).\n", file_ptr, errno);
			exit(RET_NOVALFILE);
		}

		if (feof(tp_file)) {
			break;
		}

		len -= r_len;
		ptr += r_len;
	}
}

/**
 * Store datum in database.
 *
 * @param modus GDBM_INSERT or GDBM_REPLACE.
 */
static void db_store(int modus) {
	get_value();

	open_dbm_write();
	exit(gdbm_store(dbf, k_datum, d_datum, modus));
}
	

/**
 * Insert datum into database.
 */
static void
dbm_insert(void)
{
	db_store(GDBM_INSERT);
}

/**
 * Replace/update datum in database.
 */
static void
dbm_replace(void)
{
	db_store(GDBM_REPLACE);
}

/**
 * Fetch datum from database.
 */
static void
dbm_fetch(void)
{
	datum content;

	open_dbm_read();

	content = gdbm_fetch(dbf, k_datum);
	if (content.dptr) {
		fputsn(content, end_char);
		free(content.dptr);
		exit(RET_OK);
	}

	exit(RET_NOTOK);
}

/**
 * Check if a given key exists in the database.
 */
static void dbm_exists(void) {
	open_dbm_read();

	exit(gdbm_exists(dbf, k_datum) ? RET_OKNOEXIST : RET_OK);
}

/**
 * Delete datum from database.
 */
static void dbm_delete(void) {
	open_dbm_write();

	exit((gdbm_delete(dbf, k_datum)) ? RET_NOTOK : RET_OK);
}

/**
 * Dump database to file.
 */
static void dbm_dump(void) {
	datum key, nkey, content;

	open_dbm_read();

	key = gdbm_firstkey(dbf);
	while (key.dptr) {
		fputsn(key, sep_char);
		content = gdbm_fetch(dbf, key);
		fputsn(content, end_char);
		nkey = gdbm_nextkey(dbf, key);
		free(content.dptr);
		free(key.dptr);
		key = nkey;
	}

	exit(RET_OK);
}

/**
 * Reorganize database.
 */
static void dbm_reorganize(void) {
	open_dbm_write();
	exit((gdbm_reorganize(dbf)) ? RET_NOTOK : RET_OK);
}
