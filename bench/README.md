## Exploring cost of time related operations


**OSX**

* Run benchmarks:

    ```
        make osx
    ```

* Receiving current time:

    | function name      | number of calls | avg (ns/call) |
    |--------------------|-----------------|---------------|
    | mach_absolute_time | 1000            | 35            |
    | mach_absolute_time | 1000000         | 36            |
    | mach_absolute_time | 5000000         | 37            |

    | function name  | number of calls | avg (ns/call) |
    |----------------|-----------------|---------------|
    | clock_get_time | 1000            | 852           |
    | clock_get_time | 1000000         | 831           |
    | clock_get_time | 5000000         | 829           |

* Sleeping for a specified interval:

    | function name | test duration (ms) | sleep duration (ms) | number of calls | avg (μs/call) |
    |---------------|--------------------|---------------------|-----------------|---------------|
    | clock_sleep   | 1000               | 1                   | 866             | 154           |
    | clock_sleep   | 1000               | 2                   | 441             | 267           |
    | clock_sleep   | 1000               | 10                  | 93              | 752           |
    | clock_sleep   | 1000               | 100                 | 9               | 11111         |

    | function name | test duration (ms) | sleep duration (ms) | number of calls | avg (μs/call) |
    |---------------|--------------------|---------------------|-----------------|---------------|
    | clock_sleep   | 10000              | 1                   | 8764            | 141           |
    | clock_sleep   | 10000              | 2                   | 4413            | 266           |
    | clock_sleep   | 10000              | 10                  | 929             | 764           |
    | clock_sleep   | 10000              | 100                 | 99              | 1010          |


    | function name | test duration (ms) | sleep duration (ms) | number of calls | avg (μs/call) |
    |---------------|--------------------|---------------------|-----------------|---------------|
    | clock_sleep   | 30000              | 1                   | 26326           | 139           |
    | clock_sleep   | 30000              | 2                   | 13264           | 261           |
    | clock_sleep   | 30000              | 10                  | 2784            | 775           |
    | clock_sleep   | 30000              | 100                 | 297             | 1010          |

**POSIX**

* OS type: darwin14

* Run benchmarks:

    ```
        make posix
    ```

* Receiving current time:

    | function name | number of calls | avg (ns/call) |
    |---------------|-----------------|---------------|
    | gettimeofday  | 1000            | 64            |
    | gettimeofday  | 1000000         | 59            |
    | gettimeofday  | 5000000         | 57            |

* Sleeping for a specified interval:

    | function name | test duration (ms) | sleep duration (ms) | number of calls | avg (μs/call) |
    |---------------|--------------------|---------------------|-----------------|---------------|
    | nanosleep     | 1000               | 1                   | 866             | 154           |
    | nanosleep     | 1000               | 2                   | 438             | 280           |
    | nanosleep     | 1000               | 10                  | 93              | 741           |
    | nanosleep     | 1000               | 100                 | 9               | 222           |

    | function name | test duration (ms) | sleep duration (ms) | number of calls | avg (μs/call) |
    |---------------|--------------------|---------------------|-----------------|---------------|
    | nanosleep     | 10000              | 1                   | 8623            | 159           |
    | nanosleep     | 10000              | 2                   | 4381            | 282           |
    | nanosleep     | 10000              | 10                  | 928             | 774           |
    | nanosleep     | 10000              | 100                 | 99              | 787           |

    | function name | test duration (ms) | sleep duration (ms) | number of calls | avg (μs/call) |
    |---------------|--------------------|---------------------|-----------------|---------------|
    | nanosleep     | 30000              | 1                   | 26015           | 153           |
    | nanosleep     | 30000              | 2                   | 13222           | 268           |
    | nanosleep     | 30000              | 10                  | 2785            | 769           |
    | nanosleep     | 30000              | 100                 | 297             | 808           |

## License

[MIT](LICENSE)
