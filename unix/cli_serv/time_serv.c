#include <time.h>
#include <stdio.h>
#include <unistd.h>

#include "serv.h"

static	int fd = 0;
static	FILE *fout = NULL;
static	time_t tm = 0;

int proccess_req(int sock_fd)
{
	fd = dup(sock_fd);
	if (fd == -1) {
		perror("dup");
		return -1;
	}

	fout = fdopen(fd, "w");
	if (fout == NULL) {
		perror("fdopen");
		if (close(fd) == -1)
			perror("close");
		return -1;
	}

	tm = time(NULL);
	fprintf(fout, "time here is: %s\n", ctime(&tm));

	if (fclose(fout) == -1) {
		perror("fclose");
		return -1;
	}
}
