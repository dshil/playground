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

static int wait_and_lock(int semset_id);
static int release_lock(int semset_id);

int main(int ac, char *av[])
{
	const int time_mem_key = 99;
	const int time_sem_key = 9900;
	const int seg_size = 100;

	int seg_id = shmget(time_mem_key, seg_size, 0777);
	if (seg_id == -1) {
		perror("shmget");
		exit(EXIT_FAILURE);
	}

	int semset_id = semget(time_sem_key, 2, 0);
	if (semset_id == -1) {
		perror("semget");
		exit(EXIT_FAILURE);
	}

	void *memp = shmat(seg_id, NULL, 0);
	if (memp == (void *)-1) {
		perror("shmat");
		exit(EXIT_FAILURE);
	}

	if (wait_and_lock(semset_id) == -1)
		goto error;

	printf("time: %s\n", memp);

	if (release_lock(semset_id) == -1)
		goto error;

	if (shmdt(memp) == -1) {
		perror("shmdt");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);

 error:
	if (memp != (void *)-1)
		if (shmdt(memp) == -1)
			perror("shmdt");
	exit(EXIT_FAILURE);
}

static int wait_and_lock(int semset_id)
{
	struct sembuf acts[2];

	acts[0].sem_num = 1;	/* index for writers */
	acts[0].sem_flg = SEM_UNDO;
	acts[0].sem_op = 0;

	acts[1].sem_num = 0;	/* index for readers */
	acts[1].sem_flg = SEM_UNDO;
	acts[1].sem_op = +1;

	if (semop(semset_id, acts, 2) == -1) {
		perror("semop");
		return -1;
	}

	return 0;
}

static int release_lock(int semset_id)
{
	struct sembuf act[1];
	act[0].sem_num = 0;
	act[0].sem_flg = SEM_UNDO;
	act[0].sem_op = -1;
	if (semop(semset_id, act, 1) == -1) {
		perror("semop");
		return -1;
	}
	return 0;
}
