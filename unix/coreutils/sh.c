#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <linux/limits.h>

static int execute(char **av);
static char **init_arglist(int sz);
static char** grow_arglist(char **al, int sz);
static char** arglist_from_line(char *line);
static void free_arglist(char **al);

int main(int ac, char *av[])
{
	char **arglist = NULL;
	char *line = NULL;
	size_t cap = 0;

	for (;;) {
		if (getline(&line, &cap, stdin) == -1) {
			perror("getline");
			goto error;
		}

		if (strcmp(line, "\n") == 0)
			continue;

		if (strncmp(line, "exit", 4) == 0)
			break;

		if ((arglist = arglist_from_line(line)) == NULL)
			goto error;

		if (execute(arglist) == -1)
			goto error;

		free_arglist(arglist);
	}

	free(line);
	exit(EXIT_SUCCESS);

error:
	free(line);
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
			return -1;
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

static char** arglist_from_line(char *line)
{
	char *p = NULL;
	char *q = NULL;
	char **cp = NULL;
	int idx = 0;
	int sz = 1;
	int len = 0;

	char **al = init_arglist(sz);
	if (al == NULL)
		goto error;

	for (q = line; (p = strtok(q, " ")) != NULL; q = NULL) {
		if (idx == sz) {
			sz *= 2;
			if ((cp = grow_arglist(al, sz)) == NULL)
				goto error;
			else
				al = cp;
		}

		len = strlen(p);
		if (p[len-1] == '\n') {
			strncpy(p, p, len);
			p[len-1] = '\0';
		}
		al[idx++] = p;
	}

	// Grow arglist last time to set the end of the list.
	if ((cp = grow_arglist(al, sz + 1)) == NULL)
		goto error;
	al = cp;

	al[idx] = (char *) NULL;
	return al;

error:
	free_arglist(al);
	return (char **) NULL;
}

static char **init_arglist(int sz)
{
	char **al = (char **) malloc(sz * sizeof(char *));
	if (al == NULL) {
		fprintf(stderr, "malloc, no memory\n");
		return NULL;
	}
	return al;
}

static char** grow_arglist(char **al, int sz)
{
	if ((sz  * sizeof(char *)) > ARG_MAX) {
		fprintf(stderr, "to long args\n");
		return (char **) NULL;
	}

	char **cp = realloc(al, sz);
	if (cp == NULL) {
		fprintf(stderr, "realloc, no memory\n");
		return (char **) NULL;
	}
	return cp;
}

static void free_arglist(char **al)
{
	free(al);
}
