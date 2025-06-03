#!/bin/bash

export AFL_USE_ASAN=1
export CC=afl-clang-fast
export CXX=afl-clang-fast++
export CFLAGS="-fsanitize=address -g -O1"
export CXXFLAGS="-fsanitize=address -g -O1"
export LDFLAGS="-fsanitize=address"

# クリーンアップ（エラーを無視）
make clean 2>/dev/null || true
rm -rf build CMakeCache.txt CMakeFiles/

# # 重要: CMakeLists.txtの問題のある行を修正
# echo "Checking and fixing CMakeLists.txt..."
# if [ -f CMakeLists.txt ]; then
#     # 160行目付近の add_subdirectory(build) をコメントアウト
#     sed -i.backup 's/^[[:space:]]*add_subdirectory[[:space:]]*([[:space:]]*build[[:space:]]*)/# &/' CMakeLists.txt
#     echo "Fixed problematic add_subdirectory(build) line"
# fi

# buildディレクトリを作成してビルド
mkdir -p build
cd build
echo "Running cmake..."
cmake ..  # -DCMAKE_EXPORT_COMPILE_COMMANDS=ON 

# CMakeが成功したかチェック
if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

echo "Running make..."
make -j$(nproc)

if [ $? -eq 0 ]; then
    echo "Build completed successfully!"
else
    echo "Build failed!"
    exit 1
fi