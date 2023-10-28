# build script for POSIX (linux/mac/freebsd/...) (clang)
#!/usr/bin/env bash

# recursive file searching and compilation routine
compile() {
    local DEFINES=" -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE"
    local   FLAGS="-I./lib -std=c99 -lpthread $DEFINES -Wall -Wextra -pedantic -Wno-unused-function"
    local D_FLAGS="-g"
    local P_FLAGS="-finstrument-functions -ldl -rdynamic -DINSTRUMENTATION"
    local R_FLAGS="-DNDEBUG -O2 -s"
    local   files=""

    for f in $(find . -name "*.c"); do
        files="$files $f";
    done

    mkdir -p "./bin"

    if [[ "$1" == "rel" ]]; then
        echo "Building rffmpeg in RELEASE mode..."
        echo
        set -x
        clang -o "bin/rffmpeg" $files $FLAGS $R_FLAGS
    elif [[ "$1" == "prof" ]]; then
        echo "Building rffmpeg in PROFILING mode..."
        echo
        set -x
        clang -o "bin/rffmpeg" $files $FLAGS $D_FLAGS $P_FLAGS
    else
        echo "Building rffmpeg in DEBUG mode..."
        echo
        set -x
        clang -o "bin/rffmpeg" $files $FLAGS $D_FLAGS
    fi

    set +x
}

# entry point of the script
compile $1
