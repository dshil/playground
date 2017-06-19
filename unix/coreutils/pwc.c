#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *cw(void *a);

struct argset {
	char *fname;
	int bc;
};

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t flag = PTHREAD_COND_INITIALIZER;
struct argset *mailbox;

int main(int ac, char *av[])
{
	char **fnames = av+1;
	if (pthread_mutex_lock(&lock) == -1) {
		perror("pthread_mutex_lock");
		exit(EXIT_FAILURE);
	}

	pthread_t thrds[ac-1];
	struct argset args[ac-1];
	int i = 0;

	for (; i < ac - 1; i++) {
		args[i].fname = fnames[i];
		args[i].bc = 0;

		if (pthread_create(&thrds[i], NULL, cw, (void *)&args[i]) == -1) {
			perror("pthread_create");
			exit(EXIT_FAILURE);
		}
	}

	int msg_num = 0;
	while (msg_num < (ac - 1)) {
		if (pthread_cond_wait(&flag, &lock) == -1) {
			perror("pthread_cond_wait");
			exit(EXIT_FAILURE);
		}

		printf("%d %s\n", mailbox->bc, mailbox->fname);

		mailbox = NULL;
		msg_num++;

		// Notify too fast thread that was tried to read not handled mailbox.
		pthread_cond_signal(&flag);
	}

	for (i = 0; i < ac - 1; i++)
		pthread_join(thrds[i], NULL);

	exit(EXIT_SUCCESS);
}

void *cw(void *a)
{
	struct argset *arg = a;

	FILE *f = fopen(arg->fname, "r");
	if (f == NULL) {
		perror("fopen");
		return NULL;
	}

	int c = 0;
	for (;;) {
		c = fgetc(f);

		if (ferror(f) != 0) {
			perror("fgetc");
			goto error;
		}
		if (feof(f))
			break;

		arg->bc++;
	}

	if (fclose(f) == EOF) {
		perror("fclose");
		return NULL;
	}

	if (pthread_mutex_lock(&lock) == -1) {
		perror("pthread_mutex_lock");
		goto error;
	}

	// It's possible that a thread takes lock too fast and a main thread does
	// not handle value in mailbox that was left by last thread, need to wait
	// until main thread handles this value.
	if (mailbox != NULL)
		pthread_cond_wait(&flag, &lock);

	mailbox = arg;

	if (pthread_cond_signal(&flag) == -1) {
		perror("pthread_cond_signal");
		goto error;
	}

	if (pthread_mutex_unlock(&lock) == -1) {
		perror("pthread_mutex_unlock");
		goto error;
	}

	return NULL;

error:
	if (f != NULL) {
		if (fclose(f) == EOF)
			perror("fclose");
	}
	return NULL;
}
