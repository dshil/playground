#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utmpx.h>

int main(int ac, char *av[])
{
	char *tty_name = NULL;

	if (ac > 1) {
		fprintf(stderr, "usage: whoami\n");
		exit(EXIT_FAILURE);
	} else {
		tty_name = ttyname(fileno(stdin));
		char *p, *q = NULL;

		for (q = tty_name; (p = strtok(q, "/")) != NULL; q = NULL)
			tty_name = p;
	}

	if (tty_name == NULL) {
		fprintf(stderr, "undefined terminal line\n");
		exit(EXIT_FAILURE);
	}

	setutxent(); /* reset the account database */

	struct utmpx *rec = NULL;

	while((rec = getutxent()) != NULL) {
		if (rec->ut_type != USER_PROCESS)
			continue;

		if ((strcmp(rec->ut_line, tty_name)) == 0)
			printf("%s\n", rec->ut_user);
	}

	endutxent(); /* close the account database */
	exit(EXIT_SUCCESS);
}
