#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include <pthread.h>

#include <mach/mach_time.h> /* mach_absolute_time */
#include <mach/mach.h>      /* host_get_clock_service */
#include <mach/clock.h>     /* clock_get_time */

static void handler(int sig) {}

static pthread_once_t once_control = PTHREAD_ONCE_INIT;
static double steady_factor = 0;

static void init_steady_factor() {
    mach_timebase_info_data_t info;

    kern_return_t ret = mach_timebase_info(&info);
    if (ret != KERN_SUCCESS) {
		mach_error("mach_timebase_info: ", ret);
		exit(EXIT_FAILURE);
    }

    steady_factor = (double)info.numer / info.denom;
}

static uint64_t current_time_ns() {
	int err;

    if ((err = pthread_once(&once_control, init_steady_factor))) {
		fprintf(stderr, "pthread_once: %s\n", strerror(err));
		exit(EXIT_FAILURE);
    }

    return (uint64_t) mach_absolute_time() * steady_factor;
}

static void
bench_sleep_for_fn(uint64_t wait_interval,
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
			perror("kill(SIGTERM)");
			exit(EXIT_FAILURE);
		}

		while (waitpid(-1, NULL, WNOHANG) > 0)
			;
	}
}

static uint64_t loopfn(size_t it, uint64_t (*fn)(void))
{
  uint64_t ts = 0;
  for (size_t i = 0; i < it; i++) {
      ts += fn();
  }
  return ts;
}

static void benchfn(size_t it, uint64_t (*fn)(void), const char *name)
{
	uint64_t start = current_time_ns();
	uint64_t stub = loopfn(it, fn);
	uint64_t total = current_time_ns() - start;
	uint64_t avg = total / it;

	printf("%-18s, op: %lu, total(ns): %10llu, %llu (ns/call)\n", name, it,
			total, avg);
}

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
print_sleep_stat(const char *name,
		uint64_t wait_ms,
		uint64_t sleep_ms,
		size_t op_num,
		uint64_t avg)
{
	printf("wait(ms): %llu, sleep(ms): %llu, %s(s): %lu, %llu μs/call\n",
			wait_ms, sleep_ms, name, op_num, avg);
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
	size_t it = 2000000;

	// Test receiving current time.
	benchfn(it, test_mach_absolute_time, "mach_absolute_time");
	benchfn(it, test_clock_get_time, "clock_get_time");
	benchfn(it, test_gettimeofday, "gettimeofday");

	// Test sleeping for a specified interval.
	const uint64_t wait_interval = 1000;

	bench_sleep_for_fn(wait_interval, 1, test_nanosleep, "nanosleep");
	bench_sleep_for_fn(wait_interval, 2, test_nanosleep, "nanosleep");
	bench_sleep_for_fn(wait_interval, 10, test_nanosleep, "nanosleep");
	bench_sleep_for_fn(wait_interval, 100, test_nanosleep, "nanosleep");

	bench_sleep_for_fn(wait_interval, 1, test_mach_clock_sleep, "clock_sleep");
	bench_sleep_for_fn(wait_interval, 2, test_mach_clock_sleep, "clock_sleep");
	bench_sleep_for_fn(wait_interval, 10, test_mach_clock_sleep, "clock_sleep");
	bench_sleep_for_fn(wait_interval, 100, test_mach_clock_sleep, "clock_sleep");
}
