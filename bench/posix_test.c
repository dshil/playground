#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>

#include "helpers.h"

static uint64_t test_gettimeofday(void)
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) == -1) {
		perror("gettimeofday");
		exit(EXIT_FAILURE);
    }

    return (uint64_t) tv.tv_sec * 10E9 + (uint64_t) tv.tv_usec / 10E9;
}

static void
test_nanosleep(const char *name, uint64_t total_wait_ms, uint64_t sleep_for_ms)
{
    struct timespec ts;
    ts.tv_sec = sleep_for_ms / 1000;
    ts.tv_nsec = sleep_for_ms % 1000 * 1000000;

	size_t calls_cnt = 0;
	uint64_t rem_ms = 0;
	uint64_t avg_mcs = 0;
	uint64_t total_dur_ms = 0;

	for (;;) {
		while (nanosleep(&ts, &ts) == -1) {
			if (errno == EINTR) {
				rem_ms = (uint64_t) (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
				if (rem_ms != 0)
					rem_ms = sleep_for_ms - rem_ms;

				total_dur_ms = total_wait_ms - calls_cnt * sleep_for_ms;
				avg_mcs = ((total_dur_ms - rem_ms) * 1000) / calls_cnt;

				print_sleep_stat(name,
						total_wait_ms,
						sleep_for_ms,
						calls_cnt,
						avg_mcs);

				exit(EXIT_SUCCESS);
			} else {
				perror("nanosleep");
				exit(EXIT_FAILURE);
			}
		}
		calls_cnt++;
	}
}

int main()
{
	int iterations[] = {1000, 1000000, 5000000};
	size_t i = 0;
	size_t it = 2000000;

	for (; i < sizeof(iterations)/sizeof(int); ++i) {
		benchfn(iterations[i], test_gettimeofday, "gettimeofday");
	}

	int intervals[] = {1000, 10000, 30000};
	i = 0;

	for (; i < sizeof(intervals)/sizeof(int); ++i) {
		bench_sleep_for_fn(intervals[i], 1, test_nanosleep, "nanosleep");
		bench_sleep_for_fn(intervals[i], 2, test_nanosleep, "nanosleep");
		bench_sleep_for_fn(intervals[i], 10, test_nanosleep, "nanosleep");
		bench_sleep_for_fn(intervals[i], 100, test_nanosleep, "nanosleep");
	}
}
