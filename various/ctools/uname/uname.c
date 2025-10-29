#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <windows.h>
#define MAIN
#include "common.h"

#define UTSNAMELEN 65

/* Structure describing the system and machine.  */
struct utsname {
	char sys_name[UTSNAMELEN];
	char node_name[UTSNAMELEN];
	char release[UTSNAMELEN];
	char version[UTSNAMELEN];
	char machine[UTSNAMELEN];
};

/* forward decls */
static void usage(int ex);
static int uname(struct utsname *uts);

/**
 * Write Usage.
 *
 * @param ex Exit code.
 */
static void usage(int ex) {
	FILE *f = (ex) ? stderr : stdout;

	fprintf(f, "uname %s\r\n", bstools_version);
	fprintf(f, "Usage: uname [OPTIONS]\r\n\r\n");
	fprintf(f, "[OPTIONS]\r\n");
	fprintf(f, "  -a   Print all info\r\n");
	fprintf(f, "  -h   Help (this page)\r\n");
	fprintf(f, "  -m   Machine hardware platform\r\n");
	fprintf(f, "  -n   Network name of this machine\r\n");
	fprintf(f, "  -r   Release level of the operating system\r\n");
	fprintf(f, "  -s   Name of the operating system implementation\r\n");
	fprintf(f, "  -v   Version level of the operating system\r\n");
	exit(ex);
}

/**
 * Main ...
 */
int main(int argc, char *argv[]) {
	struct {
		unsigned machine : 1;
		unsigned nodename : 1;
		unsigned release : 1;
		unsigned systemname : 1;
		unsigned version : 1;
	} flags = { 0, 0, 0, 0, 0 };;
	int space = 0;
	struct utsname u;
	int opt;

	while ((opt = getopt(argc, argv, "ahmnrsv")) != -1) {
		switch (opt) {
			case 'a':
				flags.machine = 1;
				flags.nodename = 1;
				flags.release = 1;
				flags.systemname = 1;
				flags.version = 1;
				break;

			case 'h':
				usage(0);

			case 'm':
				flags.machine = 1;
				break;

			case 'n':
				flags.nodename = 1;
				break;

			case 'r':
				flags.release = 1;
				break;

			case 's':
				flags.systemname = 1;
				break;

			case 'v':
				flags.version = 1;
				break;

			default:
				fprintf(stderr, "Unknown argument: '%c'!\r\n", (char) opt);
				exit(1);
		}
	}

	if ((flags.machine == 0) && (flags.nodename == 0) &&
	    (flags.release == 0) && (flags.systemname == 0) &&
	    (flags.version == 0)) {
		flags.systemname = 1;
	}

	if (uname(&u) != 0) {
		perror("uname");
		exit(1);
	}

	if (flags.systemname) {
		fputs(u.sys_name, stdout);
		space++;
	}

	if (flags.nodename) {
		if (space) {
			putchar(' ');
		}

		fputs(u.node_name, stdout);
		space++;
	}

	if (flags.release) {
		if (space) {
			putchar(' ');
		}

		fputs(u.release, stdout);
		space++;
	}

	if (flags.version) {
		if (space) {
			putchar(' ');
		}

		fputs(u.version, stdout);
		space++;
	}

	if (flags.machine) {
		if (space) {
			putchar(' ');
		}

		fputs(u.machine, stdout);
		space++;
	}
	
	fputs("\r\n", stdout);
	exit(0);
}

/**
 * Get name and information about current kernel.
 *
 * @param uts utsname structure.
 * @return 0 (always).
 */
static int uname(struct utsname *uts) {
	DWORD len;
	OSVERSIONINFOEX os_ver;
	SYSTEM_INFO sys_info;
	WORD arch;

	memset(uts, 0, sizeof(struct utsname));

	os_ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((OSVERSIONINFO *)&os_ver);

	strcpy(uts->sys_name, "Unknown");
	if (os_ver.dwPlatformId == VER_PLATFORM_WIN32_NT) {
		if (os_ver.dwMajorVersion == 5) {
			if (os_ver.dwMinorVersion == 0) {
				strcpy(uts->sys_name, "Windows 2000");
			} else if (os_ver.dwMinorVersion == 1) {
				strcpy(uts->sys_name, "Windows XP");
			} else if (os_ver.dwMinorVersion == 2) {
				if (GetSystemMetrics(89) == 0) {
					/* 89 = SM_SERVER2 */
					strcpy(uts->sys_name, "Windows Server 2003");
				} else {
					strcpy(uts->sys_name, "Windows Server 2003 R2");
				}
			}
		} else if (os_ver.dwMajorVersion == 6) {
			if (os_ver.dwMinorVersion == 0) {
				if (os_ver.wProductType == VER_NT_WORKSTATION) {
					strcpy(uts->sys_name, "Windows Vista");
				} else {
					strcpy(uts->sys_name, "Windows Server 2008");
				}
			} else if (os_ver.dwMinorVersion == 1) {
				if (os_ver.wProductType == VER_NT_WORKSTATION) {
					strcpy(uts->sys_name, "Windows 7");
				} else {
					strcpy(uts->sys_name, "Windows Server 2008 R2");
				}
			}
		}
	}

	sprintf(uts->version, "%ld.%02ld", os_ver.dwMajorVersion, os_ver.dwMinorVersion);

	if ((os_ver.szCSDVersion[0] != '\0') &&
	    ((strlen(os_ver.szCSDVersion) + strlen(uts->version) + 1) < UTSNAMELEN)) {
		strcat(uts->version, " ");
		strcat(uts->version, os_ver.szCSDVersion);
    	}

	sprintf(uts->release, "build %ld", os_ver.dwBuildNumber & 0xFFFF);

	GetSystemInfo(&sys_info);

	arch = sys_info.wProcessorArchitecture;
	if (arch == PROCESSOR_ARCHITECTURE_PPC) {
		strcpy(uts->machine, "ppc");
	} else if (arch == PROCESSOR_ARCHITECTURE_ALPHA) {
		strcpy (uts->machine, "alpha");
	} else if (arch == PROCESSOR_ARCHITECTURE_MIPS) {
		strcpy (uts->machine, "mips");
	} else if (arch == PROCESSOR_ARCHITECTURE_INTEL) {
		sprintf(uts->machine, "i%d86", sys_info.wProcessorLevel);
	} else {
		strcpy(uts->machine, "unknown");
	}

	len = UTSNAMELEN - 1;
	GetComputerName(uts->node_name, &len);
	return 0;
}
