/*
 * calls @fn @it times.
 */
uint64_t loopfn(size_t it, uint64_t (*fn)(void));

/*
 * calculates total, avg time @fn takes for executing @it times, prints a
 * result to stdout.
 */
void benchfn(size_t it, uint64_t (*fn)(void), const char *name);

/*
 * wait for a @wait_interval while provided @fn is executed.
 * @sleep_interval_ms is used a sleep interval for a sleep routine used in @fn.
 * @name is used as a test name.
 * @fn takes test name, @wait_interval, @sleep_interval_ms as input arguments.
 */
void bench_sleep_for_fn(
		uint64_t wait_interval,
		uint64_t sleep_interval_ms,
		void (*fn)(const char *, uint64_t, uint64_t),
		const char *name);

/*
 * pretty prints statistics for bench_sleep_for_fn routine.
 */
void print_sleep_stat(
        const char *name,
		uint64_t wait_ms,
		uint64_t sleep_ms,
		size_t op_num,
		uint64_t avg);

