#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "cli.h"

int conn_to_serv(char *host, int port)
{
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1) {
		perror("socket");
		return -1;
	}

	struct sockaddr_in addr;
	bzero((void *)&addr, sizeof(addr));

	struct hostent *hp = gethostbyname(host);
	if (hp == NULL) {
		perror("gethostbyname");
		goto error;
	}

	bcopy((void *)hp->h_addr, (void *)&addr.sin_addr, hp->h_length);
	addr.sin_port = port;
	addr.sin_family = AF_INET;

	if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		perror("connect");
		goto error;
	}

	return sock_fd;

 error:
	if (sock_fd != -1)
		if (close(sock_fd) == -1)
			perror("close");
	return -1;
}
