#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct head_config {
	char **files;
	int ac; /* number of a command line arguments */
	int is_print; /* print files names or not */
	int (*file_head) (FILE *f, char *filename);
};

static int file_head_lines(FILE *f, char *filename);
static int file_head_bytes(FILE *f, char *filename);
static int files_head(struct head_config *config);

static int parse_num(char *val, int *num);

static int nlines = 10;
static int nbytes = -1;

int main(int ac, char *av[])
{
	char *nlineval = NULL;
	char *nbyteval = NULL;

	int opt = 0;
	while((opt = getopt(ac, av, "nc:")) != -1) {
		switch(opt) {
			case 'n':
				nlineval = optarg;
				break;
			case 'c':
				nbyteval = optarg;
				break;
			default:
				fprintf(stderr, "Usage: %s [-n lines | -c bytes] [file ...]\n",
					av[0]);
				exit(EXIT_FAILURE);
		}
	}

	if (nlineval != NULL && nbyteval != NULL) {
		fprintf(stderr, "Usage: %s [-n lines | -c bytes] [file ...]\n", av[0]);
		exit(EXIT_FAILURE);
	}

	if (parse_num(nlineval, &nlines) == -1)
		exit(EXIT_FAILURE);

	if (parse_num(nbyteval, &nbytes) == -1)
		exit(EXIT_FAILURE);

	struct head_config config;
	if (nbytes != -1)
		config.file_head = file_head_bytes;
	else
		config.file_head = file_head_lines;

	if (ac == optind) {
		if (config.file_head(stdin, "stdin") == -1) {
			exit(EXIT_FAILURE);
		}
	} else {
		config.is_print = ((ac - optind) > 1);
		config.files = av;
		config.ac = ac;

		if (files_head(&config) == -1) {
			exit(EXIT_FAILURE);
		}
	}

	exit(EXIT_SUCCESS);
}

static int parse_num(char *val, int *num)
{
	if (val == NULL)
		return 0;

	errno = 0;
	int n = atoi(val);
	if (errno != 0) {
		perror(val);
		return -1;
	}

	*num = n;
	return 0;
}

static int files_head(struct head_config *conf)
{
	int i = 0;
	FILE *f = NULL;
	int is_additional_space = 0;

	for (i = optind; i < conf->ac; i++) {
		if (conf->files[i] == NULL)
			continue;

		if ((f = fopen(conf->files[i], "r")) == NULL) {
			perror(conf->files[i]);
			return -1;
		}

		if (conf->is_print) {
			if (is_additional_space)
				printf("\n");

			if (i != (conf->ac - 1))
				is_additional_space = 1;
			else
				is_additional_space = 0;

			printf("==> %s <==\n", conf->files[i]);
		}

		if (conf->file_head(f, conf->files[i]) == -1) {
			if (fclose(f) == -1) {
				perror(conf->files[i]);
				return -1;
			}
		}

		if (fclose(f) == -1) {
			perror(conf->files[i]);
			return -1;
		}
	}

	return 0;
}

static int file_head_lines(FILE *f, char *filename)
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

static int file_head_bytes(FILE *f, char *filename)
{
	char buf[nbytes];
	ssize_t n = 0;
	const int len = sizeof(buf)/sizeof(buf[0]);

	n = read(fileno(f), &buf, len);
	if (write(fileno(stdout), buf, n) != n) {
		perror(filename);
		return -1;
	}

	return 0;
}
