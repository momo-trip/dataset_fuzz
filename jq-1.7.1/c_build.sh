#!/bin/bash

make clean
export CFLAGS="-fsanitize=address -g -O1 -Wno-error=maybe-uninitialized"
export CXXFLAGS="-fsanitize=address -g -O1"
export LDFLAGS="-fsanitize=address"


autoreconf -i -f
./configure --enable-maintainer-mode
make 