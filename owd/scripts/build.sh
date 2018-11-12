# !/bin/bash

set -xe

scripts/3rdparty.sh
buildir=build

(mkdir -p $buildir && cd $buildir && cmake -G "Unix Makefiles" .. && make)

bin/test_main
