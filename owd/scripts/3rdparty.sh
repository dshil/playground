#! /bin/bash

set -xe

build_dir=3rdparty/deps
httpparser=http-parser
cpprest=cpprest
catch=catch2
crow=crow
clara=clara
cpr=cpr
json=json

function setup_httpparser() {
    httpparser_url=https://github.com/nodejs/$httpparser.git

    git clone $httpparser_url $build_dir/$httpparser
    make -C $build_dir/$httpparser package
}

function setup_catch () {
    catch_url=https://raw.githubusercontent.com/catchorg/Catch2/master/single_include/catch2/catch.hpp

    mkdir -p $build_dir/$catch
    wget $catch_url -P $build_dir/$catch
}

function setup_cpprest () {
    cpprest_url=https://github.com/Microsoft/cpprestsdk.git
    cpprest_version="v2.10.0"

    git clone $cpprest_url $build_dir/$cpprest
    (cd $build_dir/$cpprest && \
        git checkout tags/$cpprest_version -b $cpprest_version)

    mkdir -p $build_dir/$cpprest/build

    (cd $build_dir/$cpprest/build && cmake ../Release \
      -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_SHARED_LIBS=OFF \
      -DBUILD_TESTS=OFF \
      -DBUILD_SAMPLES=OFF && make)
}

function setup_crow () {
    crow_url=https://github.com/ipkn/crow/releases/download/v0.1/crow_all.h
    mkdir -p $build_dir/$crow
    wget $crow_url -P $build_dir/$crow -O $build_dir/$crow/"crow.h"
}

function setup_clara () {
    clara_url=https://raw.githubusercontent.com/catchorg/Clara/master/single_include/clara.hpp
    mkdir -p $build_dir/$clara
    wget $clara_url -P $build_dir/$clara
}

function setup_cpr () {
    cpr_url=https://github.com/whoshuu/cpr.git
    git clone $cpr_url $build_dir/$cpr
    (cd $build_dir/$cpr && git submodule update --init --recursive)
}

function setup_json() {
    json_url=https://github.com/nlohmann/json/releases/download/v3.2.0/json.hpp
    mkdir -p $build_dir/$json
    wget $json_url -P $build_dir/$json
}

if [ ! -d "$build_dir/$httpparser" ]; then
    setup_httpparser
fi

if [ ! -d "$build_dir/$catch" ]; then
    setup_catch
fi

if [ ! -d "$build_dir/$cpprest" ]; then
    setup_cpprest
fi

if [ ! -d "$build_dir/$crow" ]; then
    setup_crow
fi

if [ ! -d "$build_dir/$clara" ]; then
    setup_clara
fi

if [ ! -d "$build_dir/$cpr" ]; then
    setup_cpr
fi

if [ ! -d "$build_dir/$json" ]; then
    setup_json
fi
