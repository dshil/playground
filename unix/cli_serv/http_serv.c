#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h>

static int do_path(int fd, char *path);
static void bad_request(FILE * fout, char *msg);
static int handle_http_req(int sock_fd);

static char *HTTP_V = "HTTP/1.0";

int proccess_req(int sock_fd)
{

	pid_t pid = 0;
	if ((pid = fork()) == -1) {
		perror("fork");
		return -1;
	} else if (pid == 0) {
		if (handle_http_req(sock_fd) == -1)
			exit(EXIT_FAILURE);
	} else {
		return 0;
	}
	return 0;
}

static int handle_http_req(int sock_fd)
{
	static const int method_len = 10;
	static const int max_path = BUFSIZ;
	static const int http_ver_len = 10;

	int buf_sz = method_len + max_path + http_ver_len;

	char method[method_len];
	char path[max_path];
	char http_ver[http_ver_len];
	char buf[buf_sz];

	FILE *fin = NULL;
	FILE *fout = NULL;

	int fd_in = dup(sock_fd);
	if (fd_in == -1) {
		perror("dup");
		return -1;
	}

	fin = fdopen(fd_in, "r");
	if (fin == NULL) {
		if (close(fd_in) == -1)
			perror("close");
		return -1;
	}

	int fd_out = dup(sock_fd);
	if (fd_out == -1) {
		perror("dup");
		goto error;
	}

	fout = fdopen(fd_out, "w");
	if (fout == NULL) {
		if (close(fd_out) == -1)
			perror("close");
		goto error;
	}

	if (fgets(buf, buf_sz, fin) == NULL) {
		if (ferror(fin) != 0) {
			perror("fgets");
			goto error;
		}

		if (feof(fin)) {
			fprintf(fout, "empty message\n");
		}

		if (fclose(fin) == -1 || fclose(fout) == -1) {
			perror("fclose");
			goto error;
		}

		return 0;
	}

	strcpy(path, "./");

	if (sscanf(buf, "%s %s %s", method, path + 2, http_ver) != 3) {
		bad_request(fout, "invalid request format");

		if (fclose(fin) == -1 || fclose(fout) == -1) {
			perror("fclose");
			goto error;
		}

		return 0;
	}

	if (strcmp(method, "GET") != 0) {
		bad_request(fout, "unsupported method");
	} else {
		if (fclose(fin) == -1 || fclose(fout) == -1) {
			perror("fclose");
			goto error;
		}

		if (do_path(sock_fd, path) == -1)
			return -1;
	}

	return 0;

 error:
	if (fin != NULL)
		if (fclose(fin) == -1)
			perror("fclose");
	if (fout != NULL)
		if (fclose(fout) == -1)
			perror("fclose");
	return -1;
}

static int do_path(int fd, char *path)
{
	if (dup2(fd, STDOUT_FILENO) == -1) {
		perror("dup2");
		return -1;
	}

	if (dup2(fd, STDERR_FILENO) == -1) {
		perror("dup2");
		return -1;
	}

	if (close(fd) == -1) {
		perror("close");
		return -1;
	}

	struct stat sb;
	if (stat(path, &sb) == -1) {
		fprintf(stderr, "stat, err=");
		perror(path);
		return -1;
	}

	if (S_ISDIR(sb.st_mode)) {
		if (execl("/bin/ls", "ls", "-l", path, NULL) == -1) {
			perror("execl");
			return -1;
		}
	}

	if (S_ISREG(sb.st_mode)) {
		if (execl("/bin/cat", "cat", path, NULL) == -1) {
			perror("execl");
			return -1;
		}
	}

	return 0;
}

static void bad_request(FILE * fout, char *msg)
{
	fprintf(fout, "%s 404 %s\n", HTTP_V, msg);
}
