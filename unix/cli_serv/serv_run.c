#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "serv.h"

static void child_waiter(int sig);

int main(int ac, char *av[])
{
	signal(SIGCHLD, child_waiter);
	int port_num = 8080;

	int sock_fd = 0;
	if ((sock_fd = make_serv_sock(port_num)) == -1)
		exit(EXIT_FAILURE);

	int fd = 0;

	fprintf(stderr, "start server, port=%d\n", port_num);

	while(1) {
		errno = 0;
		fd = accept(sock_fd, NULL, NULL);
		if (fd == -1) {
			if (errno == EINTR)
				continue;

			perror("accept");
			goto error;
		}

		if (proccess_req(fd) == -1) {
			if (close(fd) == -1) {
				perror("close");
				goto error;
			}
		}

		if (close(fd) == -1) {
			perror("close");
			goto error;
		}
	}

	if (close(sock_fd) == -1) {
		perror("close");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);

error:
	if (sock_fd != -1)
		if (close(sock_fd) == -1)
			perror("close");
	exit(EXIT_FAILURE);
}

static void child_waiter(int sig)
{
	while (waitpid(-1, NULL, WNOHANG) > 0)
		;
}
