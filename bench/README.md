Exploring cost of time related operations for Linux and OS X

OS X

Receiving current time:

```
mach_absolute_time, op: 2000000, total(ns): 77327319,   38 (ns/call)
clock_get_time,     op: 2000000, total(ns): 5433013153, 2716 (ns/call)
gettimeofday,       op: 2000000, total(ns): 124868633,  62 (ns/call)
```

Sleeping for a specified interval:

- 10s wait:

    ```
    wait(ms): 10000, sleep(ms): 1, nanosleep(s): 9267, 79 μs/call
    wait(ms): 10000, sleep(ms): 2, nanosleep(s): 4640, 155 μs/call
    wait(ms): 10000, sleep(ms): 10, nanosleep(s): 956, 459 μs/call
    wait(ms): 10000, sleep(ms): 100, nanosleep(s): 99, 414 μs/call
    wait(ms): 10000, sleep(ms): 1, clock_sleep(s): 9296, 75 μs/call
    wait(ms): 10000, sleep(ms): 2, clock_sleep(s): 4634, 157 μs/call
    wait(ms): 10000, sleep(ms): 10, clock_sleep(s): 952, 504 μs/call
    wait(ms): 10000, sleep(ms): 100, clock_sleep(s): 99, 1010 μs/call
    ```

- 30s wait:

    ```
    wait(ms): 30000, sleep(ms): 1, nanosleep(s): 27793, 79 μs/call
    wait(ms): 30000, sleep(ms): 2, nanosleep(s): 13889, 159 μs/call
    wait(ms): 30000, sleep(ms): 10, nanosleep(s): 2860, 488 μs/call
    wait(ms): 30000, sleep(ms): 100, nanosleep(s): 298, 389 μs/call
    wait(ms): 30000, sleep(ms): 1000, nanosleep(s): 29, 482 μs/call
    wait(ms): 30000, sleep(ms): 1, clock_sleep(s): 27875, 76 μs/call
    wait(ms): 30000, sleep(ms): 2, clock_sleep(s): 13923, 154 μs/call
    wait(ms): 30000, sleep(ms): 10, clock_sleep(s): 2852, 518 μs/call
    wait(ms): 30000, sleep(ms): 100, clock_sleep(s): 298, 671 μs/call
    wait(ms): 30000, sleep(ms): 1000, clock_sleep(s): 29, 34482 μs/call
    ```

- 1m wait:

    ```
    wait(ms): 60000, sleep(ms): 1, nanosleep(s): 55614, 78 μs/call
    wait(ms): 60000, sleep(ms): 2, nanosleep(s): 27741, 162 μs/call
    wait(ms): 60000, sleep(ms): 10, nanosleep(s): 5707, 513 μs/call
    wait(ms): 60000, sleep(ms): 100, nanosleep(s): 598, 222 μs/call
    wait(ms): 60000, sleep(ms): 1000, nanosleep(s): 59, 559 μs/call
    wait(ms): 60000, sleep(ms): 1, clock_sleep(s): 55723, 76 μs/call
    wait(ms): 60000, sleep(ms): 2, clock_sleep(s): 27816, 157 μs/call
    wait(ms): 60000, sleep(ms): 10, clock_sleep(s): 5701, 524 μs/call
    wait(ms): 60000, sleep(ms): 100, clock_sleep(s): 598, 334 μs/call
    wait(ms): 60000, sleep(ms): 1000, clock_sleep(s): 59, 16949 μs/call
    ```
