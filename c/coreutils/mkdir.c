#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h>

static void usage(char *progname);
static int is_valid_mode(char *s);
static int mkdir_all(char *dir, mode_t m);

int main(int ac, char *av[])
{
	int opt = 0;
	int deep = 0;
	char *smode = NULL;

	while ((opt = getopt(ac, av, "m:p")) != -1) {
		switch(opt) {
			case 'm': smode = optarg; break;
			case 'p': deep = 1; break;
			default:
				  usage(av[0]);
				  exit(EXIT_FAILURE);
		}
	}
	mode_t mask = 0777;

	if (smode != NULL) {
		if (!is_valid_mode(smode)) {
			fprintf(stderr, "%s: invalid mode %s\n", *av, smode);
			exit(EXIT_FAILURE);
		}
		mask = strtol(smode, NULL, 8);
	}

	if (ac == optind) {
		fprintf(stderr, "%s: missing operand\n", av[0]);
		exit(EXIT_FAILURE);
	} else if ((ac - optind) == 1) {
		char *dir = *(av+optind);
		if (deep) {
			if (mkdir_all(dir, mask) == -1)
				exit(EXIT_FAILURE);
		} else {
			if (mkdir(dir, mask) == -1) {
				fprintf(stderr, "mkdir, err = ");
				perror(dir);
				exit(EXIT_FAILURE);
			}
		}
	} else {
		char **dirs = av + optind;
		char *dir = NULL;
		while(--ac >= optind) {
			dir = *dirs++;
			if (mkdir(dir, mask) == -1) {
				fprintf(stderr, "mkdir, err = ");
				perror(dir);
				exit(EXIT_FAILURE);
			}
		}
	}

	exit(EXIT_SUCCESS);
}

static int mkdir_all(char *dir, mode_t m)
{
	char *p = NULL, *q = NULL;
	char path[PATH_MAX];

	for (q = dir; (p = strtok(q, "/")) != NULL; q = NULL) {
		strcat(path, p);
		strcat(path, "/");

		if (mkdir(path, m) == -1) {
			fprintf(stderr, "mkdir, err = ");
			perror(path);
			return -1;
		}
	}

	return 0;
}

static int is_valid_mode(char *s)
{
	int cnt = 0;
	while (*s != '\0') {
		if (cnt++ >= 4)
			return 0;

		if (!(*s >= '0' && *s < '8')) {
			return 0;
		}
		s++;
	}
	return 1;
}

static void usage(char *progname)
{
	fprintf(stderr, "Usage: %s [-p] [-m MODE] [-Z] [file...]\n", progname);
}
