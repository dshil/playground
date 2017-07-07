#include <time.h>
#include <stdio.h>
#include <unistd.h>

#include "sock.h"

static int fd = 0;
static FILE *fout = NULL;
static FILE *fin = NULL;

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
		goto error;
	}

	fin = popen("date", "r");
	if (fin == NULL) {
		perror("popen");
		goto error;
	}

	int c = 0;
	for (;;) {
		c = fgetc(fin);
		if (ferror(fin) != 0) {
			perror("fgetc");
			goto error;
		}
		if (feof(fin))
			break;

		if (fputc(c, fout) == EOF) {
			perror("fputc");
			goto error;
		}
	}

	if (pclose(fin) == -1) {
		perror("pclose");
		goto error;
	}

	if (fclose(fout) == -1) {
		perror("fclose");
		goto error;
	}

	return 0;

error:
	if (fd != -1)
		if (close(fd) == -1)
			perror("close");

	if (fout != NULL)
		if (fclose(fout) == -1)
			perror("fclose");

	if (fin != NULL)
		if (pclose(fin) == -1)
			perror("fclose");

	return -1;
}
