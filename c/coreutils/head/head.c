#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int file_head(FILE *f, char *filename);
static int files_head(const int lo, const int hi, char *files[]);

static int nlines = 10;

int main(int ac, char *av[])
{
	char *nvalue = NULL;

	int opt = 0;
	while((opt = getopt(ac, av, "n:")) != -1) {
		switch(opt) {
			case 'n':
				nvalue = optarg;
				break;
			default:
				fprintf(stderr, "Usage: %s [-n lines] [file ...]\n", av[0]);
				exit(EXIT_FAILURE);
		}
	}

	if (nvalue != NULL) {
		errno = 0;
		int n = atoi(nvalue);

		if (errno != 0) {
			perror(nvalue);
			exit(EXIT_FAILURE);
		}

		nlines = n;
	}

	// read files or stdin
	if (ac == 1) {
		// read from stdin
		if (file_head(stdin, "stdin") == -1) {
			exit(EXIT_FAILURE);
		}
	} else {
		if (files_head(optind, ac, av) == -1) {
			exit(EXIT_FAILURE);
		}
	}

	exit(EXIT_SUCCESS);
}

static int files_head(const int lo, const int hi, char *files[])
{
	int i = 0;
	FILE *f = NULL;

	for (i = lo; i < hi; i++) {
		if (files[i] == NULL)
			continue;

		if ((f = fopen(files[i], "r")) == NULL) {
			perror(files[i]);
			return -1;
		}

		if (file_head(f, files[i]) == -1) {
			if (fclose(f) == -1) {
				perror(files[i]);
				return -1;
			}
		}

		if (fclose(f) == -1) {
			perror(files[i]);
			return -1;
		}
	}

	return 0;
}

static int file_head(FILE *f, char *filename)
{
	int c = 0;
	int n = 0; // number of lines that were already read
	int i = 0;

	// TODO: there are lines > 256 bytes length, need to handle such case.
	char buf[256];

	while ((c = getc(f)) != EOF) {
		if (ferror(f) != 0) {
			perror(filename);
			return -1;
		}

		if (c == '\n') {
			buf[i++] = '\n';

			if (++n == nlines) {
				break;
			}
		} else {
			buf[i++] = c;
		}
	}

	if (write(fileno(stdout), &buf, i) != i) {
		perror(filename);
		return -1;
	}

	return 0;
}
