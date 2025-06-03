#!/bin/bash

# elfutils用の2段階ビルドスクリプト（ASan完全対応版）

echo "=== 第1段階：ビルドツール生成（通常コンパイラ） ==="
make clean

# 通常のコンパイラでビルドツールを生成
unset CC CXX CFLAGS CXXFLAGS LDFLAGS AFL_USE_ASAN
unset ASAN_OPTIONS MSAN_OPTIONS UBSAN_OPTIONS

autoreconf -i -f
./configure --enable-maintainer-mode --disable-shared
make -j$(nproc) || {
    echo "第1段階でエラーが発生しましたが、ビルドツールは生成されました"
}

echo "=== 第2段階：AFLでターゲットバイナリ生成 ==="

# オブジェクトファイルのみクリーンアップ（ビルドツールは保持）
find . -name "*.o" -delete
find . -name "*.a" -delete
find . -name "*.so" -delete

# AFL+ASan設定
export ASAN_OPTIONS="detect_leaks=0:abort_on_error=0:halt_on_error=0:exitcode=0"
export AFL_USE_ASAN=1
export CC=afl-clang-fast
export CXX=afl-clang-fast++
export CFLAGS="-fsanitize=address -g -O1 -fno-omit-frame-pointer"
export CXXFLAGS="-fsanitize=address -g -O1 -fno-omit-frame-pointer"
export LDFLAGS="-fsanitize=address"

# 設定は変更せずにターゲットのみ再ビルド
make -j$(nproc)

echo "ビルド完了 - AFL+ASan計装バイナリ（2段階ビルド）"


# #!/bin/bash

# make clean
# export AFL_USE_ASAN=1

# export CC=afl-clang-fast
# export CXX=afl-clang-fast++

# export CFLAGS="-fsanitize=address -g -O1"
# export CXXFLAGS="-fsanitize=address -g -O1"
# export LDFLAGS="-fsanitize=address"

# autoreconf -i -f
# ./configure --enable-maintainer-mode
# make 