#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

static int execute(char **av);

int main(int ac, char *av[])
{
	const int max_arg_num = 20;
	const int max_arg_len = 100;
	char *arglist[max_arg_num+1];
	char *arg = NULL;
	char buf[100];

	ssize_t n = 0;
	int curr_arg = 0;
	int i = 0;
	int is_exec = 0;

	for (;;) {
		if (fgets(buf, max_arg_len, stdin) == NULL) {
			perror("fgets");
			goto error;
		}

		if (strncmp(buf, "exit", 4) == 0) {
			break;
		}

		is_exec = (curr_arg == max_arg_num) || *buf == '\n';

		if (is_exec) {
			arglist[curr_arg] = NULL;
			if (execute(arglist) == -1)
				goto error;
			curr_arg = 0;
			continue;
		}

		buf[strlen(buf)-1] = '\0';
		arg = (char *) malloc(strlen(buf) + 1);
		if (arg == NULL) {
			fprintf(stderr, "no memory\n");
			goto error;
		}
		strcpy(arg, buf);
		arglist[curr_arg++] = arg;
	}
	for (; i < curr_arg; i++)
		free(arglist[i]);

	exit(EXIT_SUCCESS);

error:
	for (; i < curr_arg; i++)
		free(arglist[i]);
	exit(EXIT_FAILURE);
}

static int execute(char **av)
{
	pid_t pid = 0;

	pid = fork();
	if (pid == -1) {
		perror("fork");
		return -1;
	} else if (pid == 0) {
		if (execvp(av[0], av) == -1) {
			perror("execvp");
			exit(EXIT_FAILURE);
		}
	} else {
		int chld_status = 0;
		if (wait(&chld_status) == -1) {
			perror("wait");
			return -1;
		}
	}

	return 0;
}
