#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	int seen_eof = 0, size = 0, cnt, c;

	printf("#ifndef BELL_H\n#define BELL_H\n\nunsigned char bell_data[] = \\\n");

	while (!seen_eof) {
		printf("\t\"");
		for (cnt = 0; cnt < 16; cnt++) {
			if ((c = getchar()) == EOF) {
				seen_eof = 1;
				break;
			}

			printf("\\x%02X", c);
			size++;
		}

		printf("\" \\\n");
	}

	printf(";\n\nint bell_size = %i;\n\n#endif\n", size);
	exit(0);
}
