<h1>Exploring cost of time related operations.</h1>


<h3>OSX</h3>

```
make osx
```

Receiving current time:

```
mach_absolute_time, op: 1000, total(ns):      37620, 37 (ns/call)
mach_absolute_time, op: 1000000, total(ns):   40994990, 40 (ns/call)
mach_absolute_time, op: 5000000, total(ns):  185909170, 37 (ns/call)
```

Sleeping for a specified interval:

```
=== 1s wait ===
wait(ms): 1000, sleep(ms): 1, clock_sleep(s): 874, 144 μs/call
wait(ms): 1000, sleep(ms): 2, clock_sleep(s): 437, 288 μs/call
wait(ms): 1000, sleep(ms): 10, clock_sleep(s): 91, 989 μs/call
wait(ms): 1000, sleep(ms): 100, clock_sleep(s): 9, 11111 μs/call

=== 10s wait ===
wait(ms): 10000, sleep(ms): 1, clock_sleep(s): 8789, 137 μs/call
wait(ms): 10000, sleep(ms): 2, clock_sleep(s): 4376, 285 μs/call
wait(ms): 10000, sleep(ms): 10, clock_sleep(s): 909, 1001 μs/call
wait(ms): 10000, sleep(ms): 100, clock_sleep(s): 99, 1010 μs/call

=== 30s wait ===
wait(ms): 30000, sleep(ms): 1, clock_sleep(s): 26381, 137 μs/call
wait(ms): 30000, sleep(ms): 2, clock_sleep(s): 13110, 288 μs/call
wait(ms): 30000, sleep(ms): 10, clock_sleep(s): 2740, 948 μs/call
wait(ms): 30000, sleep(ms): 100, clock_sleep(s): 298, 671 μs/call

```

<h3>POSIX</h3>

```
make posix
```

Receiving current time:

```
gettimeofday, op: 1000, total(ns):      94455, 94 (ns/call)
gettimeofday, op: 1000000, total(ns):   62007944, 62 (ns/call)
gettimeofday, op: 5000000, total(ns):  285548291, 57 (ns/call)
```

Sleeping for a specified interval:

```
=== 1s wait ===
wait(ms): 1000, sleep(ms): 1, nanosleep(s): 865, 156 μs/call
wait(ms): 1000, sleep(ms): 2, nanosleep(s): 433, 307 μs/call
wait(ms): 1000, sleep(ms): 10, nanosleep(s): 91, 890 μs/call
wait(ms): 1000, sleep(ms): 100, nanosleep(s): 9, 666 μs/call

=== 10s wait ===
wait(ms): 10000, sleep(ms): 1, nanosleep(s): 8700, 149 μs/call
wait(ms): 10000, sleep(ms): 2, nanosleep(s): 4331, 308 μs/call
wait(ms): 10000, sleep(ms): 10, nanosleep(s): 910, 987 μs/call
wait(ms): 10000, sleep(ms): 100, nanosleep(s): 99, 494 μs/call

=== 30s wait ===
wait(ms): 30000, sleep(ms): 1, nanosleep(s): 26227, 143 μs/call
wait(ms): 30000, sleep(ms): 2, nanosleep(s): 13098, 290 μs/call
wait(ms): 30000, sleep(ms): 10, nanosleep(s): 2755, 887 μs/call
wait(ms): 30000, sleep(ms): 100, nanosleep(s): 298, 637 μs/call
```
