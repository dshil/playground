#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <term.h>

static ssize_t do_more(FILE *fin, FILE *fp_tty, const int nrows, const int ncols);
static size_t see_more(FILE *fp_tty, const int nrows);

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
		perror("/dev/tty");
		goto error;
	}

	const int nrows = tigetnum("lines");
	const int ncols = tigetnum("cols");

	FILE *fin = NULL;

	if (ac == 1) {
		if ((do_more(stdin, fp_tty, nrows, ncols)) == -1) {
			perror("stdin");
			goto error;
		}
	} else {
		int i = 0;

		for (i = 1; i < ac; i++) {
			if (av[i] == NULL)
				continue;

			fin = fopen(av[i], "r");
			if (fin == NULL) {
				perror(av[i]);
				goto error;
			}

			if (do_more(fin, fp_tty, nrows, ncols) == -1) {
				perror(av[i]);
				goto error;
			}

			if (fclose(fin) == EOF) {
				perror(av[i]);
				goto error;
			}
		}
	}

	if (fp_tty != NULL) {
		if (fclose(fp_tty) == EOF) {
			perror("/dev/tty");
		}
	}

	exit(EXIT_SUCCESS);

error:
	if (fin != NULL) {
		if (fclose(fin) == EOF) {
			perror("fin");
		}
	}

	if (fp_tty != NULL) {
		if (fclose(fp_tty) == EOF) {
			perror("/dev/tty");
		}
	}

	exit(EXIT_FAILURE);
}

static ssize_t do_more(FILE *fin, FILE *fp_tty, const int nrows, const int ncols)
{
	size_t line_cnt = 0;
	size_t reply = 0;

	char *line = NULL;
	size_t linecap = 0;
	ssize_t linelen = 0;

	while ((linelen = getline(&line, &linecap, fin)) != EOF) {
		if ((ferror(fin)) != 0)
			goto do_more_error;

		if (line_cnt == nrows) {
			// ask user what to do.
			reply = see_more(fp_tty, nrows);

			if (reply == -1)
				goto do_more_error;

			if (reply == 0)
				break;

			line_cnt -= reply;
		}

		if ((fwrite(line, linelen, 1, stdout)) != 1) {
			goto do_more_error;
		}
		line_cnt++;
	}

	free(line);
	return 0;

do_more_error:
	free(line);
	return -1;
}

static size_t see_more(FILE *fp_tty, const int nrows)
{
	fprintf(stderr, "\033[7m more? \033[m\n");
	int c = 0;

	while ((c = getc(fp_tty)) != EOF) {
		if ((ferror(fp_tty)) != 0)
			return -1;

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
