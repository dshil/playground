#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/stat.h>

/*
	TODO:
	- support for the following options:
		- [] f
		- [] R
		- [] v
		- [] X
	- handle C-Z correctly.
*/

static int overwrite_file(char *filename);
static int gc(int rfd, char *r_file, int wfd, char *w_file);
static void show_usage();

static const int MAXBUFSIZ = BUFSIZ * 4;

int main(int ac, char *av[])
{
	if (ac < 2) {
		show_usage();
		exit(EXIT_FAILURE);
	}

	if (av[ac-2] == NULL || av[ac-1] == NULL) {
		show_usage();
		exit(EXIT_FAILURE);
	}

	if (strcmp(av[ac-2], av[ac-1]) == 0) {
		fprintf(stderr, "cp: %s and %s are identical (not copied).\n",
			av[1], av[2]);
		exit(EXIT_FAILURE);
	}

	bool is_file_overwrite = false;
	int opt = 0;

	while((opt = getopt(ac, av, "i")) != -1) {
		switch(opt) {
			case 'i': is_file_overwrite = true; break;
			default:
				fprintf(stderr, "Usage: %s [i] source_file target_file\n",
					av[0]);
				exit(EXIT_FAILURE);
		}
	}

	ssize_t rfd = 0;
	ssize_t wfd = 0;

	char buf[MAXBUFSIZ];
	ssize_t n = 0;

	if ((rfd = open(av[ac-2], O_RDONLY)) == -1) {
		perror(av[ac-2]);
		goto error;
	}

	if (is_file_overwrite) {
		int overwrite = overwrite_file(av[ac-1]);

		if (overwrite == -1)
			goto error;

		if (overwrite == 0) {
			fprintf(stderr, "not overwritten\n");
			goto success;
		}
	}

	mode_t mode = 0;
	if (access(av[ac-1], F_OK) != -1) {
		struct stat sb;
		if (stat(av[ac-1], &sb) == -1) {
			perror("stat");
			goto error;
		}
		mode = sb.st_mode;
	}
	if (mode == 0)
		mode = 0644;

	if ((wfd = creat(av[ac-1], mode)) == -1) {
		perror(av[ac-1]);
		goto error;
	}

	while ((n = read(rfd, buf, MAXBUFSIZ)) > 0) {
		if ((write(wfd, buf, n)) != n) {
			perror(av[ac-1]);
			goto error;
		}
	}
	goto success;

success:
	gc(rfd, av[ac-2], wfd, av[ac-1]) == -1 ? exit(EXIT_FAILURE) : exit(EXIT_SUCCESS);

error:
	gc(rfd, av[ac-2], wfd, av[ac-1]);
	exit(EXIT_FAILURE);
}

static int gc(int rfd, char *r_file, int wfd, char *w_file)
{
	int is_err = 0;

	if (rfd != -1) {
		if ((close(rfd)) == -1) {
			is_err = -1;
			perror(r_file);
		}
	}

	if (wfd != -1) {
		if ((close(wfd)) == -1) {
			is_err = -1;
			perror(w_file);
		}
	}

	return is_err;
}

static int overwrite_file(char *filename)
{
	FILE *tty = NULL;

	if (access(filename, F_OK) != -1) {
		// ask user about overwrite the existing file or not.
		fprintf(stderr, "overwrite %s? (y/n [n]) ", filename);

		tty = fopen("/dev/tty", "r");
		if (tty == NULL) {
			perror("/dev/tty");
			goto error;
		}

		int c = getc(tty);
		if (ferror(tty) != 0) {
			perror("/dev/tty");
			goto error;
		}

		if (tolower(c) != 'y') {
			// not overwritten
			if (fclose(tty) == EOF) {
				perror("/dev/tty");
				return -1;
			}
			return 0;
		}
	}

	if (tty != NULL) {
		if (fclose(tty) == EOF) {
			perror("/dev/tty");
			return -1;
		}
	}

	return 1;

error:
	if (tty != NULL)
		if (fclose(tty) == EOF) {
			perror("/dev/tty");
			return -1;
		}

	return -1;
}

static void show_usage()
{
	fprintf(stderr, "usage: %s\n%39s\n", "source_file target_file",
					"source_file ... target_directory");
}
