#include <ctype.h>
#include <libgen.h>
#include <time.h>
#include "include.h"

#ifdef WIN32
#define DEFAULT_ZEN_FILE "_zen"
#else
#define DEFAULT_ZEN_FILE ".zen"
#endif

/** forward decls */
static void _find_zen_file(void);
static void _skip_line(void);
static void _skip_to_colon(void);
static void _parse_line(void);
static struct zen_node *_new_node(void);
static void _add_node(struct zen_node *node);
static void _read_zen_file(void);
static char *_get_backup_file(void);
static int _get_hex(char c);

/** locals */
static char *hex = "0123456789abcdef";
static struct zen_node *iter_ptr = NULL;
static char *zen_mem, *z_ptr;
static int z_line, next_story_id;
static struct zen_node *zen_nodes;
static char *zen_file;

/**
 * Initialize a zen file.
 */
void zen_file_init(void)
{
	char *env_zen_file = getenv("ZEN_FILE");
	struct stat buf;
	FILE *fh;

	if (env_zen_file != NULL) {
		zen_file = env_zen_file;
	} else {
		zen_file = DEFAULT_ZEN_FILE;
	}

	if (stat(zen_file, &buf) == 0) {
		fprintf(stderr, "This root already contains a zen story file\n");
		exit(1);
	}

	if ((fh = fopen(zen_file, "w+b")) == NULL) {
		perror(zen_file);
		exit(1);
	}

	fprintf(fh, "%s\n", ZEN_MAGIC);
	fprintf(fh, "next:1\n");
	fclose(fh);

	set_file_permissions(zen_file, 0600);
}

/**
 * Find the zen file.
 */
static void _find_zen_file(void)
{
	char *env_zen_file = getenv("ZEN_FILE");
	char path1[PATH_MAX + 1], path2[PATH_MAX + 1];
	struct stat buf;
	char *s1, *s2;

	if (env_zen_file != NULL) {
		if (stat(env_zen_file, &buf) != 0) {
			fprintf(stderr, "ZEN_FILE (%s) not found\n", env_zen_file);
			exit(1);
		}

		zen_file = basename(env_zen_file);
		s1 = dirname(env_zen_file);
		if (strcmp(s1, ".") == 0) {
			fprintf(stderr, "You must use an absolute path for ZEN_FILE\n");
			exit(1);
		}

		if (chdir(s1) != 0) {
			perror(s1);
			exit(1);
		}

		return;
	}

	zen_file = DEFAULT_ZEN_FILE;
	while (stat(zen_file, &buf)) {
		s1 = getcwd(path1, PATH_MAX);
		if (chdir("..") != 0) {
			perror("..");
			exit(1);
		}

		s2 = getcwd(path2, PATH_MAX);
		if (strcmp(s1, s2) == 0) {
			fprintf(stderr, "Cannot find zen story file\n");
			exit(1);
		}
	}
}

/**
 * Fast forward z_ptr to next line.
 */
static void _skip_line(void)
{
	while ((*z_ptr) && (*z_ptr != '\n')) {
		z_ptr++;
	}

	while ((*z_ptr) && (*z_ptr == '\n')) {
		z_line++;
		*z_ptr++ = 0;
	}
}

/**
 * Skip to next colon, eol or eof.
 */
static void _skip_to_colon(void)
{
	while (*z_ptr) {
		if (*z_ptr == ':') {
			*z_ptr++ = 0;
			break;
		}

		if (*z_ptr == '\n') {
			*z_ptr++ = 0;
			z_line++;
			break;
		}

		z_ptr++;
	}
}

/**
 * Parse the current line pointed to by z_ptr.
 */
static void _parse_line(void)
{
	struct zen_node *node = _new_node();
	char *t = z_ptr;

	_skip_to_colon();

	if (strncmp(t, "next", 4) == 0) {
		t = z_ptr;
		_skip_to_colon();
		next_story_id = atoi(t);
		return;
	}

	node->next = NULL;
	node->id = atoi(t);
	t = z_ptr;
	_skip_to_colon();
	node->created = atoi(t);
	t = z_ptr;
	_skip_to_colon();
	node->modified = atoi(t);
	t = z_ptr;
	_skip_to_colon();
	node->closed = atoi(t);
	node->story = z_ptr;
	_skip_line();

	_add_node(node);

	if (node->id >= next_story_id) {
		next_story_id = node->id + 1;
	}
}

/**
 * Allocate a new node.
 *
 * @return Node.
 */
static struct zen_node *_new_node(void)
{
	struct zen_node *node = malloc(ZN_LEN);

	if (node == NULL) {
		fprintf(stderr, "Out of memory allocating new node\n");
		exit(1);
	}

	return node;
}

/**
 * Sort in a node in the zen_nodes list.
 *
 * @param node Node to add / sort in.
 */
static void _add_node(struct zen_node *node)
{
	struct zen_node **prev = &zen_nodes;
	struct zen_node *z;

	while ((z = *prev) != NULL) {
		if (z->id > node->id) {
			node->next = z;
			*prev = node;
			return;
		}

		prev = &z->next;
	}

	*prev = node;
	node->next = z;
}

/**
 * Read zen file.
 */
static void _read_zen_file(void)
{
	zen_mem = read_file(zen_file);

	if (strncmp(zen_mem, ZEN_MAGIC, strlen(ZEN_MAGIC)) != 0) {
		fprintf(stderr, "Not a zen story file\n");
		exit(1);
	}

	z_ptr = zen_mem;
	z_line = 1;
	_skip_line();

	while (*z_ptr) {
		_parse_line();
	}
}

/**
 * Locate, verify and read a zen-file.
 */
void zen_file_read(void)
{
	_find_zen_file();

	next_story_id = 1;
	zen_nodes = NULL;

	_read_zen_file();
}

/**
 * Write zen file.
 */
void zen_file_write(void)
{
	FILE *fh;

	char *zen_backup_file = _get_backup_file();
	unlink(zen_backup_file);

	if (rename(zen_file, zen_backup_file)) {
		perror(zen_file);
		fprintf(stderr, "Failed to rename zen story file - will not go on ...\n");
		exit(1);
	}

	if ((fh = fopen(zen_file, "w+b"))  == NULL) {
		perror(zen_file);
		exit(1);
	}

	fprintf(fh, "%s\n", ZEN_MAGIC);
	fprintf(fh, "next:%i\n", next_story_id);

	if (zen_nodes) {
		struct zen_node *n;

		for (n = zen_nodes; n; n = n->next) {
			if (fprintf(fh, "%i:%li:%li:%li:%s\n", n->id, (long int)n->created, (long int)n->modified, (long int)n->closed, n->story) < 0) {
				fprintf(stderr, "Cannot rewrite zen story file. File re-instate backup file manually.\n");
				exit(1);
			}
		}
	}

	fclose(fh);
}

/**
 * Return backup file from zen_file.
 *
 * @return Backup filename.
 */
char *_get_backup_file(void)
{
	int length = strlen(zen_file);
	char *mem = (char *)calloc(length + 2, 1);

	if (mem == NULL) {
		perror("calloc");
		exit(1);
	}

	strncpy(mem, zen_file, length);
	mem[length] = '~';
	return mem;
}

/**
 * Add the story to the zen_nodes list.
 *
 * @param story Story to add.
 */
void zen_add_story(char *story)
{
	time_t now = time(NULL);
	char *mem = zen_trim_story(story);
	struct zen_node *node;

	if ((mem == NULL) || (strlen(mem) < 1)) {
		fprintf(stderr, "Empty story\n");
		return;
	}

	node = _new_node();
	node->next = NULL;
	node->id = next_story_id;
	node->created = now;
	node->modified = now;
	node->closed = 0;
	node->story = zen_encode(mem);

	_add_node(node);
	next_story_id++;
}

/**
 * Trim story (remove leading and trailing whitespaces).
 *
 * @param story Story to trim.
 * @return Semi trimmed story - story will end with one linefeed.
 */
char *zen_trim_story(char *story)
{
	int len = strlen(story), idx = 0;
	char *mem;

	if ((mem = (char *) malloc(len + 2)) == NULL) {
		fprintf(stderr, "Cannot allocate %i bytes in zen_trim_story\n", (len + 2));
		exit(1);
	}

	for (; *story; story++) {
		if (!(isspace((int) *story))) {
			break;
		}
	}

	while (*story) {
		mem[idx++] = *story++;
	}

	mem[idx] = 0;
	while (--idx > 0) {
		if (isspace((int) mem[idx])) {
			mem[idx] = 0;
		} else {
			idx++;
			break;
		}
	}

	mem[idx++] = '\n';
	mem[idx] = 0;

	return mem;
}

/**
 * Get the first, next and subsequent story on the zen_node list.
 *
 * @return Node or NULL if no more nodes.
 */
struct zen_node *zen_next(void)
{
	if (iter_ptr == NULL) {
		iter_ptr = zen_nodes;
	} else {
		iter_ptr = iter_ptr->next;
	}

	return iter_ptr;
}

/**
 * Return a given node by id.
 *
 * @param id Story ID.
 * @param prev Where to store prev node - NULL if no store.
 * @return node or NULL if not found.
 */
struct zen_node *zen_node_by_id(int id, struct zen_node **prev)
{
	struct zen_node *z = zen_nodes;
	struct zen_node **zp = &zen_nodes;

	while (z) {
		if (z->id == id) {
			if (prev != NULL) {
				*prev = (struct zen_node *) zp;
			}

			return z;
		}

		zp = &z->next;
		z = z->next;
	}

	if (prev != NULL) {
		*prev = NULL;
	}

	return NULL;
}

/**
 * Encode a story.
 *
 * @param src Source story.
 * @return encoded story.
 */
char *zen_encode(char *src)
{
	int len = strlen(src);
	char *mem = malloc((len * 2) + 1);
	int dst = 0, c;

	if (mem == NULL) {
		fprintf(stderr, "Cannot allocate %i bytes in zen_encode\n", ((len * 2) + 1));
		exit(1);
	}

	while (*src) {
		c = *src++;

		mem[dst++] = hex[(c >> 4) & 0x0f];
		mem[dst++] = hex[c & 0x0f];
	}

	mem[dst] = 0;
	return mem;
}

/**
 * Decode a story.
 *
 * @param Src source story.
 * @return decoded story.
 */
char *zen_decode(char *src)
{
	int len = strlen(src), d_len, val;
	char *mem, *p;

	if ((len % 1) == 1) {
		fprintf(stderr, "Warning: story incorrectly formatted ...\n");
	}

	d_len = (len / 2) + 1;
	if ((mem = (char *) malloc(d_len)) == NULL) {
		fprintf(stderr, "Cannot allocate %i bytes in zen_decode\n", d_len);
		exit(1);
	}

	p = mem;
	while (len > 0) {
		val = _get_hex(*src++);
		val <<= 4;
		val |= _get_hex(*src++);
		*p++ = (char) val;
		len -= 2;
	}
	*p = 0;

	return mem;
}

/**
 * Convert hex to value.
 *
 * @param c Hex.
 * @return value.
 */
static int _get_hex(char c)
{
	if ((c >= '0') && (c <= '9')) {
		return c - '0';
	} else if ((c >= 'a') && (c <= 'f')) {
		return (c - 'a') + 10;
	} else if ((c >= 'A') && (c <= 'F')) {
		return (c - 'A') + 10;
	}

	fprintf(stderr, "Incorrect hex byte\n");
	exit(1);
}

/**
 * Count number of stories.
 *
 * @return count.
 */
int zen_count(void)
{
	int count = 0;

	if (!zen_nodes) {
		return count;
	}

	struct zen_node *n;
	for (n = zen_nodes; n; n = n->next) {
		count++;
	}

	return count;
}
