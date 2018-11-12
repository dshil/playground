# OWD stands for the Open Weather Data

The main goal of `OWD` is to be very simple and share formatted weather data
distributed by any provider.

## Dependencies

 * GNU make
 * CMake
 * GCC
 * Boost
 * Libuv
 * Git
 * OpenSSL

## Quick start

If you want to stay aside and keep your hands clean you should definitely use
Docker. We've prepared ready to use docker images for different OS-s:

   ```sh
    $ docker/fedora/run.sh
   ```

* Fedora (WIP)
* Ubuntu (WIP)
* OSX (WIP)

You've read to this place, so it only means that you want to do the whole dirty
stuff by yourself.

* clone repo:

    ```sh
    $ git clone git@github.com:dshil/owd.git
    ```

* install dependencies (WIP)

    * Ubuntu (WIP)
    * Fedora:

        ```sh
        $ dnf install -y gcc-c++

        $ dnf install -y gengetopt wget git

        $ dnf install -y \
            libuv-devel \
            boost-devel \
            openssl-devel \
            make \
            cmake
        ```

* build and check (Valgrind WIP)

    ```sh
    $ scripts/3rdparty.sh
    $ mkdir build && cd build && cmake .. && make
    ```

* run tests:

    ```sh
    $ bin/test_main
    ```
