#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	unsigned char data[20];
	unsigned char *p;
	char buffer[20];
	int base, exp, e, b, fd;
	long long sum;

	if (argc!=2) {
		fprintf(stderr, "Usage: tblmkr destfile\n");
		exit(1);
	}

	if ((fd=open(argv[1], O_WRONLY|O_CREAT|O_TRUNC, 0777))==-1) {
		perror("tblmkr");
		exit(1);
	}

	for (base=2; base<13; base++) {
		for (exp=2; exp<13; exp++) {
			sum=base;
			for (e=0; e<(exp-1); e++)
				sum*=base;

			sprintf(buffer, "%qd", sum);

			p=data;
			*p++=(base<<4)|exp;
			for (e=0; buffer[e]; ) {
				b=((buffer[e]-'0')+5)<<4;
				e++;
				if (!buffer[e]) {
					*p++=(unsigned char)b;
					break;
				}
				
				b|=(buffer[e]-'0')+5;
				*p++=(unsigned char)b;
				e++;
			}
			*p=0;

			if ((write(fd, data, p-data+1))==-1) {
				perror("tblmkr");
				break;
			}
		}
	}

	*data=0;
	write(fd, 0, 1);
	close(fd);
	exit(0);
}
