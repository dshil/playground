#!/bin/bash

files=( hello.go pingpong01.go pingpong02.go pingpong03.go range.go select.go block.go fanin1.go workers1.go workers2.go )

for i in "${files[@]}"
do
    docker run --rm -it \
    -e GOOS=darwin \
    -v $(pwd):/src divan/golang:gotrace \
        go build -o /src/binary /src/examples/$i
    
    ./binary 2> trace.out
    gotrace ./trace.out ./binary &
    sleep 1
    kill $!
done
