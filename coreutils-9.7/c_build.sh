#!/bin/bash

make clean
make distclean

export AFL_USE_ASAN=1
export CC=afl-clang-fast
export CXX=afl-clang-fast++

export CFLAGS="-fsanitize=address -g -O1"
export CXXFLAGS="-fsanitize=address -g -O1"
export LDFLAGS="-fsanitize=address"

./configure

sed -i 's/^man\/.*\.1:/#&/' Makefile
sed -i '/help2man/s/^/#/' Makefile

make -j$(nproc)
