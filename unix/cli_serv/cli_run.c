#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cli.h"

int main(int ac, char *av[])
{
	int port = atoi(av[2]);
	fprintf(stderr, "connecting to %s:%d\n", av[1], port);
	int sock_fd = conn_to_serv(av[1], port);
	if (sock_fd == -1)
		exit(EXIT_FAILURE);

	if (do_req(sock_fd) == -1) {
		if (close(sock_fd) == -1)
			perror("close");
		exit(EXIT_FAILURE);
	}

	if (close(sock_fd) == -1) {
		perror("close");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
