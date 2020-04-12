#include "include.h"

/** forward decls */
static char *_file_load(void);
static void _create_list(char *src);
static void _link_node(struct qotd *node);

/** globals */
static char *_file;
static struct qotd *_nodes;
static int _qotd_count;

/**
 * Deduce path to qotd file.
 * Command line overrides environment which overrides $HOME.
 *
 * @param qotd_file Full path from command line.
 * @return Path.
 */
void qotd_file_set(char *qotd_file)
{
	if (qotd_file != NULL) {
		_file = qotd_file;
		return;
	}

	_file = getenv("QOTD_FILE");
	if (_file != NULL) {
		return;
	}

	char *home = getenv("HOME");
	if (home == NULL) {
		fprintf(stderr, "$HOME unset\n");
		exit(1);
	}

	int length = strlen(home) + 1 + strlen(QOTD_FILE) + 1;
	_file = malloc(length);
	if (_file == NULL) {
		perror("malloc");
		exit(1);
	}

	snprintf(_file, length, "%s/%s", home, QOTD_FILE);
}

/**
 * Load and parse qotd file.
 *
 * @param qotd_file qotd file.
 */
void qotd_file_load(void)
{
	char *qotds = _file_load();
	_create_list(qotds);
	/** Debugging purposes
	struct qotd *node = _nodes;
	printf("Nodes: %i\n", _qotd_count);
	while (node) {
		printf("%s\n", node->text);
		node = node->next;
	}
	*/
}

/**
 * Load qotd file.
 *
 * @return file content.
 */
static char *_file_load(void)
{
	struct stat buffer;

	if (stat(_file, &buffer) != 0) {
		perror("stat");
		exit(1);
	}

	FILE *fh = fopen(_file, "rb");

	if (fh == NULL) {
		perror("fopen");
		exit(1);
	}

	size_t file_size = buffer.st_size;
	char *qotd_text = (char *)malloc(file_size + 1);
	if (qotd_text == NULL) {
		perror("malloc");
		exit(1);
	}

	size_t to_read = file_size, count;
	char *dst = qotd_text;
	while (to_read > 0) {
		count = fread(dst, 1, to_read, fh);
		to_read -= count;
		dst += count;
	}
	*dst = 0;
	fclose(fh);

	char *src = qotd_text;
	dst = src;
	while (*src) {
		if (*src != 0x0d) {
			*dst++ = *src;
		}
		src++;
	}
	*dst = 0;
	return qotd_text;
}

/**
 * Create list with qotd nodes. Maintains count of nodes in _qotd_count.
 *
 * @param src text with all qotds.
 * @return nodes.
 */
static void _create_list(char *src)
{
	struct qotd *node = NULL;

	while (*src) {
		if (*src == '%' && *(src + 1) == '\n') {
			if (node != NULL) {
				*src = 0;
				_link_node(node);
			}

			node = malloc(sizeof(struct qotd));
			if (node == NULL) {
				perror("malloc");
				exit(1);
			}
				
			src += 2;
			node->text = src;
		} else {
			src++;
		}
	}

	if (!node) {
		return;
	}

	if (strlen(node->text) == 0) {
		return;
	}

	_link_node(node);
}

/**
 * Add node to start of list.
 *
 * @param node node to add.
 */
static void _link_node(struct qotd *node)
{
	node->next = _nodes;
	_nodes = node;
	_qotd_count++;
}

/**
 * Pick one random quote of the day.
 *
 * @param preferred preferred index or -1 to pick random.
 * @return quote of the day (text).
 */
char *qotd_file_pick_one(int preferred)
{
	if (preferred == -1) {
		srand(time(NULL));
		preferred = rand() % _qotd_count;
	} else {
		if (preferred < 1 || preferred > _qotd_count) {
			fprintf(stderr, "Preferred index %i out of range (1 ... %i)\n", preferred, _qotd_count);
			exit(1);
		}

		preferred--;
		preferred = _qotd_count - preferred;
	}

	struct qotd *node = _nodes;
	while (node && preferred > 1) {
		node = node->next;
		preferred--;
	}

	return node->text;
}

/**
 * Output parse quote of the day to stream.
 *
 * @param sh stream handle.
 * @param qotd unparsed quoted of the day.
 */
void qotd_file_output(FILE *sh, char *qotd)
{
	int in_bold = 0, in_italic = 0;
	char c;
	char *str = &c;

	while (*qotd) {
		c = *qotd++;
		if (c == '\\') {
			fwrite(qotd, 1, 1, sh);
			qotd++;
		} else if (c == '\n') {
			fwrite("\n", 1, 1, sh);
		} else if (c == '*') {
			if (in_bold) {
				fwrite("\033[0m", 1, 4, sh);
			} else {
				fwrite("\033[1m", 1, 4, sh);
			}
			in_bold = !in_bold;
		} else if (c == '_') {
			if (in_italic) {
				fwrite("\033[0m", 1, 4, sh);
			} else {
				fwrite("\033[7m", 1, 4, sh);
			}
			in_italic = !in_italic;
		} else if (c == '#') {
			fwrite(" ", 1, 1, sh);
		} else {
			fwrite(str, 1, 1, sh);
		}
	}
}
