#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <utmpx.h>
#include <string.h>

static void gc(struct utmpx *rec, char *tty_name); /* free allocated memory */

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
			if ((rec = pututxline(rec)) == NULL) {
				perror("logout");
				goto error;
			}
		}
	}

	gc(rec, tty_name);
	exit(EXIT_SUCCESS);

error:
	gc(rec, tty_name);
	exit(EXIT_FAILURE);
}

static void gc(struct utmpx *rec, char *tty_name)
{
	free(rec);
	free(tty_name);
	endutxent(); /* close the database */
}
