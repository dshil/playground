#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

static void show_usage();

int main(int ac, char *av[])
{
	if (ac < 2) {
		show_usage();
		exit(EXIT_FAILURE);
	}

	if (av[1] == NULL || av[2] == NULL) {
		show_usage();
		exit(EXIT_FAILURE);
	}

	if (strcmp(av[1], av[2]) == 0) {
		fprintf(stderr, "cp: %s and %s are identical (not copied).\n",
			av[1], av[2]);
		exit(EXIT_FAILURE);
	}

	ssize_t rfd = 0;
	ssize_t wfd = 0;

	if ((rfd = open(av[1], O_RDONLY)) == -1) {
		perror(av[1]);
		exit(EXIT_FAILURE);
	}

	if ((wfd = creat(av[2], 0644)) == -1) {
		perror(av[2]);
		goto error;
	}

	char buf[BUFSIZ];
	ssize_t n = 0;

	while ((n = read(rfd, buf, BUFSIZ)) > 0) {
		if ((write(wfd, buf, n)) != n) {
			perror(av[2]);
			goto error;
		}
	}

	exit(EXIT_SUCCESS);

error:
	if (rfd != -1) {
		if ((close(rfd)) == -1) {
			perror(av[1]);
		}
	}

	if (wfd != -1) {
		if ((close(wfd)) == -1) {
			perror(av[2]);
		}
	}

	exit(EXIT_FAILURE);
}

static void show_usage()
{
	fprintf(stderr, "usage: %s\n%39s\n", "source_file target_file",
					"source_file ... target_directory");
}
