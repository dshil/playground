#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include <mach/mach_time.h> /* mach_absolute_time */
#include <mach/mach_error.h>

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

uint64_t current_time_ns() {
	int err;

    if ((err = pthread_once(&once_control, init_steady_factor))) {
		fprintf(stderr, "pthread_once: %s\n", strerror(err));
		exit(EXIT_FAILURE);
    }

    return (uint64_t) mach_absolute_time() * steady_factor;
}
