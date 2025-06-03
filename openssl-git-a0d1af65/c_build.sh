#!/bin/bash

# make clean
# export CC=afl-gcc
# export CFLAGS="-fprofile-arcs -ftest-coverage"
# export LDFLAGS="-lgcov --coverage"
# ./Configure linux-x86_64
# bear -- make


# CFLAGS="-g -O0" ./config --prefix=$PWD/build_orig no-shared no-module -DPEDANTIC enable-tls1_3 enable-weak-ssl-ciphers enable-rc5 enable-md2 enable-ssl3 enable-ssl3-method enable-nextprotoneg enable-ec_nistp_64_gcc_128 -fno-sanitize=alignment --debug
# make -j
# make install
# make clean


make clean

export AFL_USE_ASAN=1

export CC=afl-clang-fast
export CXX=afl-clang-fast++

export CFLAGS="-fsanitize=address -g -O1"
export CXXFLAGS="-fsanitize=address -g -O1"
export LDFLAGS="-fsanitize=address"


./config --prefix=$PWD/build_orig no-shared no-module -DPEDANTIC enable-tls1_3 enable-weak-ssl-ciphers enable-rc5 enable-md2 enable-ssl3 enable-ssl3-method enable-nextprotoneg enable-ec_nistp_64_gcc_128 -fno-sanitize=alignment --debug
bear -- make