#include <stdio.h>
#include <errno.h>
#include <utmpx.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ttyent.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <linux/limits.h>

static int execute(char **av);
static char **init_list(int sz);
static char** grow_list(char **al, int sz);
static char** list_from_line(char *line, const char *delim);
static void free_list(char **al);

static void sig_handler(int signum);
static void set_signals(void);

static char *get_host(void);
static char *get_curr_dir(void);
static void prompt(void);

static char *tty_name = NULL;
static char *cur_dir = NULL;
static char *host = NULL;

int main(int ac, char *av[])
{
	set_signals();

	if ((tty_name = getlogin()) == NULL) {
		perror("getlogin");
		exit(EXIT_FAILURE);
	}

	if ((host = get_host()) == NULL)
		exit(EXIT_FAILURE);

	if ((cur_dir = get_curr_dir()) == NULL)
		exit(EXIT_FAILURE);

	char **arglist = NULL;
	char **cmds = NULL;
	char **cmds_cp = NULL;
	char *cmd = NULL;
	char *line = NULL;
	size_t cap = 0;

	for (;;) {
		prompt();

		if (getline(&line, &cap, stdin) == -1) {
			perror("getline");
			goto error;
		}

		if (strcmp(line, "\n") == 0)
			continue;

		if (strncmp(line, "exit", 4) == 0) {
			printf("exit\n");
			break;
		}

		if ((cmds = list_from_line(line, ";")) == NULL)
				goto error;

		cmds_cp = cmds;
		while (*cmds_cp != NULL) {
			if ((arglist = list_from_line(*cmds_cp, " ")) == NULL) {
				free_list(cmds);
				goto error;
			}

			if (execute(arglist) == -1) {
				free_list(cmds);
				free_list(arglist);
				goto error;
			}

			cmds_cp++;
			free_list(arglist);
		}
		free_list(cmds);
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

static char** list_from_line(char *line, const char *delim)
{
	char *p = NULL;
	char *q = NULL;
	char **cp = NULL;
	int idx = 0;
	int sz = 1;
	int len = 0;

	char **al = init_list(sz);
	if (al == NULL)
		goto error;

	for (q = line; (p = strtok(q, delim)) != NULL; q = NULL) {
		if (idx == sz) {
			sz *= 2;
			if ((cp = grow_list(al, sz)) == NULL)
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
	if ((cp = grow_list(al, sz + 1)) == NULL)
		goto error;
	al = cp;

	al[idx] = NULL;
	return al;

error:
	free_list(al);
	return NULL;
}

static char **init_list(int sz)
{
	char **al = (char **) malloc(sz * sizeof(char *));
	if (al == NULL) {
		fprintf(stderr, "malloc, no memory\n");
		return NULL;
	}
	return al;
}

static char** grow_list(char **al, int sz)
{
	if ((sz  * sizeof(char *)) > ARG_MAX) {
		fprintf(stderr, "to long args\n");
		return NULL;
	}

	char **cp = realloc(al, sz);
	if (cp == NULL) {
		fprintf(stderr, "realloc, no memory\n");
		return NULL;
	}
	return cp;
}

static void free_list(char **al)
{
	free(al);
}

static void set_signals(void)
{
	int sigs[] = {SIGINT, SIGQUIT};
	const int sig_num = sizeof(sigs)/sizeof(int);
	int i = 0;
	for (; i < sig_num; i++)
		signal(sigs[i], sig_handler);
}

static void sig_handler(int signum)
{
	if (signum == SIGINT || signum == SIGQUIT) {
		printf("\n");
		prompt();
		fflush(stdout);
	}
}

static void prompt(void)
{
	printf("[%s@%s %s]$ ", tty_name, host, cur_dir);
}

static char *get_host(void)
{
	static char buf[255];
	if (gethostname(buf, 255) == -1) {
		perror("gethostname");
		return NULL;
	}
	char *q = buf;
	char *h = strtok(q, ".");
	if (h == NULL) {
		perror("strtok");
		return NULL;
	}
	return h;
}

static char *get_curr_dir(void)
{
	static char buf[PATH_MAX];
	if (getcwd(buf, PATH_MAX) == NULL) {
		perror("getcwd");
		return NULL;
	}
	char *dir = NULL;
	char *p = NULL;
	char *q = NULL;
	for (q = buf; (p = strtok(q, "/")) != NULL; q = NULL)
		dir = p;

	return dir;
}
