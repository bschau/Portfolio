#define MAIN
#include "yoctobasic.h"

/* forwards */
static void load_program(char *filename);

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: ybas basic.yb\n");
		exit(1);
	}

	load_program(argv[1]);

	reset();
	run();

	exit(0);
}

/**
 * Load program into memory.
 */
static void load_program(char *filename)
{
	if (access(filename, R_OK)) {
		perror("access");
		exit(1);
	}

	struct stat sbuf;

	if (stat(filename, &sbuf)) {
		perror("stat");
		exit(1);
	}

	if ((prom = (char *)calloc(sbuf.st_size + 1, 1)) == NULL) {
		perror("calloc");
		exit(1);
	}

	FILE *fh = fopen(filename, "rb");
	if (fh == NULL) {
		perror("fopen");
		exit(1);
	}

	if (fread(prom, sbuf.st_size, 1, fh) != 1) {
		fprintf(stderr, "fread: cannot read %li bytes\n", sbuf.st_size);
		exit(1);
	}

	fclose(fh);
}

/**
 * Reset runtime environment.
 */
void reset(void)
{
	memset(&variables, 0, sizeof(variables));
	line_number = 1;
	ip = prom;
	stack_reset();
}
