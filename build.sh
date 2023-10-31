#!/bin/sh
# build script for POSIX systems (Linux/MacOS/FreeBSD)

CC="clang"
DEFINES="-D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE"
FLAGS="-std=c99 -I./lib -pthread $DEFINES -Wall -Wextra -pedantic"
D_FLAGS="-g -ggdb"
P_FLAGS="-finstrument-functions -ldl -rdynamic -DINSTRUMENTATION"
R_FLAGS="-DNDEBUG -O2 -s"

for f in $(find . -name "*.c"); do
    files="$files $f";
done

mkdir -p "./bin"

if [ "$1" = "rel" ]; then
    printf "Building rffmpeg in RELEASE mode...\n\n"
    args="$files $FLAGS $R_FLAGS"
elif [ "$1" = "prof" ]; then
    printf "Building rffmpeg in PROFILING mode...\n\n"
    args="$files $FLAGS $D_FLAGS $P_FLAGS"
else
    printf "Building rffmpeg in DEBUG mode...\n\n"
    args="$files $FLAGS $D_FLAGS"
fi

set -x
$CC -o "bin/rffmpeg" $args || exit 1
