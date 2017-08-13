#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <utmpx.h>
#include <string.h>

/*
	TODO: logout doesn't actually exit from the currently active terminal,
	it just removes the record from the account database. This behavior is
	incorrect and will be fixed later.
*/

int main(int ac, char *av[])
{
	char *tty_name = ttyname(fileno(stdin));

	char *p = NULL;
	char *q = NULL;
	char *curr_tty = NULL;

	for (q = tty_name; (p = strtok(q, "/")) != NULL; q = NULL) {
		curr_tty = p;
	}

	setutxent(); /* reset the database */
	struct utmpx *rec = NULL;

	while((rec = getutxent()) != NULL) {
		if (rec->ut_type != USER_PROCESS) {
			continue;
		}

		if ((strcmp(rec->ut_line, curr_tty)) == 0) {
			rec->ut_type = DEAD_PROCESS;
			if (pututxline(rec) == NULL) {
				perror("logout");
				goto error;
			}
		}
	}

	endutxent(); /* close the database */
	exit(EXIT_SUCCESS);

error:
	endutxent(); /* close the database */
	exit(EXIT_FAILURE);
}
