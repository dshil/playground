#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>

static const int MAX_HOST_LEN = 256;

static int make_net_addr(char *buf, int port, struct sockaddr_in *addr);
static int make_sock(int port, int type);

int make_stream_sock(int port)
{
	return make_sock(port, SOCK_STREAM);
}

int make_dgram_sock(int port)
{
	return make_sock(port, SOCK_DGRAM);
}

static int make_sock(int port, int type)
{
	int sock_fd = 0;
	sock_fd = socket(AF_INET, type, 0);
	if (sock_fd == -1) {
		perror("socket");
		return -1;
	}

	char hostname[MAX_HOST_LEN];
	struct sockaddr_in saddr;
	if (make_net_addr(hostname, port, &saddr) == -1)
		return -1;

	if (bind(sock_fd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
		perror("bind");
		goto error;
	}

	if (type == SOCK_STREAM) {
		if (listen(sock_fd, 1) == -1) {
			perror("listen");
			goto error;
		}
	}

	return sock_fd;

error:
	if (sock_fd != -1) {
		if (close(sock_fd) == -1)
			perror("close");
	}
	return -1;
}

static int make_net_addr(char *hostname, int port, struct sockaddr_in *saddr)
{
	bzero((void *)saddr, sizeof(struct sockaddr_in));

	struct hostent *hp;
	gethostname(hostname, MAX_HOST_LEN);
	hp = gethostbyname(hostname);
	if (hp == NULL) {
		perror("gethostbyname");
		return -1;
	}

	bcopy((void *)hp->h_addr, (void*)&saddr->sin_addr, hp->h_length);

	saddr->sin_port = htons(port);
	saddr->sin_family = AF_INET;

	return 0;
}
