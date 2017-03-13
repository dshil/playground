#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "reader.h"

static int read_tail_lines(FILE *f, char *filename);
static int read_tail_bytes(FILE *f, char *filename);

static int nlines = 10;
static int nbytes = -1;

int main(int ac, char *av[])
{
	char *nlineval = NULL;
	char *nbyteval = NULL;
	int suppress_file_name = 0;

	int opt = 0;
	while((opt = getopt(ac, av, "qnc:")) != -1) {
		switch(opt) {
			case 'q': suppress_file_name = 1; break;
			case 'n': nlineval = optarg; break;
			case 'c': nbyteval = optarg; break;
			default:
				fprintf(stderr,
						"Usage: %s [-q] [-n lines | -b blocks | -c bytes] [file ...]\n",
						av[0]);
				exit(EXIT_FAILURE);
		}
	}

	if (nlineval != NULL && nbyteval != NULL) {
		fprintf(stderr,
				"Usage: %s [-q] [-n lines | -b blocks | -c bytes] [file ...]\n",
				av[0]);
		exit(EXIT_FAILURE);
	}

	if (parse_num(nlineval, &nlines) == -1)
		exit(EXIT_FAILURE);

	if (parse_num(nbyteval, &nbytes) == -1)
		exit(EXIT_FAILURE);

	struct read_config config;

	if (nbytes != -1)
		config.read_file = read_tail_bytes;
	else
		config.read_file = read_tail_lines;

	if (ac == optind) {
		if (config.read_file(stdin, "stdin") == -1)
			exit(EXIT_FAILURE);
	} else {
		config.is_print = ((ac - optind) > 1) && !suppress_file_name;
		config.argv = av;
		config.ac = ac;

		if (read_files(&config) == -1) {
			exit(EXIT_FAILURE);
		}
	}

	exit(EXIT_SUCCESS);
}

static int read_tail_lines(FILE *f, char *filename)
{
	int c = 0;
	int n = 0;
	int nc = 0; /* number of characters to print */

	if (fseek(f, 0, SEEK_END) == -1) {
		perror(filename);
		return -1;
	}

	do {
		if (fseek(f, -1, SEEK_CUR) == -1) {
			// the beginning of the file was reached, it's time to break
			break;
		}

		c = getc(f);
		if (ferror(f) != 0) {
			perror(filename);
			return -1;
		}

		if (c == '\n' && (++n == (nlines + 1)))
			continue;

		nc++;
		if (ungetc(c, f) == EOF) {
			perror(filename);
			return -1;
		}

	} while (n != (nlines+1));

	return read_and_print_bytes(f,filename, nc);
}

static int read_tail_bytes(FILE *f, char *filename)
{
	if (fseek(f, -nbytes, SEEK_END) == -1) {
		perror(filename);
		return -1;
	}

	return read_and_print_bytes(f, filename, nbytes);
}
