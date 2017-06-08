#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

static int pipe_open(int fds[]);
static int pipe_close(int fds[]);
static int dup2pipe(int to[], int from[]);

int main(int ac, char *av[])
{
	int to_dc[2];
	int from_dc[2];

	if (pipe(to_dc) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	if (pipe(from_dc) == -1) {
		perror("pipe");
		pipe_close(to_dc);
		exit(EXIT_FAILURE);
	}

	FILE *fin = NULL;
	FILE *fout = NULL;

	pid_t pid = 0;
	pid = fork();

	if (pid == -1) {
		perror("fork");

		pipe_close(to_dc);
		pipe_close(from_dc);

		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		if (dup2pipe(to_dc, from_dc) == -1)
			exit(EXIT_FAILURE);
		if (execlp("dc", "dc", "-", NULL) == -1)
			exit(EXIT_FAILURE);
	} else {
		if (close(to_dc[0]) == -1) {
			perror("close");
			goto error;
		}

		if (close(from_dc[1]) == -1) {
			perror("close");
			goto error;
		}

		char buf[BUFSIZ];
		int op = 0;
		int num1 = 0;
		int num2 = 0;

		fout = fdopen(to_dc[1], "w");
		fin = fdopen(from_dc[0], "r");
		if (fout == NULL || fin == NULL) {
			perror("fdopen");
			goto error;
		}

		for (;;) {
			fgets(buf, BUFSIZ, stdin);
			if (feof(stdin))
				break;
			if (ferror(stdin) != 0) {
				perror("fgets");
				goto error;
			}

			if (sscanf(buf, "%d%[-+*/^]%d", &num1, &op, &num2) != 3) {
				fprintf(stderr, "invalid format, use num1 op num2\n");
				continue;
			}

			if (fprintf(fout, "%d\n%d\n%c\np\n", num1, num2, op) == EOF) {
				perror("fprintf");
				goto error;
			}

			if (fflush(fout) == -1) {
				perror("fflush");
				goto error;
			}

			fgets(buf, BUFSIZ, fin);
			if (ferror(fin) != 0)
				fprintf(stderr, "empty dc response\n");
			printf("%d %c %d = %s\n", num1, op, num2, buf);
		}
	}

	if (fclose(fout) == -1) {
		perror("fclose");
		fclose(fin);
		exit(EXIT_FAILURE);
	}

	if (fclose(fin) == -1) {
		perror("fclose");
		exit(EXIT_FAILURE);
	}

	if (wait(NULL) == -1) {
		perror("wait");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
error:
	if (fin != NULL)
		fclose(fin);
	if (fout != NULL)
		fclose(fout);
	if (to_dc[0] != -1)
		close(to_dc[0]);
	if (from_dc[1] != -1)
		close(from_dc[1]);

	wait(NULL);
	exit(EXIT_FAILURE);
}

static int pipe_close(int fds[])
{
	if (close(fds[0]) == -1) {
		perror("close");
		close(fds[1]);
		return -1;
	}

	if (close(fds[1]) == -1) {
		perror("close");
		return -1;
	}

	return 0;
}

static int dup2pipe(int to[], int from[])
{
	if (close(to[1]) == -1) {
		perror("close");
		goto error;
	}

	if (dup2(to[0], STDIN_FILENO) == -1) {
		perror("dup2");
		goto error;
	}

	if (close(to[0]) == -1) {
		perror("close");
		goto error;
	}

	if (close(from[0]) == -1) {
		perror("close");
		goto error;
	}

	if (dup2(from[1], STDOUT_FILENO) == -1) {
		perror("dup2");
		goto error;
	}

	if (close(from[1]) == -1) {
		perror("close");
		goto error;
	}

	return 0;

error:
	if (to[0] != -1)
		close(to[0]);
	if (to[1] != -1)
		close(to[1]);
	if (from[0] != -1)
		close(from[0]);
	if (from[1] != -1)
		close(from[1]);

	return -1;
}
