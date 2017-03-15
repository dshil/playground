#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "reader.h"

static int read_cat(FILE *f, char *filename);

int main(int ac, char *av[])
{
	int suppress_file_name = 0;

	int opt = 0;
	while((opt = getopt(ac, av, "q")) != -1) {
		switch(opt) {
			case 'q': suppress_file_name = 1; break;
			default:
					  fprintf(stderr, "Usage %s [-q] [file ...]\n", av[0]);
					  exit(EXIT_FAILURE);
		}
	}

	struct read_config config;
	config.read_file = read_cat;

	if (ac == optind) {
		if (config.read_file(stdin, "stdin") == -1)
			exit(EXIT_FAILURE);
	} else {
		config.is_print = ((ac - optind) > 1) && !suppress_file_name;
		config.argv = av;
		config.ac = ac;

		if (read_files(&config) == -1)
			exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

static int read_cat(FILE *f, char *filename)
{
	int c = 0;
	while ((c = getc(f)) != EOF) {
		if (ferror(f) != 0) {
			perror(filename);
			return -1;
		}

		if (putchar(c) == EOF) {
			perror(filename);
			return -1;
		}
	}

	return 0;
}
