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
test_nanosleep(const char *name, uint64_t total_wait_ms, uint64_t ms)
{
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = ms % 1000 * 1000000;

	size_t cnt = 0;
	uint64_t avg = 0;
	uint64_t rem = 0;

	for (;;) {
		while (nanosleep(&ts, &ts) == -1) {
			if (errno == EINTR) {
				rem = (uint64_t) (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
				if (rem == 0)
					rem = ms;

				// microseconds.
				avg = ((total_wait_ms - cnt * ms - (ms - rem)) * 1000) / cnt;

				print_sleep_stat(name, total_wait_ms, ms, cnt, avg);
				exit(EXIT_SUCCESS);
			} else {
				perror("nanosleep");
				exit(EXIT_FAILURE);
			}
		}
		cnt++;
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
