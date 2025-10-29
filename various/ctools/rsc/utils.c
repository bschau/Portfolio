#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include "include.h"

#define LINE_LEN 1024
#define BUFFER_LEN (1024 * 1024)

static int is_token(char *p, char *t);
extern int errno;

void config_read(void)
{
	FILE *fp = fopen(RCFILE, "rb");
	int line = 0;
	char buffer[LINE_LEN + 2];
	char *s, *token;

	if (fp == NULL)
	{
		perror(RCFILE);
		exit(1);
	}

	while ((s = xgets(buffer, LINE_LEN, fp)) != NULL)
	{
		line++;
		if (!*s || *s == '#')
		{
			continue;
		}

		token = s;
		for (; *s && isalnum((int) *s); s++)
			;

		if (!*s)
		{
			fprintf(stderr, "Token '%s' with no value on line %i\n", token, line);
			exit(1);
		}

		*s++ = 0;
		for (; *s && isspace((int) *s); s++)
			;

		if (!*s)
		{
			fprintf(stderr, "Token '%s' with no value on line %i\n", token, line);
			exit(1);
		}

		if (strcmp(token, "command") == 0)
		{
			command = xstrdup(s);
		}
		else if (strcmp(token, "module") == 0)
		{
			module = xstrdup(s);
		}
		else if (strcmp(token, "remoteconfig") == 0)
		{
			remote_config = xstrdup(s);
		}
		else if (strcmp(token, "remotersync") == 0)
		{
			remote_rsync = xstrdup(s);
		}
		else
		{
			fprintf(stderr, "Unknown token '%s' on line %i\n", token, line);
			exit(1);
		}
	}

	fclose(fp);

	stamp = 0;
	fp = fopen(STAMPFILE, "rb");
	if (fp != NULL)
	{
		s = xgets(buffer, LINE_LEN, fp);
		if (s != NULL)
		{
			stamp = (time_t) strtoul(s, NULL, 10);
		}

		fclose(fp);
	}
}

void config_write(void)
{
	FILE *fp;

	fp = fopen(RCFILE, "wb");
	if (fp == NULL)
	{
		perror(RCFILE);
		exit(1);
	}

	fprintf(fp, "command %s\n", command);
	fprintf(fp, "module %s\n", module);
	fprintf(fp, "remoteconfig %s\n", remote_config);
	fprintf(fp, "remotersync %s\n", remote_rsync);
	fclose(fp);
}

void config_find(void)
{
	char path1[PATH_MAX + 1], path2[PATH_MAX + 1];
	struct stat s_buf;
	char *s1, *s2;

	while (stat(RCFILE, &s_buf))
	{
		s1 = getcwd(path1, PATH_MAX);
		if (chdir("..") != 0)
		{
			perror("..");
			exit(1);
		}

		s2 = getcwd(path2, PATH_MAX);
		if (strcmp(s1, s2) == 0)
		{
			fprintf(stderr, "Cannot find root .RSC\n");
			exit(1);
		}
	}

	if (getcwd(root_dir, PATH_MAX) == NULL)
	{
		fprintf(stderr, "Cannot get root directory\n");
		exit(1);
	}

	root_dir_len = strlen(root_dir);
}

void config_write_stamp(void)
{
	time_t now = time(NULL);
	char p[PATH_MAX + 1];
	FILE *fp;

	snprintf(p, PATH_MAX, "%s/%s", root_dir, STAMPFILE);
	exit_if_link(p);

	fp = fopen(p, "wb");
	if (fp == NULL)
	{
		fprintf(stderr, "Cannot write to stamp file\n");
		exit(1);
	}

	fprintf(fp, "%i", (int) now);
	fclose(fp);
}

char *xgets(char *dst, int len, FILE *fp)
{
	char *s = fgets(dst, len, fp);

	if (s)
	{
		char *start;

		for (; *s && isspace((int) *s); s++)
			;

		if (!*s)
		{
			return s;
		}

		start = s;
		s += strlen(s) - 1;
		while (s >= start)
		{
			if (isspace((int) *s))
			{
				*s = 0;
			}
			else
			{
				break;
			}

			s--;
		}

		return start;
	}

	return NULL;
}

void set_file_permissions(char *path, int mode)
{
	if (chmod(path, mode))
	{
		fprintf(stderr, "Warning: cannot set permissions on %s to %#o\n", path, mode);
	}
}

void get_cwd(char *dst)
{
	if (getcwd(dst, PATH_MAX) == NULL)
	{
		fprintf(stderr, "where am I?\n");
		exit(1);
	}
}

int is_locked(void)
{
	char p[PATH_MAX + 1];
	char *s;
	FILE *fp;
	struct stat s_buf;

	snprintf(p, PATH_MAX, "%s/%s", root_dir, LOCKFILE);
	if (stat(p, &s_buf))
	{
		return -1;
	}

	exit_if_link(p);

	fp = fopen(p, "rb");
	if (fp == NULL)
	{
		fprintf(stderr, "Cannot open lockfile\n");
		exit(1);
	}

	s = fgets(p, PATH_MAX, fp);
	fclose(fp);

	if (s)
	{
		return atoi(s);
	}

	return 0;
}

void exit_if_link(char *p)
{
#ifndef WIN32
	struct stat s_buf;

	if (lstat(p, &s_buf) == 0)
	{
		if (S_ISLNK(s_buf.st_mode))
		{
			fprintf(stderr, "%s is a symbolic link\n", p);
			fprintf(stderr, "Please remove the link or turn it into a regular file\n");
			exit(1);
		}
	}
#endif
}

StringBuilder command_expand(int show)
{
	StringBuilder s_bld = stringbuilder_new(512);
	char *p;

	if (s_bld == NULL)
	{
		fprintf(stderr, "Out of memory in cmd_expand\n");
		exit(1);
	}

	for (p = command; *p; )
	{
		if (*p == '$') {
			p++;
			if (is_token(p, "RSCSHOW"))
			{
				if (show)
				{
					stringbuilder_append(s_bld, "-n", 2);
				}
				p += 7;
			}
			else if (is_token(p, "RSCMODULE"))
			{
				stringbuilder_append(s_bld, module, strlen(module));
				p += 9;
			}
			else if (is_token(p, "RSCREMOTECONFIG"))
			{
				stringbuilder_append(s_bld, remote_config, strlen(remote_config));
				p += 15;
			}
			else if (is_token(p, "RSCREMOTERSYNC"))
			{
				stringbuilder_append(s_bld, remote_rsync, strlen(remote_rsync));
				p += 14;
			}
			else
			{
				fprintf(stderr, "Unknown token '%s'\n", p);
				exit(1);
			}
		}
		else
		{
			stringbuilder_append_char(s_bld, *p);
			p++;
		}
	}

	return s_bld;
}

static int is_token(char *p, char *t)
{
	int t_len = strlen(t);

	if (strncmp(p, t, t_len) == 0)
	{
		p += t_len;
		if (isalnum((int) *p) || *p == '_')
		{
			return 0;
		}

		return 1;
	}

	return 0;
}
