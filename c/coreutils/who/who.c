#include <stdio.h>
#include <stdlib.h>
#include <utmpx.h>
#include <sys/time.h>
#include <string.h>

/*
	TODO:
		- who am I.
		- dead processes.
		- time of the last system boot.
*/

static void gc();

int main(int ac, char *av[])
{
	struct utmpx *rec = NULL;
	struct tm *tm_info = NULL;
	char time_str [20];

	setutxent(); /* reset the database */

	while((rec = getutxent()) != NULL) {
		if (rec->ut_type != USER_PROCESS)
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

	gc();
	exit(EXIT_SUCCESS);

error:
	gc();
	exit(EXIT_FAILURE);
}

static void gc()
{
	endutxent(); /* close the database */
}
