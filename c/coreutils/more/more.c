#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <term.h>

void do_more(FILE *fin, int nrows, int ncols);
size_t see_more(FILE *fp_tty, int nrows);

int main(int ac, char *av[])
{
	int errret = 0;
	// set the terminal type for the later terminfo usage.
	setupterm(NULL, fileno(stderr), &errret);
	if (!errret) {
		exit(EXIT_FAILURE);
	}

	int nrows = tigetnum("lines");
	int ncols = tigetnum("cols");

	size_t i;
	FILE *fin;

	if (ac == 1) {
		// read from stdin.
		do_more(stdin, nrows, ncols);
	} else {
		// read all input files.
		for (i = 1; i < ac; i++) {
			if ((fin = fopen(av[i], "r")) == NULL) {
				fprintf(stderr, "can't read a file %s\n", av[i]);
			} else {
				do_more(fin, nrows, ncols);
				fclose(fin);
			}
		}
	}

	exit(EXIT_SUCCESS);
}

void do_more(FILE *fin, int nrows, int ncols)
{
	size_t line_cnt = 0;
	size_t reply = 0;

	char *line = NULL;
	size_t linecap = 0;
	ssize_t linelen;

	FILE *fp_tty;
	if ((fp_tty = fopen("/dev/tty", "r")) == NULL) {
		exit(EXIT_FAILURE);
	}

	while ((linelen = getline(&line, &linecap, fin)) > 0) {
		if (line_cnt == nrows) {
			// ask user what to do.
			reply = see_more(fp_tty, nrows);
			if (reply == 0) {
				fclose(fp_tty);
				return;
			}
			line_cnt -= reply;
		}

		fwrite(line, linelen, 1, stdout);
		line_cnt++;
	}

	if (line) {
		free(line);
	}
}

size_t see_more(FILE *fp_tty, int nrows)
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
