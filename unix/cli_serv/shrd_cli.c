#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

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

	void *memp = shmat(seg_id, NULL, 0);
	if (memp == (void *) -1) {
		perror("shmat");
		exit(EXIT_FAILURE);
	}

	printf("time: %s\n", memp);

	if (shmdt(memp) == -1) {
		perror("shmdt");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
