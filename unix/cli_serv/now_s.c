#include <time.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>

int main(int ac, char *av[])
{
	int sock_fd = 0;
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	const int hostlen = 256;

	struct sockaddr_in saddr;
	struct hostent *hp;
	char hostname[hostlen];

	bzero((void *)&saddr, sizeof(saddr));

	gethostname(hostname, hostlen);
	hp = gethostbyname(hostname);
	if (hp == NULL) {
		perror("gethostbyname");
		goto error;
	}

	bcopy((void *)hp->h_addr, (void*)&saddr.sin_addr, hp->h_length);
	saddr.sin_port = htons(8080);
	saddr.sin_family = AF_INET;

	if (bind(sock_fd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
		perror("bind");
		goto error;
	}

	if (listen(sock_fd, 1) == -1) {
		perror("listen");
		goto error;
	}

	int fd = 0;
	FILE *fout = NULL;
	time_t tm = 0;

	while(1) {
		fd = accept(sock_fd, NULL, NULL);
		if (fd == -1) {
			perror("accept");
			goto error;
		}

		fout = fdopen(fd, "w");
		if (fout == NULL) {
			perror("fdopen");
			close(fd);
			goto error;
		}

		tm = time(NULL);
		fprintf(fout, "time here is: %s\n", ctime(&tm));

		if (fclose(fout) == -1) {
			perror("fclose");
			goto error;
		}
	}

	if (close(sock_fd) == -1) {
		perror("close");
		goto error;
	}

	exit(EXIT_SUCCESS);

error:
	if (sock_fd != -1) {
		if (close(sock_fd) == -1)
			perror("close");
	}
	exit(EXIT_FAILURE);
}
