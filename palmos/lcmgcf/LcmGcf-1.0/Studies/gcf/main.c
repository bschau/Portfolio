#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
	int num, fact;

	for (num=1; num<201; num++) {
		printf("%03u: 1", num);

		for (fact=2; fact<num; fact++) {
			if (num%fact==0)
				printf(", %u", fact);
		}
		printf(", %u\r\n", num);
	}

	exit(0);
}
