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
    mode="RELEASE"
    args="$files $FLAGS $R_FLAGS"
elif [ "$1" = "prof" ]; then
    mode="PROFILING"
    args="$files $FLAGS $D_FLAGS $P_FLAGS"
else
    mode="DEBUG"
    args="$files $FLAGS $D_FLAGS"
fi

printf "\033[1;44mBuilding rffmpeg in $mode mode...\033[0m\n"

set -x
$CC -o "bin/rffmpeg" $args || exit 1
