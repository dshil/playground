#include <curses.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <term.h>

static void do_more(FILE *fin, FILE *fp_tty, int nrows, int ncols);
static size_t see_more(FILE *fp_tty, int nrows);

int main(int ac, char *av[])
{
	int errret = 0;
	// set the terminal type for the later terminfo usage.
	setupterm(NULL, fileno(stderr), &errret);
	if (!errret) {
		exit(EXIT_FAILURE);
	}

	FILE *fp_tty = fopen("/dev/tty", "r");
	if (fp_tty == NULL) {
		perror("can't open /dev/tty");

		if (fclose(fp_tty) == EOF) {
			perror("can't close /dev/tty");
		}

		exit(EXIT_FAILURE);
	}

	int nrows = tigetnum("lines");
	int ncols = tigetnum("cols");

	if (ac == 1) {
		do_more(stdin, fp_tty, nrows, ncols);
	} else {
		size_t i = 0;
		FILE *fin = NULL;

		for (i = 1; i < ac; i++) {
			fin = fopen(av[i], "r");
			if (fin == NULL) {
				fprintf(stderr, "can't open a file %s: %s\n", av[i],
					strerror(errno));

				if (fclose(fin) == EOF) {
					fprintf(stderr, "can't close a file %s: %s\n", av[i],
						strerror(errno));
				}

				if (fclose(fp_tty) == EOF) {
					perror("can't close /dev/tty");
				}

				exit(EXIT_FAILURE);
			}

			do_more(fin, fp_tty, nrows, ncols);
			if (fclose(fin) == EOF) {
				fprintf(stderr, "can't close a file %s: %s\n", av[i],
					strerror(errno));

				if (fclose(fp_tty) == EOF) {
					perror("can't close /dev/tty");
				}

				exit(EXIT_FAILURE);
			}
		}
	}

	if (fclose(fp_tty) == EOF) {
		perror("can't close /dev/tty");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}

static void do_more(FILE *fin, FILE *fp_tty, int nrows, int ncols)
{
	size_t line_cnt = 0;
	size_t reply = 0;

	char *line = NULL;
	size_t linecap = 0;
	ssize_t linelen;

	while ((linelen = getline(&line, &linecap, fin)) > 0) {
		if (line_cnt == nrows) {
			// ask user what to do.
			reply = see_more(fp_tty, nrows);
			if (reply == 0) {
				break;
			}
			line_cnt -= reply;
		}

		fwrite(line, linelen, 1, stdout);
		line_cnt++;
	}

	free(line);
}

static size_t see_more(FILE *fp_tty, int nrows)
{
	fprintf(stderr, "\033[7m more? \033[m\n");
	char c;

	while ((c = getc(fp_tty)) != EOF) {
		if (c == 'q') {
			return 0;
		} else if (c == ' ') {
			return nrows;
		} else if (c == '\n') {
			return 1;
		}
	}

	return 0;
}
