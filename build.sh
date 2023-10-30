#!/bin/sh
# build script for POSIX systems (Linux/MacOS/FreeBSD)

CC="clang"
DEFINES=" -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE"
FLAGS="-std=c99 -I./lib -pthread $DEFINES -Wall -Wextra -pedantic"
D_FLAGS="-g -ggdb"
P_FLAGS="-finstrument-functions -ldl -rdynamic -DINSTRUMENTATION"
R_FLAGS="-DNDEBUG -O2 -s"

for f in $(find . -name "*.c"); do
    files="$files $f";
done

mkdir -p "./bin"

if [ "$1" = "rel" ]; then
    echo "Building rffmpeg in RELEASE mode..."
    echo
    set -x
    $CC -o "bin/rffmpeg" $files $FLAGS $R_FLAGS || exit 1
elif [ "$1" = "prof" ]; then
    echo "Building rffmpeg in PROFILING mode..."
    echo
    set -x
    $CC -o "bin/rffmpeg" $files $FLAGS $D_FLAGS $P_FLAGS || exit 1
else
    echo "Building rffmpeg in DEBUG mode..."
    echo
    set -x
    $CC -o "bin/rffmpeg" $files $FLAGS $D_FLAGS || exit 1
fi
