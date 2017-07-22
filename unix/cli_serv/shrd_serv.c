#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>

static void cleanup(int signum);
static int sem_set(int semset_id, int semnum, int val);
static int wait_and_lock(int semset_id, const struct timespec *timeout);
static int release_lock(int semset_id);

sig_atomic_t seg_id = -1;
sig_atomic_t semset_id = -1;

int main(int ac, char *av[])
{
	const int time_mem_key = 99;
	const int time_sem_key = 9900;
	const int seg_size = 100;

	signal(SIGINT, cleanup);

	seg_id = shmget(time_mem_key, seg_size, IPC_CREAT | 0777);
	if (seg_id == -1) {
		perror("shmget");
		exit(EXIT_FAILURE);
	}

	void *memp = shmat(seg_id, NULL, 0);
	if (memp == (void *)-1) {
		perror("shmat");
		goto error;
	}

	errno = 0;
	semset_id = semget(time_sem_key, 2, (0666 | IPC_CREAT | IPC_EXCL));
	if (semset_id == -1) {
		if (errno == EEXIST)
			semset_id = semget(time_sem_key, 2, 0);

		if (semset_id == -1) {
			perror("semget");
			goto error;
		}
	}

	if (sem_set(semset_id, 0, 0) == -1 || sem_set(semset_id, 1, 0) == -1)
		goto error;

	struct timespec timeout;
	timeout.tv_sec = 1;
	timeout.tv_nsec = 0;

	long now = 0;
	char *tm_str = NULL;
	int len = 0;
	while (1) {
		// Break in case of SIGINT.
		if (semset_id == -1 && seg_id == -1)
			break;

		time(&now);

		if (wait_and_lock(semset_id, &timeout) == -1)
			goto error;

		// No need to add '\n' in the end of string.
		tm_str = ctime(&now);
		len = strlen(tm_str);
		tm_str[len - 1] = '\0';
		strcpy(memp, tm_str);

		release_lock(semset_id);

		sleep(1);
	}

	cleanup(0);
	exit(EXIT_SUCCESS);

 error:
	cleanup(0);
	exit(EXIT_FAILURE);
}

/*
 * cleanup is used for a removing semaphores and a shared memory segment. It
 * is used as a signal handler also (SIGINT).
 */
static void cleanup(int signum)
{
	if (seg_id != -1) {
		if (shmctl(seg_id, IPC_RMID, NULL) == -1)
			perror("shmctl");
		seg_id = -1;
	}
	if (semset_id != -1) {
		if (semctl(semset_id, 0, IPC_RMID, NULL) == -1)
			perror("semctl");
		semset_id = -1;
	}
}

union semun {
	int val;		/* Value for SETVAL */
	struct semid_ds *buf;	/* Buffer for IPC_STAT, IPC_SET */
	unsigned short *array;	/* Array for GETALL, SETALL */
	struct seminfo *__buf;	/* Buffer for IPC_INFO
				   (Linux-specific) */
};

/*
 * sets a semaphores value to zero.
 */
static int sem_set(int semset_id, int semnum, int val)
{
	union semun init_val;
	init_val.val = val;
	if (semctl(semset_id, semnum, SETVAL, init_val) == -1) {
		perror("semctl");
		return -1;
	}
	return 0;
}

/*
 * waits until writers numbers will be equal 0, readers number will be equal 0,
 * note, that waiting zero readers number is a timeout based, it was done to
 * avoid a writer to wait infinitely with a lot of readers number.
 * 0 sem number stands for readers.
 * 1 sem number stands for writers.
 */
static int wait_and_lock(int semset_id, const struct timespec *timeout)
{
	struct sembuf acts[3];

	acts[0].sem_num = 1;
	acts[0].sem_flg = SEM_UNDO;
	acts[0].sem_op = 0;

	acts[1].sem_num = 0;
	acts[1].sem_flg = SEM_UNDO;
	acts[1].sem_op = 0;

	acts[2].sem_num = 1;
	acts[2].sem_flg = SEM_UNDO;
	acts[2].sem_op = +1;

	errno = 0;
	semtimedop(semset_id, acts, 3, timeout);
	if (errno != 0) {
		if (errno == EINTR)
			return -1;

		perror("semop");
		return -1;
	}

	return 0;
}

/*
 * A write operation to a shared memory segment was done, number of writers
 * will be decreased.
 */
static int release_lock(int semset_id)
{
	struct sembuf act[1];
	act[0].sem_num = 1;
	act[0].sem_flg = SEM_UNDO;
	act[0].sem_op = -1;

	errno = 0;
	semop(semset_id, act, 1);
	if (errno != 0) {
		if (errno == EINTR)
			return -1;

		perror("semop");
		return -1;
	}
	return 0;
}
