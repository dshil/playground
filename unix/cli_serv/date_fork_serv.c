#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "sock.h"

int proccess_req(int sock_fd)
{
	pid_t pid = 0;
	pid = fork();

	if (pid == -1) {
		perror("fork");
		return -1;
	} else if (pid == 0) {
		if (dup2(sock_fd, STDOUT_FILENO) == -1) {
			perror("dup2");
			exit(EXIT_FAILURE);
		}

		if (close(sock_fd) == -1) {
			perror("close");
			exit(EXIT_FAILURE);
		}

		if (execl("/bin/date", "date", NULL) == -1) {
			perror("execl");
			exit(EXIT_FAILURE);
		}
	} else {
		if (wait(NULL) == -1) {
			perror("wait");
			return -1;
		}
	}

	return 0;
}
