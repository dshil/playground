#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define PAGELEN 24
#define LINELEN 512

void do_more(FILE *fin);
size_t see_more();

int main(int ac, char *av[])
{
	size_t i;
	FILE *fin;

	if (ac == 1) {
		// read from stdin.
		do_more(stdin);
	} else {
		// read all input files.
		for (i = 0; i < ac; i++) {
			if ((fin = fopen(av[i], "r")) == NULL) {
				fprintf(stderr, "can't read a file %s\n", av[i]);
			} else {
				do_more(fin);
				fclose(fin);
			}
		}
	}

	exit(EXIT_SUCCESS);
}

void do_more(FILE *fin)
{
	size_t line_cnt = 0;
	size_t reply = 0;

	char *line = NULL;
	size_t linecap = 0;
	ssize_t linelen;

	while ((linelen = getline(&line, &linecap, fin)) > 0) {
		if (line_cnt == PAGELEN) {
			// ask user what to do.
			reply = see_more();
			if (reply == 0) {
				break;
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

size_t see_more()
{
	fprintf(stderr, "\033[7m more? \033[m\n");
	char c;

	while ((c = getchar()) != EOF) {
		if (c == 'q') {
			return 0;
		} else if (c == ' ') {
			return PAGELEN;
		} else if (c == '\n') {
			return 1;
		}
	}

	return 0;
}
