#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include "posix_time.h"

static void handler(int sig) {}

void bench_sleep_for_fn(
		uint64_t wait_interval,
		uint64_t sleep_interval_ms,
		void (*fn)(const char *, uint64_t, uint64_t),
		const char *name)
{
	pid_t pid = fork();
	if (pid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		signal(SIGINT, handler);

		fn(name, wait_interval, sleep_interval_ms);
	} else {
		struct timespec ts;
		ts.tv_sec = wait_interval / 1000;
		ts.tv_nsec = wait_interval % 1000 * 1000000;

		while (nanosleep(&ts, &ts) == -1) {
			if (errno != EINTR) {
				perror("nanosleep");
				exit(EXIT_FAILURE);
			}
		}

		if (kill(pid, SIGINT) == -1) {
			perror("kill(SIGINT)");
			exit(EXIT_FAILURE);
		}

		while (waitpid(-1, NULL, WNOHANG) > 0)
			;
	}
}

uint64_t loopfn(size_t it, uint64_t (*fn)(void))
{
  uint64_t ts = 0;
  for (size_t i = 0; i < it; i++) {
      ts += fn();
  }
  return ts;
}

void benchfn(size_t it, uint64_t (*fn)(void), const char *name)
{
	uint64_t start = current_time_ns();
	uint64_t stub = loopfn(it, fn);
	uint64_t total = current_time_ns() - start;
	uint64_t avg = total / it;

	printf("%-18s, op: %lu, total(ns): %10llu, %llu (ns/call)\n", name, it,
			total, avg);
}

void
print_sleep_stat(const char *name,
		uint64_t wait_ms,
		uint64_t sleep_ms,
		size_t op_num,
		uint64_t avg)
{
	printf("wait(ms): %llu, sleep(ms): %llu, %s(s): %lu, %llu μs/call\n",
			wait_ms, sleep_ms, name, op_num, avg);
}
