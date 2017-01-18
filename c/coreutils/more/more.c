#include <stdio.h>
#include <stdlib.h>

#define PAGELEN 24
#define LINELEN 512

void do_more(FILE *fin);

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
	char buf[LINELEN];

	while (fgets(buf, LINELEN, fin)) {
		if (line_cnt == PAGELEN) {
			// ask user what to do.
			return;
		}

		if (fputs(buf, stdout) == EOF) {
			exit(1);
		}
		line_cnt++;
	}
}
