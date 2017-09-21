#include <stdlib.h>
#include <pthread.h>

#include <mach/mach.h>      /* host_get_clock_service */
#include <mach/clock.h>     /* clock_get_time */

#include "helpers.h"
#include "posix_time.h"

static uint64_t test_mach_absolute_time(void)
{
	return current_time_ns();
}

static uint64_t test_clock_get_time(void)
{
	kern_return_t ret = KERN_SUCCESS;
	clock_serv_t host_clock;

	ret = host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &host_clock);
	if (ret != KERN_SUCCESS) {
		mach_error("host_get_clock_service: ", ret);
		exit(EXIT_FAILURE);
	}

    mach_timespec_t now;
    ret = clock_get_time(host_clock, &now);
    if (ret != KERN_SUCCESS) {
		mach_error("clock_get_time: ", ret);
		exit(EXIT_FAILURE);
    }

    ret = mach_port_deallocate(mach_task_self(), host_clock);
    if (ret != KERN_SUCCESS) {
		mach_error("mach_port_deallocate: ", ret);
		exit(EXIT_FAILURE);
    }

	return (uint64_t) (now.tv_sec * 10E9 + (now.tv_nsec) / 10E9);
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

int main()
{
	size_t it = 2000000;

	// Test receiving current time.
	benchfn(it, test_mach_absolute_time, "mach_absolute_time");
	benchfn(it, test_clock_get_time, "clock_get_time");

	uint64_t wait_interval = 1000;

	bench_sleep_for_fn(wait_interval, 1, test_mach_clock_sleep, "clock_sleep");
	bench_sleep_for_fn(wait_interval, 2, test_mach_clock_sleep, "clock_sleep");
	bench_sleep_for_fn(wait_interval, 10, test_mach_clock_sleep, "clock_sleep");
	bench_sleep_for_fn(wait_interval, 100, test_mach_clock_sleep, "clock_sleep");
}