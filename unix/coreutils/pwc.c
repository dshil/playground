#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *cw(void *a);

struct argset {
	char *fname;
	int cnt;
};

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t flag = PTHREAD_COND_INITIALIZER;
struct argset *mailbox;

int main(int ac, char *av[])
{
	pthread_t t1, t2;
	struct argset arg1, arg2;
	arg1.fname = av[1];
	arg1.cnt = 0;
	arg2.fname = av[2];
	arg2.cnt = 0;

	int nc = 0;

	if (pthread_mutex_lock(&lock) == -1) {
		perror("pthread_mutex_lock");
		exit(EXIT_FAILURE);
	}

	if (pthread_create(&t1, NULL, cw, (void *)&arg1) == -1 ||
		pthread_create(&t2, NULL, cw, (void *)&arg2) == -1) {
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}

	while (nc < 2) {
		if (pthread_cond_wait(&flag, &lock) == -1) {
			perror("pthread_cond_wait");
			exit(EXIT_FAILURE);
		}

		if (mailbox == &arg1)
			pthread_join(t1, NULL);
		if (mailbox == &arg2)
			pthread_join(t2, NULL);

		printf("%s, %d\n", mailbox->fname,
				mailbox->cnt);

		mailbox = NULL;
		nc++;

		// Notify too fast thread that was tried to read not handled mailbox.
		pthread_cond_signal(&flag);
	}
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

		arg->cnt++;
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
