#!/bin/sh
# build script for POSIX systems (Linux/MacOS/FreeBSD)

DEFINES=" -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE"
  FLAGS="-I./lib -std=c99 -lpthread $DEFINES -Wall -Wextra -pedantic -Wno-unused-function"
D_FLAGS="-g"
P_FLAGS="-finstrument-functions -ldl -rdynamic -DINSTRUMENTATION"
R_FLAGS="-DNDEBUG -O2 -s"

files=""
   CC="clang"


for f in $(find . -name "*.c"); do
    files="$files $f";
done

mkdir -p "./bin"

if [ "$1" == "rel" ]; then
    echo "Building rffmpeg in RELEASE mode..."
    echo
    set -x
    $CC -o "bin/rffmpeg" $files $FLAGS $R_FLAGS
elif [ "$1" == "prof" ]; then
    echo "Building rffmpeg in PROFILING mode..."
    echo
    set -x
    $CC -o "bin/rffmpeg" $files $FLAGS $D_FLAGS $P_FLAGS
else
    echo "Building rffmpeg in DEBUG mode..."
    echo
    set -x
    $CC -o "bin/rffmpeg" $files $FLAGS $D_FLAGS
fi

exit 0
