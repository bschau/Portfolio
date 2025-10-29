#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

/* protos */
static void GenerateTables(void);

/* S Boxes */
static unsigned int FwdSBox[256];
static unsigned int RvsSBox[256];

/* Tables */
static unsigned int FwdTab0[256], FwdTab1[256], FwdTab2[256], FwdTab3[256];
static unsigned int RvsTab0[256], RvsTab1[256], RvsTab2[256], RvsTab3[256];

/* Macros */
#define Rotate(x) (((x<<24)&0xffffffff)|((x&0xffffffff)>>8))
#define xtime(x) ((x<<1)^((x&0x80) ? 0x1b : 0x00))
#define mult(x,y) ((x&&y) ? pow[(log[x]+log[y])%255] : 0)

/*
 * GenerateTables
 *
 * Generate the S Boxes and Forward/Reverse tables.
 */
static void
GenerateTables(void)
{
	unsigned char pow[256], log[256], x, y;
	int i;

	/* GF(2^8) pow/log tables */
	x=1;
	for (i=0; i<256; i++)
	{
		pow[i]=x;
		log[x]=i;
		x^=xtime(x);
	}

	/* Generate S Boxes */
	FwdSBox[0x00] = 0x63;
	RvsSBox[0x63] = 0x00;

	for (i=1; i<256; i++)
	{
		x=pow[255-log[i]];

		y=x;
		y=(y<<1)|(y>>7);
		x^=y;
		y=(y<<1)|(y>>7);
		x^=y;
		y=(y<<1)|(y>>7);
		x^=y;
		y=(y<<1)|(y>>7);
		x^=y^0x63;

		FwdSBox[i]=x;
		RvsSBox[x]=i;
	}

	/* Generate tables */
	for (i=0; i<256; i++)
	{
		x=(unsigned char)FwdSBox[i];
		y=xtime(x);

		FwdTab0[i]=((unsigned int)(x^y)^((unsigned int)x<<8)^((unsigned int)x<<16)^((unsigned int)y<<24))&0xffffffff;

		FwdTab1[i]=Rotate(FwdTab0[i]);
		FwdTab2[i]=Rotate(FwdTab1[i]);
		FwdTab3[i]=Rotate(FwdTab2[i]);

		y=(unsigned char)RvsSBox[i];

		RvsTab0[i]=(((unsigned int)mult(0x0b, y))^((unsigned int)mult(0x0d, y)<<8)^((unsigned int)mult(0x09, y)<<16)^((unsigned int)mult(0x0e, y)<<24))&0xffffffff;
		RvsTab1[i]=Rotate(RvsTab0[i]);
		RvsTab2[i]=Rotate(RvsTab1[i]);
		RvsTab3[i]=Rotate(RvsTab2[i]);
	}
}

/*
 * DumpTable
 *
 * Output a table.
 *
 *  -> name		File name.
 *  -> table		Table to output.
 */
static void
DumpTable(char *name, unsigned int *table)
{
	unsigned int *t=table, v;
	char fName[256];
	int i;
	FILE *f;
	size_t s;

	for (i=0; i<256; i++) {
		v=((*t)>>24)&0xff;
		v|=(((*t)>>16)&0xff)<<8;
		v|=(((*t)>>8)&0xff)<<16;
		v|=(*t&0xff)<<24;
		*t++=v;
	}

	sprintf(fName, "%s.bin", name);
	if ((f=fopen(fName, "wb+"))==NULL) {
		perror(fName);
		exit(1);
	}

	printf("Outputting %s table to %s\n", name, fName);
	s=fwrite(table, sizeof(unsigned int), 256, f);
	fclose(f);

	if (s==256)
		return;

	fprintf(stderr, "Failed to write 256 entries to %s\n", fName);
	exit(1);
}

/*
 * main
 */
int
main(int argc, char *argv[])
{
	GenerateTables();
	if (argc>1) {
		printf("Outputting files to %s\n", argv[1]);
#ifdef WIN32
		_chdir(argv[1]);
#else
		chdir(argv[1]);
#endif
	} else
		printf("Outputting files to current working directory\n");

	DumpTable("sbox-fwd", FwdSBox);
	DumpTable("sbox-rvs", RvsSBox);

	DumpTable("FwdTab0", FwdTab0);
	DumpTable("FwdTab1", FwdTab1);
	DumpTable("FwdTab2", FwdTab2);
	DumpTable("FwdTab3", FwdTab3);

	DumpTable("RvsTab0", RvsTab0);
	DumpTable("RvsTab1", RvsTab1);
	DumpTable("RvsTab2", RvsTab2);
	DumpTable("RvsTab3", RvsTab3);
	exit(0);
}
