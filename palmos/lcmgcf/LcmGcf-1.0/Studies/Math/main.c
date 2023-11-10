#include <stdio.h>
#include <stdlib.h>

static int gcf(int, int);
static int lcm(int, int);

int
main(int argc, char *argv[])
{
	int n1=1, n2=1, n3=1;

	if (argc<2) {
		fprintf(stderr, "Usage: %s num1 num2 [num3]\n", argv[0]);
		exit(1);
	}

	n1=atoi(argv[1]);
	n2=atoi(argv[2]);
	if (argc==4)
		n3=atoi(argv[3]);

	if (argc==4) {
		printf("LCM(%u, %u, %u) = %u\n", n1, n2, n3, lcm(n3, lcm(n1, n2)));
		printf("GCF(%u, %u, %u) = %u\n", n1, n2, n3, gcf(n3, gcf(n1, n2)));
	} else {
		printf("LCM(%u, %u) = %u\n", n1, n2, (n1*n2)/gcf(n1, n2));
		printf("GCF(%u, %u) = %u\n", n1, n2, gcf(n1, n2));
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

static int
lcm(int a, int b)
{
	return (a*b)/gcf(a, b);
}
