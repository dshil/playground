#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "sock.h"

static int
get_net_addr(char *buf, int sz, int *port, struct sockaddr_in *addr);

int main(int ac, char *av[])
{
	int port = 13000;

	int sock_fd = make_dgram_sock(port);
	if (sock_fd == -1)
		exit(EXIT_FAILURE);

	fprintf(stderr, "start dgram server on port=%d\n", port);

	struct sockaddr_in saddr;
	socklen_t saddrlen = sizeof(saddr);
	char buf[BUFSIZ];
	size_t n = 0;

	char src_host[MAX_HOST_LEN];
	int src_port = 0;

	for (;;) {
		n = recvfrom(sock_fd, buf, BUFSIZ, 0,
				(struct sockaddr *) &saddr, &saddrlen);
		if (n <= 0)
			break;

		buf[n] = '\0';
		get_net_addr(src_host, MAX_HOST_LEN, &src_port, &saddr);
		printf("dgram: receive msg=%s from %s:%d\n", buf, src_host, src_port);
	}
}

static int
get_net_addr(char *buf, int sz, int *port, struct sockaddr_in *addr)
{
	strncpy(buf, inet_ntoa(addr->sin_addr), sz);
	*port = ntohs(addr->sin_port);
	return 0;
}
