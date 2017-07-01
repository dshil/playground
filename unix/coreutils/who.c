#include <time.h>
#include <utmpx.h>
#include <stdio.h>
#include <stdlib.h>

int main(int ac, char *av[])
{
	struct utmpx *rec = NULL;
	setutxent(); /* reset the database */

	time_t tm = 0;
	char *tm_str = NULL;

	while((rec = getutxent()) != NULL) {
		if (rec->ut_type != USER_PROCESS)
			continue;

		tm = rec->ut_tv.tv_sec;
		tm_str = 4+ctime(&tm);
		// No need to include '\n' character.
		tm_str[strlen(tm_str)-1] = '\0';

		printf("%s\t%s\t%s\t(%s)\n",
				rec->ut_user,
				rec->ut_line,
				tm_str,
				rec->ut_host);
	}

	endutxent(); /* close the database */
	exit(EXIT_SUCCESS);
}
