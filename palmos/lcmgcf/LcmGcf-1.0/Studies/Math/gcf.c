#include <stdio.h>
#include <stdlib.h>

static int gcf(int, int);

int
main(int argc, char *argv[])
{
	int d;

	switch (argc) {
	case 3:
		printf("GCF(%s, %s) = %u\n", argv[1], argv[2], gcf(atoi(argv[1]), atoi(argv[2])));
		break;
	case 4:
		d=gcf(atoi(argv[1]), atoi(argv[2]));
		printf("GCF(%s, %s, %s) = %u\n", argv[1], argv[2], argv[3], gcf(d, atoi(argv[3])));
		break;
	default:
		fprintf(stderr, "Usage: %s num1 num2 [num3]\n", argv[0]);
		exit(1);
	}

	exit(0);
}

static int
gcf(int a, int b)
{
	int r;

	while ((r=a%b)>0) {
		a=b;
		b=r;
		r=a%b;
	}
	
	return b;
}
