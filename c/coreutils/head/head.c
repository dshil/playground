#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int file_head(FILE *f, char *filename);
static int files_head(char *files[], const int lo, const int hi, const int print_name);

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

	if (ac == 1) {
		if (file_head(stdin, "stdin") == -1) {
			exit(EXIT_FAILURE);
		}
	} else {
		const int is_print_file_name = ((ac - optind) > 1);
		if (files_head(av, optind, ac, is_print_file_name) == -1) {
			exit(EXIT_FAILURE);
		}
	}

	exit(EXIT_SUCCESS);
}

static int
files_head(char *files[], const int lo, const int hi, const int print_name)
{
	int i = 0;
	FILE *f = NULL;
	int is_additional_space = 0;

	for (i = lo; i < hi; i++) {
		if (files[i] == NULL)
			continue;

		if ((f = fopen(files[i], "r")) == NULL) {
			perror(files[i]);
			return -1;
		}

		if (print_name) {
			if (is_additional_space)
				printf("\n");

			if (i != (hi - 1))
				is_additional_space = 1;
			else
				is_additional_space = 0;

			printf("==> %s <==\n", files[i]);
		}

		if (file_head(f, files[i]) == -1)
			goto error;

		if (fclose(f) == -1) {
			perror(files[i]);
			return -1;
		}
	}

	return 0;

error:
	if (f != NULL) {
		if (fclose(f) == -1) {
			perror(files[i]);
			return -1;
		}
	}

	return -1;
}

static int file_head(FILE *f, char *filename)
{
	int c = 0;
	int n = 0; // number of lines that were already read

	while ((c = getc(f)) != EOF) {
		if (ferror(f) != 0) {
			perror(filename);
			return -1;
		}

		if (c == '\n') {
			if (++n == nlines) {
				if (putchar(c) == EOF) {
					perror(filename);
					return -1;
				}

				break;
			}

		}

		if (putchar(c) == EOF) {
			perror(filename);
			return -1;
		}
	}

	return 0;
}
