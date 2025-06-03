#!/bin/bash

rm -rf build

# Phase 1: Build tools with normal compiler
echo "Phase 1: Building tools..."
export CC=gcc
export CXX=g++
unset AFL_USE_ASAN
unset CFLAGS
unset CXXFLAGS
unset LDFLAGS

mkdir build
cd build
cmake .. -DBUILD_wireshark=OFF

# lemonツールのみ先にビルド
make lemon

# Phase 2: Build with AFL++
echo "Phase 2: Building with AFL++..."
export AFL_USE_ASAN=1
export CC=afl-clang-fast
export CXX=afl-clang-fast++
export CFLAGS="-fsanitize=address -g -O1"
export CXXFLAGS="-fsanitize=address -g -O1"
export LDFLAGS="-fsanitize=address"

# メインビルド（依存関係が自動的に解決される）
make


# #!/bin/bash

# rm -rf build

# export AFL_USE_ASAN=1

# export CC=afl-clang-fast
# export CXX=afl-clang-fast++

# export CFLAGS="-fsanitize=address -g -O1"
# export CXXFLAGS="-fsanitize=address -g -O1"
# export LDFLAGS="-fsanitize=address"

# # export CC=afl-gcc
# # export CFLAGS="-fprofile-arcs -ftest-coverage"
# # export LDFLAGS="-lgcov --coverage"

# mkdir build
# cd build
# cmake .. -DBUILD_wireshark=OFF ..
# make