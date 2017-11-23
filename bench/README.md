<h1>Exploring cost of time related operations.</h1>


**OSX**

    ```
    make osx
    ```

* Receiving current time:

    ```
    mach_absolute_time, op: 1000,     total(ns):      70481, 70 (ns/call)
    mach_absolute_time, op: 1000000,  total(ns):   40923309, 40 (ns/call)
    mach_absolute_time, op: 5000000,  total(ns):  176957351, 35 (ns/call)

    clock_get_time,     op : 1000,    total(ns):     848424, 848 (ns/call)
    clock_get_time,     op : 1000000, total(ns):  806914482, 806 (ns/call)
    clock_get_time,     op : 5000000, total(ns): 4025621753, 805 (ns/call)
    ```

* Sleeping for a specified interval:

    ```
    === 1s wait ===
    wait(ms): 1000, sleep(ms): 1,   clock_sleep(s): 870, 149 μs/call
    wait(ms): 1000, sleep(ms): 2,   clock_sleep(s): 436, 293 μs/call
    wait(ms): 1000, sleep(ms): 10,  clock_sleep(s): 91,  989 μs/call
    wait(ms): 1000, sleep(ms): 100, clock_sleep(s): 9,   11111 μs/call

    === 10s wait ===
    wait(ms): 10000, sleep(ms): 1,   clock_sleep(s): 8792, 137 μs/call
    wait(ms): 10000, sleep(ms): 2,   clock_sleep(s): 4366, 290 μs/call
    wait(ms): 10000, sleep(ms): 10,  clock_sleep(s): 911,  976 μs/call
    wait(ms): 10000, sleep(ms): 100, clock_sleep(s): 99,   1010 μs/call

    === 30s wait ===
    wait(ms): 30000, sleep(ms): 1,   clock_sleep(s): 26336, 139 μs/call
    wait(ms): 30000, sleep(ms): 2,   clock_sleep(s): 13107, 288 μs/call
    wait(ms): 30000, sleep(ms): 10,  clock_sleep(s): 2737,  960 μs/call
    wait(ms): 30000, sleep(ms): 100, clock_sleep(s): 297,   1010 μs/call
    ```

**POSIX**

OSTYPE: darwin14

```
make posix
```

* Receiving current time:

    | function name | number of calls | avg(ns/call) | total(ns) |
    |---------------|-----------------|--------------|-----------|
    | gettimeofday  | 1000            | 64           | 64185     |
    | gettimeofday  | 1000000         | 59           | 59517756  |
    | gettimeofday  | 1000            | 57           | 288058833 |

* Sleeping for a specified interval:

    ```
    === 1s wait ===
    wait(ms): 1000, sleep(ms): 1,   nanosleep(s): 862, 160 μs/call
    wait(ms): 1000, sleep(ms): 2,   nanosleep(s): 436, 293 μs/call
    wait(ms): 1000, sleep(ms): 10,  nanosleep(s): 91,  912 μs/call
    wait(ms): 1000, sleep(ms): 100, nanosleep(s): 9,   777 μs/call

    === 10s wait ===
    wait(ms): 10000, sleep(ms): 1,   nanosleep(s): 8689, 150 μs/call
    wait(ms): 10000, sleep(ms): 2,   nanosleep(s): 4330, 309 μs/call
    wait(ms): 10000, sleep(ms): 10,  nanosleep(s): 916,  915 μs/call
    wait(ms): 10000, sleep(ms): 100, nanosleep(s): 99,   545 μs/call

    === 30s wait ===
    wait(ms): 30000, sleep(ms): 1,   nanosleep(s): 26110, 148 μs/call
    wait(ms): 30000, sleep(ms): 2,   nanosleep(s): 13008, 306 μs/call
    wait(ms): 30000, sleep(ms): 10,  nanosleep(s): 2735,  968 μs/call
    wait(ms): 30000, sleep(ms): 100, nanosleep(s): 298,   496 μs/call
    ```
