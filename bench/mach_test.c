#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <mach/mach.h>      /* host_get_clock_service */
#include <mach/clock.h>     /* clock_get_time */

#include "helpers.h"
#include "posix_time.h"

static uint64_t test_mach_absolute_time(void)
{
	return current_time_ns();
}

clock_serv_t host_clock;

static uint64_t test_clock_get_time(void)
{
	kern_return_t ret = KERN_SUCCESS;
    mach_timespec_t now;

    ret = clock_get_time(host_clock, &now);
    if (ret != KERN_SUCCESS) {
		mach_error("clock_get_time: ", ret);
		exit(EXIT_FAILURE);
    }

	return (uint64_t) (now.tv_sec * 10E9 +
			(now.tv_nsec) % 1000000000);
}

static void
test_mach_clock_sleep(const char *name, uint64_t total_wait_ms, uint64_t ms)
{
    mach_timespec_t ts;
    ts.tv_sec = (unsigned int) ms / 1000;
    ts.tv_nsec = (int) ms % 1000 * 1000000;

	size_t cnt = 0;
	uint64_t avg = 0;

    kern_return_t ret = KERN_SUCCESS;
	for (;;) {
		for (;;) {
			ret = clock_sleep(MACH_PORT_NULL, TIME_RELATIVE, ts, NULL);
			if (ret == KERN_SUCCESS) {
				cnt++;
				break;
			}

			if (ret == KERN_ABORTED) {
				avg = ((total_wait_ms - cnt * ms) * 1000) / cnt;
				print_sleep_stat(name, total_wait_ms, ms, cnt, avg);
				exit(EXIT_SUCCESS);
			}

			mach_error("clock_sleep: ", ret);
			exit(EXIT_FAILURE);
		}
	}
}

static void init_host_clock(void)
{
	kern_return_t ret = KERN_SUCCESS;

	ret = host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &host_clock);
	if (ret != KERN_SUCCESS) {
		mach_error("host_get_clock_service: ", ret);
		exit(EXIT_FAILURE);
	}
}

static void start_mach_clock_get_time_test(int *iterations, int iter_len)
{
	init_host_clock();

	for (int i = 0; i < iter_len; ++i) {
		benchfn(*iterations++, test_clock_get_time, "clock_get_time");
	}

	kern_return_t ret = mach_port_deallocate(mach_task_self(), host_clock);
	if (ret != KERN_SUCCESS) {
		mach_error("mach_port_deallocate: ", ret);
		exit(EXIT_FAILURE);
    }
}

int main()
{

	int iterations[] = {1000, 1000000, 5000000};
	size_t it = 2000000;

	for (int i = 0; i < sizeof(iterations)/sizeof(int); ++i) {
		benchfn(iterations[i], test_mach_absolute_time, "mach_absolute_time");
	}

	start_mach_clock_get_time_test(iterations, sizeof(iterations)/sizeof(int));

	int intervals[] = {1000, 10000, 30000};

	for (int i = 0; i < sizeof(intervals)/sizeof(int); ++i) {
		bench_sleep_for_fn(intervals[i], 1, test_mach_clock_sleep, "clock_sleep");
		bench_sleep_for_fn(intervals[i], 2, test_mach_clock_sleep, "clock_sleep");
		bench_sleep_for_fn(intervals[i], 10, test_mach_clock_sleep, "clock_sleep");
		bench_sleep_for_fn(intervals[i], 100, test_mach_clock_sleep, "clock_sleep");
	}
}
