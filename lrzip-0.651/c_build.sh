#!/bin/bash

make clean

export AFL_USE_ASAN=1

export CC=afl-clang-fast
export CXX=afl-clang-fast++

export CFLAGS="-fsanitize=address -g -O1"
export CXXFLAGS="-fsanitize=address -g -O1"
export LDFLAGS="-fsanitize=address"


./autogen.sh
./configure
bear -- make
