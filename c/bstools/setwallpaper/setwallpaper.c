#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <windows.h>
#define MAIN
#include "common.h"

#define OS_VER_UNKNOWN 0
#define OS_VER_WIN2000 1
#define OS_VER_WINXP 2
#define OS_VER_WIN2003 3
#define OS_VER_WIN2003R2 4
#define OS_VER_VISTA 5
#define OS_VER_WIN2008 6
#define OS_VER_WINDOWS7 7
#define OS_VER_WIN2008R2 8

/* list with filenames */
struct node {
	struct node *next;
};

/* forward decls */
static void usage(int ex);
static int _main(int argc, char *argv[]);
static int is_valid_file(char *f);
static int strrpos(char *str, int ch);
static void list_files(void);
static int discover_windows(void);
static void set_wallpaper(char *f);

/* globals */
static struct node *lst = NULL;
static int lst_cnt = 0;

/**
 * Write Usage.
 *
 * @param ex Exit code.
 */
static void usage(int ex) {
	FILE *f = (ex) ? stderr : stdout;

	fprintf(f, "setwallpaper %s\r\n", bstools_version);
	fprintf(f, "Usage: setwallpaper directory|file\r\n\r\n");
	exit(ex);
}

/**
 * Main ...
 */
#ifdef CAN_WINMAIN
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int nCmdShow) {
	char *argv[] = { "setwallpaper.exe", NULL, NULL };

	argv[1] = cmdLine;
	return _main(2, argv);
}

#else
int main(int argc, char *argv[]) {
	return _main(argc, argv);
}
#endif

/**
 * Real main.
 */
int _main(int argc, char *argv[]) {
	struct stat s_buf;

	if (argc != 2) {
		usage(1);
	}

	if (stat(argv[1], &s_buf) != 0) {
		perror(argv[1]);
		exit(1);
	}

	if (S_ISREG(s_buf.st_mode)) {
		if (!is_valid_file(argv[1])) {
			fprintf(stderr, "Not a wallpaper file (*.gif, *.jpg, *.png)\r\n");
			exit(1);
		}

		set_wallpaper(argv[1]);
	} else if (S_ISDIR(s_buf.st_mode)) {
		struct node *n;
		int idx;

		if (chdir(argv[1])) {
			perror(argv[1]);
			exit(1);
		}

		srand(time(NULL));
		list_files();
		if (lst_cnt > 0) {
			idx = rand() % lst_cnt;
			for (n = lst; (n) && (idx > 0); n = n->next) {
				idx--;
			}

			set_wallpaper(((char *) n) + sizeof(struct node));
		}
	} else {
		fprintf(stderr, "'%s' is not a file or directory\r\n", argv[1]);
		exit(1);
	}

	exit(0);
}

/**
 * Is the supplied file a valid file? (Only the extension is checked).
 *
 * @param f File name.
 * @return 1 if this file may be used, 0 otherwise.
 */
static int is_valid_file(char *f) {
	int i = strrpos(f, '.');

	if (i > -1) {
		f += i;
		f++;
		if ((stricmp(f, "jpg") == 0) || (stricmp(f, "jpeg") == 0) ||
		    (stricmp(f, "png") == 0) || (stricmp(f, "gif") == 0) ||
		    (stricmp(f, "bmp") == 0)) {
			return 1;
		}
	}

	return 0;
}

/**
 * Find the last occurrence of ch in str.
 *
 * @param str String to search.
 * @param ch Char to find.
 * @return position or -1 if not found.
 */
static int strrpos(char *str, int ch) {
	int len = strlen(str);

	for (len = strlen(str) - 1; len >= 0; len--) {
		if (str[len] == ch) {
			return len;
		}
	}

	return -1;
}

/**
 * Build the list of files in the current directory.
 */
static void list_files(void) {
	WIN32_FIND_DATA f_data;
	HANDLE dh;
	struct node *n;

	if ((dh = FindFirstFile(".\\*", &f_data)) == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "Cannot open directory for reading\r\n");
		exit(1);
	}

	while (FindNextFile(dh, &f_data)) {
		if (is_valid_file(f_data.cFileName)) {
			int len = strlen(f_data.cFileName) + 1;

			if ((n = (struct node *) malloc(sizeof(struct node) + len)) == NULL) {
				perror("malloc");
				exit(1);
			}

			memmove(((char *) n) + sizeof(struct node), f_data.cFileName, len);
			n->next = lst;
			lst = n;

			lst_cnt++;
		}
	}

	FindClose(dh);
	if (GetLastError() != ERROR_NO_MORE_FILES) {
		fprintf(stderr, "Error while reading directory\r\n");
		exit(1);
	}
}

/**
 * Discover windows version.
 *
 * @return windows version.
 */
static int discover_windows(void) {
	int os_version = OS_VER_UNKNOWN;
	OSVERSIONINFOEX os_ver;

	os_ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((OSVERSIONINFO *) &os_ver);

	if (os_ver.dwPlatformId == VER_PLATFORM_WIN32_NT) {
		if (os_ver.dwMajorVersion == 5) {
			switch (os_ver.dwMinorVersion) {
				case 0:
					os_version = OS_VER_WIN2000;
					break;

				case 1:
					os_version = OS_VER_WINXP;
					break;

				case 2:
					if (GetSystemMetrics(89) == 0) {
						os_version = OS_VER_WIN2003;
					} else {
						os_version = OS_VER_WIN2003R2;
					}
					break;
			}
		} else if (os_ver.dwMajorVersion == 6) {
			switch (os_ver.dwMinorVersion) {
				case 0:
					if (os_ver.wProductType == VER_NT_WORKSTATION) {
						os_version = OS_VER_VISTA;
					} else {
						os_version = OS_VER_WIN2008;
					}
					break;

				case 1:
					if (os_ver.wProductType == VER_NT_WORKSTATION) {
						os_version = OS_VER_WINDOWS7;
					} else {
						os_version = OS_VER_WIN2008R2;
					}
					break;
			}
		}
	}

	return os_version;
}

/**
 * Set wallpaper.
 *
 * @param f File.
 */
static void set_wallpaper(char *f) {
	struct stat s_buf;

	if (stat(f, &s_buf) == 0) {
		int os = discover_windows();

		if (os < OS_VER_VISTA) {
			SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, f, SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
			PostMessage(GetDesktopWindow(), WM_SETTINGCHANGE, 0, 0);
		} else {
			SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, f, SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
		}
	}
}
