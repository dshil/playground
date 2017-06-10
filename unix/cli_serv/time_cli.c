#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cli.h"

int do_req(int sock_fd)
{
	int fd = dup(sock_fd);
	if (fd == -1) {
		perror("dup");
		return -1;
	}

	FILE *f = fdopen(fd, "r");
	if (f == NULL) {
		perror("fdopen");
		if (close(fd) == -1)
			perror("close");
		return -1;
	}

	int c = 0;

	while ((c = fgetc(f)) != EOF)
		putchar(c);

	if (fclose(f) == -1) {
		perror("fclose");
		return -1;
	}

	return 0;
}
