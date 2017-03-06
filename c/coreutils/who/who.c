#include <stdio.h>
#include <stdlib.h>
#include <utmpx.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>

/*
	TODO:
		- dead processes.
		- time of the last system boot.
*/

int main(int ac, char *av[])
{
	struct utmpx *rec = NULL;
	struct tm *tm_info = NULL;
	char time_str [20];

	setutxent(); /* reset the database */

	char *tty_name = NULL;
	if (ac == 3) {
		// who am i
		tty_name = ttyname(fileno(stdin));
		char *p, *q = NULL;

		for (q = tty_name; (p = strtok(q, "/")) != NULL; q = NULL)
			tty_name = p;

		if (tty_name == NULL) {
			fprintf(stderr, "undefined terminal line\n");
			goto error;
		}
	}

	if (ac == 2) {
		// replace default utmp file.

		// TODO: mac os x doesn't support setting the custom file for the
		// acount database, will be fixed later when the migration to debian
		// ubuntu will be done.
	}

	while((rec = getutxent()) != NULL) {
		if (rec->ut_type != USER_PROCESS)
			continue;

		if (tty_name != NULL && strcmp(rec->ut_line, tty_name) != 0)
			continue;

		if ((tm_info = localtime(&rec->ut_tv.tv_sec)) == NULL) {
			perror("localtime");
			goto error;
		}

		// format time in human readable.
		strftime(time_str, sizeof(time_str), "%b %e %H:%M", tm_info);
		printf("%-8.8s %-8.8s %-12s %-8.8s\n",
				rec->ut_user,
				rec->ut_line,
				time_str,
				rec->ut_host);
	}

	endutxent(); /* close the database */
	exit(EXIT_SUCCESS);

error:
	endutxent(); /* close the database */
	exit(EXIT_FAILURE);
}