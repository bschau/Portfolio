/**
 * Reading and parsing of properties files.
 */
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "Include.h"

/* protos */
static void ReadFile(char *, char *, size_t);
static void PropParse(char *, char *);

/**
 * Read a property file and merge it into the vars table.
 *
 * @param file Property file to read.
 */
void
PropertiesRead(char *file)
{
	struct stat sBuf;
	char *mem;

	if (stat(file, &sBuf)!=0) {
		XError(NULL, "Failed to stat %s", file);
	}

	mem=UAlloc(NULL, sBuf.st_size+2);
	ReadFile(file, mem, sBuf.st_size);
	PropParse(file, mem);
	free(mem);
}

/**
 * Read the file into memory.
 *
 * @param file File name.
 * @param mem Memory.
 * @param size Size of file.
 */
static void
ReadFile(char *file, char *mem, size_t size)
{
	FILE *fp=fopen(file, "rb");

	if (fp==NULL) {
		XError(NULL, "Failed to open %s", file);
	}

	if (fread(mem, size, 1, fp)!=1) {
		XError(NULL, "Failed to read %s", file);
	}
	
	mem[size]=0;
	mem[size+1]=0;
	fclose(fp);
}

/**
 * PropParse the property file.
 *
 * @param file File name.
 * @param mem Memory.
 */
static void
PropParse(char *file, char *mem)
{
	StringBuilder sb=SBNew(512);
	char *token, *value, *s;
	int lineNo=1;

	if (sb==NULL) {
		XError(NULL, "Failed to allocate StringBuilder");
	}

	for (EVER) {
		for (; *mem && isspace(*mem); mem++) {
			if (*mem=='\n') {
				lineNo++;
			}
		}

		if (!*mem) {
			break;
		}

		if (*mem=='#') {
			for (; *mem; mem++) {
				if (*mem=='\n') {
					lineNo++;
					break;
				}
			}
			continue;
		}

		token=mem;
		for (; *mem && !isspace(*mem); mem++) {
			if (*mem=='\n') {
				XError(NULL, "%i:Unterminated identifier", lineNo);
			}
		}

		if (!*mem) {
			XError(NULL, "%i:Unterminated identifier", lineNo);
		}

		*mem++=0;

		for (; *mem && isspace(*mem); mem++) {
			if (*mem=='\n') {
				XError(NULL, "%i:Unterminated identifier", lineNo);
			}
		}

		if (*mem!='=') {
			XError(NULL, "%i:Invalid declaration - must be identifier = token", lineNo);
		}

		mem++;

		for (; *mem && isspace(*mem); mem++) {
			if (*mem=='\n') {
				lineNo++;
				break;
			}
		}

		if ((*mem=='\n') || (!*mem)) {
			mem++;
			HashDelete(vars, token);
		} else {
			char c=*mem;

			if ((c=='"') || (c=='\'')) {
				mem++;
			} else {
				c=0;
			}

			value=mem;
			for (; *mem; mem++) {
				if (c) {
			       		if (*mem==c) {
						*mem=0;
						break;
					} else if (*mem=='\n') {
						lineNo++;
					}
				} else {
					if (*mem=='\n') {
						lineNo++;
					}
					if (isspace(*mem)) {
						*mem=0;
						break;
					}
				}
			}

			s=UAlloc(NULL, mem-value);
			memmove(s, value, mem-value);
			XOut(XODebug, "Adding (%s, %s)", token, s);
			HashAdd(vars, token, s, mem-value, 1);

			mem++;
		}
	}
}
