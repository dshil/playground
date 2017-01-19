#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define PAGELEN 24
#define LINELEN 512

void do_more(FILE *fin);
int see_more();

int main(int ac, char *av[])
{
	int i;
	FILE *fin;

	if (ac == 1) {
		// read from stdin.
		do_more(stdin);
	} else {
		// read all input files.
		for (i = 0; i < ac; i++) {
			if ((fin = fopen(av[i], "r")) == NULL) {
				printf("can't read a file %s\n", av[i]);
			} else {
				do_more(fin);
				fclose(fin);
			}
		}
	}

	return 0;
}

void do_more(FILE *fin)
{
	int line_cnt = 0;
	int reply = 0;

	char buf[LINELEN];

	while (fgets(buf, LINELEN, fin)) {
		if (line_cnt == PAGELEN) {
			// ask user what to do.
			reply = see_more();
			if (reply == 0) {
				break;
			}
			line_cnt -= reply;
		}

		if (fputs(buf, stdout) == EOF) {
			exit(1);
		}
		line_cnt++;
	}
}

int see_more()
{
	printf("\033[7m more? \033[m\n");
	int c;

	while ((c = getchar()) != EOF) {
		if (c == 'q') {
			return 0;
		} else if (c == '\0') {
			return PAGELEN;
		} else if (c == '\n') {
			return 1;
		}
	}

	return 0;
}
