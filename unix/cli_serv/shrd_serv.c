#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>

static void cleanup(int signum);

sig_atomic_t seg_id = -1;
sig_atomic_t semset_id = -1;

int main(int ac, char *av[])
{
	const int time_mem_key = 99;
	const int time_sem_key = 9900;
	const int seg_size = 100;

	signal(SIGINT, cleanup);

	seg_id = shmget(time_mem_key, seg_size, IPC_CREAT|0777);
	if (seg_id == -1) {
		perror("shmget");
		exit(EXIT_FAILURE);
	}

	void *memp = shmat(seg_id, NULL, 0);
	if (memp == (void *)-1) {
		perror("shmat");
		goto error;
	}

	semset_id = semget(time_sem_key, 2, (0666|IPC_CREAT|IPC_EXCL));
	if (semset_id == -1) {
		perror("semget");
		goto error;
	}

	long now = 0;
	while (1) {
		time(&now);
		strcpy(memp, ctime(&now));
		sleep(10);
	}

	cleanup(0);
	exit(EXIT_SUCCESS);

error:
	cleanup(0);
	exit(EXIT_FAILURE);
}

static void cleanup(int signum)
{
	if (seg_id != -1)
		if (shmctl(seg_id, IPC_RMID, NULL) == -1)
			perror("shmctl");
	if (semset_id != -1)
		if (semctl(semset_id, 0, IPC_RMID, NULL) == -1)
			perror("semctl");
}
