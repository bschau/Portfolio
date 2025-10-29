#include <sys/stat.h>
#include <ctype.h>
#ifndef WIN32
#include <libgen.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define MAIN
#include "common.h"

/* macros */
#define EVER ;;
#define SECTORS (36)
#define F_FLAGS_DELETED	(0x00)
#define F_FLAGS_SEQFILE	(0x01)
#define F_FLAGS_PRGFILE	(0x02)
#define F_FLAGS_USRFILE	(0x03)
#define F_FLAGS_RELFILE	(0x04)
#define F_FLAGS_LOCKED	(0x40)		/* Set produces ">" locked files */
#define F_FLAGS_CLOSED	(0x80)		/* Not set produces "*" (splat) file */
#define F_FLAGS_MASKTYPE	(0x07)
#define DIR_ENTRIES_PR_SECTOR	(8)
#define MAX_DIR_ENTRIES		(18)
#define DIR_LIST_SIZE		(DIR_ENTRIES_PR_SECTOR * MAX_DIR_ENTRIES)
#define D64_TYPE_UNKNOWN	(0)
#define D64_TYPE_35NE		(1)
#define D64_TYPE_35E		(2)
#define D64_TYPE_40NE		(3)
#define D64_TYPE_40E		(4)

/* structs */
/**
 * On disk entry.
 */
typedef struct {
	unsigned char next_dir_track;	/* 0, 0 = no more entries */
	unsigned char next_dir_sector;
	unsigned char file_flags;
	unsigned char file_track;
	unsigned char file_sector;
	unsigned char filename[16];	/* Padded with 0xa0 */
	unsigned char rel_track;
	unsigned char rel_sector;
	unsigned char rel_length;
	unsigned char unused[6];
	unsigned char fsis_l;		/* file size in sectors low */
	unsigned char fsis_h;		/* file size in sectors high */
					/* To calulate size on disk:
					 *
					 * 	(fsisH*256)+fsisL
					 */
} dir_entry;

/**
 * Block Availability Map entry.
 */
struct BAM {
	unsigned char dir_track;
	unsigned char dir_sector;
	unsigned char dos_version;
	unsigned char unused1;
	unsigned char bam[140];
	unsigned char diskname[16];
	unsigned char a0[2];
	unsigned char disk_id[2];
	unsigned char a4;
	unsigned char dos_type[2];
	unsigned char unused2[4];
	unsigned char unused3[85];
};

/**
 * Virtual Container node. The virtual container is a linked list
 * of entries to add to the output file.
 */
struct VC {
	struct VC *next;
	char name[16];
	int s_track;
	int s_sector;
	int prg_size;
	int load_address;
	char prg_type;
};

/* forward decls */
#ifdef WIN32
static char *basename(char *path);
#endif
static void init_offsets(void);
static int tstoi(int t, int s);
static int seek_ts(int t, int s);
static dir_entry *get_dir_entry(int p_num);
static int file_size(dir_entry *d);
static int load_address(dir_entry *d);
static int get_type(char *name);
static void read_file(char *n);
static unsigned char *unpetify(unsigned char *src, int len);
static void petify(unsigned char *src, int len);
static void is_supported(char *name);
static void usage(int ec);
static void list(void);
static void bam(void);
static void tslink(int prog_num);
static void convert_to_t64(char *ouput, char *tape_name, char *track_specs);
static void extract(int t);
static FILE *create_file(char *src, char *ext);
static void safe_write(FILE *h, void *ptr, int len);
static void safe_seek(FILE *h, int pos);
static void write_signature(FILE *h);
static void write_directory_info(FILE *h, int entries, char *name);
static void write_directory_entries(FILE *h);
static void write_file_entries(FILE *h);

/* globals */
static const int sectors[SECTORS]= { 0,	/* ILLEGAL */
	21, 21, 21, 21, 21, 21, 21, 21, 21, 21,	/*  1-10 */
	21, 21, 21, 21, 21, 21, 21, 19, 19, 19,	/* 11-20 */
	19, 19, 19, 19, 18, 18, 18, 18, 18, 18, /* 21-30 */
	17, 17, 17, 17, 17 };			/* 31-35 */
static char *input_file;
static unsigned char *mem_file;
static unsigned int mem_size;
static int *offsets;
static FILE *t64h = NULL;
static char track_seen[DIR_LIST_SIZE];
static int vc_entries;
static struct VC *vc_list;
static struct VC **prev;

/******************************************************************************
 *
 * Support routines.
 *
 */

#ifdef WIN32
/**
 * Return the last component of this path.
 *
 * @param path Path.
 * @return Pointer to last component.
 */
static char *basename(char *path) {
	char *p = strrchr(path, '\\');

	if (p) {
		return p + 1;
	}

	if ((p = strrchr(path, '/'))) {
		return p + 1;
	}

	return path;
}
#endif

/**
 * Initialize offsets table.
 */
static void init_offsets(void) {
	int offset = 0, dst_idx = 1, track;

	if ((offsets = (int *) malloc(SECTORS * sizeof(int))) == NULL) {
		fprintf(stderr, "OOM in D64InitSectors\n");
		exit(1);
	}

	for (track = 1; track < SECTORS; track++) {
		offsets[dst_idx++] = offset;
		offset += sectors[track] * 256;
	}
}

/**
 * Convert a track/sector pair into index value.
 *
 * @param t Track num.
 * @param s Sector num.
 * @return index or -1 (error).
 */
static int tstoi(int t, int s) {
	if ((t < 1) || (t > SECTORS) || (s < 0) || (s > sectors[t])) {
		return -1;
	}

	return offsets[t] + (s * 256);
}

/**
 * Seek to a given track/sector.
 *
 * @param t Track num.
 * @param s Sector num.
 * @return index.
 */
static int seek_ts(int t, int s) {
	int idx;

	if ((idx = tstoi(t, s)) < 0) {
		fprintf(stderr, "Seek out of range: %i, %i\n", t, s);
		exit(1);
	}

	if ((idx < 0) || (idx >= mem_size)) {
		fprintf(stderr, "Seek out of range: %i, %i\n", t, s);
		exit(0);
	}

	return idx;
}

/**
 * Return directory entry for the given program.
 *
 * @param p_num Program num #.
 */
static dir_entry *get_dir_entry(int p_num) {
	int t = 0x12, s = 0x01, c = 0, idx;
	dir_entry *d;

	p_num--;
	do {
		if (!t) {
			fprintf(stderr, "Seek error on program num: %i\n", p_num + 1);
			exit(1);
		}

		idx = seek_ts(t, s);
		d = (dir_entry *) (mem_file + idx);
		t = d->next_dir_track;
		s = d->next_dir_sector;
		c++;
	} while (c < (p_num / 8));

	p_num %= 8;
	if (!p_num) {
		return d;
	}

	return (dir_entry *) (mem_file + idx + (p_num * sizeof(dir_entry)));
}

/**
 * Calculate file size.
 *
 * @param d dir_entry.
 */
static int file_size(dir_entry *d) {
	int size = 0, t = d->file_track, s = d->file_sector, idx;

	idx = seek_ts(d->file_track, d->file_sector);
	while (*(mem_file + idx)) {
		size += 254;

		t = (int) *(mem_file + idx);
		s = (int) *(mem_file + idx + 1);
		idx = seek_ts(t, s);
	}

	if (*(mem_file + idx + 1)) {
		size += (int) *(mem_file + idx + 1);
	}

	size -= 2;	/* Load address */

	return size;
}

/**
 * Get the load address of this program. The address is stored at 0x02-0x03 in
 * the first sector of this program.
 *
 * @param d dir_entry.
 * @return Load address.
 */
static int load_address(dir_entry *d) {
	int idx = seek_ts(d->file_track, d->file_sector);

	return (mem_file[idx + 3] * 256) + mem_file[idx + 2];
}

/**
 * Get type of D64 disk image.
 *
 * @param name File name.
 * @return D64 Type.
 */
static int get_type(char *name) {
	struct stat sbuf;

	if (stat(name, &sbuf)) {
		fprintf(stderr, "Failed to stat '%s'\n", name);
		exit(1);
	}

	switch (sbuf.st_size) {
	case 174848:
		return D64_TYPE_35NE;

	case 175531:
		return D64_TYPE_35E;

	case 196608:
		return D64_TYPE_40NE;

	case 197376:
		return D64_TYPE_40E;
	}

	return D64_TYPE_UNKNOWN;
}

/**
 * Read entire file into memory.
 *
 * @param n File name.
 */
static void read_file(char *n) {
	struct stat sbuf;
	FILE *fh;

	is_supported(n);

	if (stat(n, &sbuf)) {
		fprintf(stderr, "Failed to stat '%s'\n", n);
		exit(1);
	}

	if ((mem_file = (unsigned char *) malloc(sbuf.st_size))==NULL) {
		fprintf(stderr, "Failed to allocate %i bytes\n", (int) sbuf.st_size);
		exit(1);
	}

	mem_size = sbuf.st_size;
	if ((fh = fopen(n, "rb")) == NULL) {
		fprintf(stderr, "Failed to open %s for reading\n", n);
		exit(1);
	}

	if (fread(mem_file, mem_size, 1, fh) != 1) {
		fprintf(stderr, "Failed to read from %s\n", n);
		exit(1);
	}

	fclose(fh);
}

/**
 * Convert a name in PETASCII to ascii.
 *
 * @param src Source name in PETASCII.
 * @param len Source length.
 * @return Ascii name.
 * @note Caller must free name.
 */
static unsigned char *unpetify(unsigned char *src, int len) {
	unsigned char *dst = (unsigned char *) malloc(len + 1);

	if (dst == NULL) {
		fprintf(stderr, "OOM in unpetify\n");
		exit(1);
	}

	memmove(dst, src, len);
	dst[len] = 0;

	len--;
	while (len >= 0) {
		if (dst[len] == 0xa0) {
			dst[len] = 0;
		} else {
			break;
		}

		len--;
	}

	return dst;
}

/**
 * Petify a name (upper case).
 *
 * @param src Source string.
 * @param len Length.
 * @note This will done in place!
 */
static void petify(unsigned char *src, int len) {
	while (len >= 0) {
		*src = toupper(*src);
		src++;
		len--;
	}
}

/**
 * See if the given filename is a supported disk image.
 *
 * @param name File name.
 */
static void is_supported(char *name) {
	switch (get_type(name)) {
	case D64_TYPE_35NE:
	case D64_TYPE_35E:
		return;
	}

	fprintf(stderr, "Unsupported D64 disk image\n");
	exit(1);
}

/**
 * Write usage information.
 *
 * @param ec Error code.
 */
static void usage(int ec) {
	FILE *f = (ec) ? stderr : stdout;

	fprintf(f, "d64 %s\n", bstools_version);
	fprintf(f, "Usage: d64 [OPTIONS] [COMMAND] diskfile.d64 [cmdargs]\n\n");
	fprintf(f, "[OPTIONS]\n");
	fprintf(f, "  -h            Help (this page).\n");
	fprintf(f, "  -n tape name  (x) Tape name. Defaults to input name.\n");
	fprintf(f, "  -o file       (x) Output to file (only for tracks).\n\n");
	fprintf(f, "\n[COMMAND]\n");
	fprintf(f, "  b             List Block Availability Map (BAM).\n");
	fprintf(f, "  f (prog num)  Follow track/sector links:\n");
	fprintf(f, "                Requires a program number.\n");
	fprintf(f, "  l             List content of diskfile.\n");
	fprintf(f, "  t             List type of d64 file.\n");
	fprintf(f, "  x trackspecs  Extract to t64 image. Trackspecs:\n");
	fprintf(f, "                i1            Extract track i1.\n");
	fprintf(f, "                i1,i2,i4      Extract tracks i1, i2 and i4.\n");
	fprintf(f, "                i1-i3         Extract tracks i1, i2 and i3.\n");
	fprintf(f, "                i1,i3-i7,i9   Extract tracks i1, i3, i4, i5, i6, i7 and i9.\n\n");
	exit(ec);
}

/**
 * Main.
 */
int main(int argc, char *argv[]) {
	char *output = NULL, *tape_name = NULL;
	int opt;

	while ((opt = getopt(argc, argv, "hn:o:"))!=-1) {
		switch (opt) {
		case 'h':
			usage(0);

		case 'n':
			tape_name=optarg;
			break;

		case 'o':
			output=optarg;
			break;

		default:
			fprintf(stderr, "Unknown option '%c'\n", opt);
			usage(1);
		}
	}

	if ((argc - optind) < 2) {
		usage(1);
	}

	opt = *argv[optind++];
	if (opt == 't') {
		switch (get_type(argv[optind])) {
		case D64_TYPE_35NE:
			printf("35 tracks, no errors\n");
			break;

		case D64_TYPE_35E:
			printf("35 tracks, 683 error bytes\n");
			break;

		case D64_TYPE_40NE:
			printf("40 tracks, no errors\n");
			break;

		case D64_TYPE_40E:
			printf("40 tracks, 768 error bytes\n");
			break;

		default:
			printf("Type unknown or not a D64 disk image file\n");
			break;
		}

		exit(0);
	}

	input_file = argv[optind++];
	read_file(input_file);
	init_offsets();
	switch (opt) {
		case 'b':
			bam();
			break;

		case 'f':
			tslink(atoi(argv[optind]));
			break;

		case 'l':
			list();
			break;

		case 'x':
			convert_to_t64(output, tape_name, argv[optind]);
			break;

		default:
			fprintf(stderr, "Unknown command '%c'", *argv[optind - 1]);
			usage(1);
	}

	exit(0);
}

/******************************************************************************
 *
 * Functions
 *
 */

/**
 * List content of disc.
 */
static void list(void) {
	char types[] = " SEQ PRG USR REL ??? ??? ??? ???";
	int t = 0x12, s = 1, prg_num = 1, idx, cnt;
	unsigned char type[6];
	char flag;
	dir_entry *d, *d_save;
	unsigned char *tmp;

	type[4] = 0;

	idx = seek_ts(0x12, 0);
	tmp = unpetify(idx + mem_file + 0x90, 16);
	printf("Label: %s\n", tmp);
	free(tmp);

	while (t) {
		idx = seek_ts(t, s);
		d = (dir_entry *)(mem_file + idx);
		d_save=d;

		/*      xxx  xx/xx  xxxxxx   xxxxxx    fxxxxxxxxxxxxxxxx    xxxx    xxxxxx */
		printf("  #   t/s  sectors     size     name                type      load\n");

		for (cnt = 0; cnt < 8; cnt++) {
			if (d->file_flags & F_FLAGS_MASKTYPE) {
				tmp = unpetify((unsigned char *) (d->filename), 16);
				if (d->file_flags & F_FLAGS_LOCKED) {
					flag = '>';
				} else if (!(d->file_flags & F_FLAGS_CLOSED)) {
					flag = '*';
				} else {
					flag = ' ';
				}

				memmove(type, types + (((d->file_flags & F_FLAGS_MASKTYPE) - 1) * 4), 4);
				printf("%3i  %2i/%i   %6i   %6i    %c%-16s    %s    0x%04x\n", prg_num, t, s, (((int) d->fsis_h) * 256) + (int) d->fsis_l, file_size(d), flag, tmp, type, load_address(d));
				free(tmp);
			}

			prg_num++;
			d++;
		}

		t=d_save->next_dir_track;
		s=d_save->next_dir_sector;
	}
}

/**
 * List BAM.
 */
static void bam(void) {
	struct BAM *bam;
	int track, idx, bam_idx, v, b, tmp_idx;
	unsigned char *tmp;

	idx = seek_ts(0x12, 0);
	bam = (struct BAM *) (mem_file + idx);

	tmp = unpetify(bam->diskname, 16);
	printf("Label:            %s\n", tmp);
	free(tmp);
	printf("Disk ID:          0x%04x\n", (((int) (bam->disk_id[0])) * 256) + ((int) (bam->disk_id[1])));
	printf("Track/Sector:     %i, %i\n", (int) (bam->dir_track), (int) (bam->dir_sector));

	if (bam->dos_version == 0x41) {
		printf("Disk DOS Version: 'A' (0x41)\n");
	} else {
		if (isprint((int) bam->dos_version)) {
			printf("Disk DOS Version: '%c' (0x%02x) (unusual)\n", bam->dos_version, bam->dos_version);
		} else {
			printf("Disk DOS Version: 0x%02x (unusual)\n", bam->dos_version);
		}
	}

	if ((bam->dos_type[0] == '2') && (bam->dos_type[1] == 'A')) {
		printf("Disk DOS Type:    2A (0x%04x)\n", (((int) (bam->dos_type[0])) * 256) + ((int) (bam->dos_type[1])));
	} else {
		printf("Disk DOS Type:    0x%04x (unusual)\n", (((int) (bam->dos_type[0])) * 256) + ((int) (bam->dos_type[1])));
	}
	
	if ((tmp = (unsigned char *) malloc(35 * 22)) == NULL) {
		fprintf(stderr, "OOM in bam\n");
		exit(1);
	}

	memset(tmp, '-', 35 * 22);
	for (idx = 0; idx < 35; idx++) {
		tmp[(idx * 22) + 21] = 0;
	}

	bam_idx = 1;
	tmp_idx = 0;
	for (track = 1; track < 36; track++) {
		v = (bam->bam[bam_idx + 2] << 16) | (bam->bam[bam_idx + 1] << 8) | (bam->bam[bam_idx]);
		for (idx = 0; idx < sectors[track]; idx++) {
			b = v & 1;
			v >>= 1;
			if (b) {
				*(tmp + tmp_idx + idx) = ' ';
			} else {
				*(tmp + tmp_idx + idx) = '#';
			}
		}

		tmp_idx += 22;
		bam_idx += 4;
	}

	printf("\nBlock Availability Map:\n\n");
	printf("  s|           11111111112\n");
	printf("t  | 012345678901234567890\n");
	printf("---+-----------------------+\n");
	for (idx = 0; idx < 35; idx++) {
		printf("%02i | %s |\n", idx + 1, tmp + (idx * 22));
	}

	printf("   +-----------------------+\n");
}

/**
 * Follow the track/sector link for a given program.
 *
 * @param prog_num Program number.
 */
static void tslink(int prog_num) {
	dir_entry *d = get_dir_entry(prog_num);
	int s_size = 252, idx, t, s;

	idx = seek_ts(d->file_track, d->file_sector);
	if (!*(mem_file + idx)) {
		printf("%i, %i (%i)\n", d->file_track, d->file_sector, (int) *(mem_file + idx + 1));
	} else {
		t = d->file_track;
		s = d->file_sector;
		do {
			printf("%i, %i (%i)\n", t, s, s_size);
			s_size = 254;
			t = *(mem_file + idx);
			s = *(mem_file + idx + 1);
			idx = seek_ts(t, s);
		} while (*(mem_file + idx));

		if (*(mem_file + idx + 1)) {
			printf("%i, %i (%i)\n", t, s, (int)*(mem_file + idx + 1));
		}
	}
}

/**
 * Convert d64 (or part of it) to a t64 image file.
 */
static void convert_to_t64(char *output, char *tape_name, char *track_specs) {
	char *t;
	int idx1, idx2;

	vc_entries = 0;
	vc_list = NULL;
	prev = NULL;

	if (!tape_name) {
		tape_name = basename(input_file);

		if ((t = strrchr(tape_name, '.'))) {
			idx1 = t - tape_name;
			if ((t = (char *) malloc(idx1 + 1)) == NULL) {
				fprintf(stderr, "OOM in Main\n");
				exit(1);
			}

			memmove(t, tape_name, idx1);
			t[idx1] = 0;
			tape_name = t;
		}
	}

	memset(track_seen, 0, sizeof(track_seen));
	for (; *track_specs; ) {
		idx1 = strtol(track_specs, &t, 10);
		if (*t == '-') {
			track_specs = t + 1;
			idx2 = strtol(track_specs, &t, 10);
			if ((t == track_specs) || (*t && (*t != ','))) {
				fprintf(stderr, "Invalid track specifier: '%s'\n", track_specs);
				exit(1);
			}
		} else {
			idx2 = 0;
		}

		if (!idx1) {
			fprintf(stderr, "Invalid track specifier: '%i'\n", idx1);
			exit(1);
		}

		do {
			extract(idx1++);
		} while (idx1 <= idx2);

		if (!*t) {
			break;
		}

		track_specs = t + 1;
	}

	if (vc_entries < 32) {
		if (output == NULL) {
			t64h = create_file(basename(input_file), "t64");
		} else {
			t64h = create_file(output, NULL);
		}
	
		write_signature(t64h);
		write_directory_info(t64h, vc_entries, tape_name);
		write_directory_entries(t64h);
		write_file_entries(t64h);

		fclose(t64h);
	} else {
		fprintf(stderr, "More than 31 files are unsupported\n");
		exit(1);
	}

#if 0
	{
		struct VC *n = vc_List;

		printf("Entries: %i\n", vc_entries);
		while (n) {
			printf("(%i, %i):%i:%s\n", n->s_track, n->s_sector, n->prg_size, n->name);
			n = n->next;
		}
	}
#endif
}

/**
 * Extract a given d64 track to the t64 file.
 *
 * @param t Track num.
 */
void extract(int t) {
	struct VC *vc;
	dir_entry *d;
	unsigned char *name;

	if ((t < 1) || (t > DIR_LIST_SIZE)) {
		fprintf(stderr, "Track out of range: %i\n", t);
		return;
	}

	if (track_seen[t - 1]) {
		return;
	}

	if ((vc = (struct VC *) malloc(sizeof(struct VC))) == NULL) {
		fprintf(stderr, "OOM in Extract\n");
		exit(1);
	}

	memset(vc, 0, sizeof(struct VC));
	vc->next = NULL;

	d = get_dir_entry(t);

	name = unpetify((unsigned char *) (d->filename), 16);
	memset(vc->name, 0, 16);
	int name_len = strlen((const char *)name);
	if (name_len < 16) {
		memcpy(vc->name, name, name_len);
	} else {
		memcpy(vc->name, name, 16);
	}

	vc->prg_type = d->file_flags & F_FLAGS_MASKTYPE;
	vc->s_track = d->file_track;
	vc->s_sector = d->file_sector;
	vc->prg_size = file_size(d);
	vc->load_address = load_address(d);

	if (prev) {
		*prev = vc;
	} else {
		vc_list = vc;
	}

	prev = &vc->next;

	vc_entries++;
}

/**
 * Create file given template and extension.
 *
 * @param src Source name.
 * @param ext Extension (or NULL).
 */
static FILE *create_file(char *src, char *ext) {
	char *p = NULL, *t;
	FILE *f;

	if (ext) {
		int e_len = strlen(ext), len;

		if ((p = strrchr(src, '.'))) {
			len = p - src;
		} else {
			len = strlen(src);
		}

		if ((p = (char *) malloc(len + 1 + e_len + 1)) == NULL) {
			fprintf(stderr, "OOM in create_file\n");
			exit(1);
		}

		t = p;
		memmove(t, src, len);
		t += len;
		*t++ = '.';
		memmove(t, ext, e_len);
		t[e_len] = 0;

		src = p;
	}

	if ((f = fopen(src, "wb+")) == NULL) {
		fprintf(stderr, "Failed to create '%s'\n", src);
		exit(1);
	}

	if (p) {
		free(p);
	}

	return f;
}

/**
 * Write to file.
 *
 * @param h File handle.
 * @param ptr Pointer to data.
 * @param len Amount to write.
 */
static void safe_write(FILE *h, void *ptr, int len) {
	if (fwrite(ptr, len, 1, h) != 1) {
		fprintf(stderr, "Failed to write to tape file\n");
		exit(1);
	}
}

/**
 * Seek in file.
 *
 * @param h File handle.
 * @param pos Position in file.
 */
static void safe_seek(FILE *h, int pos) {
	if (fseek(h, pos, SEEK_SET)) {
		fprintf(stderr, "Failed to seek in tape file\n");
		exit(1);
	}
}

/**
 * Write signature to file.
 *
 * @param h File handle.
 */
static void write_signature(FILE *h) {
	char sig[32];

	safe_seek(h, 0);
	memset(sig, 0, sizeof(sig));
	strncpy(sig, "C64S tape file", sizeof(sig) - 1);
	safe_write(h, sig, sizeof(sig));
}


/**
 * Update the directory info block in the t64 file.
 *
 * @param h File handle.
 * @param entries Number of entries (and used entries) in file.
 * @param name Tape name.
 */
static void write_directory_info(FILE *h, int entries, char *name) {
	int n_len = strlen(name);
	unsigned char d_info[32];

	d_info[0] = 0;		/* Tape version */
	d_info[1] = 1;

	d_info[2] = entries & 0xff;	/* Max num entries on this tape */
	d_info[3] = (entries >> 8) & 0xff;

	d_info[4] = entries & 0xff;	/* Actual num entries on this tape */
	d_info[5] = (entries >> 8) & 0xff;

	d_info[6] = 0;		/* Not used */
	d_info[7] = 0;

	if (n_len > 24) {
		n_len = 24;
	}

	memmove(d_info + 8, name, n_len);
	n_len += 8;
	if (n_len < 32) {
		memset(d_info + n_len, 32, 32 - n_len);
	}

	petify(d_info + 8, 24);
	safe_seek(h, 32);
	safe_write(h, d_info, 32);
}

/**
 * Write the directory entries for the virtual container.
 *
 * @param h File handle.
 */
static void write_directory_entries(FILE *h) {
	struct VC *vc = vc_list;
	int addr = 64 + (vc_entries * 32), tmp;
	unsigned char d_info[32];

	safe_seek(h, 64);

	while (vc) {
		memset(d_info, 0, 16);
		memset(d_info + 16, 32, 16);

		d_info[0] = 1;		/* C64s filetype (normal file) */
		d_info[1] = vc->prg_type | F_FLAGS_CLOSED;	/* 1541 filetype */

		d_info[2] = vc->load_address & 0xff;	/* Load address 0x0801 */
		d_info[3] = (vc->load_address >> 8) & 0xff;

		tmp = vc->load_address + vc->prg_size;
		d_info[4] = tmp & 0xff;	/* End address */
		d_info[5] = (tmp >> 8) & 0xff;

		d_info[8] = addr & 0xff;	/* Offset into file (program) */
		d_info[9] = (addr >> 8) & 0xff;

		memmove(d_info + 16, vc->name, strlen(vc->name));
		petify(d_info + 16, 16);

		safe_write(h, d_info, sizeof(d_info));

		addr += vc->prg_size;
		vc = vc->next;
	}

	if (vc_entries < 32) {
		memset(d_info, 0, sizeof(d_info));
		for (addr = 0; addr < (31 - vc_entries); addr++) {
			safe_write(h, d_info, sizeof(d_info));
		}
	}
}

/**
 * Write the file entries for the virtual container.
 *
 * @param h File handle.
 */
static void write_file_entries(FILE *h) {
	struct VC *vc = vc_list;
	int len, idx, t, s;

	safe_seek(h, 64 + (vc_entries * 32));
	while (vc) {
		idx = seek_ts(vc->s_track, vc->s_sector);

		/* First track is short as it contains the load address */
		len = (*(mem_file + idx)) ? 252 : *(mem_file + idx + 1);
		safe_write(h, mem_file + idx + 4, len);

		/* Write inbetween tracks */
		while (*(mem_file + idx)) {
			t = (int) *(mem_file + idx);
			s = (int) *(mem_file + idx + 1);
			idx = seek_ts(t, s);

			if (!*(mem_file + idx)) {
				/* Write last track */
				if (*(mem_file + idx + 1)) {
					safe_write(h, mem_file + idx + 2, (int) ((*(mem_file + idx + 1))));
				}

				break;
			} else {
				safe_write(h, mem_file + idx + 2, 254);
			}
		}

		vc = vc->next;
	}
}
